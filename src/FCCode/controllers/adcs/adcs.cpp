/** @file controllers/adcs.cpp
 * @author Tanishq Aggarwal
 * @date 6 Feb 2018
 * @brief Contains implementation for the ADCS state controller.
 */

#include "../controllers.hpp"
#include <ADCS/global.hpp>
#include <rwmutex.hpp>
#include "../../state/state_holder.hpp"
#include "../../deployment_timer.hpp"
#include "../../data_collection/data_collection.hpp"
#include <AttitudePDcontrol.hpp>
#include <AttitudeEstimator.hpp>
#include <AttitudeMath.hpp>
#include <tensor.hpp>
#include <MomentumControl.hpp>

namespace RTOSTasks {
    THD_WORKING_AREA(adcs_controller_workingArea, 2048);
    threads_queue_t adcs_detumbled; // TODO implement into the functions below
    threads_queue_t adcs_pointing_accomplished; // TODO implement into the functions below
}
using State::ADCS::ADCSState;
using State::ADCS::adcs_state_lock;
using Devices::adcs_system;

using namespace ADCSControllers;

static unsigned char ssa_mode = SSAMode::IN_PROGRESS;
static unsigned int ssa_tries = 0; // Number of consecutive loops that we've tried to
                                   // collect SSA data
static void read_adcs_data() {
    adcs_system.set_ssa_mode(ssa_mode);

    rwMtxRLock(&State::GNC::gnc_state_lock);
	for(int i = 0; i < 3; i++) ADCSControllers::Estimator::r_gps_ecef[i] = State::GNC::gps_position[i];
	for(int i = 0; i < 3; i++) ADCSControllers::Estimator::v_gps_ecef[i] = State::GNC::gps_velocity[i];
	for(int i = 0; i < 3; i++) ADCSControllers::Estimator::r2other_gps_ecef[i] = State::GNC::gps_position_other[i];
	for(int i = 0; i < 3; i++) ADCSControllers::Estimator::v2other_gps_ecef[i] = State::GNC::gps_velocity_other[i];
    for(int i = 0; i < 4; i++) ADCSControllers::Estimator::q_gps_ecef[i] = State::GNC::ecef_to_eci[i];
    ADCSControllers::Estimator::time = (unsigned int) State::GNC::get_current_time();
    rwMtxRUnlock(&State::GNC::gnc_state_lock);
    
    float rwa_speed_cmds_rd[3], rwa_ramps_rd[3];
    adcs_system.get_rwa(rwa_speed_cmds_rd, ADCSControllers::Estimator::hwheel_sensor_body, rwa_ramps_rd);
    adcs_system.get_imu(ADCSControllers::Estimator::rate_sensor_body, ADCSControllers::Estimator::magfield_sensor_body);
    rwMtxWLock(&State::ADCS::adcs_state_lock);
        for(int i = 0; i < 3; i++) State::ADCS::rwa_ramps_rd[i] = rwa_ramps_rd[i];
        for(int i = 0; i < 3; i++) State::ADCS::rwa_speed_cmds_rd[i] = rwa_speed_cmds_rd[i];
        for(int i = 0; i < 3; i++) State::ADCS::rwa_speeds_rd[i] = ADCSControllers::Estimator::hwheel_sensor_body[i];
        for(int i = 0; i < 3; i++) State::ADCS::gyro_data[i] = ADCSControllers::Estimator::rate_sensor_body[i];
        for(int i = 0; i < 3; i++) State::ADCS::mag_data[i] = ADCSControllers::Estimator::magfield_sensor_body[i];
    rwMtxWUnlock(&State::ADCS::adcs_state_lock);

    std::array<float, 3> ssa_vec;
    if (ssa_mode == SSAMode::IN_PROGRESS) {
        adcs_system.get_ssa(ssa_mode, ADCSControllers::Estimator::sun2sat_sensor_body);
        rwMtxWLock(&State::ADCS::adcs_state_lock);
            for(int i = 0; i < 3; i++) State::ADCS::ssa_vec[i] = ADCSControllers::Estimator::sun2sat_sensor_body[i];
        rwMtxWLock(&State::ADCS::adcs_state_lock);
        if (ssa_mode == SSAMode::COMPLETE) {
            rwMtxWLock(&adcs_state_lock);
            State::ADCS::ssa_vec = ssa_vec;
            State::ADCS::is_sun_vector_determination_working = true;
            State::ADCS::is_sun_vector_collection_working = true;
            rwMtxWUnlock(&adcs_state_lock);
            ssa_mode = SSAMode::IN_PROGRESS;
            ssa_tries = 0;
        }
        else if (ssa_mode == SSAMode::FAILURE) {
            rwMtxWLock(&adcs_state_lock);
            State::ADCS::is_sun_vector_determination_working = false;
            State::ADCS::is_sun_vector_collection_working = true;
            rwMtxWUnlock(&adcs_state_lock);
            // Don't worry; just keep trying, but let the ground know that SSA collection
            // is failing
            ssa_mode = SSAMode::IN_PROGRESS;
            ssa_tries = 0;
        }
        else if (ssa_tries < 5) {
            ssa_tries++;
        }
        else {
            // Tried too many times to collect SSA data and failed.
            State::ADCS::is_sun_vector_determination_working = false;
            State::ADCS::is_sun_vector_collection_working = false;
            // Don't worry; just keep trying, but let the ground know that SSA collection
            // is failing
            ssa_mode = SSAMode::IN_PROGRESS;
            ssa_tries = 0;
        }
    }
}

