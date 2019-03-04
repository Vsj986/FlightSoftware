/** @file device_declarations.cpp
 * @author Tanishq Aggarwal
 * @date 6 Feb 2018
 * @brief Contains initializations for device objects representing devices in the satellite.
 */

#include <i2c_t3.h>
#include "device_states.hpp"
#include "state_holder.hpp"

namespace Devices {
    Gomspace gomspace(&State::Gomspace::gomspace_data,
                      &State::Gomspace::gomspace_config,
                      &State::Gomspace::gomspace_config2, 
                      Wire, Gomspace::ADDRESS);
    QLocate quake(&Serial3, QLocate::DEFAULT_NR_PIN, QLocate::DEFAULT_TIMEOUT);
    Piksi piksi(Serial4);
    DCDC dcdc(DCDC::DEFAULT_ENABLE_PIN);
    SpikeAndHold spike_and_hold(SpikeAndHold::DEFAULT_VALVE_PINS, 
                                SpikeAndHold::DEFAULT_ENABLE_PIN);
    ADCS adcs_system(Wire, ADCS::ADDRESS);
    PressureSensor pressure_sensor(PressureSensor::DEFAULT_LOW_PRESSURE_PIN,
                                   PressureSensor::DEFAULT_HIGH_PRESSURE_PIN);
    TempSensor temp_sensor_inner(TempSensor::DEFAULT_PIN_INNER_TANK);
    TempSensor temp_sensor_outer(TempSensor::DEFAULT_PIN_OUTER_TANK);
    DockingSwitch docking_switch(DockingSwitch::DEFAULT_SWITCH_PIN);
    DockingMotor docking_motor(DockingMotor::DEFAULT_MOTOR_I1_PIN,
                               DockingMotor::DEFAULT_MOTOR_I2_PIN,
                               DockingMotor::DEFAULT_MOTOR_DIRECTION_PIN,
                               DockingMotor::DEFAULT_MOTOR_SLEEP_PIN,
                               DockingMotor::DEFAULT_MOTOR_STEP_PIN);
}

namespace State {
namespace Hardware {
    std::map<std::string, Devices::Device&> devices = {
        {Devices::adcs_system.name(), Devices::adcs_system},
        {Devices::dcdc.name(), Devices::dcdc},
        {Devices::docking_motor.name(), Devices::docking_motor},
        {Devices::docking_switch.name(), Devices::docking_switch},
        {Devices::gomspace.name(), Devices::gomspace},
        {Devices::piksi.name(), Devices::piksi},
        {Devices::pressure_sensor.name(), Devices::pressure_sensor},
        {Devices::quake.name(), Devices::quake},
        {Devices::spike_and_hold.name(), Devices::spike_and_hold},
        {Devices::temp_sensor_inner.name(), Devices::temp_sensor_inner},
        {Devices::temp_sensor_outer.name(), Devices::temp_sensor_outer}
    };
    static Hardware::DeviceState adcs_device_state = {false, false, false, false};
    static Hardware::DeviceState dcdc_device_state = {false, false, false, false};
    static Hardware::DeviceState docking_motor_device_state = {false, false, false, false};
    static Hardware::DeviceState docking_switch_device_state = {false, false, false, false};
    static Hardware::DeviceState gomspace_device_state = {false, false, false, false};
    static Hardware::DeviceState piksi_device_state = {false, false, false, false};
    static Hardware::DeviceState pressure_sensor_device_state = {false, false, false, false};
    static Hardware::DeviceState quake_device_state = {false, false, false, false};
    static Hardware::DeviceState sph_device_state = {false, false, false, false};
    static Hardware::DeviceState temp_sensor_inner_device_state = {false, false, false, false};
    static Hardware::DeviceState temp_sensor_outer_device_state = {false, false, false, false};
    std::map<std::string, Hardware::DeviceState&> hat {
        {Devices::adcs_system.name(), adcs_device_state},
        {Devices::dcdc.name(), dcdc_device_state},
        {Devices::docking_motor.name(), docking_motor_device_state},
        {Devices::docking_switch.name(), docking_switch_device_state},
        {Devices::gomspace.name(), gomspace_device_state},
        {Devices::piksi.name(), piksi_device_state},
        {Devices::pressure_sensor.name(), pressure_sensor_device_state},
        {Devices::quake.name(), quake_device_state},
        {Devices::spike_and_hold.name(), sph_device_state},
        {Devices::temp_sensor_inner.name(), temp_sensor_inner_device_state},
        {Devices::temp_sensor_outer.name(), temp_sensor_outer_device_state},
    };
    bool is_hardware_setup = false;
    rwmutex_t hat_lock;

    mutex_t dcdc_lock;
    mutex_t adcs_device_lock;
    mutex_t spike_and_hold_lock;
    mutex_t piksi_device_lock;
    mutex_t quake_device_lock;
    mutex_t gomspace_device_lock;
}

namespace ADCS {
    static Hardware::DeviceState adcs_gyro_state = {false, false, false, false};
    static Hardware::DeviceState adcs_magnetometer_state = {false, false, false, false};
    static Hardware::DeviceState adcs_magnetorquer_x_state = {false, false, false, false};
    static Hardware::DeviceState adcs_magnetorquer_y_state = {false, false, false, false};
    static Hardware::DeviceState adcs_magnetorquer_z_state = {false, false, false, false};
    static Hardware::DeviceState adcs_motorpot_state = {false, false, false, false};
    static Hardware::DeviceState adcs_motor_x_state = {false, false, false, false};
    static Hardware::DeviceState adcs_motor_y_state = {false, false, false, false};
    static Hardware::DeviceState adcs_motor_z_state = {false, false, false, false};
    static Hardware::DeviceState adcs_adc_motor_x_state = {false, false, false, false};
    static Hardware::DeviceState adcs_adc_motor_y_state = {false, false, false, false};
    static Hardware::DeviceState adcs_adc_motor_z_state = {false, false, false, false};
    static Hardware::DeviceState adcs_ssa_adc_1_state = {false, false, false, false};
    static Hardware::DeviceState adcs_ssa_adc_2_state = {false, false, false, false};
    static Hardware::DeviceState adcs_ssa_adc_3_state = {false, false, false, false};
    static Hardware::DeviceState adcs_ssa_adc_4_state = {false, false, false, false};
    static Hardware::DeviceState adcs_ssa_adc_5_state = {false, false, false, false};
    std::map<std::string, Hardware::DeviceState&> adcs_hat {
        {"Gyroscope", adcs_gyro_state},
        {"Magnetometer", adcs_magnetometer_state},
        {"Magnetorquer X", adcs_magnetorquer_x_state},
        {"Magnetorquer Y", adcs_magnetorquer_y_state},
        {"Magnetorquer Z", adcs_magnetorquer_z_state},
        {"Motor Potentiometer", adcs_motorpot_state},
        {"Motor X", adcs_motor_x_state},
        {"Motor Y", adcs_motor_y_state},
        {"Motor Z", adcs_motor_z_state},
        {"Motor X ADC", adcs_adc_motor_x_state},
        {"Motor Y ADC", adcs_adc_motor_y_state},
        {"Motor Z ADC", adcs_adc_motor_z_state},
        {"Sun Sensor ADC 1", adcs_ssa_adc_1_state},
        {"Sun Sensor ADC 2", adcs_ssa_adc_2_state},
        {"Sun Sensor ADC 3", adcs_ssa_adc_3_state},
        {"Sun Sensor ADC 4", adcs_ssa_adc_4_state},
        {"Sun Sensor ADC 5", adcs_ssa_adc_5_state},
    };
}
}