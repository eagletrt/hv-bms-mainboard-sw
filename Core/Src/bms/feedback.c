/**
 * @file feedback.c
 * @date 2024-08-10
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Feedback management function
 */

#include "feedback.h"

#include <string.h>

#ifdef CONF_FEEDBACK_MODULE_ENABLE

_STATIC _FeedbackHandler hfeedback;

/**
 * @brief Get the feedback identifier from the digital feedback bit position
 *
 * @param bit The bit position
 *
 * @return FeedbackId The feedback identifier or -1 if not valid
 */
FeedbackId _feedback_get_id_from_digital_bit(const FeedbackDigitalBit bit) {
    switch (bit) {
        case FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM:
            return FEEDBACK_ID_AIRN_OPEN_COM;
        case FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM:
            return FEEDBACK_ID_AIRP_OPEN_COM;
        case FEEDBACK_DIGITAL_BIT_SD_IMD_FB:
            return FEEDBACK_ID_SD_IMD_FB;
        case FEEDBACK_DIGITAL_BIT_SD_BMS_FB:
            return FEEDBACK_ID_SD_BMS_FB;
        case FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM:
            return FEEDBACK_ID_PRECHARGE_OPEN_COM;
        case FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC:
            return FEEDBACK_ID_PRECHARGE_OPEN_MEC;
        case FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V:
            return FEEDBACK_ID_TS_LESS_THAN_60V;
        case FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED:
            return FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED;
        case FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE:
            return FEEDBACK_ID_PLAUSIBLE_STATE;
        case FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED:
            return FEEDBACK_ID_BMS_FAULT_COCKPIT_LED;
        case FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED:
            return FEEDBACK_ID_IMD_FAULT_COCKPIT_LED;
        case FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED:
            return FEEDBACK_ID_INDICATOR_CONNECTED;
        case FEEDBACK_DIGITAL_BIT_LATCH_RESET:
            return FEEDBACK_ID_LATCH_RESET;
        case FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED:
            return FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED;
        case FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED:
            return FEEDBACK_ID_BMS_FAULT_LATCHED;
        case FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED:
            return FEEDBACK_ID_IMD_FAULT_LATCHED;
        case FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED:
            return FEEDBACK_ID_EXT_FAULT_LATCHED;
        default:
            return -1;
    }
}

/**
 * @brief Get the feedback identifier from the analog feedback index
 *
 * @param index The index of the analog feedback
 *
 * @return FeedbackId The feedback identifier or -1 if not valid
 */
FeedbackId _feedback_get_id_from_analog_index(const FeedbackAnalogIndex index) {
    switch (index) {
        case FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC:
            return FEEDBACK_ID_AIRN_OPEN_MEC;
        case FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC:
            return FEEDBACK_ID_AIRP_OPEN_MEC;
        case FEEDBACK_ANALOG_INDEX_IMD_OK:
            return FEEDBACK_ID_IMD_OK;
        case FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC:
            return FEEDBACK_ID_PLAUSIBLE_STATE_RC;
        case FEEDBACK_ANALOG_INDEX_TSAL_GREEN:
            return FEEDBACK_ID_TSAL_GREEN;
        case FEEDBACK_ANALOG_INDEX_PROBING_3V3:
            return FEEDBACK_ID_PROBING_3V3;
        case FEEDBACK_ANALOG_INDEX_SD_OUT:
            return FEEDBACK_ID_SD_OUT;
        case FEEDBACK_ANALOG_INDEX_SD_IN:
            return FEEDBACK_ID_SD_IN;
        case FEEDBACK_ANALOG_INDEX_SD_END:
            return FEEDBACK_ID_SD_END;
        case FEEDBACK_ANALOG_INDEX_V5_MCU:
            return FEEDBACK_ID_V5_MCU;
        default:
            return -1;
    }
}

/**
 * @brief Get the status from an raw analog feedback value
 *
 * @details The 3V3 feedback is the only feedback considered high if between
 * the two thresholds, otherwise it is considered in the error state
 *
 * @param index The index of the analog feedback
 *
 * @return FeedbackStatus The status of the feedback
 */
