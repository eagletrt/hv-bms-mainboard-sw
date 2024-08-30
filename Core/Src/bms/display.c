/**
 * @file tdsr0760.c
 * @date 2024-07-04
 * @author Ettore Beltrame [ettore.beltrame@studenti.unitn.com]
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief 7-segment display handling functions
 */

#include "display.h"

#include <string.h>

#ifdef CONF_DISPLAY_MODULE_ENABLE

_STATIC _Display hdisplay;

DisplayReturnCode display_init(
    display_segment_set_state_callback_t set,
    display_segment_toggle_state_callback_t toggle)
{
    if (set == NULL || toggle == NULL)
        return DISPLAY_NULL_POINTER;
    memset(&hdisplay, 0U, sizeof(hdisplay));
    hdisplay.set = set;
    hdisplay.toggle = toggle;
    tdsr0760_init(&hdisplay.tdsr0760);
    return DISPLAY_OK;
}

DisplayCharacterCode display_get_code_from_hex_digit(uint8_t digit) {
    switch (digit) {
        case 0U: return DISPLAY_CHARACTER_CODE_0;
        case 1U: return DISPLAY_CHARACTER_CODE_1;
        case 2U: return DISPLAY_CHARACTER_CODE_2;
        case 3U: return DISPLAY_CHARACTER_CODE_3;
        case 4U: return DISPLAY_CHARACTER_CODE_4;
        case 5U: return DISPLAY_CHARACTER_CODE_5;
        case 6U: return DISPLAY_CHARACTER_CODE_6;
        case 7U: return DISPLAY_CHARACTER_CODE_7;
        case 8U: return DISPLAY_CHARACTER_CODE_8;
        case 9U: return DISPLAY_CHARACTER_CODE_9;
        case 10U: return DISPLAY_CHARACTER_CODE_A_UPCASE;
        case 11U: return DISPLAY_CHARACTER_CODE_B_DOWNCASE;
        case 12U: return DISPLAY_CHARACTER_CODE_C_UPCASE;
        case 13U: return DISPLAY_CHARACTER_CODE_D_DOWNCASE;
        case 14U: return DISPLAY_CHARACTER_CODE_E_UPCASE;
        case 15U: return DISPLAY_CHARACTER_CODE_F_UPCASE;
        default: return DISPLAY_CHARACTER_CODE_SPACE;
    }
}

