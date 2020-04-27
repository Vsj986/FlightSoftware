/**
 * WARNING: THIS FILE IS AUTOGENERATED. ALL CHANGES WILL BE OVERWRITTEN.
 */

#include "flow_data.hpp"

const std::vector<DownlinkProducer::FlowData> PAN::flow_data = {
{1, true, {"pan.state", "pan.deployed", "pan.sat_designation"}},
{2, true, {"orbit.baseline_pos", "orbit.pos", "piksi.time"}},
{3, true, {"attitude_estimator.q_body_eci", "attitude_estimator.w_body", "adcs_monitor.ssa_mode"}},
{4, true, {"adcs_monitor.wheel1_fault.base", "adcs_monitor.wheel1_fault.override", "adcs_monitor.wheel1_fault.persistence", "adcs_monitor.wheel1_fault.suppress", "adcs_monitor.wheel1_fault.unsignal", "adcs_monitor.wheel2_fault.base", "adcs_monitor.wheel2_fault.override", "adcs_monitor.wheel2_fault.persistence", "adcs_monitor.wheel2_fault.suppress", "adcs_monitor.wheel2_fault.unsignal", "adcs_monitor.wheel3_fault.base", "adcs_monitor.wheel3_fault.override", "adcs_monitor.wheel3_fault.persistence", "adcs_monitor.wheel3_fault.suppress", "adcs_monitor.wheel3_fault.unsignal", "adcs_monitor.wheel_pot_fault.base", "adcs_monitor.wheel_pot_fault.override", "adcs_monitor.wheel_pot_fault.persistence", "adcs_monitor.wheel_pot_fault.suppress", "adcs_monitor.wheel_pot_fault.unsignal"}},
{5, true, {"prop.state", "prop.overpressured.base", "prop.overpressured.override", "prop.overpressured.persistence", "prop.overpressured.suppress", "prop.overpressured.unsignal", "prop.pressurize_fail.base", "prop.pressurize_fail.override", "prop.pressurize_fail.persistence", "prop.pressurize_fail.suppress", "prop.pressurize_fail.unsignal"}},
{6, true, {"gomspace.batt_threshold", "gomspace.low_batt.base", "gomspace.low_batt.override", "gomspace.low_batt.persistence", "gomspace.low_batt.suppress", "gomspace.low_batt.unsignal", "gomspace.get_hk.base", "gomspace.get_hk.override", "gomspace.get_hk.persistence", "gomspace.get_hk.suppress", "gomspace.get_hk.unsignal"}},
{7, true, {"adcs_monitor.rwa_speed_rd", "adcs_monitor.rwa_torque_rd", "adcs_cmd.rwa_speed_cmd", "adcs_cmd.rwa_torque_cmd"}},
{8, true, {}},
{9, true, {"gomspace.vbatt", "gomspace.cursun", "gomspace.cursys", "gomspace.vboost.output1", "gomspace.vboost.output2", "gomspace.vboost.output3", "gomspace.curin.output1", "gomspace.curin.output2", "gomspace.curin.output3"}},
{10, true, {"gomspace.curout.output1", "gomspace.curout.output2", "gomspace.curout.output3", "gomspace.curout.output4", "gomspace.curout.output5", "gomspace.curout.output6", "gomspace.output.output1", "gomspace.output.output2", "gomspace.output.output3", "gomspace.output.output4", "gomspace.output.output5", "gomspace.output.output6"}},
{11, true, {"gomspace.counter_boot", "gomspace.temp.output1", "gomspace.temp.output2", "gomspace.temp.output3", "gomspace.temp.output4", "gomspace.bootcause", "gomspace.battmode", "gomspace.pptmode"}},
{12, true, {"gomspace.power_cycle_output1_cmd", "gomspace.power_cycle_output2_cmd", "gomspace.power_cycle_output3_cmd", "gomspace.power_cycle_output4_cmd", "gomspace.power_cycle_output5_cmd", "gomspace.power_cycle_output6_cmd"}},
{13, true, {"adcs.state", "adcs.paired", "adcs_monitor.functional", "adcs_monitor.functional_fault.base", "adcs_monitor.functional_fault.override", "adcs_monitor.functional_fault.persistence", "adcs_monitor.functional_fault.suppress", "adcs_monitor.functional_fault.unsignal", "adcs_cmd.mtr_cmd"}},
{14, true, {"adcs.compute.vec1.current", "adcs.compute.vec1.desired", "adcs.compute.vec2.current", "adcs.compute.vec2.desired"}},
{15, true, {"adcs_monitor.havt_device0", "adcs_monitor.havt_device1", "adcs_monitor.havt_device2", "adcs_monitor.havt_device3", "adcs_monitor.havt_device4", "adcs_monitor.havt_device5", "adcs_monitor.havt_device6", "adcs_monitor.havt_device7", "adcs_monitor.havt_device8", "adcs_monitor.havt_device9", "adcs_monitor.havt_device10", "adcs_monitor.havt_device11", "adcs_monitor.havt_device12", "adcs_monitor.havt_device13", "adcs_monitor.havt_device14", "adcs_monitor.havt_device15", "adcs_monitor.havt_device16", "adcs_monitor.havt_device17"}},
{16, true, {"radio.err"}},
{17, true, {"docksys.docked", "docksys.dock_config"}},
{18, true, {"adcs_monitor.gyr_vec_flag", "adcs_monitor.mag1_vec_flag", "adcs_monitor.mag2_vec_flag", "adcs_monitor.speed_rd_flag", "adcs_monitor.torque_rd_flag"}},
{19, true, {"piksi.state", "piksi.pos", "piksi.vel", "piksi.baseline_pos", "piksi.fix_error_count"}},
{20, true, {"dcdc.SpikeDock", "dcdc.ADCSMotor"}},
{21, true, {"adcs_monitor.ssa_voltage0", "adcs_monitor.ssa_voltage1", "adcs_monitor.ssa_voltage2", "adcs_monitor.ssa_voltage3", "adcs_monitor.ssa_voltage4", "adcs_monitor.ssa_voltage5", "adcs_monitor.ssa_voltage6", "adcs_monitor.ssa_voltage7", "adcs_monitor.ssa_voltage8", "adcs_monitor.ssa_voltage9", "adcs_monitor.ssa_voltage10", "adcs_monitor.ssa_voltage11", "adcs_monitor.ssa_voltage12", "adcs_monitor.ssa_voltage13", "adcs_monitor.ssa_voltage14", "adcs_monitor.ssa_voltage15", "adcs_monitor.ssa_voltage16", "adcs_monitor.ssa_voltage17", "adcs_monitor.ssa_voltage18", "adcs_monitor.ssa_voltage19"}},
{22, false, {"dcdc.ADCSMotor_cmd", "dcdc.SpikeDock_cmd", "dcdc.disable_cmd", "dcdc.reset_cmd"}},
{23, false, {"adcs_cmd.imu_gyr_filter", "adcs_cmd.imu_gyr_temp_desired", "adcs_cmd.imu_gyr_temp_filter", "adcs_cmd.imu_gyr_temp_kd", "adcs_cmd.imu_gyr_temp_ki", "adcs_cmd.imu_gyr_temp_kp", "adcs_cmd.imu_mag_filter", "adcs_cmd.mag1_mode", "adcs_cmd.mag2_mode", "adcs_cmd.mtr_limit", "adcs_cmd.mtr_mode", "adcs_cmd.rwa_mode", "adcs_cmd.rwa_ramp_filter", "adcs_cmd.rwa_speed_filter", "adcs_cmd.ssa_voltage_filter"}},
{24, false, {"adcs_monitor.gyr_temp", "adcs_monitor.gyr_vec", "adcs_monitor.mag1_vec", "adcs_monitor.mag2_vec", "adcs_monitor.ssa_vec", "adcs_monitor.gyr_temp_flag"}},
{25, false, {"adcs_monitor.ssa_voltage0", "adcs_monitor.ssa_voltage1", "adcs_monitor.ssa_voltage2", "adcs_monitor.ssa_voltage3", "adcs_monitor.ssa_voltage4", "adcs_monitor.ssa_voltage5", "adcs_monitor.ssa_voltage6", "adcs_monitor.ssa_voltage7", "adcs_monitor.ssa_voltage8", "adcs_monitor.ssa_voltage9", "adcs_monitor.ssa_voltage10", "adcs_monitor.ssa_voltage11", "adcs_monitor.ssa_voltage12", "adcs_monitor.ssa_voltage13", "adcs_monitor.ssa_voltage14", "adcs_monitor.ssa_voltage15", "adcs_monitor.ssa_voltage16", "adcs_monitor.ssa_voltage17", "adcs_monitor.ssa_voltage18", "adcs_monitor.ssa_voltage19"}},
{26, false, {"detumble_safety_factor", "docking_timeout_limit", "fault_handler.enabled", "max_radio_silence", "trigger_dist.close_approach", "trigger_dist.docking"}},
{27, false, {"downlink.shift_id1", "downlink.shift_id2", "downlink.toggle_id"}},
{28, false, {"gomspace.heater", "gomspace.heater_cmd"}},
{29, false, {"gomspace.wdt_i2c_time_left", "gomspace.counter_wdt_i2c"}},
{30, false, {"gomspace.gs_reboot_cmd", "gomspace.gs_reset_cmd", "gomspace.heater_cmd", "gomspace.pv1_cmd", "gomspace.pv2_cmd", "gomspace.pv3_cmd", "gomspace.pptmode_cmd", "gomspace.counter_reset_cmd"}},
{31, false, {"timing.adcs_commander.avg_wait", "timing.adcs_commander.num_lates", "timing.adcs_controller.avg_wait", "timing.adcs_controller.num_lates", "timing.adcs_estimator.avg_wait", "timing.adcs_estimator.num_lates", "timing.adcs_monitor.avg_wait", "timing.adcs_monitor.num_lates", "timing.attitude_computer.avg_wait", "timing.attitude_computer.num_lates", "timing.clock_ct.avg_wait", "timing.clock_ct.num_lates", "timing.dcdc_ct.avg_wait", "timing.dcdc_ct.num_lates", "timing.debug.avg_wait", "timing.debug.num_lates"}},
{32, false, {"timing.docking_ct.avg_wait", "timing.docking_ct.num_lates", "timing.downlink_ct.avg_wait", "timing.downlink_ct.num_lates", "timing.eeprom_ct.avg_wait", "timing.eeprom_ct.num_lates", "timing.gomspace_rd.avg_wait", "timing.gomspace_rd.num_lates", "timing.mission_ct.avg_wait", "timing.mission_ct.num_lates", "timing.piksi.avg_wait", "timing.piksi.num_lates", "timing.quake.avg_wait", "timing.quake.num_lates", "timing.uplink_ct.avg_wait", "timing.uplink_ct.num_lates"}},
{33, false, {"adcs_cmd.havt_disable0", "adcs_cmd.havt_disable1", "adcs_cmd.havt_disable2", "adcs_cmd.havt_disable3", "adcs_cmd.havt_disable4", "adcs_cmd.havt_disable5", "adcs_cmd.havt_disable6", "adcs_cmd.havt_disable7", "adcs_cmd.havt_disable8", "adcs_cmd.havt_disable9", "adcs_cmd.havt_disable10", "adcs_cmd.havt_disable11", "adcs_cmd.havt_disable12", "adcs_cmd.havt_disable13", "adcs_cmd.havt_disable14", "adcs_cmd.havt_disable15", "adcs_cmd.havt_disable16", "adcs_cmd.havt_disable17", "adcs_cmd.havt_reset0", "adcs_cmd.havt_reset1", "adcs_cmd.havt_reset2", "adcs_cmd.havt_reset3", "adcs_cmd.havt_reset4", "adcs_cmd.havt_reset5", "adcs_cmd.havt_reset6", "adcs_cmd.havt_reset7", "adcs_cmd.havt_reset8", "adcs_cmd.havt_reset9", "adcs_cmd.havt_reset10", "adcs_cmd.havt_reset11", "adcs_cmd.havt_reset12", "adcs_cmd.havt_reset13", "adcs_cmd.havt_reset14", "adcs_cmd.havt_reset15", "adcs_cmd.havt_reset16", "adcs_cmd.havt_reset17"}},
{34, false, {"radio.max_transceive", "radio.max_wait"}},
{35, false, {"docksys.is_turning", "docksys.config_cmd"}},
{36, false, {"piksi_fh.no_cdpgs_max_wait", "piksi_fh.cdpgs_delay_max_wait", "piksi_fh.enabled"}}
};
