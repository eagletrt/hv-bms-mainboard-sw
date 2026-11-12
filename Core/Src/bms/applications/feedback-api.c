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

// TODO(TEST): see if when connected to handcart the V5_MCU high threshold needs to be lowered to
// FEEDBACK_THRESHOLD_HIGH_HANDCART_V like in the old code. If so, add a handcart profile and select
// it for V5_MCU when the handcart is connected, without modifying the profiles above.

bool prv_feedback_digital_majority_filter(const volatile bool *buffer) {
    uint8_t count = 0;
    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        count += (uint8_t)buffer[i];
    }

    return count > (FEEDBACK_BUFFER_SIZE / 2);
}

volt prv_feedback_analog_mean_filter(const volatile volt *buffer) {
    volt sum = 0.F;
    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        sum += buffer[i];
    }

    return sum / (volt)FEEDBACK_BUFFER_SIZE;
}

enum FeedbackAnalogStatus prv_feedback_analog_get_status(enum FeedbackAnalogIndex index, volt value) {
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
            return FEEDBACK_ANALOG_STATUS_ERROR;
        default:
            return FEEDBACK_ANALOG_STATUS_ERROR;
    }
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
        feedback->status = prv_feedback_analog_get_status(index, feedback->filtered);
    }

    return FEEDBACK_RC_OK;
}