FeedbackStatus _feedback_get_analog_status(const FeedbackAnalogIndex index) {
    // 3V3 probing is handled differently from the other feedbacks
    if (index == FEEDBACK_ANALOG_INDEX_PROBING_3V3) {
        if (hfeedback.analog[index] >= FEEDBACK_THRESHOLD_LOW_V &&
            hfeedback.analog[index] <= FEEDBACK_THRESHOLD_HIGH_V)
            return FEEDBACK_STATUS_HIGH;
        return FEEDBACK_STATUS_ERROR;
    }

    volt_t thr_high = FEEDBACK_THRESHOLD_HIGH_V;
    volt_t thr_low = FEEDBACK_THRESHOLD_LOW_V;

    // BUG: Feedback voltage is too high
    if (index == FEEDBACK_ANALOG_INDEX_IMD_OK ||
        index == FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC ||
        index == FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC)
        thr_low = 1.4f;

    if (hfeedback.analog[index] >= thr_high)
        return FEEDBACK_STATUS_HIGH;
    if (hfeedback.analog[index] <= thr_low)
        return FEEDBACK_STATUS_LOW;
    return FEEDBACK_STATUS_ERROR;
}

FeedbackReturnCode feedback_init(const feedback_read_digital_all_callback_t read_all, const feedback_start_analog_conversion_callback_t start_conversion) {
    if (read_all == NULL || start_conversion == NULL)
        return FEEDBACK_NULL_POINTER;
    memset(&hfeedback, 0U, sizeof(hfeedback)); 
    hfeedback.read_digital = read_all;
    hfeedback.start_conversion = start_conversion;
    return FEEDBACK_OK;
}

FeedbackReturnCode feedback_update_digital_feedback_all(void) {
    hfeedback.digital = hfeedback.read_digital();
    return FEEDBACK_OK;
}

FeedbackReturnCode feedback_start_analog_conversion_all(void) {
    hfeedback.start_conversion();
    return FEEDBACK_OK;
}

FeedbackReturnCode feedback_update_analog_feedback(const FeedbackAnalogIndex index, const volt_t value) {
    if (index >=FEEDBACK_ANALOG_INDEX_COUNT)
        return FEEDBACK_INVALID_INDEX;
    hfeedback.analog[index] = value; 
    return FEEDBACK_OK;
}

bool feedback_get_digital(const FeedbackDigitalBit bit) {
    if (bit >= FEEDBACK_DIGITAL_BIT_COUNT)
        return false;
    return MAINBOARD_BIT_GET(hfeedback.digital, bit);
}

volt_t feedback_get_analog(const FeedbackAnalogIndex index) {
    if (index >= FEEDBACK_ANALOG_INDEX_COUNT)
        return 0U;
    return hfeedback.analog[index];
}

FeedbackReturnCode feedback_update_status(void) {
    // Update the status of the digital feedbacks
    for (FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        const FeedbackId id = _feedback_get_id_from_digital_bit(bit);
        hfeedback.status[id] = MAINBOARD_BIT_GET(hfeedback.digital, bit) ?
            FEEDBACK_STATUS_HIGH :
            FEEDBACK_STATUS_LOW;
    }

    // Update the status of the analog feedback
    for (FeedbackAnalogIndex i = 0U; i < FEEDBACK_ANALOG_INDEX_COUNT; ++i) {
        const FeedbackId id = _feedback_get_id_from_analog_index(i);
        hfeedback.status[id] = _feedback_get_analog_status(i);
    }
    return FEEDBACK_OK;
}

FeedbackStatus feedback_get_status(const FeedbackId id) {
    if (id >= FEEDBACK_ID_COUNT)
        return FEEDBACK_STATUS_ERROR;
    return hfeedback.status[id];
}

bool feedback_check_values(
    const bit_flag32_t mask,
    const bit_flag32_t value,
    FeedbackId * const out)
{
    for (FeedbackId i = 0U; i < FEEDBACK_ID_COUNT; ++i) {
        // Skip feedback not present inside the bitmask
        if (MAINBOARD_BIT_GET(mask, i) == 0U)
            continue;

        // Check if expected value does not match the current value
        const bool expected_value = MAINBOARD_BIT_GET(value, i);
        if ((hfeedback.status[i] != FEEDBACK_STATUS_LOW || expected_value) &&
            (hfeedback.status[i] != FEEDBACK_STATUS_HIGH || !expected_value))
        {
            // Save the identifier of the feedback with the unexpected values
            if (out != NULL)
                *out = i;
            return false;
        }
    }
    // If this point is reached every checked feedback match the expected value
    if (out != NULL)
        *out = FEEDBACK_ID_UNKNOWN;
    return true;
}

