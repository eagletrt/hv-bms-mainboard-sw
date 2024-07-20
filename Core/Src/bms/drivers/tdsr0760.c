/**
 * @file tdsr0760.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#include "tdsr0760.h"

#include <stddef.h>
#include <string.h>

Tdsr0760ReturnCode tdsr0760_init(Tdsr0760Handler * handler) {
    if (handler == NULL)
        return TDSR0760_NULL_POINTER;
    memset(handler, 0U, sizeof(*handler));
    for (size_t i = 0U; i < TDSR0760_SEGMENT_COUNT; ++i)
        handler->segments[i] = TDSR0760_SEGMENT_STATUS_UNKNOWN;
    return TDSR0760_OK;
}

Tdsr0760SegmentStatus tdsr0760_get_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment)
{
    if (handler == NULL || segment >= TDSR0760_SEGMENT_COUNT)
        return TDSR0760_SEGMENT_STATUS_UNKNOWN;
    return handler->segments[segment];
}

Tdsr0760ReturnCode tdsr0760_set_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment,
    Tdsr0760SegmentStatus state)
{
    if (handler == NULL)
        return TDSR0760_NULL_POINTER;
    if (segment >= TDSR0760_SEGMENT_COUNT)
        return TDSR0760_INVALID_SEGMENT;
    handler->segments[segment] = state;
    return TDSR0760_OK;
}

Tdsr0760ReturnCode tdsr0760_toggle_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment)
{
    if (handler == NULL)
        return TDSR0760_NULL_POINTER;
    if (segment >= TDSR0760_SEGMENT_COUNT)
        return TDSR0760_INVALID_SEGMENT;
    Tdsr0760SegmentStatus status = TDSR0760_SEGMENT_STATUS_UNKNOWN;
    switch (handler->segments[segment]) {
        case TDSR0760_SEGMENT_STATUS_ON:
            status = TDSR0760_SEGMENT_STATUS_OFF;
            break;
        case TDSR0760_SEGMENT_STATUS_OFF:
            status = TDSR0760_SEGMENT_STATUS_ON;
            break; 
        default:
            status = TDSR0760_SEGMENT_STATUS_ON;
            break;
    }
    handler->segments[segment] = status;
    return TDSR0760_OK;
}
