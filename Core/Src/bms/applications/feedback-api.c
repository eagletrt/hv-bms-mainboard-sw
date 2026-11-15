/*!
 * \file feedback-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback handling (SD, IMD, AIRs, precharge, ...)
 */

#include "feedback-api.h"

#include <stdint.h>
#include <string.h>

#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-status-api.h"
#include "feedback.h"
#include "mainboard-status.h"

EAGLETRT_STATIC struct FeedbackHandler feedback_handler;

/*! \brief Standard profile, used by most of the analog feedback */
EAGLETRT_STATIC const struct FeedbackAnalogThreshold feedback_thr_standard = {
    .low = FEEDBACK_THRESHOLD_LOW_V,
    .high = FEEDBACK_THRESHOLD_HIGH_V,
    .mode = FEEDBACK_ANALOG_THRESHOLD_MODE_STEP
};

/*! \brief Profile of the AIRs and IMD feedback (raised low threshold) */
EAGLETRT_STATIC const struct FeedbackAnalogThreshold feedback_thr_air_imd = {
    .low = FEEDBACK_THRESHOLD_LOW_AIR_IMD_V,
    .high = FEEDBACK_THRESHOLD_HIGH_V,
    .mode = FEEDBACK_ANALOG_THRESHOLD_MODE_STEP
};

/*! \brief Profile of the 3V3 probing feedback (window) */
EAGLETRT_STATIC const struct FeedbackAnalogThreshold feedback_thr_probing_3v3 = {
    .low = FEEDBACK_THRESHOLD_PROBING_3V3_LOW_V,
    .high = FEEDBACK_THRESHOLD_PROBING_3V3_HIGH_V,
    .mode = FEEDBACK_ANALOG_THRESHOLD_MODE_WINDOW
};

/*!
 * \brief Threshold profile of every analog feedback
 *
 * \details Every index must have a profile, a missing one (NULL) makes the
 *          feedback status always ERROR
 */
EAGLETRT_STATIC const struct FeedbackAnalogThreshold *const feedback_analog_thresholds[FEEDBACK_ANALOG_INDEX_COUNT] = {
    [FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = &feedback_thr_air_imd,
    [FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC] = &feedback_thr_air_imd,
    [FEEDBACK_ANALOG_INDEX_IMD_OK] = &feedback_thr_air_imd,
    [FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_PROBING_3V3] = &feedback_thr_probing_3v3,
    [FEEDBACK_ANALOG_INDEX_SD_IMD_FB] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_SD_AMS_FB] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_SD_OUT] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_SD_IN] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_SD_END] = &feedback_thr_standard,
    [FEEDBACK_ANALOG_INDEX_V5_MCU] = &feedback_thr_standard
};

/*! \brief Mainboard status condition of every digital feedback */
EAGLETRT_STATIC const enum MainboardStatusCondition feedback_digital_conditions[FEEDBACK_DIGITAL_INDEX_COUNT] = {
    [FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC] = MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED,
    [FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V] = MAINBOARD_STATUS_CONDITION_TS_BELOW_60V,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED] = MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_PERSISTED,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE] = MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE,
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED] = MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON,
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED] = MAINBOARD_STATUS_CONDITION_IMD_COCKPIT_LED_ON,
    [FEEDBACK_DIGITAL_INDEX_INDICATOR_CONNECTED] = MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED,
    [FEEDBACK_DIGITAL_INDEX_LATCH_RESET] = MAINBOARD_STATUS_CONDITION_LATCH_RESET_ACTIVE,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED] = MAINBOARD_STATUS_CONDITION_PLAUSIBILITY_FAULT_LATCHED,
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_LATCHED] = MAINBOARD_STATUS_CONDITION_AMS_FAULT_LATCHED,
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_LATCHED] = MAINBOARD_STATUS_CONDITION_IMD_FAULT_LATCHED,
    [FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED] = MAINBOARD_STATUS_CONDITION_EXTERNAL_FAULT_LATCHED,
    [FEEDBACK_DIGITAL_INDEX_HANDCART_CONNECTED] = MAINBOARD_STATUS_CONDITION_HANDCART_CONNECTED
};

/*!
 * \brief True if the digital feedback has the opposite polarity of its condition
 *
 * \details E.g. PRECHARGE_OPEN_MEC is high when the precharge is open, while
 *          the condition is PRECHARGE_CLOSED; the NOT_* pins are active low
 */
EAGLETRT_STATIC const bool feedback_digital_inverted[FEEDBACK_DIGITAL_INDEX_COUNT] = {
    [FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC] = true,
    [FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V] = false,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED] = false,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE] = false,
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED] = true,
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED] = true,
    [FEEDBACK_DIGITAL_INDEX_INDICATOR_CONNECTED] = false,
    [FEEDBACK_DIGITAL_INDEX_LATCH_RESET] = true,
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED] = true,
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_LATCHED] = true,
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_LATCHED] = true,
    [FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED] = true,
    [FEEDBACK_DIGITAL_INDEX_HANDCART_CONNECTED] = false
};

