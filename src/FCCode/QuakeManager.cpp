
#include "QuakeManager.h"
#include "QLocate.hpp"

/**
 * QuakeManager Implementation Info: 
 * 
 * QuakeManager is implemented as a state machine. 
 * Transitions occur when a state has exhausted its allocated cycles or 
 * the QuakeControlTask is in the IDLE state since that indicates that it 
 * has finished its current task. 
 * 
 * Dependencies: QuakeControlTask, Devices
 * 
 */

// Quake driver setup is initialized when QuakeController constructor is called
QuakeManager::QuakeManager(StateFieldRegistry &registry) : ControlTask<bool>(registry),
    radio_mode_f("radio.mode"),
    qct(registry)
{ 
    radio_mode_f.set(static_cast<unsigned int>(radio_mode_t::config));
    control_cycle_count_fp = find_readable_field<unsigned int>("pan.cycle_no", __FILE__, __LINE__);
    snapshot_size_fp = registry.find_field("downlink_producer.snap_size");
    radio_mo_packet_fp = registry.find_field("downlink_producer.mo_ptr");
    radio_mt_packet_fp = registry.find_field("downlink_producer.mt_ptr");
    radio_err_fp = registry.find_field("downlink_producer.radio_err_ptr");
    last_checkin_cycle = control_cycle_count_fp->get();
}

bool QuakeManager::execute() {
    radio_mode_t mode = static_cast<radio_mode_t>(radio_mode_f.get());
    switch(mode){
        case radio_mode_t::config:
        return dispatch_config();
        case radio_mode_t::wait:
        return dispatch_wait();
        case radio_mode_t::transceive:
        return dispatch_transceive();
        case radio_mode_t::read:
        return dispatch_read();
        case radio_mode_t::write:
        return dispatch_write();
        case radio_mode_t::manual:
        return dispatch_manual();
        default:
            printf(debug_severity::error, "Radio state not defined: %d\n", 
            static_cast<unsigned int>(mode));
            return false;
    }
}

bool QuakeManager::dispatch_config() {
    if (qct.get_current_state() != CONFIG) 
        return write_to_error(Devices::WRONG_STATE);

    if ( no_more_cycles(
            max_config_cycles, 
            static_cast<unsigned int>(radio_mode_t::wait)) )
        return false;

    int err_code = qct.execute();
    // If we have finished executing this command, then transition to waiting
    if ( qct.get_current_state() == IDLE )
        transition_radio_state(static_cast<unsigned int>(radio_mode_t::wait));
    return write_to_error(err_code);
}

bool QuakeManager::dispatch_wait() {
    // If we are done waiting --> load a message
    if ( no_more_cycles(
            max_wait_cycles, 
            static_cast<unsigned int>(radio_mode_t::write)) )
        return false;
    return true;
}

bool QuakeManager::dispatch_transceive() {
    if (qct.get_current_state() != SBDIX) 
        return write_to_error(Devices::WRONG_STATE);

    // If we run out of cycles --> go back to waiting
    if ( no_more_cycles(
            max_transceive_cycles, 
            static_cast<unsigned int>(radio_mode_t::wait)) )
        return false;

    int err_code = qct.execute();

    // If we have finished executing SBDIX, then see if we have a message
    if ( qct.get_current_state() == IDLE )
    {
        // Case 1: We have no comms --> try again
        if (qct.quake.sbdix_r[0] > 4) 
            return write_to_error(err_code);

        // Case 2: We have comms and we have message --> read message
        if (qct.quake.sbdix_r[4] > 0) 
        {
            qct.request_state(SBDRB);
            transition_radio_state(static_cast<unsigned int>(radio_mode_t::read));
        }
        // Case 3: We have comms and we have no message --> load next message
        else
        {
            qct.request_state(SBDWB);
            transition_radio_state(static_cast<unsigned int>(radio_mode_t::write));
        }
    }
    return write_to_error(err_code);
}

bool QuakeManager::dispatch_read() {
    if (qct.get_current_state() != SBDRB) 
        return write_to_error(Devices::WRONG_STATE);

    if ( no_more_cycles(
            max_read_cycles, 
            static_cast<unsigned int>(radio_mode_t::wait)) )
        return false;

    int err_code = qct.execute();

    // If we are done with SBDRB --> save message and load next message
    if (qct.get_current_state() == IDLE)
    {
        radio_mt_packet_fp->set(qct.quake.get_message());
        qct.request_state(SBDWB);
        transition_radio_state(static_cast<unsigned int>(radio_mode_t::write));
    }
    return write_to_error(err_code);
}

bool QuakeManager::dispatch_write() {
    if (qct.get_current_state() != SBDWB) 
        return write_to_error(Devices::WRONG_STATE);

    if ( no_more_cycles(
            max_write_cycles, 
            static_cast<unsigned int>(radio_mode_t::wait)) )
        return false;

    // If we just entered this state --> load current snapshot
    if (qct.get_current_fn_number == 0)
        // TODO: !!!!!!! Find better way to get message length
        qct.set_downlink_msg(radio_mt_packet_fp->get(), strlen(radio_mt_packet_fp->get()));

    int err_code = qct.execute();

    // If we are done with loading messages --> try to transceive
    if (qct.get_current_state() == IDLE)
    {
        qct.request_state(SBDIX);
        transition_radio_state(static_cast<unsigned int>(radio_mode_t::transceive));
    }

    return write_to_error(err_code);
}

bool QuakeManager::dispatch_manual(){
    // In this mode keep checking for comms
    return false;
}

bool QuakeManager::write_to_error(int err_code)
{
    if (err_code == Devices::OK) return true;

    radio_err_fp->set(err_code);

    printf(debug_severity::error, 
        "[Quake Error] Execution failed at radio state %d, quake control state \
        %d, and fn_number %d with error code %d\n", 
        radio_mode_f.get(),
        qct.get_current_state(),
        qct.get_current_fn_number(), 
        (error));
    transition_radio_state(static_cast<unsigned int>(radio_mode_t::wait));
    return false;
}

bool QuakeManager::no_more_cycles(size_t max_cycles, unsigned int new_state)
{
    if (control_cycle_count_fp->get() - last_checkin_cycle > max_cycles)
    {
        printf(debug_severity::notice, 
            "[Quake Notice] Radio State %d has ran out of cycles. \
            Transitioning to %d", 
            radio_mode_f.get(),
            new_state);
        transition_radio_state(new_state);
        return false;
    }
    return true;
}

void QuakeManager::transition_radio_state(unsigned int new_state)
{
    printf(debug_severity::info, 
        "[Quake Info] Transitioning from radio state %d to %d\r\n", 
        radio_mode_f.get(),
        new_state);
    // Update the last checkin cycle
    last_checkin_cycle = control_cycle_count_fp->get();
    radio_mode_f.set(new_state);
}
