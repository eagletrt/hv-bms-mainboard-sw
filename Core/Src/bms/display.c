/**
 * @file tdsr0760.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#include "display.h"

#ifdef CONF_DISPLAY_MODULE_ENABLE

/**
 * 7-segment display handler structure
 *
 * @param set A pointer to the function callback that should set the state of a selected segment
 * @param toggle A pointer to the function callback that should toggle the state of a selected segment
 */
_STATIC struct {
    display_segment_set_state_callback_t set;
    display_segment_toggle_state_callback_t toggle;

    Tdsr0760Handler tdsr0760;
} hdisplay;

DisplayReturnCode display_init(
    display_segment_set_state_callback_t set,
    display_segment_toggle_state_callback_t toggle)
{
    if (set == NULL || toggle == NULL)
        return DISPLAY_NULL_POINTER;
    hdisplay.set = set;
    hdisplay.toggle = toggle;
    tdsr0760_init(&hdisplay.tdsr0760);
    return DISPLAY_OK;
}

DisplayReturnCode display_set_segment(DisplaySegment segment, DisplaySegmentStatus state) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return DISPLAY_INVALID_SEGMENT;
    Tdsr0760ReturnCode code = tdsr0760_set_segment(
        &hdisplay.tdsr0760,
        (Tdsr0760Segment)segment,
        (Tdsr0760SegmentStatus)state
    );
    if (code == TDSR0760_OK)
        hdisplay.set(segment, state);
    return DISPLAY_OK;
}

#ifdef CONF_DISPLAY_STRINGS_ENABLE

_STATIC char * display_module_name = "display";

_STATIC char * display_return_code_name[] = {
    [DISPLAY_OK] = "ok",
    [DISPLAY_NULL_POINTER] = "null pointer"
};

_STATIC char * display_return_code_description[] = {
    [DISPLAY_OK] = "executed succefully",
    [DISPLAY_NULL_POINTER] = "attempt to dereference a null pointer"
};

#endif // CONF_DISPLAY_STRINGS_ENABLE

#endif // CONF_DISPLAY_MODULE_ENABLE