bool feedback_is_digital(const FeedbackId id) {
    switch (id) {
        case FEEDBACK_ID_AIRN_OPEN_COM:
        case FEEDBACK_ID_PRECHARGE_OPEN_COM:
        case FEEDBACK_ID_AIRP_OPEN_COM: 
        case FEEDBACK_ID_PRECHARGE_OPEN_MEC:
        case FEEDBACK_ID_SD_IMD_FB:
        case FEEDBACK_ID_SD_BMS_FB:
        case FEEDBACK_ID_TS_LESS_THAN_60V:
        case FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED:
        case FEEDBACK_ID_PLAUSIBLE_STATE:
        case FEEDBACK_ID_BMS_FAULT_COCKPIT_LED:
        case FEEDBACK_ID_IMD_FAULT_COCKPIT_LED:
        case FEEDBACK_ID_INDICATOR_CONNECTED:
        case FEEDBACK_ID_LATCH_RESET:
        case FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED:
        case FEEDBACK_ID_BMS_FAULT_LATCHED:
        case FEEDBACK_ID_IMD_FAULT_LATCHED:
        case FEEDBACK_ID_EXT_FAULT_LATCHED:
            return true;
        default:
            return false;
    }
}

FeedbackDigitalBit feedback_get_digital_bit_from_id(const FeedbackId id) {
    switch (id) {
        case FEEDBACK_ID_AIRN_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM;
        case FEEDBACK_ID_PRECHARGE_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM;
        case FEEDBACK_ID_AIRP_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM;
        case FEEDBACK_ID_PRECHARGE_OPEN_MEC:
        	return FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC;
        case FEEDBACK_ID_SD_IMD_FB:
        	return FEEDBACK_DIGITAL_BIT_SD_IMD_FB;
        case FEEDBACK_ID_SD_BMS_FB:
        	return FEEDBACK_DIGITAL_BIT_SD_BMS_FB;
        case FEEDBACK_ID_TS_LESS_THAN_60V:
        	return FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V;
        case FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED:
            return FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED;
        case FEEDBACK_ID_PLAUSIBLE_STATE:
            return FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE;
        case FEEDBACK_ID_BMS_FAULT_COCKPIT_LED:
            return FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED;
        case FEEDBACK_ID_IMD_FAULT_COCKPIT_LED:
            return FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED;
        case FEEDBACK_ID_INDICATOR_CONNECTED:
            return FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED;
        case FEEDBACK_ID_LATCH_RESET:
            return FEEDBACK_DIGITAL_BIT_LATCH_RESET;
        case FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED:
            return FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED;
        case FEEDBACK_ID_BMS_FAULT_LATCHED:
            return FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED;
        case FEEDBACK_ID_IMD_FAULT_LATCHED:
        	return FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED;
        case FEEDBACK_ID_EXT_FAULT_LATCHED:
        	return FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED;
        default:
            return FEEDBACK_DIGITAL_BIT_UNKNOWN;
    }
}

FeedbackAnalogIndex feedback_get_analog_index_from_id(const FeedbackId id) {
    switch (id) {
        case FEEDBACK_ID_AIRN_OPEN_MEC:
        	return FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC;
        case FEEDBACK_ID_AIRP_OPEN_MEC:
        	return FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC;
        case FEEDBACK_ID_IMD_OK:
        	return FEEDBACK_ANALOG_INDEX_IMD_OK;
        case FEEDBACK_ID_PLAUSIBLE_STATE_RC:
        	return FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC;
        case FEEDBACK_ID_TSAL_GREEN:
        	return FEEDBACK_ANALOG_INDEX_TSAL_GREEN;
        case FEEDBACK_ID_PROBING_3V3:
        	return FEEDBACK_ANALOG_INDEX_PROBING_3V3;
        case FEEDBACK_ID_SD_OUT:
        	return FEEDBACK_ANALOG_INDEX_SD_OUT;
        case FEEDBACK_ID_SD_IN:
        	return FEEDBACK_ANALOG_INDEX_SD_IN;
        case FEEDBACK_ID_SD_END:
        	return FEEDBACK_ANALOG_INDEX_SD_END;
        case FEEDBACK_ID_V5_MCU:
        	return FEEDBACK_ANALOG_INDEX_V5_MCU;
        default:
            return FEEDBACK_ANALOG_INDEX_UNKNOWN;
    }
}

