/*!
 * \file feedback-api.c
 * \date 2026-05-2
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */

#include "feedback-api.h"

#include <string.h>

#ifdef CONF_FEEDBACK_MODULE_ENABLE

EAGLETRT_STATIC struct FeedbackHandler feedback_handler;

/*!
 * \brief Get the feedback identifier from the digital feedback bit position
 *
 * \param bit The bit position
 *
 * \return enum FeedbackId The feedback identifier or -1 if not valid
 */
enum FeedbackId prv_feedback_get_id_from_digital_bit(const enum FeedbackDigitalBit bit) {
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

/*!
 * \brief Get the feedback identifier from the analog feedback index
 *
 * \param index The index of the analog feedback
 *
 * \return enum FeedbackId The feedback identifier or -1 if not valid
 */
enum FeedbackId prv_feedback_get_id_from_analog_index(const enum FeedbackAnalogIndex index) {
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

/*!
 * \brief Get the status from an raw analog feedback value
 *
 * \details The 3V3 feedback is the only feedback considered high if between
 * the two thresholds, otherwise it is considered in the error state
 *
 * \param index The index of the analog feedback
 *
 * \return enum FeedbackStatus The status of the feedback
 */
enum FeedbackStatus prv_feedback_api_get_analog_status(const enum FeedbackAnalogIndex index) {
    // 3V3 probing is handled differently from the other feedbacks
    if (index == FEEDBACK_ANALOG_INDEX_PROBING_3V3) {
        if (feedback_handler.analog[index] >= FEEDBACK_THRESHOLD_LOW_V &&
            feedback_handler.analog[index] <= FEEDBACK_THRESHOLD_HIGH_V) {
            return FEEDBACK_STATUS_HIGH;
        }
        return FEEDBACK_STATUS_ERROR;
    }

    constexpr volt_t thr_high = FEEDBACK_THRESHOLD_HIGH_V;
    volt_t thr_low = FEEDBACK_THRESHOLD_LOW_V;

    constexpr volt_t special_low_threshold = 1.6F;

    // BUG: Feedback voltage is too high
    if (index == FEEDBACK_ANALOG_INDEX_IMD_OK ||
        index == FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC ||
        index == FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC) {
        thr_low = special_low_threshold;
    }

    if (feedback_handler.analog[index] >= thr_high) {
        return FEEDBACK_STATUS_HIGH;
    }
    if (feedback_handler.analog[index] <= thr_low) {
        return FEEDBACK_STATUS_LOW;
    }
    return FEEDBACK_STATUS_ERROR;
}

enum FeedbackReturnCode feedback_api_init(const feedback_read_digital_all_callback read_all, const feedback_start_analog_conversion_callback start_conversion) {
    if (read_all == NULL || start_conversion == NULL) {
        return FEEDBACK_RC_NULL_POINTER;
    }
    memset(&feedback_handler, 0U, sizeof(feedback_handler));
    feedback_handler.read_digital = read_all;
    feedback_handler.start_conversion = start_conversion;
    return FEEDBACK_RC_OK;
}

enum FeedbackReturnCode feedback_api_update_digital_feedback_all(void) {
    feedback_handler.digital = feedback_handler.read_digital();
    return FEEDBACK_RC_OK;
}

enum FeedbackReturnCode feedback_api_start_analog_conversion_all(void) {
    feedback_handler.start_conversion();
    return FEEDBACK_RC_OK;
}

enum FeedbackReturnCode feedback_api_update_analog_feedback(const enum FeedbackAnalogIndex index, const volt_t value) {
    if (index >= FEEDBACK_ANALOG_INDEX_COUNT) {
        return FEEDBACK_RC_INVALID_INDEX;
    }
    feedback_handler.analog[index] = value;
    return FEEDBACK_RC_OK;
}

bool feedback_api_get_digital(const enum FeedbackDigitalBit bit) {
    if (bit >= FEEDBACK_DIGITAL_BIT_COUNT) {
        return false;
    }
    return EAGLETRT_API_BIT_GET(feedback_handler.digital, bit);
}

volt_t feedback_api_get_analog(const enum FeedbackAnalogIndex index) {
    if (index >= FEEDBACK_ANALOG_INDEX_COUNT) {
        return 0U;
    }
    return feedback_handler.analog[index];
}

uint32_t debug_cnt = 0U;
enum FeedbackReturnCode feedback_api_update_status(void) {
    // Update the status of the digital feedbacks
    for (enum FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        const enum FeedbackId id_fb = prv_feedback_get_id_from_digital_bit(bit);
        feedback_handler.status[id_fb] = EAGLETRT_API_BIT_GET(feedback_handler.digital, bit) ? FEEDBACK_STATUS_HIGH : FEEDBACK_STATUS_LOW;
    }

