/**
 * @file display.h
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief 7-segment display handling functions
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "tdsr0760.h"

/**
 * @brief Return code for the 7-segment display module function
 *
 * @details
 *     - DISPLAY_OK the function executed succesfully
 *     - DISPLAY_NULL_POINTER a NULL pointer was given to a function
 *     - DISPLAY_INVALID_SEGMENT the given segment does not exist
 */
typedef enum {
    DISPLAY_OK,
    DISPLAY_NULL_POINTER,
    DISPLAY_INVALID_SEGMENT
} DisplayReturnCode;

/**
 * @brief Type definition for the status of a display segment
 *
 * @details
 *     - DISPLAY_SEGMENT_OFF the segment is turned off
 *     - DISPLAY_SEGMENT_ON the segment is turned on
 */
typedef enum {
    DISPLAY_SEGMENT_OFF = TDSR0760_SEGMENT_OFF,
    DISPLAY_SEGMENT_ON = TDSR0760_SEGMENT_ON
} DisplaySegmentStatus;

/**
 * @brief Type definition for the display segments
 *
 * @details Each segment is labeled with its physical position in space (except the decimal point)
 */
typedef enum {
    DISPLAY_SEGMENT_TOP = TDSR0760_SEGMENT_TOP,
    DISPLAY_SEGMENT_TOP_RIGHT = TDSR0760_SEGMENT_TOP_RIGHT,
    DISPLAY_SEGMENT_TOP_LEFT = TDSR0760_SEGMENT_TOP_LEFT,
    DISPLAY_SEGMENT_MIDDLE = TDSR0760_SEGMENT_MIDDLE,
    DISPLAY_SEGMENT_BOTTOM = TDSR0760_SEGMENT_BOTTOM,
    DISPLAY_SEGMENT_BOTTOM_RIGHT = TDSR0760_SEGMENT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BOTTOM_LEFT = TDSR0760_SEGMENT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_DECIMAL_POINT = TDSR0760_SEGMENT_DECIMAL_POINT,

    DISPLAY_SEGMENT_COUNT = TDSR0760_SEGMENT_COUNT
} DisplaySegment;

/**
 * @brief Type definition for a function callback that should set the state of the display segment
 *
 * @param segment The segment to select
 * @param state The new state to set
 */
typedef void (* display_segment_set_state_callback_t)(DisplaySegment segment, DisplaySegmentStatus state);

/**
 * @brief Type definition for a function callback that should toggle the state of the display segment
 *
 * @param segment The segment to select
 */
typedef void (* display_segment_toggle_state_callback_t)(DisplaySegment segment);

#ifdef CONF_DISPLAY_MODULE_ENABLE

/**
 * @brief Initialization of the 7-segment display internal structure
 * 
 * @param set A pointer to the function callback that should set the state of the chosen segment
 * @param toggle A pointer to the function callback that should toggle the state of the chosen segment
 *
 * @return DisplayReturnCode
 *     - DISPLAY_NULL_POINTER if any of the given parameter is NULL
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_init(
    display_segment_set_state_callback_t set,
    display_segment_toggle_state_callback_t toggle
);

/**
 * @brief Set the status of a single segment of the 7-segment display
 *
 * @param segment The segment to select
 * @param state The new state to set
 *
 * @return DisplayReturnCode
 *     - DISPLAY_OK
 */
DisplayReturnCode display_set_segment(DisplaySegment segment, DisplaySegmentStatus state);

#else  // CONF_DISPLAY_MODULE_ENABLE

#define display_init(set, toggle) (DISPLAY_OK)
#define display_set_segment(segment, state) (DISPLAY_OK)

#endif // CONF_DISPLAY_MODULE_ENABLE

#endif  // DISPLAY_H