DisplayCharacterCode display_get_code_from_character(
    char c,
    bool ignore_case,
    bool prefer_upcase)
{
    switch (c) {
        // Symbols
        case ' ': return DISPLAY_CHARACTER_CODE_SPACE;
        case '.': return DISPLAY_CHARACTER_CODE_DOT;
        case '-': return DISPLAY_CHARACTER_CODE_HYPEN;
        case '_': return DISPLAY_CHARACTER_CODE_UNDERSCORE;

        // Numbers
        case '0': return DISPLAY_CHARACTER_CODE_0;
        case '1': return DISPLAY_CHARACTER_CODE_1;
        case '2': return DISPLAY_CHARACTER_CODE_2;
        case '3': return DISPLAY_CHARACTER_CODE_3;
        case '4': return DISPLAY_CHARACTER_CODE_4;
        case '5': return DISPLAY_CHARACTER_CODE_5;
        case '6': return DISPLAY_CHARACTER_CODE_6;
        case '7': return DISPLAY_CHARACTER_CODE_7;
        case '8': return DISPLAY_CHARACTER_CODE_8;
        case '9': return DISPLAY_CHARACTER_CODE_9;

        // Upper case alphabet
        case 'A':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_A_UPCASE;
            return DISPLAY_CHARACTER_CODE_A_DOWNCASE;
        case 'B':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_B_DOWNCASE;
            break;
        case 'C':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_C_UPCASE;
            return DISPLAY_CHARACTER_CODE_C_DOWNCASE;
        case 'D':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_D_DOWNCASE;
            break;
        case 'E': return DISPLAY_CHARACTER_CODE_E_UPCASE;
        case 'F': return DISPLAY_CHARACTER_CODE_F_UPCASE;
        case 'G': return DISPLAY_CHARACTER_CODE_G_UPCASE;
        case 'H':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_H_UPCASE;
            return DISPLAY_CHARACTER_CODE_H_DOWNCASE;
        case 'I':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_I_UPCASE;
            return DISPLAY_CHARACTER_CODE_I_DOWNCASE;
        case 'J':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_J_UPCASE;
            return DISPLAY_CHARACTER_CODE_J_DOWNCASE;
        case 'K': return DISPLAY_CHARACTER_CODE_K_UPCASE;
        case 'L': return DISPLAY_CHARACTER_CODE_L_UPCASE;
        case 'M': return DISPLAY_CHARACTER_CODE_M_UPCASE;
        case 'N':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_N_DOWNCASE;
            break;
        case 'O':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_O_DOWNCASE;
            break;
        case 'P': return DISPLAY_CHARACTER_CODE_P_UPCASE;
        case 'Q':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_Q_DOWNCASE;
            break;
        case 'R':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_R_DOWNCASE;
            break;
        case 'S': return DISPLAY_CHARACTER_CODE_S_UPCASE;
        case 'T':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_T_DOWNCASE;
            break;
        case 'U':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_U_UPCASE;
            return DISPLAY_CHARACTER_CODE_U_DOWNCASE;
        case 'V': return DISPLAY_CHARACTER_CODE_V_UPCASE;
        case 'W': return DISPLAY_CHARACTER_CODE_W_UPCASE;
        case 'X':
            if (!ignore_case || prefer_upcase)
                return DISPLAY_CHARACTER_CODE_X_UPCASE;
            return DISPLAY_CHARACTER_CODE_X_DOWNCASE;
        case 'Y':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_Y_DOWNCASE;
            break;
        case 'Z': return DISPLAY_CHARACTER_CODE_Z_UPCASE;

        // Lower case alphabet
        case 'a':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_A_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_A_UPCASE;
        case 'b': return DISPLAY_CHARACTER_CODE_B_DOWNCASE;
        case 'c':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_C_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_C_UPCASE;
        case 'd': return DISPLAY_CHARACTER_CODE_D_DOWNCASE;
        case 'e':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_E_UPCASE;
            break;
        case 'f':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_F_UPCASE;
            break;
        case 'g':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_G_UPCASE;
            break;
        case 'h':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_H_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_H_UPCASE;
        case 'i':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_I_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_I_UPCASE;
        case 'j':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_J_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_J_UPCASE;
        case 'k':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_K_UPCASE;
            break;
        case 'l':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_L_UPCASE;
            break;
        case 'm':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_M_UPCASE;
            break;
        case 'n': return DISPLAY_CHARACTER_CODE_N_DOWNCASE;
        case 'o': return DISPLAY_CHARACTER_CODE_O_DOWNCASE;
        case 'p':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_P_UPCASE;
            break;
        case 'q': return DISPLAY_CHARACTER_CODE_Q_DOWNCASE;
        case 'r': return DISPLAY_CHARACTER_CODE_R_DOWNCASE;
        case 's':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_S_UPCASE;
            break;
        case 't': return DISPLAY_CHARACTER_CODE_T_DOWNCASE;
        case 'u':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_U_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_U_UPCASE;
        case 'v':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_V_UPCASE;
            break;
        case 'w':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_W_UPCASE;
            break;
        case 'x':
            if (!ignore_case || !prefer_upcase)
                return DISPLAY_CHARACTER_CODE_X_DOWNCASE;
            return DISPLAY_CHARACTER_CODE_X_UPCASE;
        case 'y': return DISPLAY_CHARACTER_CODE_Y_DOWNCASE;
        case 'z':
            if (ignore_case)
                return DISPLAY_CHARACTER_CODE_Z_UPCASE;
            break;

        default:
            break;
    }
    return DISPLAY_CHARACTER_CODE_SPACE;
}

DisplaySegmentStatus display_get_segment(DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return DISPLAY_SEGMENT_STATUS_UNKNOWN;
    return (DisplaySegmentStatus)tdsr0760_get_segment(&hdisplay.tdsr0760, (Tdsr0760Segment)segment);
}

DisplayReturnCode display_set_segment(DisplaySegment segment, DisplaySegmentStatus status) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return DISPLAY_INVALID_SEGMENT;
    if (status == DISPLAY_SEGMENT_STATUS_UNKNOWN)
        return DISPLAY_INVALID_STATUS;
    Tdsr0760ReturnCode code = tdsr0760_set_segment(
        &hdisplay.tdsr0760,
        (Tdsr0760Segment)segment,
        (Tdsr0760SegmentStatus)status
    );
    if (code != TDSR0760_OK)
        return DISPLAY_DRIVER_ERROR;
    hdisplay.set(segment, status);
    return DISPLAY_OK;
}