/*! \brief Mainboard status condition of every analog feedback */
EAGLETRT_STATIC const enum MainboardStatusCondition feedback_analog_conditions[FEEDBACK_ANALOG_INDEX_COUNT] = {
    [FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = MAINBOARD_STATUS_CONDITION_AIRN_CLOSED,
    [FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC] = MAINBOARD_STATUS_CONDITION_AIRP_CLOSED,
    [FEEDBACK_ANALOG_INDEX_IMD_OK] = MAINBOARD_STATUS_CONDITION_IMD_OK,
    [FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC] = MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_FILTERED,
    [FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = MAINBOARD_STATUS_CONDITION_TSAL_GREEN,
    [FEEDBACK_ANALOG_INDEX_PROBING_3V3] = MAINBOARD_STATUS_CONDITION_PROBING_3V3_OK,
    [FEEDBACK_ANALOG_INDEX_SD_IMD_FB] = MAINBOARD_STATUS_CONDITION_SD_IMD_NODE_CLOSED,
    [FEEDBACK_ANALOG_INDEX_SD_AMS_FB] = MAINBOARD_STATUS_CONDITION_SD_AMS_NODE_CLOSED,
    [FEEDBACK_ANALOG_INDEX_SD_OUT] = MAINBOARD_STATUS_CONDITION_SD_OUT_CLOSED,
    [FEEDBACK_ANALOG_INDEX_SD_IN] = MAINBOARD_STATUS_CONDITION_SD_IN_CLOSED,
    [FEEDBACK_ANALOG_INDEX_SD_END] = MAINBOARD_STATUS_CONDITION_SD_END_CLOSED,
    [FEEDBACK_ANALOG_INDEX_V5_MCU] = MAINBOARD_STATUS_CONDITION_MCU_5V_OK
};

/*!
 * \brief True if the analog feedback has the opposite polarity of its condition
 *
 * \details E.g. AIRN_OPEN_MEC is HIGH when the AIR- is open, while the
 *          condition is AIRN_CLOSED
 */
EAGLETRT_STATIC const bool feedback_analog_inverted[FEEDBACK_ANALOG_INDEX_COUNT] = {
    [FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = true,
    [FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC] = true,
    [FEEDBACK_ANALOG_INDEX_IMD_OK] = false,
    [FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC] = false,
    [FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = false,
    [FEEDBACK_ANALOG_INDEX_PROBING_3V3] = false,
    [FEEDBACK_ANALOG_INDEX_SD_IMD_FB] = false,
    [FEEDBACK_ANALOG_INDEX_SD_AMS_FB] = false,
    [FEEDBACK_ANALOG_INDEX_SD_OUT] = false,
    [FEEDBACK_ANALOG_INDEX_SD_IN] = false,
    [FEEDBACK_ANALOG_INDEX_SD_END] = false,
    [FEEDBACK_ANALOG_INDEX_V5_MCU] = false
};

// TODO(TEST): see if when connected to handcart the V5_MCU high threshold needs to be lowered to
// FEEDBACK_THRESHOLD_HIGH_HANDCART_V like in the old code. If so, add a handcart profile and select
// it for V5_MCU when the handcart is connected, without modifying the profiles above.

EAGLETRT_STATIC enum FeedbackAnalogStatus prv_feedback_analog_get_status(enum FeedbackAnalogIndex index, volt value) {
    const struct FeedbackAnalogThreshold *thr = feedback_analog_thresholds[index];

    if (thr == NULL) {
        return FEEDBACK_ANALOG_STATUS_ERROR;
    }

    switch (thr->mode) {
        case FEEDBACK_ANALOG_THRESHOLD_MODE_STEP:
            if (value >= thr->high) {
                return FEEDBACK_ANALOG_STATUS_HIGH;
            }
            if (value <= thr->low) {
                return FEEDBACK_ANALOG_STATUS_LOW;
            }
            return FEEDBACK_ANALOG_STATUS_ERROR;
        case FEEDBACK_ANALOG_THRESHOLD_MODE_WINDOW:
            if (value >= thr->low && value <= thr->high) {
                return FEEDBACK_ANALOG_STATUS_HIGH;
            }
            return FEEDBACK_ANALOG_STATUS_LOW;
        default:
            return FEEDBACK_ANALOG_STATUS_ERROR;
    }
}

/*!
 * \brief Apply a majority filter to a digital feedback
 *
 * \param buffer The buffer containing the feedback values
 * \return The filtered value
 */
EAGLETRT_STATIC bool prv_feedback_digital_majority_filter(const volatile bool *buffer) {
    uint8_t count = 0;
    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        count += (uint8_t)buffer[i];
    }

    return count > (FEEDBACK_BUFFER_SIZE / 2);
}

/*!
 * \brief Apply a majority filter to an analog feedback
 *
 * \param buffer The buffer containing the feedback values
 * \param index The index of the analog feedback
 * \return The filtered value or FEEDBACK_ANALOG_STATUS_COUNT if no majority was found
 */
EAGLETRT_STATIC enum FeedbackAnalogStatus prv_feedback_analog_majority_filter(const volatile volt *buffer, enum FeedbackAnalogIndex index) {
    uint8_t votes[FEEDBACK_ANALOG_STATUS_COUNT] = { 0 };

    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        votes[prv_feedback_analog_get_status(index, buffer[i])]++;
    }

    uint8_t max_index = 0;

    for (size_t i = 1; i < FEEDBACK_ANALOG_STATUS_COUNT; i++) {
        max_index = (votes[max_index] < votes[i] ? i : max_index);
    }

    return votes[max_index] > (FEEDBACK_BUFFER_SIZE / 2) ? (enum FeedbackAnalogStatus)max_index : FEEDBACK_ANALOG_STATUS_COUNT;
}

EAGLETRT_STATIC volt prv_feedback_analog_mean_filter(const volatile volt *buffer) {
    volt sum = 0.F;
    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        sum += buffer[i];
    }

    return sum / (volt)FEEDBACK_BUFFER_SIZE;
}

void feedback_api_init(void) {
    memset(&feedback_handler, 0, sizeof(feedback_handler));
}

enum FeedbackReturnCode feedback_api_set_digital_value(enum FeedbackDigitalIndex index, bool value) {

    if (index >= FEEDBACK_DIGITAL_INDEX_COUNT) {
        return FEEDBACK_RC_INVALID_INDEX;
    }

    struct FeedbackDigitalValue *feedback = &feedback_handler.digital[index];

    if (feedback->head >= FEEDBACK_BUFFER_SIZE) {
        return FEEDBACK_RC_INVALID_INDEX;
    }

    feedback->buffer[feedback->head] = value;

    feedback->head = (feedback->head + 1) % FEEDBACK_BUFFER_SIZE;

    feedback->ready = (bool)(feedback->ready || feedback->head == 0);

    if (feedback->ready) {
        feedback->filtered = prv_feedback_digital_majority_filter(feedback->buffer);
    }

    return FEEDBACK_RC_OK;
}

enum FeedbackReturnCode feedback_api_set_analog_value(enum FeedbackAnalogIndex index, volt value) {

    if (index >= FEEDBACK_ANALOG_INDEX_COUNT) {
        return FEEDBACK_RC_INVALID_INDEX;
    }

    struct FeedbackAnalogValue *feedback = &feedback_handler.analog[index];

    if (feedback->head >= FEEDBACK_BUFFER_SIZE) {
        return FEEDBACK_RC_INVALID_INDEX;
    }

    feedback->buffer[feedback->head] = value;

    feedback->head = (feedback->head + 1) % FEEDBACK_BUFFER_SIZE;

    feedback->ready = (bool)(feedback->ready || feedback->head == 0);

    if (feedback->ready) {
        feedback->filtered = prv_feedback_analog_mean_filter(feedback->buffer);

        enum FeedbackAnalogStatus status = prv_feedback_analog_majority_filter(feedback->buffer, index);

        feedback->status = (status == FEEDBACK_ANALOG_STATUS_COUNT) ? feedback->status : status;
    }

    return FEEDBACK_RC_OK;
}

bool feedback_api_is_ready(void) {
    return feedback_handler.is_ready;
}

enum FeedbackReturnCode feedback_api_routine(void) {

    bool module_ready = true;

    for (size_t i = 0; i < FEEDBACK_DIGITAL_INDEX_COUNT; i++) {
        struct FeedbackDigitalValue *feedback = &feedback_handler.digital[i];

        if (!feedback->ready) {
            module_ready = false;
            continue;
        }

        bool condition_met = feedback->filtered;

        if (feedback_digital_inverted[i]) {
            condition_met = (bool)!condition_met;
        }

        mainboard_status_api_set_condition(feedback_digital_conditions[i], condition_met);
    }

    for (size_t i = 0; i < FEEDBACK_ANALOG_INDEX_COUNT; i++) {
        struct FeedbackAnalogValue *feedback = &feedback_handler.analog[i];

        if (!feedback->ready) {
            module_ready = false;
            continue;
        }

        bool condition_met = (feedback->status == FEEDBACK_ANALOG_STATUS_HIGH);

        if (feedback->status == FEEDBACK_ANALOG_STATUS_ERROR) {
            mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_FEEDBACK_ERROR);
        }

        if (feedback_analog_inverted[i]) {
            condition_met = (bool)!condition_met;
        }

        mainboard_status_api_set_condition(feedback_analog_conditions[i], condition_met);
    }

    feedback_handler.is_ready = module_ready;

    return FEEDBACK_RC_OK;
}