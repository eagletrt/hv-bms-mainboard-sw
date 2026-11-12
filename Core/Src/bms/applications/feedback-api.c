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

EAGLETRT_STATIC struct FeedbackAnalogTreshold standard_thr = {
    .high_thr = FEEDBACK_THRESHOLD_HIGH_V,
    .low_thr = FEEDBACK_THRESHOLD_LOW_V,
    .contained = false
};

EAGLETRT_STATIC struct FeedbackAnalogTreshold probing_3v3 = {
    .high_thr = FEEDBACK_THRESHOLD_LOW_V,
    .low_thr = FEEDBACK_THRESHOLD_HIGH_V,
    .contained = true
};

// TODO(TEST): see if when connected to handcart the high threshold needs to be lowered to 1V like in the old code.

bool prv_feedback_digital_majority_filter(const volatile bool *buffer) {
    uint8_t count = 0;
    for (size_t i = 0; i < FEEDBACK_BUFFER_SIZE; i++) {
        count += (uint8_t)buffer[i];
    }

    return count > (FEEDBACK_BUFFER_SIZE / 2);
}

void feedback_api_init(void) {
    memset(&feedback_handler, 0, sizeof(feedback_handler));
}

enum FeedbackReturnCode feedback_api_set_digital_value(enum FeedbackDigitalIndex index, bool value) {

    if (index > FEEDBACK_DIGITAL_INDEX_COUNT) {
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

    if (index > FEEDBACK_ANALOG_INDEX_COUNT) {
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
        feedback->filtered = prv_feedback_analog_majority_filter(feedback->buffer);
    }

    return FEEDBACK_RC_OK;
}