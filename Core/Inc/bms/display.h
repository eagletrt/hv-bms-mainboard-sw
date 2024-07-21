/**
 * @file display.h
 * @date 2024-07-04
 * @author Ettore Beltrame [ettore.beltrame@studenti.unitn.com]
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
 *     - DISPLAY_INVALID_STATUS the given segment status is not valid
 *     - DISPLAY_INVALID_CHARACTER the given character does not exist
 *     - DISPLAY_DRIVER_ERROR error caused by the display driver
 */
typedef enum {
    DISPLAY_OK,
    DISPLAY_NULL_POINTER,
    DISPLAY_INVALID_SEGMENT,
    DISPLAY_INVALID_STATUS,
    DISPLAY_INVALID_CHARACTER,
    DISPLAY_DRIVER_ERROR
} DisplayReturnCode;

/**
 * @brief Type definition for the status of a display segment
 *
 * @details
 *     - DISPLAY_SEGMENT_STATUS_OFF the segment is turned off
 *     - DISPLAY_SEGMENT_STATUS_ON the segment is turned on
 *     - DISPLAY_SEGMENT_STATUS_UNKNOWN the segment status is unknown
 */
typedef enum {
    DISPLAY_SEGMENT_STATUS_OFF = TDSR0760_SEGMENT_STATUS_OFF,
    DISPLAY_SEGMENT_STATUS_ON = TDSR0760_SEGMENT_STATUS_ON,
    DISPLAY_SEGMENT_STATUS_UNKNOWN = TDSR0760_SEGMENT_STATUS_UNKNOWN
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
 * @brief Bit represented by each segment of the display
 *
 * @details The position of the bit for each segment is defined by the DisplaySegment enum
 */
typedef enum { 
    DISPLAY_SEGMENT_BIT_NONE = 0U,
    DISPLAY_SEGMENT_BIT_TOP = 1U << DISPLAY_SEGMENT_TOP,
    DISPLAY_SEGMENT_BIT_TOP_RIGHT = 1U << DISPLAY_SEGMENT_TOP_RIGHT,
    DISPLAY_SEGMENT_BIT_TOP_LEFT = 1U << DISPLAY_SEGMENT_TOP_LEFT,
    DISPLAY_SEGMENT_BIT_MIDDLE = 1U << DISPLAY_SEGMENT_MIDDLE,
    DISPLAY_SEGMENT_BIT_BOTTOM = 1U << DISPLAY_SEGMENT_BOTTOM,
    DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT = 1U << DISPLAY_SEGMENT_BOTTOM_RIGHT,
    DISPLAY_SEGMENT_BIT_BOTTOM_LEFT = 1U << DISPLAY_SEGMENT_BOTTOM_LEFT,
    DISPLAY_SEGMENT_BIT_DECIMAL_POINT = 1U << DISPLAY_SEGMENT_DECIMAL_POINT
} DisplaySegmentBit;

/**
 * @brief Supported characters code for the display
 *
 * @details Each code is a bitflag where each bit represent the state of a
 * single segment of the display
 */
typedef enum {
    DISPLAY_CHARACTER_CODE_0 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM |
                               DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_TOP_LEFT,
    DISPLAY_CHARACTER_CODE_1 = DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_2 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                               DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_3 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                               DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_4 = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_5 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                               DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                               DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_6 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                               DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                               DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_7 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_8 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                               DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM |
                               DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_9 = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                               DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                               DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_A_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_A_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                        DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_B_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_C_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_C_DOWNCASE = DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_D_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_E_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_F_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_G_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_H_UPCASE = DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_H_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_I_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_I_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_J_UPCASE = DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_J_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_K_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_L_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_M_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_MIDDLE |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_N_DOWNCASE = DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_O_DOWNCASE = DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_P_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_Q_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                        DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_R_DOWNCASE = DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_LEFT,
    DISPLAY_CHARACTER_CODE_S_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_T_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_MIDDLE |
                                        DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_U_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_U_DOWNCASE = DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_V_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_W_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_X_UPCASE = DISPLAY_SEGMENT_BIT_TOP_LEFT | DISPLAY_SEGMENT_BIT_TOP_RIGHT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_X_DOWNCASE = DISPLAY_SEGMENT_BIT_BOTTOM_LEFT | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT,
    DISPLAY_CHARACTER_CODE_Y_DOWNCASE = DISPLAY_SEGMENT_BIT_TOP_RIGHT | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                        DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT |
                                        DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_Z_UPCASE = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_TOP_LEFT |
                                      DISPLAY_SEGMENT_BIT_BOTTOM_RIGHT | DISPLAY_SEGMENT_BIT_BOTTOM,
    DISPLAY_CHARACTER_CODE_SPACE = DISPLAY_SEGMENT_BIT_NONE,
    DISPLAY_CHARACTER_CODE_DOT = DISPLAY_SEGMENT_BIT_DECIMAL_POINT,
    DISPLAY_CHARACTER_CODE_HYPEN = DISPLAY_SEGMENT_BIT_MIDDLE,
    DISPLAY_CHARACTER_CODE_UNDERSCORE = DISPLAY_SEGMENT_BIT_BOTTOM
} DisplayCharacterCode;

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
 * @param tdsr0760 Handler structure of the 7-segment display driver
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
 * @brief Get the status of a single segment of the 7-segment display
 *
 * @param segment The segment to select
 *
 * @return DisplaySegmentStatus The current status of the display segment, or
 * DISPLAY_SEGMENT_STATUS_UNKNOWN on error
 */
DisplaySegmentStatus display_get_segment(DisplaySegment segment);

/**
 * @brief Set the status of a single segment of the 7-segment display
 *
 * @param segment The segment to select
 * @param state The state to set
 *
 * @return DisplayReturnCode
 *     - DISPLAY_INVALID_SEGMENT the selected segment does not exists
 *     - DISPLAY_INVALID_STATUS the given segment status is not valid
 *     - DISPLAY_DRIVER_ERROR error cause by the display driver
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_set_segment(DisplaySegment segment, DisplaySegmentStatus state);

/**
 * @brief Set the status of a single segment of the 7-segment display
 *
 * @param segment The segment to select
 * @param state The state to set
 *
 * @return DisplayReturnCode
 *     - DISPLAY_INVALID_SEGMENT the selected segment does not exists
 *     - DISPLAY_DRIVER_ERROR error cause by the display driver
 *     - DISPLAY_INVALID_STATUS the new segment status is not valid
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_toggle_segment(DisplaySegment segment);

/**
 * @brief Set the status of all the segment of the 7-segment display
 *
 * @details The bit index of the bitflag is defined inside the DisplaySegment enum
 *
 * @param bits A bitflag where each bit represents a single segment
 * 
 * @return DisplayReturnCode
 *     - DISPLAY_DRIVER_ERROR error cause by the display driver
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_set_segment_all(bit_flag8_t bits);

/**
 * @brief Shows an hexadecimal digit on the 7-segment display
 *
 * @param digit A number from 0 to 15
 *
 * @return DisplayReturnCode
 *     - DISPLAY_INVALID_CHARACTER if the given digit is greater than 15
 *     - DISPLAY_DRIVER_ERROR error cause by the display driver
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_set_digit(uint8_t digit);

/**
 * @brief Shows a character on the 7-segment display 
 *
 * @attention Only supports a small subset of all the ASCII characters
 * 
 * @param character The character to show
 *
 * @return DisplayReturnCode
 *     - DISPLAY_DRIVER_ERROR error cause by the display driver
 *     - DISPLAY_OK otherwise
 */
DisplayReturnCode display_set_character(char character);

#else  // CONF_DISPLAY_MODULE_ENABLE

#define display_init(set, toggle) (DISPLAY_OK)
#define display_get_segment(segment) (DISPLAY_SEGMENT_STATUS_UNKOWN)
#define display_set_segment(segment, state) (DISPLAY_OK)
#define display_toggle_segment(segment) (DISPLAY_OK)
#define display_set_segment_all(bits) (DISPLAY_OK)
#define display_set_digit(digit) (DISPLAY_OK)
#define display_set_character(character) (DISPLAY_OK)

#endif // CONF_DISPLAY_MODULE_ENABLE

#endif  // DISPLAY_H
