/**
 * @file tdsr0760.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#include "tdsr0760.h"

#include <stddef.h>

Tdsr0760ReturnCode tdsr0760_init(Tdsr0760Handler * handler) {
    if (handler == NULL)
        return TDSR0760_NULL_POINTER;
    memset(handler, 0U, sizeof(*handler));
    return TDSR0760_OK;
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

