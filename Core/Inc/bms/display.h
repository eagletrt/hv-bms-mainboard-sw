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
    DISPLAY_INVALID_SEGMENT,
    DISPLAY_INVALID_CHARACTER,
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

typedef enum {
    DISPLAY_CHAR_0 = 0b01110111,
    DISPLAY_CHAR_1 = 0b00100010,
    DISPLAY_CHAR_2 = 0b01011011,
    DISPLAY_CHAR_3 = 0b00111011,
    DISPLAY_CHAR_4 = 0b00101110,
    DISPLAY_CHAR_5 = 0b00111101,
    DISPLAY_CHAR_6 = 0b01111101,
    DISPLAY_CHAR_7 = 0b00100011,
    DISPLAY_CHAR_8 = 0b01111111,
    DISPLAY_CHAR_9 = 0b00111111,
    DISPLAY_CHAR_A = 0b01101111,
    DISPLAY_CHAR_b = 0b01111100,
    DISPLAY_CHAR_C = 0b01010101,
    DISPLAY_CHAR_c = 0b01011000,
    DISPLAY_CHAR_d = 0b01111010,
    DISPLAY_CHAR_E = 0b01011101,
    DISPLAY_CHAR_F = 0b01001101,
    DISPLAY_CHAR_G = 0b01110101,
    DISPLAY_CHAR_H = 0b01101110,
    DISPLAY_CHAR_h = 0b01101100,
    DISPLAY_CHAR_I = 0b01000100,
    DISPLAY_CHAR_i = 0b00100000,
    DISPLAY_CHAR_J = 0b00110010,
    DISPLAY_CHAR_L = 0b01010100,
    DISPLAY_CHAR_N = 0b01100111,
    DISPLAY_CHAR_n = 0b01101000,
    DISPLAY_CHAR_o = 0b01111000,
    DISPLAY_CHAR_P = 0b01001111,
    DISPLAY_CHAR_r = 0b01001000,
    DISPLAY_CHAR_t = 0b01011100,
    DISPLAY_CHAR_U = 0b01110110,
    DISPLAY_CHAR_u = 0b01110000,
    DISPLAY_CHAR_y = 0b00111110,
    DISPLAY_CHAR_space = 0b00000000,
    DISPLAY_CHAR_dot = 0b10000000,
    DISPLAY_CHAR_minus = 0b00001000,
    DISPLAY_CHAR_underscore = 0b00010000,

} DisplayCharacter;

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

/**
 * 7-segment display handler structure
 * 
 * @attention Do not use this struct outside this module
 * 
 * @param set A pointer to the function callback that should set the state of a selected segment
 * @param toggle A pointer to the function callback that should toggle the state of a selected segment
 */
typedef struct {
    display_segment_set_state_callback_t set;
    display_segment_toggle_state_callback_t toggle;

    Tdsr0760Handler tdsr0760;
} _Display;

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

/**
 * @brief Set the status of all the segment of the 7-segment display
 *
 * @param state An uint8_t representing the state of the segments
 * 
 * @details LSB to MSB
 * TOP
 * TOP_RIGHT
 * TOP_LEFT
 * MIDDLE
 * BOTTOM
 * BOTTOM_RIGHT
 * BOTTOM_LEFT
 * DECIMAL_POINT
 *
 * @return DisplayReturnCode
 *     - DISPLAY_OK
 */
DisplayReturnCode display_set_state(uint8_t state);

/**
 * @brief Shows an hexadecimal digit on the 7-segment display
 *
 * @param num Number to show [0 - F]
 * @param decimal The new state of the decimal point
 *
 * @return DisplayReturnCode
 *     - DISPLAY_OK
 */
DisplayReturnCode display_set_number(uint8_t num, DisplaySegmentStatus decimal);

/**
 * @brief Shows a character on the 7-segment display 
 *
 * @attention Only supports a subset of all characters
 * 
 * @param cha Character to show
 * @param decimal The new state of the decimal point
 *
 * @return DisplayReturnCode
 *     - DISPLAY_OK
 */
DisplayReturnCode display_set_char(char cha, DisplaySegmentStatus decimal);

#else  // CONF_DISPLAY_MODULE_ENABLE

#define display_init(set, toggle) (DISPLAY_OK)
#define display_set_segment(segment, state) (DISPLAY_OK)

#endif // CONF_DISPLAY_MODULE_ENABLE

#endif  // DISPLAY_H