static THD_WORKING_AREA(adcs_loop_workingArea, 4096);
static THD_FUNCTION(adcs_loop, arg) {
    chRegSetThreadName("ADCS LOOP");

    systime_t t0 = chVTGetSystemTimeX();
    while(true) {
        t0 += S2ST(1000);

        // Reading and writing to Kalman filter
        // Read
        read_adcs_data();
        //State::ADCS::Estimator::update();

        // State machine
        rwMtxRLock(&State::ADCS::adcs_state_lock);
        bool adcs_control_allowed = (State::ADCS::adcs_state != State::ADCS::ADCS_SAFE_HOLD);
        rwMtxRUnlock(&State::ADCS::adcs_state_lock);
        if (adcs_control_allowed) {
            rwMtxRLock(&adcs_state_lock);
                ADCSState adcs_state = State::ADCS::adcs_state;
            rwMtxRUnlock(&adcs_state_lock);
            switch(adcs_state) {
                case ADCSState::ADCS_DETUMBLE: {
                    for(int i = 0; i < 3; i++) MomentumControl::magfield[i] = ADCSControllers::Estimator::magfield_filter_body[i];
                    for(int i = 0; i < 3; i++) MomentumControl::momentum[i] = ADCSControllers::Estimator::htotal_filter_body[i];
                    MomentumControl::update();
                    rwMtxWLock(&adcs_state_lock);
                        for(int i = 0; i < 3; i++) State::ADCS::mtr_cmds[i] = MomentumControl::moment[i];
                    rwMtxWUnlock(&adcs_state_lock);
                    rwMtxRLock(&adcs_state_lock);
                        std::array<float, 3> mtr_cmds = State::ADCS::mtr_cmds;
                        adcs_system.set_mtr_cmd(mtr_cmds.data());
                    rwMtxRUnlock(&adcs_state_lock);
                }
                break;
                case ADCSState::ZERO_TORQUE: {
                    // Set MTR to zero in state
                    rwMtxWLock(&adcs_state_lock);
                        for(int i = 0; i < 3; i++) State::ADCS::mtr_cmds[i] = 0.0f;
                    rwMtxWUnlock(&adcs_state_lock);
                    // Command constant wheel speed and MTR
                    std::array<float, 3> mtr_cmds, rwa_speed_cmds;
                    rwMtxRLock(&adcs_state_lock);
                        mtr_cmds = State::ADCS::mtr_cmds;
                        rwa_speed_cmds = State::ADCS::rwa_speed_cmds;
                    rwMtxRUnlock(&adcs_state_lock);
                    adcs_system.set_mtr_cmd(mtr_cmds.data());
                    adcs_system.set_rwa_mode(RWAMode::SPEED_CTRL, rwa_speed_cmds.data());
                }
                break;
                case ADCSState::POINTING: {
                    rwMtxRLock(&adcs_state_lock);
                        quat_rot_diff(State::ADCS::cmd_attitude.data(), State::ADCS::cur_attitude.data(), AttitudePD::deltaquat);
                        for(int i = 0; i < 3; i++) AttitudePD::angrate[i] = State::ADCS::cur_ang_rate[i];
                    rwMtxRUnlock(&adcs_state_lock);
                    AttitudePD::update();
                    rwMtxWLock(&adcs_state_lock);
                        for(int i = 0; i < 3; i++) State::ADCS::rwa_ramps[i] = AttitudePD::torque[i];
                    rwMtxWUnlock(&adcs_state_lock);
                    adcs_system.set_rwa_mode(RWAMode::ACCEL_CTRL, AttitudePD::torque);
                }
                break;
                case ADCSState::ADCS_SAFE_HOLD: {
                    adcs_system.set_mode(Mode::PASSIVE);
                }
                break;
                default: {
                    rwMtxRLock(&adcs_state_lock);
                        State::ADCS::adcs_state = ADCSState::ADCS_SAFE_HOLD;
                    rwMtxRUnlock(&adcs_state_lock);
                }
            }
        }
        chThdSleepUntil(t0);
    }
}

static THD_WORKING_AREA(check_hat_workingArea, 1024);
static THD_FUNCTION(check_hat, args) {
    systime_t t = chVTGetSystemTimeX();
    while(true) {
        t += MS2ST(RTOSTasks::LoopTimes::ADCS_HAT_CHECK);

        chMtxLock(&State::Hardware::adcs_device_lock);
        adcs_system.update_hat();
        chMtxUnlock(&State::Hardware::adcs_device_lock);

        rwMtxWLock(&State::ADCS::adcs_state_lock);
        // TODO write to actual state
        rwMtxWUnlock(&State::ADCS::adcs_state_lock);

        chThdSleepUntil(t);
    }
}

void RTOSTasks::adcs_controller(void *arg) {
    chRegSetThreadName("ADCS");
    debug_println("ADCS controller process has started.");
    
    rwMtxObjectInit(&adcs_state_lock);
    chThdCreateStatic(adcs_loop_workingArea, sizeof(adcs_loop_workingArea), 
        RTOSTasks::adcs_thread_priority, adcs_loop, NULL);

    DataCollection::initialize_adcs_history_timers();

    // Create HAT checker thread
    chThdCreateStatic(check_hat_workingArea, 
        sizeof(check_hat_workingArea), RTOSTasks::adcs_thread_priority, check_hat, NULL);

    debug_println("Waiting for deployment timer to finish.");
    rwMtxRLock(&State::Master::master_state_lock);
        bool is_deployed = State::Master::is_deployed;
    rwMtxRUnlock(&State::Master::master_state_lock);
    if (!is_deployed) chThdEnqueueTimeoutS(&deployment_timer_waiting, S2ST(DEPLOYMENT_LENGTH));
    debug_println("Deployment timer has finished.");
    
    debug_println("Initializing main operation...");
    chMtxLock(&State::Hardware::adcs_device_lock);
        adcs_system.set_mode(Mode::ACTIVE);
    chMtxLock(&State::Hardware::adcs_device_lock);

    chThdExit((msg_t)0);
}