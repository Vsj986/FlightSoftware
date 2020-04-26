# Runs mission from startup state to standby state.
from .base import Case, SingleSatOnlyCase, TestCaseFailure

class MissionStateMachineUtilities(object):
    def setup_initial_conditions(self):
        self.deployment_hold_length = 100 # Number of cycles for which the satellite will be in a deployment hold. This
                                          # is an item that is configured on Flight Software.
        self.elapsed_deployment = int(self.sim.flight_controller.read_state("pan.deployment.elapsed"))
        self.max_detumble_cycles = 100 # Number of cycles for which we expect the satellite to be in detumble

    def setup_boot_to_standby(self):
        # Let's be generous with what angular rate is allowable as "detumbled."
        self.sim.flight_controller.write_state("detumble_safety_factor", 10)

        # Prevent ADCS faults from causing transition to initialization hold
        self.sim.flight_controller.write_state("adcs_monitor.functional_fault.suppress", "true")
        self.sim.flight_controller.write_state("adcs_monitor.wheel1_fault.suppress", "true")
        self.sim.flight_controller.write_state("adcs_monitor.wheel2_fault.suppress", "true")
        self.sim.flight_controller.write_state("adcs_monitor.wheel3_fault.suppress", "true")
        self.sim.flight_controller.write_state("adcs_monitor.wheel_pot_fault.suppress", "true")

    def query_fc(self):
        """Query satellite state on every cycle for mission manager-specific data"""

        satellite_state = self.mission_states.get_by_num(\
            int(self.sim.flight_controller.read_state("pan.state"))]
        true_elapsed = self.sim.flight_controller.read_state("pan.deployment.elapsed")

        return satellite_state, true_elapsed

    def run_boot_to_standby(self):
        satellite_state, true_elapsed = self.query_fc()

        if not hasattr(self, "boot_stage"):
            self.boot_stage = "startup"

        if self.boot_stage == 'startup':
            self.logger.put("")
            self.logger.put("[TESTCASE] Entering startup state.")

            if not satellite_state == "startup":
                raise TestCaseFailure(f"Satellite was not in state {self.boot_stage}.")
            self.boot_stage = 'deployment_hold'
            self.logger.put("[TESTCASE] Waiting for the deployment period to be over.")

        elif self.boot_stage == 'deployment_hold':
            self.elapsed_deployment += 1

            if self.elapsed_deployment == self.deployment_hold_length:
                if satellite_state == "detumble":
                    self.logger.put("[TESTCASE] Deployment period is over. Entering detumble state.")
                    self.num_detumble_cycles = 0
                    self.boot_stage = 'detumble_wait'
                elif satellite_state == "initialization_hold":
                    raise TestCaseFailure("Satellite went to initialization hold instead of detumble.")
                else:
                    raise TestCaseFailure(f"Satellite failed to exit deployment wait period. \
                        Elapsed deployment period was {true_elapsed}.")

        elif self.boot_stage == 'detumble_wait':
            self.num_detumble_cycles += 1
            if self.num_detumble_cycles >= self.max_detumble_cycles or satellite_state == "standby":
                if satellite_state == "standby":
                    self.logger.put("[TESTCASE] Successfully detumbled. Now in standby state.")
                    self.boot_stage = 'standby'
                else:
                    raise TestCaseFailure("Satellite failed to exit detumble.")

        return self.boot_stage

    def run_boot_to_follower(self):
        # Boot to standby first
        if not self.run_boot_to_standby() == "standby": return False
        self.sim.flight_controller.write_state("pan.state", self.mission_states["follower"])
        self.logger.put("[TESTCASE] Now in follower state.")
        return True

    def run_boot_to_leader(self):
        # Boot to standby first
        if not self.run_boot_to_standby() == "standby": return False
        self.sim.flight_controller.write_state("pan.state", self.mission_states["leader"])
        self.logger.put("[TESTCASE] Now in leader state.")
        return True

class DeploymentToStandby(SingleSatOnlyCase, MissionStateMachineUtilities):
    @property
    def sim_duration(self):
        return float("inf")

    @property
    def sim_initial_state(self):
        # Start the spacecraft with zero angular rate in the body frame, 
        # so that we exit detumble almost immediately.
        return "startup"

    def setup_case_singlesat(self):
        self.setup_initial_conditions()
        self.setup_boot_to_standby()

    def run_case_singlesat(self):
        if self.run_boot_to_standby() == 'standby':
            self.finish()