primary_hv_feedback_status_converted_t * feedback_get_status_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hfeedback.status_can_payload);
    hfeedback.status_can_payload.airn_open_com = (primary_hv_feedback_status_airn_open_com)hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM];
    hfeedback.status_can_payload.precharge_open_com = (primary_hv_feedback_status_precharge_open_com)hfeedback.status[FEEDBACK_ID_PRECHARGE_OPEN_COM];
    hfeedback.status_can_payload.airp_open_com = (primary_hv_feedback_status_airp_open_com)hfeedback.status[FEEDBACK_ID_AIRP_OPEN_COM];
    hfeedback.status_can_payload.airn_open_mec = (primary_hv_feedback_status_airn_open_mec)hfeedback.status[FEEDBACK_ID_AIRN_OPEN_MEC];
    hfeedback.status_can_payload.precharge_open_mec = (primary_hv_feedback_status_precharge_open_mec)hfeedback.status[FEEDBACK_ID_PRECHARGE_OPEN_MEC];
    hfeedback.status_can_payload.airp_open_mec = (primary_hv_feedback_status_airp_open_mec)hfeedback.status[FEEDBACK_ID_AIRP_OPEN_MEC];
    hfeedback.status_can_payload.sd_imd_fb = (primary_hv_feedback_status_sd_imd_fb)hfeedback.status[FEEDBACK_ID_SD_IMD_FB];
    hfeedback.status_can_payload.sd_bms_fb = (primary_hv_feedback_status_sd_bms_fb)hfeedback.status[FEEDBACK_ID_SD_BMS_FB];
    hfeedback.status_can_payload.ts_less_than_60v = (primary_hv_feedback_status_ts_less_than_60v)hfeedback.status[FEEDBACK_ID_TS_LESS_THAN_60V];
    hfeedback.status_can_payload.plausible_state_persisted = (primary_hv_feedback_status_plausible_state_persisted)hfeedback.status[FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED];
    hfeedback.status_can_payload.plausible_state = (primary_hv_feedback_status_plausible_state)hfeedback.status[FEEDBACK_ID_PLAUSIBLE_STATE];
    hfeedback.status_can_payload.not_bms_fault_cockpit_led = (primary_hv_feedback_status_not_bms_fault_cockpit_led)hfeedback.status[FEEDBACK_ID_BMS_FAULT_COCKPIT_LED];
    hfeedback.status_can_payload.not_imd_fault_cockpit_led = (primary_hv_feedback_status_not_imd_fault_cockpit_led)hfeedback.status[FEEDBACK_ID_IMD_FAULT_COCKPIT_LED];
    hfeedback.status_can_payload.indicator_connected = (primary_hv_feedback_status_indicator_connected)hfeedback.status[FEEDBACK_ID_INDICATOR_CONNECTED];
    hfeedback.status_can_payload.not_latch_reset = (primary_hv_feedback_status_not_latch_reset)hfeedback.status[FEEDBACK_ID_LATCH_RESET];
    hfeedback.status_can_payload.plausible_state_latched = (primary_hv_feedback_status_plausible_state_latched)hfeedback.status[FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED];
    hfeedback.status_can_payload.not_bms_fault_latched = (primary_hv_feedback_status_not_bms_fault_latched)hfeedback.status[FEEDBACK_ID_BMS_FAULT_LATCHED];
    hfeedback.status_can_payload.not_imd_fault_latched = (primary_hv_feedback_status_not_imd_fault_latched)hfeedback.status[FEEDBACK_ID_IMD_FAULT_LATCHED];
    hfeedback.status_can_payload.not_ext_fault_latched = (primary_hv_feedback_status_not_ext_fault_latched)hfeedback.status[FEEDBACK_ID_EXT_FAULT_LATCHED];
    hfeedback.status_can_payload.imd_ok = (primary_hv_feedback_status_imd_ok)hfeedback.status[FEEDBACK_ID_IMD_OK];
    hfeedback.status_can_payload.plausible_state_rc = (primary_hv_feedback_status_plausible_state_rc)hfeedback.status[FEEDBACK_ID_PLAUSIBLE_STATE_RC];
    hfeedback.status_can_payload.tsal_green = (primary_hv_feedback_status_tsal_green)hfeedback.status[FEEDBACK_ID_TSAL_GREEN];
    hfeedback.status_can_payload.probing_3v3 = (primary_hv_feedback_status_probing_3v3)hfeedback.status[FEEDBACK_ID_PROBING_3V3];
    hfeedback.status_can_payload.sd_out = (primary_hv_feedback_status_sd_out)hfeedback.status[FEEDBACK_ID_SD_OUT];
    hfeedback.status_can_payload.sd_in = (primary_hv_feedback_status_sd_in)hfeedback.status[FEEDBACK_ID_SD_IN];
    hfeedback.status_can_payload.sd_end = (primary_hv_feedback_status_sd_end)hfeedback.status[FEEDBACK_ID_SD_END];
    hfeedback.status_can_payload.v5_mcu = (primary_hv_feedback_status_v5_mcu)hfeedback.status[FEEDBACK_ID_V5_MCU];
    return &hfeedback.status_can_payload; 
}