DisplayReturnCode display_toggle_segment(DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return DISPLAY_INVALID_SEGMENT;
    Tdsr0760ReturnCode code = tdsr0760_toggle_segment(
        &hdisplay.tdsr0760,
        (Tdsr0760Segment)segment
    );
    if (code != TDSR0760_OK)
        return DISPLAY_DRIVER_ERROR;   
    Tdsr0760SegmentStatus status = tdsr0760_get_segment(
        &hdisplay.tdsr0760,
        (Tdsr0760Segment) segment
    );
    if (status == TDSR0760_SEGMENT_STATUS_UNKNOWN)
        return DISPLAY_INVALID_STATUS;
    hdisplay.set(segment, (DisplaySegmentStatus)status);
    return DISPLAY_OK;
}

DisplayReturnCode display_set_segment_all(bit_flag8_t bits){
    DisplayReturnCode code = DISPLAY_OK;
    for (Tdsr0760Segment segment = 0U; segment < TDSR0760_SEGMENT_COUNT; ++segment) {
        Tdsr0760SegmentStatus status = MAINBOARD_BIT_GET(bits, segment) ?
            TDSR0760_SEGMENT_STATUS_ON :
            TDSR0760_SEGMENT_STATUS_OFF;
        Tdsr0760ReturnCode ret = tdsr0760_set_segment(&hdisplay.tdsr0760, segment, status);
        if (ret == TDSR0760_OK)
            hdisplay.set((DisplaySegment)segment, (DisplaySegmentStatus)status);
        else
            code = DISPLAY_DRIVER_ERROR;
    }
    return code;
}

DisplayReturnCode display_set_digit(uint8_t digit) {
    if (digit > 0x0F)
        return DISPLAY_INVALID_CHARACTER;
    DisplayCharacterCode code = display_get_code_from_hex_digit(digit);
    return display_set_segment_all(code);
}

DisplayReturnCode display_set_character(
    char c,
    bool ignore_case,
    bool prefer_upcase)
{
    DisplayCharacterCode code = display_get_code_from_character(c, ignore_case, prefer_upcase);
    return display_set_segment_all(code);
}

DisplayReturnCode display_run_animation(
    const DisplaySegmentBit * animation,
    size_t size,
    ticks_t ticks_per_frame,
    ticks_t t)
{
    if (animation == NULL)
        return DISPLAY_NULL_POINTER;
    if (ticks_per_frame <= 0U)
        ticks_per_frame = 1U;
    // Display a step of the animation based on the current time
    const size_t i = (t / ticks_per_frame) % size;
    return display_set_segment_all(animation[i]);
}

DisplayReturnCode display_run_animation_string(
    const char * string,
    size_t size,
    ticks_t ticks_per_frame,
    ticks_t t)
{
    if (string == NULL)
        return DISPLAY_NULL_POINTER;
    if (ticks_per_frame <= 0U)
        ticks_per_frame = 1U;

    // TODO: Add intraframe to make the animation smoother
    // Calculate the number of intraframe based on directions
    // const size_t intraframe_count = (dir == DISPLAY_DIRECTION_UP ||
    //     dir == DISPLAY_DIRECTION_DOWN) ?
    //     DISPLAY_INTRAFRAME_VERTICAL_COUNT :
    //     DISPLAY_INTRAFRAME_HORIZONTAL_COUNT;

    // Display a step of the animation based on the current time
    const size_t frame = t / ticks_per_frame;
    // const size_t intraframe = t / (ticks_per_frame / intraframe_count);
    const size_t i = frame % size;
    // const size_t j = intraframe % intraframe_count;
    return display_set_character(string[i], true, true); 
}

#ifdef CONF_DISPLAY_STRINGS_ENABLE

_STATIC char * display_module_name = "display";

_STATIC char * display_return_code_name[] = {
    [DISPLAY_OK] = "ok",
    [DISPLAY_NULL_POINTER] = "null pointer",
    [DISPLAY_INVALID_SEGMENT] = "invalid segment",
    [DISPLAY_INVALID_STATUS] = "invalid status",
    [DISPLAY_INVALID_CHARACTER] = "invalid character",
    [DISPLAY_DRIVER_ERROR] = "driver error"
};

_STATIC char * display_return_code_description[] = {
    [DISPLAY_OK] = "executed succefully",
    [DISPLAY_NULL_POINTER] = "attempt to dereference a null pointer",
    [DISPLAY_INVALID_SEGMENT] = "the selected segment does not exist",
    [DISPLAY_INVALID_STATUS] = "the given status is not valid",
    [DISPLAY_INVALID_CHARACTER] = "the character cannot be displayed",
    [DISPLAY_DRIVER_ERROR] = "error caused by the driver"
};

#endif // CONF_DISPLAY_STRINGS_ENABLE

#endif // CONF_DISPLAY_MODULE_ENABLE
