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
    for (Tdsr0760Segment segment = 0U; segment < DISPLAY_SEGMENT_COUNT; ++segment) {
        Tdsr0760SegmentStatus status = MAINBOARD_BIT_GET(bits, segment) ?
            TDSR0760_SEGMENT_STATUS_ON :
            TDSR0760_SEGMENT_STATUS_OFF;
        if (tdsr0760_set_segment(&hdisplay.tdsr0760, segment, status) != TDSR0760_OK)
            code = DISPLAY_DRIVER_ERROR;
    }
    return code;
}

DisplayReturnCode display_set_digit(uint8_t digit) {
    if (digit > 0x0F)
        return DISPLAY_INVALID_CHARACTER;
    const DisplayCharacterCode hex[] = {
        DISPLAY_CHARACTER_CODE_0,
        DISPLAY_CHARACTER_CODE_1,
        DISPLAY_CHARACTER_CODE_2,
        DISPLAY_CHARACTER_CODE_3,
        DISPLAY_CHARACTER_CODE_4,
        DISPLAY_CHARACTER_CODE_5,
        DISPLAY_CHARACTER_CODE_6,
        DISPLAY_CHARACTER_CODE_7,
        DISPLAY_CHARACTER_CODE_8,
        DISPLAY_CHARACTER_CODE_9,
        DISPLAY_CHARACTER_CODE_A_UPCASE,
        DISPLAY_CHARACTER_CODE_B_DOWNCASE,
        DISPLAY_CHARACTER_CODE_C_UPCASE,
        DISPLAY_CHARACTER_CODE_D_DOWNCASE,
        DISPLAY_CHARACTER_CODE_E_UPCASE,
        DISPLAY_CHARACTER_CODE_F_UPCASE
    };
    return display_set_segment_all(hex[digit]);
}

DisplayReturnCode display_set_character(char character) {
    uint8_t code = DISPLAY_CHARACTER_CODE_SPACE;
    switch (character) {
        // Numbers
        case '0': code = DISPLAY_CHARACTER_CODE_0; break;
        case '1': code = DISPLAY_CHARACTER_CODE_1; break;
        case '2': code = DISPLAY_CHARACTER_CODE_2; break;
        case '3': code = DISPLAY_CHARACTER_CODE_3; break;
        case '4': code = DISPLAY_CHARACTER_CODE_4; break;
        case '5': code = DISPLAY_CHARACTER_CODE_5; break;
        case '6': code = DISPLAY_CHARACTER_CODE_6; break;
        case '7': code = DISPLAY_CHARACTER_CODE_7; break;
        case '8': code = DISPLAY_CHARACTER_CODE_8; break;
        case '9': code = DISPLAY_CHARACTER_CODE_9; break;

        // Uppercase characters
        case 'A': code = DISPLAY_CHARACTER_CODE_A_UPCASE; break;
        case 'B': code = DISPLAY_CHARACTER_CODE_B_DOWNCASE; break;
        case 'C': code = DISPLAY_CHARACTER_CODE_C_UPCASE; break;
        case 'D': code = DISPLAY_CHARACTER_CODE_D_DOWNCASE; break;
        case 'E': code = DISPLAY_CHARACTER_CODE_E_UPCASE; break;
        case 'F': code = DISPLAY_CHARACTER_CODE_F_UPCASE; break;
        case 'G': code = DISPLAY_CHARACTER_CODE_G_UPCASE; break;
        case 'H': code = DISPLAY_CHARACTER_CODE_H_UPCASE; break;
        case 'I': code = DISPLAY_CHARACTER_CODE_I_UPCASE; break;
        case 'J': code = DISPLAY_CHARACTER_CODE_J_UPCASE; break;
        case 'L': code = DISPLAY_CHARACTER_CODE_L_UPCASE; break;
        case 'N': code = DISPLAY_CHARACTER_CODE_N_UPCASE; break;
        case 'O': code = DISPLAY_CHARACTER_CODE_O_DOWNCASE; break;
        case 'P': code = DISPLAY_CHARACTER_CODE_P_UPCASE; break;
        case 'R': code = DISPLAY_CHARACTER_CODE_R_DOWNCASE; break;
        case 'T': code = DISPLAY_CHARACTER_CODE_T_DOWNCASE; break; 
        case 'U': code = DISPLAY_CHARACTER_CODE_U_UPCASE; break;
        case 'Y': code = DISPLAY_CHARACTER_CODE_Y_DOWNCASE; break;

        // Lowercase characters
        case 'a': code = DISPLAY_CHARACTER_CODE_A_UPCASE; break;
        case 'b': code = DISPLAY_CHARACTER_CODE_B_DOWNCASE; break;
        case 'c': code = DISPLAY_CHARACTER_CODE_C_DOWNCASE; break;
        case 'd': code = DISPLAY_CHARACTER_CODE_D_DOWNCASE; break;
        case 'e': code = DISPLAY_CHARACTER_CODE_E_UPCASE; break;
        case 'f': code = DISPLAY_CHARACTER_CODE_F_UPCASE; break;
        case 'g': code = DISPLAY_CHARACTER_CODE_G_UPCASE; break;
        case 'h': code = DISPLAY_CHARACTER_CODE_H_DOWNCASE; break;
        case 'i': code = DISPLAY_CHARACTER_CODE_I_DOWNCASE; break;
        case 'j': code = DISPLAY_CHARACTER_CODE_J_UPCASE; break;
        case 'l': code = DISPLAY_CHARACTER_CODE_L_UPCASE; break;
        case 'n': code = DISPLAY_CHARACTER_CODE_N_DOWNCASE; break;
        case 'o': code = DISPLAY_CHARACTER_CODE_O_DOWNCASE; break;
        case 'p': code = DISPLAY_CHARACTER_CODE_P_UPCASE; break;
        case 'r': code = DISPLAY_CHARACTER_CODE_R_DOWNCASE; break;
        case 't': code = DISPLAY_CHARACTER_CODE_T_DOWNCASE; break;
        case 'u': code = DISPLAY_CHARACTER_CODE_U_DOWNCASE; break;
        case 'y': code = DISPLAY_CHARACTER_CODE_Y_DOWNCASE; break;

        // Symbols
        case ' ': code = DISPLAY_CHARACTER_CODE_SPACE; break;
        case '.': code = DISPLAY_CHARACTER_CODE_DOT; break;
        case '-': code = DISPLAY_CHARACTER_CODE_HYPEN; break;
        case '_': code = DISPLAY_CHARACTER_CODE_UNDERSCORE; break;

        default:
            return DISPLAY_INVALID_CHARACTER;
    }
    return display_set_segment_all(code);
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
