/*!
 * \file display-api.h
 * \date 2024-07-04
 * \author Ettore Beltrame [ettore.beltrame@studenti.unitn.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief 7-segment display handling functions
 */

#ifndef DISPLAY_API_H
#define DISPLAY_API_H

#include "display.h"

#ifdef CONF_DISPLAY_MODULE_ENABLE

/*!
 * \brief Initialization of the 7-segment display internal structure
 *
 * \param set A pointer to the function callback that should set the state of the chosen segment
 * \param toggle A pointer to the function callback that should toggle the state of the chosen segment
 *
 * \retval DISPLAY_RC_NULL_POINTER if any of the given parameter is NULL
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_init(display_segment_set_state_callback set, display_segment_toggle_state_callback toggle);

/*!
 * \brief Get the hexadecimal digit code of the display from a number
 *
 * \details The value range from 0 to 15 (0xF)
 *
 * \param digit The hexadecimal digit to get the code from
 *
 * \returns enum DisplayCharacterCode The code corresponding to the digit or
 * DISPLAY_CHARACTER_CODE_SPACE if not valid
 */
enum DisplayCharacterCode display_get_code_from_hex_digit(uint8_t digit);

/*!
 * \brief Get the hexadecimal digit code of the display from a number
 *
 * \attention Only supports a small subset of all the ASCII characters
 *
 * \details If ignore case is set to false the prefer upcase flag is ignored
 *
 * \param symbol The character to get the code from
 * \param prefer_upcase Flag used to prefer upper case or lower case codes if the
 * code for the case sensitive character does not exists
 *
 * \returns enum DisplayCharacterCode The code corresponding to the character or
 * DISPLAY_CHARACTER_CODE_SPACE if not valid
 */
enum DisplayCharacterCode display_get_code_from_character(char symbol, bool prefer_upcase);

/*!
 * \brief Get the status of a single segment of the 7-segment display
 *
 * \param segment The segment to select
 *
 * \returns enum DisplaySegmentStatus The current status of the display segment, or
 * DISPLAY_SEGMENT_STATUS_UNKNOWN on error
 */
enum DisplaySegmentStatus display_get_segment(enum DisplaySegment segment);

/*!
 * \brief Set the status of a single segment of the 7-segment display
 *
 * \param segment The segment to select
 * \param status The state to set
 *
 * \retval DISPLAY_RC_INVALID_SEGMENT the selected segment does not exists
 * \retval DISPLAY_RC_INVALID_STATUS the given segment status is not valid
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_set_segment(enum DisplaySegment segment, enum DisplaySegmentStatus status);

/*!
 * \brief Set the status of a single segment of the 7-segment display
 *
 * \param segment The segment to select
 *
 * \retval DISPLAY_RC_INVALID_SEGMENT the selected segment does not exists
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_INVALID_STATUS the new segment status is not valid
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_toggle_segment(enum DisplaySegment segment);

/*!
 * \brief Set the status of all the segment of the 7-segment display
 *
 * \details The bit index of the bitflag is defined inside the enum DisplaySegment enum
 *
 * \param bits A bitflag where each bit represents a single segment
 *
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_set_segment_all(bit_flag8_t bits);

/*!
 * \brief Shows an hexadecimal digit on the 7-segment display
 *
 * \param digit A number from 0 to 15
 *
 * \retval DISPLAY_RC_INVALID_CHARACTER if the given digit is greater than 15
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_set_digit(uint8_t digit);

/*!
 * \brief Shows a character on the 7-segment display
 *
 * \attention Only supports a small subset of all the ASCII characters
 *
 * \param symbol The character to show
 * \param prefer_upcase Flag used to prefer upper case or lower case codes if the
 * code for the case sensitive character does not exists
 *
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_set_character(char symbol, bool prefer_upcase);

/*!
 * \brief Run a single step of an animation on the 7-segment display
 *
 * \param animation A pointer to the array of frames of the display animation
 * \param size The number of frames in the animation
 * \param ticks_pre_frame The number of ticks per frame
 * \param ticks The current number of ticks
 *
 * \retval DISPLAY_RC_NULL_POINTER if the pointer to the animation is NULL
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_run_animation(
    const enum DisplaySegmentBit *animation,
    size_t size,
    ticks_t ticks_per_frame,
    ticks_t ticks);

/*!
 * \brief Run a single step of an animation that shows the character of a string
 * on the 7-segment display
 *
 * \param string The string to display
 * \param size The lenght of the string
 * \param ticks_pre_frame The number of ticks per frame
 * \param ticks The current number of ticks
 *
 * \retval DISPLAY_RC_NULL_POINTER if the pointer to the animation is NULL
 * \retval DISPLAY_RC_DRIVER_ERROR error cause by the display driver
 * \retval DISPLAY_RC_OK otherwise
 */
enum DisplayReturnCode display_run_animation_string(const char *string, size_t size, ticks_t ticks_per_frame, ticks_t ticks);

#else // CONF_DISPLAY_MODULE_ENABLE

#define display_init(set, toggle) (DISPLAY_RC_OK)
#define display_get_code_from_hex_digit(digit) (DISPLAY_CHARACTER_CODE_SPACE)
#define display_get_code_from_character(c, ignore_case, prefer_upcase) (DISPLAY_CHARACTER_CODE_SPACE)
#define display_get_segment(segment) (DISPLAY_SEGMENT_STATUS_UNKOWN)
#define display_set_segment(segment, status) (DISPLAY_RC_OK)
#define display_toggle_segment(segment) (DISPLAY_RC_OK)
#define display_set_segment_all(bits) (DISPLAY_RC_OK)
#define display_set_digit(digit) (DISPLAY_RC_OK)
#define display_set_character(c, ignore_case, prefer_upcase) (DISPLAY_RC_OK)
#define display_run_animation(animation, size, ticks_per_frame, t) (DISPLAY_RC_OK)
#define display_run_animation_string(string, size, ticks_per_frame, t) (DISPLAY_RC_OK)

#endif // CONF_DISPLAY_MODULE_ENABLE

#endif // DISPLAY_API_H
