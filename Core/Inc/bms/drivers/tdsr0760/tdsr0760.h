/*!
 * \file tdsr0760.h
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#ifndef TDSR0760_H
#define TDSR0760_H

/*!
 * \brief Return code for the TSDR0760 module function
 */
enum Tdsr0760ReturnCode {
    TDSR0760_RC_OK,             /*!< Function executed successfully */
    TDSR0760_RC_NULL_POINTER,   /*!< A NULL pointer was given to a function */
    TDSR0760_RC_INVALID_SEGMENT /*!< The given segment does not exist */
};

/*!
 * \brief Type definition for the status of a display segment
 */
enum Tdsr0760SegmentStatus {
    TDSR0760_SEGMENT_STATUS_OFF = 0, /*!< The segment is turned off */
    TDSR0760_SEGMENT_STATUS_ON,      /*!< The segment is turned on */
    TDSR0760_SEGMENT_STATUS_UNKNOWN  /*!< The segment status is unknown */
};

/*!
 * \brief Type definition for the segments of the display
 *
 * \details Each segment is labeled with a letter of the alphabet, with the
 * exception of the decimal point
 * There are also aliases based on the phisycal position of the segments
 */
enum Tdsr0760Segment {
    TDSR0760_SEGMENT_A = 0,
    TDSR0760_SEGMENT_B = 1,
    TDSR0760_SEGMENT_C = 2,
    TDSR0760_SEGMENT_D = 3,
    TDSR0760_SEGMENT_E = 4,
    TDSR0760_SEGMENT_F = 5,
    TDSR0760_SEGMENT_G = 6,
    TDSR0760_SEGMENT_DECIMAL_POINT = 7,
    TDSR0760_SEGMENT_COUNT = 8,

    // Aliases for the segments
    TDSR0760_SEGMENT_TOP = TDSR0760_SEGMENT_A,
    TDSR0760_SEGMENT_TOP_RIGHT = TDSR0760_SEGMENT_B,
    TDSR0760_SEGMENT_BOTTOM_RIGHT = TDSR0760_SEGMENT_C,
    TDSR0760_SEGMENT_BOTTOM = TDSR0760_SEGMENT_D,
    TDSR0760_SEGMENT_BOTTOM_LEFT = TDSR0760_SEGMENT_E,
    TDSR0760_SEGMENT_TOP_LEFT = TDSR0760_SEGMENT_F,
    TDSR0760_SEGMENT_MIDDLE = TDSR0760_SEGMENT_G
};

/*!
 * \brief Tdsr0760 handler structure definition
 */
struct Tdsr0760Handler {
    enum Tdsr0760SegmentStatus segments[TDSR0760_SEGMENT_COUNT]; /*< The array of segment with the corresponding status */
};

#endif // TDSR0760_H