primary_hv_feedback_digital_converted_t * feedback_get_digital_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hfeedback.digital_can_payload);
    hfeedback.digital_can_payload.digital_airn_open_com = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    hfeedback.digital_can_payload.digital_precharge_open_com = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM);
    hfeedback.digital_can_payload.digital_airp_open_com = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM);
    hfeedback.digital_can_payload.digital_precharge_open_mec = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC);
    hfeedback.digital_can_payload.digital_sd_imd_fb = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_SD_IMD_FB);
    hfeedback.digital_can_payload.digital_sd_bms_fb = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_SD_BMS_FB);
    hfeedback.digital_can_payload.digital_ts_less_than_60v = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V);
    hfeedback.digital_can_payload.digital_plausible_state_persisted = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED);
    hfeedback.digital_can_payload.digital_plausible_state = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);
    hfeedback.digital_can_payload.digital_not_bms_fault_cockpit_led = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED);
    hfeedback.digital_can_payload.digital_not_imd_fault_cockpit_led = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED);
    hfeedback.digital_can_payload.digital_indicator_connected = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);
    hfeedback.digital_can_payload.digital_not_latch_reset = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_LATCH_RESET);
    hfeedback.digital_can_payload.digital_plausible_state_latched = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED);
    hfeedback.digital_can_payload.digital_not_bms_fault_latched = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED);
    hfeedback.digital_can_payload.digital_not_imd_fault_latched = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED);
    hfeedback.digital_can_payload.digital_not_ext_fault_latched = MAINBOARD_BIT_GET(hfeedback.digital, FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED);
    return &hfeedback.digital_can_payload; 
}

primary_hv_feedback_analog_converted_t * feedback_get_analog_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hfeedback.analog_can_payload);
    hfeedback.analog_can_payload.analog_airn_open_mec = hfeedback.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC];
    hfeedback.analog_can_payload.analog_airp_open_mec = hfeedback.analog[FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC];
    hfeedback.analog_can_payload.analog_imd_ok = hfeedback.analog[FEEDBACK_ANALOG_INDEX_IMD_OK];
    hfeedback.analog_can_payload.analog_plausible_state_rc = hfeedback.analog[FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC];
    hfeedback.analog_can_payload.analog_tsal_green = hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN];
    hfeedback.analog_can_payload.analog_probing_3v3 = hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3];
    hfeedback.analog_can_payload.analog_v5_mcu = FEEDBACK_VOLTAGE_TO_5V_VOLT(hfeedback.analog[FEEDBACK_ANALOG_INDEX_V5_MCU]);
    return &hfeedback.analog_can_payload; 
}

primary_hv_feedback_analog_sd_converted_t * feedback_get_analog_sd_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hfeedback.analog_sd_can_payload);
    hfeedback.analog_sd_can_payload.sd_out = FEEDBACK_VOLTAGE_TO_SD_VOLT(hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_OUT]);
    hfeedback.analog_sd_can_payload.sd_in = FEEDBACK_VOLTAGE_TO_SD_VOLT(hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_IN]);
    hfeedback.analog_sd_can_payload.sd_end = FEEDBACK_VOLTAGE_TO_SD_VOLT(hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_END]);
    return &hfeedback.analog_sd_can_payload; 
}

