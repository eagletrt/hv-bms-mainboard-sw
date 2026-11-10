/*!
 * \file tdsr0760-api.c
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#include "tdsr0760-api.h"

#include <stddef.h>
#include <string.h>

enum Tdsr0760ReturnCode tdsr0760_api_init(struct Tdsr0760Handler *const handler) {
    if (handler == NULL) {
        return TDSR0760_RC_NULL_POINTER;
    }
    memset(handler, 0U, sizeof(*handler));
    for (size_t i = 0U; i < TDSR0760_SEGMENT_COUNT; ++i) {
        handler->segments[i] = TDSR0760_SEGMENT_STATUS_UNKNOWN;
    }
    return TDSR0760_RC_OK;
}

enum Tdsr0760SegmentStatus tdsr0760_api_get_segment(struct Tdsr0760Handler *const handler, const enum Tdsr0760Segment segment) {
    if (handler == NULL || segment >= TDSR0760_SEGMENT_COUNT) {
        return TDSR0760_SEGMENT_STATUS_UNKNOWN;
    }
    return handler->segments[segment];
}

enum Tdsr0760ReturnCode tdsr0760_api_set_segment(
    struct Tdsr0760Handler *const handler,
    const enum Tdsr0760Segment segment,
    const enum Tdsr0760SegmentStatus state) {
    if (handler == NULL) {
        return TDSR0760_RC_NULL_POINTER;
    }
    if (segment >= TDSR0760_SEGMENT_COUNT) {
        return TDSR0760_RC_INVALID_SEGMENT;
    }
    handler->segments[segment] = state;
    return TDSR0760_RC_OK;
}

enum Tdsr0760ReturnCode tdsr0760_api_toggle_segment(struct Tdsr0760Handler *const handler, const enum Tdsr0760Segment segment) {
    if (handler == NULL) {
        return TDSR0760_RC_NULL_POINTER;
    }
    if (segment >= TDSR0760_SEGMENT_COUNT) {
        return TDSR0760_RC_INVALID_SEGMENT;
    }
    enum Tdsr0760SegmentStatus status = TDSR0760_SEGMENT_STATUS_UNKNOWN;
    switch (handler->segments[segment]) {
        case TDSR0760_SEGMENT_STATUS_ON:
            status = TDSR0760_SEGMENT_STATUS_OFF;
            break;
        case TDSR0760_SEGMENT_STATUS_OFF: //NOLINT
            status = TDSR0760_SEGMENT_STATUS_ON;
            break;
        default:
            status = TDSR0760_SEGMENT_STATUS_ON;
            break;
    }
    handler->segments[segment] = status;
    return TDSR0760_RC_OK;
}