    // Update the status of the analog feedback
    for (enum FeedbackAnalogIndex i = 0U; i < FEEDBACK_ANALOG_INDEX_COUNT; ++i) {
        const enum FeedbackId id_fb = prv_feedback_get_id_from_analog_index(i);
        // feedback_handler.status[id_fb] = prv_feedback_api_get_analog_status(i);
        enum FeedbackStatus status = prv_feedback_api_get_analog_status(i);
        // BUG: Noise cause AIR feedbacks voltage to change too much
        if (i == FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC || i == FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC) {
            if (status == FEEDBACK_STATUS_ERROR) {
                ++debug_cnt;
                status = feedback_handler.analog[i] >= FEEDBACK_THRESHOLD_HIGH_V ? FEEDBACK_STATUS_HIGH : FEEDBACK_STATUS_LOW;
            } else {
                debug_cnt = 0U;
            }
        }
        feedback_handler.status[id_fb] = status;
    }
    return FEEDBACK_RC_OK;
}

enum FeedbackStatus feedback_api_get_status(const enum FeedbackId id_fb) {
    if (id_fb >= FEEDBACK_ID_COUNT) {
        return FEEDBACK_STATUS_ERROR;
    }
    return feedback_handler.status[id_fb];
}

bool feedback_api_check_values(const bit_flag32_t mask, const bit_flag32_t value, enum FeedbackId *const out) {
    for (enum FeedbackId i = 0U; i < FEEDBACK_ID_COUNT; ++i) {
        // Skip feedback not present inside the bitmask
        if (EAGLETRT_API_BIT_GET(mask, i) == 0U) {
            continue;
        }

        // Check if expected value does not match the current value
        const bool expected_value = EAGLETRT_API_BIT_GET(value, i);
        if ((feedback_handler.status[i] != FEEDBACK_STATUS_LOW || expected_value) &&
            (feedback_handler.status[i] != FEEDBACK_STATUS_HIGH || !expected_value)) {
            // Save the identifier of the feedback with the unexpected values
            if (out != NULL) {
                *out = i;
            }
            return false;
        }
    }
    // If this point is reached every checked feedback match the expected value
    if (out != NULL) {
        *out = FEEDBACK_ID_UNKNOWN;
    }
    return true;
}

bool feedback_api_is_digital(const enum FeedbackId id_fb) {
    switch (id_fb) {
        case FEEDBACK_ID_AIRN_OPEN_COM: //NOLINT
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

enum FeedbackDigitalBit feedback_api_get_digital_bit_from_id(const enum FeedbackId id_fb) {
    switch (id_fb) {
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

enum FeedbackAnalogIndex feedback_api_get_analog_index_from_id(const enum FeedbackId id_fb) {
    switch (id_fb) {
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

primary_hv_feedback_status_converted_t *feedback_api_get_status_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(feedback_handler.status_can_payload);
    }
    feedback_handler.status_can_payload.airn_open_com = (primary_hv_feedback_status_airn_open_com)feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM];
    feedback_handler.status_can_payload.precharge_open_com = (primary_hv_feedback_status_precharge_open_com)feedback_handler.status[FEEDBACK_ID_PRECHARGE_OPEN_COM];
    feedback_handler.status_can_payload.airp_open_com = (primary_hv_feedback_status_airp_open_com)feedback_handler.status[FEEDBACK_ID_AIRP_OPEN_COM];
    feedback_handler.status_can_payload.airn_open_mec = (primary_hv_feedback_status_airn_open_mec)feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_MEC];
    feedback_handler.status_can_payload.precharge_open_mec = (primary_hv_feedback_status_precharge_open_mec)feedback_handler.status[FEEDBACK_ID_PRECHARGE_OPEN_MEC];
    feedback_handler.status_can_payload.airp_open_mec = (primary_hv_feedback_status_airp_open_mec)feedback_handler.status[FEEDBACK_ID_AIRP_OPEN_MEC];
    feedback_handler.status_can_payload.sd_imd_fb = (primary_hv_feedback_status_sd_imd_fb)feedback_handler.status[FEEDBACK_ID_SD_IMD_FB];
    feedback_handler.status_can_payload.sd_bms_fb = (primary_hv_feedback_status_sd_bms_fb)feedback_handler.status[FEEDBACK_ID_SD_BMS_FB];
    feedback_handler.status_can_payload.ts_less_than_60v = (primary_hv_feedback_status_ts_less_than_60v)feedback_handler.status[FEEDBACK_ID_TS_LESS_THAN_60V];
    feedback_handler.status_can_payload.plausible_state_persisted = (primary_hv_feedback_status_plausible_state_persisted)feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED];
    feedback_handler.status_can_payload.plausible_state = (primary_hv_feedback_status_plausible_state)feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE];
    feedback_handler.status_can_payload.not_bms_fault_cockpit_led = (primary_hv_feedback_status_not_bms_fault_cockpit_led)feedback_handler.status[FEEDBACK_ID_BMS_FAULT_COCKPIT_LED];
    feedback_handler.status_can_payload.not_imd_fault_cockpit_led = (primary_hv_feedback_status_not_imd_fault_cockpit_led)feedback_handler.status[FEEDBACK_ID_IMD_FAULT_COCKPIT_LED];
    feedback_handler.status_can_payload.indicator_connected = (primary_hv_feedback_status_indicator_connected)feedback_handler.status[FEEDBACK_ID_INDICATOR_CONNECTED];
    feedback_handler.status_can_payload.not_latch_reset = (primary_hv_feedback_status_not_latch_reset)feedback_handler.status[FEEDBACK_ID_LATCH_RESET];
    feedback_handler.status_can_payload.plausible_state_latched = (primary_hv_feedback_status_plausible_state_latched)feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED];
    feedback_handler.status_can_payload.not_bms_fault_latched = (primary_hv_feedback_status_not_bms_fault_latched)feedback_handler.status[FEEDBACK_ID_BMS_FAULT_LATCHED];
    feedback_handler.status_can_payload.not_imd_fault_latched = (primary_hv_feedback_status_not_imd_fault_latched)feedback_handler.status[FEEDBACK_ID_IMD_FAULT_LATCHED];
    feedback_handler.status_can_payload.not_ext_fault_latched = (primary_hv_feedback_status_not_ext_fault_latched)feedback_handler.status[FEEDBACK_ID_EXT_FAULT_LATCHED];
    feedback_handler.status_can_payload.imd_ok = (primary_hv_feedback_status_imd_ok)feedback_handler.status[FEEDBACK_ID_IMD_OK];
    feedback_handler.status_can_payload.plausible_state_rc = (primary_hv_feedback_status_plausible_state_rc)feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE_RC];
    feedback_handler.status_can_payload.tsal_green = (primary_hv_feedback_status_tsal_green)feedback_handler.status[FEEDBACK_ID_TSAL_GREEN];
    feedback_handler.status_can_payload.probing_3v3 = (primary_hv_feedback_status_probing_3v3)feedback_handler.status[FEEDBACK_ID_PROBING_3V3];
    feedback_handler.status_can_payload.sd_out = (primary_hv_feedback_status_sd_out)feedback_handler.status[FEEDBACK_ID_SD_OUT];
    feedback_handler.status_can_payload.sd_in = (primary_hv_feedback_status_sd_in)feedback_handler.status[FEEDBACK_ID_SD_IN];
    feedback_handler.status_can_payload.sd_end = (primary_hv_feedback_status_sd_end)feedback_handler.status[FEEDBACK_ID_SD_END];
    feedback_handler.status_can_payload.v5_mcu = (primary_hv_feedback_status_v5_mcu)feedback_handler.status[FEEDBACK_ID_V5_MCU];
    return &feedback_handler.status_can_payload;
}

