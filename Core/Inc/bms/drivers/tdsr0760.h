/**
 * @file tdsr0760.h
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#ifndef TDSR0760_H
#define TDSR0760_H

/*
 * @brief Return code for the TSDR0760 module function
 *
 * @details
 *     - TDSR0760_OK the function executed succefully
 *     - TDSR0760_NULL_POINTER a NULL pointer was given to a function
 *     - TDSR0760_INVALID_SEGMENT the given segment does not exist
 */
typedef enum {
    TDSR0760_OK,
    TDSR0760_NULL_POINTER,
    TDSR0760_INVALID_SEGMENT
} Tdsr0760ReturnCode;

/**
 * @brief Type definition for the status of a display segment
 *
 * @details
 *     - TDSR0760_SEGMENT_STATUS_OFF the segment is turned off
 *     - TDSR0760_SEGMENT_STATUS_ON the segment is turned on
 *     - TDSR0760_SEGMENT_STATUS_UNKNOWN the segment status is unkown
 */
typedef enum {
    TDSR0760_SEGMENT_STATUS_OFF = 0,
    TDSR0760_SEGMENT_STATUS_ON,
    TDSR0760_SEGMENT_STATUS_UNKNOWN,
} Tdsr0760SegmentStatus;

/**
 * @brief Type definition for the segments of the display
 *
 * @details Each segment is labeled with a letter of the alphabet, with the
 * exception of the decimal point
 * There are also aliases based on the phisycal position of the segments
 */
typedef enum {
    TDSR0760_SEGMENT_A = 0,
    TDSR0760_SEGMENT_B,
    TDSR0760_SEGMENT_C,
    TDSR0760_SEGMENT_D,
    TDSR0760_SEGMENT_E,
    TDSR0760_SEGMENT_F,
    TDSR0760_SEGMENT_G,
    TDSR0760_SEGMENT_DECIMAL_POINT,
    TDSR0760_SEGMENT_COUNT,

    // Aliases for the segments
    TDSR0760_SEGMENT_TOP = TDSR0760_SEGMENT_A,
    TDSR0760_SEGMENT_TOP_RIGHT = TDSR0760_SEGMENT_B,
    TDSR0760_SEGMENT_BOTTOM_RIGHT = TDSR0760_SEGMENT_C,
    TDSR0760_SEGMENT_BOTTOM = TDSR0760_SEGMENT_D,
    TDSR0760_SEGMENT_BOTTOM_LEFT = TDSR0760_SEGMENT_E,
    TDSR0760_SEGMENT_TOP_LEFT = TDSR0760_SEGMENT_F,
    TDSR0760_SEGMENT_MIDDLE = TDSR0760_SEGMENT_G
} Tdsr0760Segment;

/**
 * @brief Tdsr0760 handler structure definition
 *
 * @param segments The array of segment with the corresponding status
 */
typedef struct {
    Tdsr0760SegmentStatus segments[TDSR0760_SEGMENT_COUNT];
} Tdsr0760Handler;

/**
 * @brief Initialize the 7-segment display handler structure
 *
 * @param handler A pointer to the display handler structure
 * 
 * @return Tdsr0760ReturnCode
 *     - TDSR0760_NULL_POINTER if any of the parameters is NULL
 *     - TDSR0760_OK otherwise
 */
Tdsr0760ReturnCode tdsr0760_init(Tdsr0760Handler * handler);

/**
 * @brief Get the status of a single segment
 *
 * @param handler A pointer to the 7-segment display handler structure
 * @param segment The segment to select
 *
 * @return Tdsr0760SegmentStatus The current status of the segment, or
 * TDSR0760_SEGMENT_STATUS_UNKOWN on error
 */
Tdsr0760SegmentStatus tdsr0760_get_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment
);

/**
 * @brief Set the status of a single segment
 *
 * @param handler A pointer to the 7-segment display handler structure
 * @param segment The segment to select
 * @param state The new status to set
 *
 * @return Tdsr0760ReturnCode
 *     - TDSR0760_NULL_POINTER if any of the parameters is NULL
 *     - TDSR0760_INVALID_SEGMENT if the selected segment does not exists
 *     - TDSR0760_OK otherwise
 */
Tdsr0760ReturnCode tdsr0760_set_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment,
    Tdsr0760SegmentStatus state
);

/**
 * @brief Toggle the status of a single segment
 *
 * @details If the status is unknown it is set to TDSR0760_SEGMENT_STATUS_ON
 *
 * @param handler A pointer to the 7-segment display handler structure
 * @param segment The segment to select
 *
 * @return Tdsr0760ReturnCode
 *     - TDSR0760_NULL_POINTER if any of the parameters is NULL
 *     - TDSR0760_INVALID_SEGMENT if the selected segment does not exists
 *     - TDSR0760_OK otherwise
 */
Tdsr0760ReturnCode tdsr0760_toggle_segment(
    Tdsr0760Handler * handler,
    Tdsr0760Segment segment
);

#endif  // TDSR0760_H
