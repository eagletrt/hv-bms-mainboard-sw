/*!
 * \file feedback-api.c
 * \date 2026-05-2
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */

#include "feedback-api.h"
#include "error-api.h"
#include "error.h"
#include "feedback.h"

#include <string.h>

#include "imd-api.h"

#ifdef CONF_FEEDBACK_MODULE_ENABLE

EAGLETRT_STATIC struct FeedbackHandler feedback_handler;

/*!
 * \brief Get the feedback identifier from the digital feedback bit position
 *
 * \param bit The bit position
 *
 * \returns The feedback identifier
 */
enum FeedbackId prv_feedback_get_id_from_digital_bit(const enum FeedbackDigitalBit bit) {
    switch (bit) {
        case FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM:
            return FEEDBACK_ID_AIRN_OPEN_COM;
        case FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM:
            return FEEDBACK_ID_AIRP_OPEN_COM;
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
            return FEEDBACK_ID_INVALID;
    }
}

/*!
 * \brief Get the feedback identifier from the analog feedback index
 *
 * \param index The index of the analog feedback
 *
 * \returns The feedback identifier
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
        case FEEDBACK_ANALOG_INDEX_SD_IMD_FB:
            return FEEDBACK_ID_SD_IMD_FB;
        case FEEDBACK_ANALOG_INDEX_SD_BMS_FB:
            return FEEDBACK_ID_SD_BMS_FB;
        case FEEDBACK_ANALOG_INDEX_SD_OUT:
            return FEEDBACK_ID_SD_OUT;
        case FEEDBACK_ANALOG_INDEX_SD_IN:
            return FEEDBACK_ID_SD_IN;
        case FEEDBACK_ANALOG_INDEX_SD_END:
            return FEEDBACK_ID_SD_END;
        case FEEDBACK_ANALOG_INDEX_V5_MCU:
            return FEEDBACK_ID_V5_MCU;
        default:
            return FEEDBACK_ID_INVALID;
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
 * \return The status of the feedback
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
        return 0;
    }
    return feedback_handler.analog[index];
}

uint32_t debug_cnt = 0U;
enum FeedbackReturnCode feedback_api_update_status(void) {
    // Update the status of the digital feedbacks
    for (enum FeedbackDigitalBit bit = 0U; bit < FEEDBACK_DIGITAL_BIT_COUNT; ++bit) {
        const enum FeedbackId feedback = prv_feedback_get_id_from_digital_bit(bit);
        feedback_handler.status[feedback] = EAGLETRT_API_BIT_GET(feedback_handler.digital, bit) ? FEEDBACK_STATUS_HIGH : FEEDBACK_STATUS_LOW;

        // Check voltage indicator connector
        if (feedback == FEEDBACK_ID_INDICATOR_CONNECTED) {
            if (feedback_handler.status[feedback] == FEEDBACK_STATUS_HIGH) {
                error_api_reset(ERROR_GROUP_CONNECTOR_DISCONNECTED, 0);
            } else {
                error_api_set(ERROR_GROUP_CONNECTOR_DISCONNECTED, 0);
            }
        } else if (feedback == FEEDBACK_ID_IMD_OK) {
            if (feedback_handler.status[feedback] == FEEDBACK_STATUS_HIGH) {
                error_api_reset(ERROR_GROUP_IMD, 0);
            } else {
                error_api_set(ERROR_GROUP_IMD, 0);
            }
        }
    }

    // Update the status of the analog feedback
    for (enum FeedbackAnalogIndex i = 0U; i < FEEDBACK_ANALOG_INDEX_COUNT; ++i) {
        const enum FeedbackId feedback = prv_feedback_get_id_from_analog_index(i);
        // feedback_handler.status[feedback] = prv_feedback_api_get_analog_status(i);
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
        feedback_handler.status[feedback] = status;
    }
    return FEEDBACK_RC_OK;
}

enum FeedbackStatus feedback_api_get_status(const enum FeedbackId feedback) {
    if (feedback >= FEEDBACK_ID_COUNT) {
        return FEEDBACK_STATUS_ERROR;
    }
    return feedback_handler.status[feedback];
}

// TODO: Change this function to return the feedback ID instead of a boolean value and remove the id from the parameters
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
        *out = FEEDBACK_ID_INVALID;
    }
    return true;
}

bool feedback_api_is_digital(const enum FeedbackId feedback) {
    switch (feedback) {
        case FEEDBACK_ID_AIRN_OPEN_COM: //NOLINT
        case FEEDBACK_ID_PRECHARGE_OPEN_COM:
        case FEEDBACK_ID_AIRP_OPEN_COM:
        case FEEDBACK_ID_PRECHARGE_OPEN_MEC:
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

enum FeedbackDigitalBit feedback_api_get_digital_bit_from_id(const enum FeedbackId feedback) {
    switch (feedback) {
        case FEEDBACK_ID_AIRN_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM;
        case FEEDBACK_ID_PRECHARGE_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM;
        case FEEDBACK_ID_AIRP_OPEN_COM:
            return FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM;
        case FEEDBACK_ID_PRECHARGE_OPEN_MEC:
            return FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC;
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
            return FEEDBACK_DIGITAL_BIT_INVALID;
    }
}

enum FeedbackAnalogIndex feedback_api_get_analog_index_from_id(const enum FeedbackId feedback) {
    switch (feedback) {
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
            return FEEDBACK_ANALOG_INDEX_INVALID;
    }
}

union CanPrimaryMessages *feedback_api_get_feedaback_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardfeedback;
    }
    struct CanPrimaryTsacmainboardfeedback *payload = &feedback_handler.libcan_message_feedback.tsacmainboardfeedback;
    payload->airnopencom = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    payload->prechargeopencom = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM);
    payload->airpopencom = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM);
    payload->airnopenmec = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC));
    payload->prechargeopenmec = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC);
    payload->airpopenmec = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC));
    payload->tslessthan60v = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V);
    payload->plausiblestatelatched = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED);
    payload->plausiblestatepersisted = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED);
    payload->plausiblestate = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);
    payload->plausiblestaterc = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC));
    payload->bmsfaultcockpitled = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED);
    payload->imdfaultcockpitled = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED);
    payload->extfaultcockpitled = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED);
    payload->tsalgreen = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_TSAL_GREEN));
    return &feedback_handler.libcan_message_feedback;
}

union CanPrimaryMessages *feedback_api_get_shutdown_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardshutdown;
    }
    struct CanPrimaryTsacmainboardshutdown *payload = &feedback_handler.libcan_message_shutdown.tsacmainboardshutdown;
    payload->interlocktsacvoltage = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_SD_OUT));
    payload->amsvoltage = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_SD_BMS_FB));
    payload->imdstatus = imd_api_get_status();
    payload->imdvoltage = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_SD_IMD_FB));
    payload->airnvoltage = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC));
    payload->airpvoltage = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_api_get_analog(FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC));
    payload->prechargevoltage = feedback_api_get_digital(FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC) ? FEEDBACK_SD_VREF : 0;
    return &feedback_handler.libcan_message_shutdown;
}

union CanPrimaryMessages *feedback_api_get_feedback_shutdown_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardfeedbackshutdown;
    }
    struct CanPrimaryTsacmainboardfeedbackshutdown *payload = &feedback_handler.libcan_message_feedback_shutdown.tsacmainboardfeedbackshutdown;
    payload->sdin = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN]);
    payload->sdend = FEEDBACK_VOLTAGE_TO_SD_VOLT(feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_END]);
    return &feedback_handler.libcan_message_feedback_shutdown;
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

const char *const feedback_api_get_feedback_id_name(const enum FeedbackId feedback) {
    if (feedback >= FEEDBACK_ID_COUNT)
        return "unknown";
    return feedback_id_name[feedback];
}

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#endif // CONF_FEEDBACK_MODULE_ENABLE