primary_hv_feedback_digital_converted_t *feedback_api_get_digital_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(feedback_handler.digital_can_payload);
    }
    feedback_handler.digital_can_payload.digital_airn_open_com = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    feedback_handler.digital_can_payload.digital_precharge_open_com = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM);
    feedback_handler.digital_can_payload.digital_airp_open_com = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM);
    feedback_handler.digital_can_payload.digital_precharge_open_mec = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC);
    feedback_handler.digital_can_payload.digital_sd_imd_fb = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_SD_IMD_FB);
    feedback_handler.digital_can_payload.digital_sd_bms_fb = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_SD_BMS_FB);
    feedback_handler.digital_can_payload.digital_ts_less_than_60v = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V);
    feedback_handler.digital_can_payload.digital_plausible_state_persisted = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED);
    feedback_handler.digital_can_payload.digital_plausible_state = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);
    feedback_handler.digital_can_payload.digital_not_bms_fault_cockpit_led = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED);
    feedback_handler.digital_can_payload.digital_not_imd_fault_cockpit_led = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED);
    feedback_handler.digital_can_payload.digital_indicator_connected = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);
    feedback_handler.digital_can_payload.digital_not_latch_reset = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_LATCH_RESET);
    feedback_handler.digital_can_payload.digital_plausible_state_latched = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED);
    feedback_handler.digital_can_payload.digital_not_bms_fault_latched = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED);
    feedback_handler.digital_can_payload.digital_not_imd_fault_latched = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED);
    feedback_handler.digital_can_payload.digital_not_ext_fault_latched = EAGLETRT_API_BIT_GET(feedback_handler.digital, FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED);
    return &feedback_handler.digital_can_payload;
}

