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
FeedbackId _feedback_get_id_from_digital_bit(FeedbackDigitalBit bit) {
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
FeedbackId _feedback_get_id_from_analog_index(FeedbackAnalogIndex index) {
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
FeedbackStatus _feedback_get_analog_status(FeedbackAnalogIndex index) {
    // 3V3 probing is handled differently from the other feedbacks
    if (index == FEEDBACK_ANALOG_INDEX_PROBING_3V3) {
        if (hfeedback.analog[index] >= FEEDBACK_THRESHOLD_LOW &&
            hfeedback.analog[index] <= FEEDBACK_THRESHOLD_HIGH)
            return FEEDBACK_STATUS_HIGH;
        return FEEDBACK_STATUS_ERROR;
    }

    if (hfeedback.analog[index] >= FEEDBACK_THRESHOLD_HIGH)
        return FEEDBACK_STATUS_HIGH;
    if (hfeedback.analog[index] <= FEEDBACK_THRESHOLD_LOW)
        return FEEDBACK_STATUS_LOW;
    return FEEDBACK_STATUS_ERROR;
}

FeedbackReturnCode feedback_init(
    feedback_read_digital_all_callback_t read_all,
    feedback_start_analog_conversion_callback_t start_conversion)
{
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

FeedbackReturnCode feedback_update_analog_feedback(FeedbackAnalogIndex index, raw_volt_t value) {
    if (index >=FEEDBACK_ANALOG_INDEX_COUNT)
        return FEEDBACK_INVALID_INDEX;
    hfeedback.analog[index] = value; 
    return FEEDBACK_OK;
}

FeedbackReturnCode feedback_update_status(void) {
    // Update the status of the digital feedbacks
    for (FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        FeedbackId id = _feedback_get_id_from_digital_bit(bit);
        hfeedback.status[id] = MAINBOARD_BIT_GET(hfeedback.digital, bit) ?
            FEEDBACK_STATUS_HIGH :
            FEEDBACK_STATUS_LOW;
    }

    // Update the status of the analog feedback
    for (FeedbackAnalogIndex i = 0U; i < FEEDBACK_ANALOG_INDEX_COUNT; ++i) {
        FeedbackId id = _feedback_get_id_from_analog_index(i);
        hfeedback.status[id] = _feedback_get_analog_status(i);
    }
    return FEEDBACK_OK;
}

FeedbackStatus feedback_get_status(FeedbackId id) {
    if (id >= FEEDBACK_ID_COUNT)
        return FEEDBACK_STATUS_ERROR;
    return hfeedback.status[id];
}

bool feedback_check_values(bit_flag32_t mask, bit_flag32_t value) {
    for (bit_pos_t i = 0U; i <= FEEDBACK_COUNT; ++i) {
        // Skip feedback not present inside the bitmask
        if (MAINBOARD_BIT_GET(mask, i) == 0U)
            continue;

        // Check if expected value does not match the current value
        bool expected_value = MAINBOARD_BIT_GET(value, i);
        if ((hfeedback.status[i] != FEEDBACK_STATUS_LOW || expected_value) &&
            (hfeedback.status[i] != FEEDBACK_STATUS_HIGH || !expected_value))
            return false;
    }
    return true;
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
    [FEEDBACK_ID_TS_LESS_THAN_60V] = "ts < 60V",
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
    [FEEDBACK_ID_PROBING_3V3] = "probing 3V3",
    [FEEDBACK_ID_SD_OUT] = "shutdown out",
    [FEEDBACK_ID_SD_IN] = "shutdown in",
    [FEEDBACK_ID_SD_END] = "shutdown end",
    [FEEDBACK_ID_V5_MCU] = "MCU 5V"
};

const char * const feedback_get_feedback_id_name(FeedbackId id) {
    if (id >= FEEDBACK_ID_COUNT)
        return "";
    return feedback_id_name[id];
}

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#endif // CONF_FEEDBACK_MODULE_ENABLE
