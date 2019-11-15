#pragma once

#ifndef DOCKING_CONTROLLER_HPP_
#define DOCKING_CONTROLLER_HPP_

#include "../../test/StateFieldRegistryMock.hpp"
#include <TimedControlTask.hpp>
#include "../../lib/Drivers/DockingSystem.hpp"

class DockingController : public TimedControlTask<void> {
   public:
    
    /**
     * @brief Construct a new Docking Controller object
     * 
     * @param registry 
     * @param offset
     * @param docksys 
     */
    DockingController(StateFieldRegistry& registry, unsigned int offset,
        Devices::DockingSystem &docksys);

    /**
     * @brief Sets statefield variables and moves the motor to docking/undocking
     * configuration should mission manager request it.
     */
    void execute() override;

   protected:
    Devices::DockingSystem& docksys;

    //shared pointer set by mission manager - tells control task to dock or undock motor
    std::shared_ptr<WritableStateField<bool>> docking_config_cmd_fp;

    //state field returns whether or not the spacecraft are docked with one another
    Serializer<bool> docked_sr;
    ReadableStateField<bool> docked_f;

    //state field returns whether or not the motor is in the docking configuration
    Serializer<bool> dock_config_sr;
    ReadableStateField<bool> dock_config_f;

    //state field returns whether or not the motor is turning/in the process of (un)docking
    Serializer<bool> is_turning_sr;
    ReadableStateField<bool> is_turning_f;

};

#endif