primary_hv_feedback_analog_converted_t *feedback_api_get_analog_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(feedback_handler.analog_can_payload);
    }
    feedback_handler.analog_can_payload.analog_airn_open_mec = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC];
    feedback_handler.analog_can_payload.analog_airp_open_mec = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC];
    feedback_handler.analog_can_payload.analog_imd_ok = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK];
    feedback_handler.analog_can_payload.analog_plausible_state_rc = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC];
    feedback_handler.analog_can_payload.analog_tsal_green = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN];
    feedback_handler.analog_can_payload.analog_probing_3v3 = feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3];
    feedback_handler.analog_can_payload.analog_v5_mcu = FEEDBACK_VOLTAGE_TO_5V_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_V5_MCU]);
    return &feedback_handler.analog_can_payload;
}

primary_hv_feedback_analog_sd_converted_t *feedback_api_get_analog_sd_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(feedback_handler.analog_sd_can_payload);
    }
    feedback_handler.analog_sd_can_payload.sd_out = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT]);
    feedback_handler.analog_sd_can_payload.sd_in = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN]);
    feedback_handler.analog_sd_can_payload.sd_end = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_END]);
    return &feedback_handler.analog_sd_can_payload;
}

primary_hv_feedback_enzomma_converted_t *feedback_api_get_enzomma_payload(const enum FeedbackId id_fb, size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(feedback_handler.enzomma_can_payload);
    }
    const bool is_digital = feedback_api_is_digital(id_fb);
    feedback_handler.enzomma_can_payload.feedback = (primary_hv_feedback_enzomma_feedback)id_fb;
    feedback_handler.enzomma_can_payload.status = (primary_hv_feedback_enzomma_status)feedback_api_get_status(id_fb);
    feedback_handler.enzomma_can_payload.is_digital = is_digital;
    if (is_digital) {
        feedback_handler.enzomma_can_payload.digital = feedback_api_get_digital(feedback_api_get_digital_bit_from_id(id_fb));
        feedback_handler.enzomma_can_payload.analog = 0.F;
    } else {
        feedback_handler.enzomma_can_payload.digital = 0U;
        const volt_t volt = feedback_api_get_analog(feedback_api_get_analog_index_from_id(id_fb));
        feedback_handler.enzomma_can_payload.analog = volt;
    }
    return &feedback_handler.enzomma_can_payload;
}

#ifdef CONF_FEEDBACK_STRINGS_ENABLE

EAGLETRT_STATIC char *feedback_module_name = "feedback";

EAGLETRT_STATIC char *feedback_return_code_name[] = {
    [FEEDBACK_RC_OK] = "ok",
    [FEEDBACK_RC_NULL_POINTER] = "null pointer",
    [FEEDBACK_RC_INVALID_INDEX] = "invalid index"
};

EAGLETRT_STATIC char *feedback_return_code_description[] = {
    [FEEDBACK_RC_OK] = "executed succesfully",
    [FEEDBACK_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [FEEDBACK_RC_INVALID_INDEX] = "the given index is not valid"
};

EAGLETRT_STATIC char *feedback_id_name[] = {
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

const char *const feedback_api_get_feedback_id_name(const enum FeedbackId id_fb) {
    if (id_fb >= FEEDBACK_ID_COUNT)
        return "unknown";
    return feedback_id_name[id_fb];
}

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#endif // CONF_FEEDBACK_MODULE_ENABLE
