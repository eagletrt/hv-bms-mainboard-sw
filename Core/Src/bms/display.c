/**
 * @file tdsr0760.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Drivers for the TDSR0760 7-segment 1 digit display
 */

#include "display.h"

#ifdef CONF_DISPLAY_MODULE_ENABLE

_STATIC _Display hdisplay;

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

DisplayReturnCode display_set_state(uint8_t state){
    DisplayReturnCode code = DISPLAY_OK;
    code |= display_set_segment(DISPLAY_SEGMENT_TOP,           (state & (1<<0)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_TOP_RIGHT,     (state & (1<<1)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_TOP_LEFT,      (state & (1<<2)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_MIDDLE,        (state & (1<<3)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_BOTTOM,        (state & (1<<4)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_BOTTOM_RIGHT,  (state & (1<<5)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_BOTTOM_LEFT,   (state & (1<<6)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    code |= display_set_segment(DISPLAY_SEGMENT_DECIMAL_POINT, (state & (1<<7)) ? DISPLAY_SEGMENT_ON : DISPLAY_SEGMENT_OFF);
    return code;
}

DisplayReturnCode display_set_number(uint8_t num, DisplaySegmentStatus decimal){
    if(num > 0x0F){
        return DISPLAY_INVALID_CHARACTER;
    }
    DisplayCharacter hexnums[16] = {
        DISPLAY_CHAR_0,
        DISPLAY_CHAR_1,
        DISPLAY_CHAR_2,
        DISPLAY_CHAR_3,
        DISPLAY_CHAR_4,
        DISPLAY_CHAR_5,
        DISPLAY_CHAR_6,
        DISPLAY_CHAR_7,
        DISPLAY_CHAR_8,
        DISPLAY_CHAR_9,
        DISPLAY_CHAR_A,
        DISPLAY_CHAR_b,
        DISPLAY_CHAR_C,
        DISPLAY_CHAR_d,
        DISPLAY_CHAR_E,
        DISPLAY_CHAR_F,
    };
    return display_set_state(hexnums[num & 0x0F] | ((decimal == DISPLAY_SEGMENT_ON) << 7));
}

DisplayReturnCode display_set_char(char cha, DisplaySegmentStatus decimal){
    uint8_t state;
    switch (cha){
    case '0':
        state = DISPLAY_CHAR_0;
        break;
    case '1':
        state = DISPLAY_CHAR_1;
        break;
    case '2':
        state = DISPLAY_CHAR_2;
        break;
    case '3':
        state = DISPLAY_CHAR_3;
        break;
    case '4':
        state = DISPLAY_CHAR_4;
        break;
    case '5':
        state = DISPLAY_CHAR_5;
        break;
    case '6':
        state = DISPLAY_CHAR_6;
        break;
    case '7':
        state = DISPLAY_CHAR_7;
        break;
    case '8':
        state = DISPLAY_CHAR_8;
        break;
    case '9':
        state = DISPLAY_CHAR_9;
        break;
    case 'A':
    case 'a':
        state = DISPLAY_CHAR_A;
        break;
    case 'b':
    case 'B':
        state = DISPLAY_CHAR_b;
        break;
    case 'C':
        state = DISPLAY_CHAR_C;
        break;
    case 'c':
        state = DISPLAY_CHAR_c;
        break;
    case 'd':
    case 'D':
        state = DISPLAY_CHAR_d;
        break;
    case 'E':
    case 'e':
        state = DISPLAY_CHAR_E;
        break;
    case 'F':
    case 'f':
        state = DISPLAY_CHAR_F;
        break;
    case 'G':
    case 'g':
        state = DISPLAY_CHAR_G;
        break;
    case 'H':
        state = DISPLAY_CHAR_H;
        break;
    case 'h':
        state = DISPLAY_CHAR_h;
        break;
    case 'I':
        state = DISPLAY_CHAR_I;
        break;
    case 'i':
        state = DISPLAY_CHAR_i;
        break;
    case 'J':
    case 'j':
        state = DISPLAY_CHAR_J;
        break;
    case 'L':
    case 'l':
        state = DISPLAY_CHAR_L;
        break;
    case 'n':
    case 'N':
        state = DISPLAY_CHAR_n;
        break;
    case 'O':
    case 'o':
        state = DISPLAY_CHAR_o;
        break;
    case 'P':
    case 'p':
        state = DISPLAY_CHAR_P;
        break;
    case 'r':
    case 'R':
        state = DISPLAY_CHAR_r;
        break;
    case 'U':
        state = DISPLAY_CHAR_U;
        break;
    case 'u':
        state = DISPLAY_CHAR_u;
        break;
    default:
        return DISPLAY_INVALID_CHARACTER;
        break;
    }
    return display_set_state((decimal == DISPLAY_SEGMENT_ON) << 7);
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