primary_hv_feedback_enzomma_converted_t * feedback_get_enzomma_payload(const FeedbackId id, size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hfeedback.enzomma_can_payload);
    const bool is_digital = feedback_is_digital(id);
    hfeedback.enzomma_can_payload.feedback = (primary_hv_feedback_enzomma_feedback)id;
    hfeedback.enzomma_can_payload.status = (primary_hv_feedback_enzomma_status)feedback_get_status(id);
    hfeedback.enzomma_can_payload.is_digital = is_digital;
    if (is_digital) {
        hfeedback.enzomma_can_payload.digital = feedback_get_digital(feedback_get_digital_bit_from_id(id));
        hfeedback.enzomma_can_payload.analog = 0.f;
    }
    else {
        hfeedback.enzomma_can_payload.digital = 0U;
        const volt_t volt = feedback_get_analog(feedback_get_analog_index_from_id(id));
        hfeedback.enzomma_can_payload.analog = volt;
    }
    return &hfeedback.enzomma_can_payload;
}

#ifdef CONF_FEEDBACK_STRINGS_ENABLE

_STATIC char * feedback_module_name = "feedback";

_STATIC char * feedback_return_code_name[] = {
    [FEEDBACK_OK] = "ok",
    [FEEDBACK_NULL_POINTER] = "null pointer",
    [FEEDBACK_INVALID_INDEX] = "invalid index"
};

_STATIC char * feedback_return_code_description[] = {
    [FEEDBACK_OK] = "executed succesfully",
    [FEEDBACK_NULL_POINTER] = "attempt to dereference a null pointer",
    [FEEDBACK_INVALID_INDEX] = "the given index is not valid"
};

_STATIC char * feedback_id_name[] = {
    [FEEDBACK_ID_AIRN_OPEN_COM] = "air- open com",
    [FEEDBACK_ID_PRECHARGE_OPEN_COM] = "precharge open com",
    [FEEDBACK_ID_AIRP_OPEN_COM] = "air+ open com", 
    [FEEDBACK_ID_AIRN_OPEN_MEC] = "air- open mec",
    [FEEDBACK_ID_PRECHARGE_OPEN_MEC] = "precharge open mec",
    [FEEDBACK_ID_AIRP_OPEN_MEC] = "air+ open mec", 
    [FEEDBACK_ID_SD_IMD_FB] = "imd shutdown",
    [FEEDBACK_ID_SD_BMS_FB] = "bms shutdowm",
    [FEEDBACK_ID_TS_LESS_THAN_60V] = "ts < 60v",
    [FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED] = "plausible state persisted",
    [FEEDBACK_ID_PLAUSIBLE_STATE] = "plausible state",
    [FEEDBACK_ID_BMS_FAULT_COCKPIT_LED] = "bms fault cockpit led",
    [FEEDBACK_ID_IMD_FAULT_COCKPIT_LED] = "imd fault cockpit led",
    [FEEDBACK_ID_INDICATOR_CONNECTED] = "indicator connected",
    [FEEDBACK_ID_LATCH_RESET] = "latch reset",
    [FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED] = "plausible state latched",
    [FEEDBACK_ID_BMS_FAULT_LATCHED] = "bms fault latched",
    [FEEDBACK_ID_IMD_FAULT_LATCHED] = "imd fault latched",
    [FEEDBACK_ID_EXT_FAULT_LATCHED] = "ext fault latched",
    [FEEDBACK_ID_IMD_OK] = "imd ok",
    [FEEDBACK_ID_PLAUSIBLE_STATE_RC] = "plausible state rc",
    [FEEDBACK_ID_TSAL_GREEN] = "tsal green",
    [FEEDBACK_ID_PROBING_3V3] = "probing 3v3",
    [FEEDBACK_ID_SD_OUT] = "shutdown out",
    [FEEDBACK_ID_SD_IN] = "shutdown in",
    [FEEDBACK_ID_SD_END] = "shutdown end",
    [FEEDBACK_ID_V5_MCU] = "mcu 5v"
};

const char * const feedback_get_feedback_id_name(const FeedbackId id) {
    if (id >= FEEDBACK_ID_COUNT)
        return "unknown";
    return feedback_id_name[id];
}

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#endif // CONF_FEEDBACK_MODULE_ENABLE
