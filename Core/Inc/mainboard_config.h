#ifndef MAINBOARD_CONFIG_H
#define MAINBOARD_CONFIG_H

enum {
    adc1_ch0_sd_out = 0,
    adc1_ch1_sd_in,
    adc1_ch2_sd_end,
    adc1_ch3_v5_mcu,
    adc1_ch4_not_used,
    adc1_ch5_inlet_liquid_temp,
    adc1_ch6_outlet_liquid_temp_1,
    adc1_ch7_outlet_liquid_temp_2,
    adc1_ch8_outlet_liquid_temp_5,
    adc1_ch9_outlet_liquid_temp_6,
    adc1_ch10_cooling_consumption,
    adc1_ch11_its_hc_f,
    adc1_ch12_its_lc_f,
    adc1_n_channels
};

enum {
    adc3_ch0_not_used = 0,
    adc3_ch1_not_used,
    adc3_ch2_not_used,
    adc3_ch3_not_used,
    adc3_ch4_plausible_state_rc,
    adc3_ch5_tsal_green,
    adc3_ch6_not_used,
    adc3_ch7_amb_temp,
    adc3_ch8_probing_3v3,
    adc3_ch9_airn_open_mec,
    adc3_ch10_not_used,
    adc3_ch11_not_used,
    adc3_ch12_not_used,
    adc3_ch13_not_used,
    adc3_ch14_airp_open_mec,
    adc3_ch15_imd_ok,
    adc3_n_channels
};

enum {
    dig_fb_precharge_open_com,
    dig_fb_sd_bms_fb,
    dig_fb_sd_imd_fb,
    dig_fb_ext_fault_latched,
    dig_fb_imd_fault_latched,
    dig_fb_bms_fault_latched,
    dig_fb_implausible_state_latched,
    dig_fb_latch_reset,
    dig_fb_airp_open_com,
    dig_fb_airn_open_com,
    dig_fb_bms_ok,
    dig_fb_precharge,
    dig_fb_airn_off,
    dig_fb_airp_off,
    dig_fb_indicator_connected,
    dig_fb_imd_fault_cockpid_led,
    dig_fb_bms_fault_cockpid_led,
    dig_fb_plausible_state,
    dig_fb_plausible_state_persisted,
    dig_fb_ts_less_than_60v,
    dig_fb_precharge_open_mec,
    n_digital_feedbacks
};

#endif // MAINBOARD_CONFIG_H

