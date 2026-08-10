/*!
 * \file tdsr0760.c
 * \date 2024-07-04
 * \author Ettore Beltrame [ettore.beltrame@studenti.unitn.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief 7-segment display handling functions
 */

#include "display-api.h"
#include "display.h"

#include <stdint.h>
#include <string.h>

#include "eagletrt-api.h"
#include "tdsr0760-api.h"
#include "tdsr0760.h"

#include "usart.h"

#ifdef CONF_DISPLAY_MODULE_ENABLE

EAGLETRT_STATIC struct DisplayHandler display_handler;

EAGLETRT_STATIC const enum DisplayCharacterCode display_upper_codes[26] = {
    DISPLAY_CHARACTER_CODE_A_UPCASE,   // A
    DISPLAY_CHARACTER_CODE_B_DOWNCASE, // B (no upcase)
    DISPLAY_CHARACTER_CODE_C_UPCASE,   // C
    DISPLAY_CHARACTER_CODE_D_DOWNCASE, // D (no upcase)
    DISPLAY_CHARACTER_CODE_E_UPCASE,   // E
    DISPLAY_CHARACTER_CODE_F_UPCASE,   // F
    DISPLAY_CHARACTER_CODE_G_UPCASE,   // G
    DISPLAY_CHARACTER_CODE_H_UPCASE,   // H
    DISPLAY_CHARACTER_CODE_I_UPCASE,   // I
    DISPLAY_CHARACTER_CODE_J_UPCASE,   // J
    DISPLAY_CHARACTER_CODE_K_UPCASE,   // K
    DISPLAY_CHARACTER_CODE_L_UPCASE,   // L
    DISPLAY_CHARACTER_CODE_M_UPCASE,   // M
    DISPLAY_CHARACTER_CODE_N_DOWNCASE, // N (no upcase)
    DISPLAY_CHARACTER_CODE_O_DOWNCASE, // O (no upcase)
    DISPLAY_CHARACTER_CODE_P_UPCASE,   // P
    DISPLAY_CHARACTER_CODE_Q_DOWNCASE, // Q (no upcase)
    DISPLAY_CHARACTER_CODE_R_DOWNCASE, // R (no upcase)
    DISPLAY_CHARACTER_CODE_S_UPCASE,   // S
    DISPLAY_CHARACTER_CODE_T_DOWNCASE, // T (no upcase)
    DISPLAY_CHARACTER_CODE_U_UPCASE,   // U
    DISPLAY_CHARACTER_CODE_V_UPCASE,   // V
    DISPLAY_CHARACTER_CODE_W_UPCASE,   // W
    DISPLAY_CHARACTER_CODE_X_UPCASE,   // X
    DISPLAY_CHARACTER_CODE_Y_DOWNCASE, // Y (no upcase)
    DISPLAY_CHARACTER_CODE_Z_UPCASE,   // Z
};

EAGLETRT_STATIC const enum DisplayCharacterCode display_lower_codes[26] = {
    DISPLAY_CHARACTER_CODE_A_DOWNCASE, // a
    DISPLAY_CHARACTER_CODE_B_DOWNCASE, // b
    DISPLAY_CHARACTER_CODE_C_DOWNCASE, // c
    DISPLAY_CHARACTER_CODE_D_DOWNCASE, // d
    DISPLAY_CHARACTER_CODE_E_UPCASE,   // e (no downcase)
    DISPLAY_CHARACTER_CODE_F_UPCASE,   // f (no downcase)
    DISPLAY_CHARACTER_CODE_G_UPCASE,   // g (no downcase)
    DISPLAY_CHARACTER_CODE_H_DOWNCASE, // h
    DISPLAY_CHARACTER_CODE_I_DOWNCASE, // i
    DISPLAY_CHARACTER_CODE_J_DOWNCASE, // j
    DISPLAY_CHARACTER_CODE_K_UPCASE,   // k (no downcase)
    DISPLAY_CHARACTER_CODE_L_UPCASE,   // l (no downcase)
    DISPLAY_CHARACTER_CODE_M_UPCASE,   // m (no downcase)
    DISPLAY_CHARACTER_CODE_N_DOWNCASE, // n
    DISPLAY_CHARACTER_CODE_O_DOWNCASE, // o
    DISPLAY_CHARACTER_CODE_P_UPCASE,   // p (no downcase)
    DISPLAY_CHARACTER_CODE_Q_DOWNCASE, // q
    DISPLAY_CHARACTER_CODE_R_DOWNCASE, // r
    DISPLAY_CHARACTER_CODE_S_UPCASE,   // s (no downcase)
    DISPLAY_CHARACTER_CODE_T_DOWNCASE, // t
    DISPLAY_CHARACTER_CODE_U_DOWNCASE, // u
    DISPLAY_CHARACTER_CODE_V_UPCASE,   // v (no downcase)
    DISPLAY_CHARACTER_CODE_W_UPCASE,   // w (no downcase)
    DISPLAY_CHARACTER_CODE_X_DOWNCASE, // x
    DISPLAY_CHARACTER_CODE_Y_DOWNCASE, // y
    DISPLAY_CHARACTER_CODE_Z_UPCASE,   // z (no downcase)
};

EAGLETRT_STATIC const enum DisplayCharacterCode display_digit_codes[16] = {
    DISPLAY_CHARACTER_CODE_0,          // 0
    DISPLAY_CHARACTER_CODE_1,          // 1
    DISPLAY_CHARACTER_CODE_2,          // 2
    DISPLAY_CHARACTER_CODE_3,          // 3
    DISPLAY_CHARACTER_CODE_4,          // 4
    DISPLAY_CHARACTER_CODE_5,          // 5
    DISPLAY_CHARACTER_CODE_6,          // 6
    DISPLAY_CHARACTER_CODE_7,          // 7
    DISPLAY_CHARACTER_CODE_8,          // 8
    DISPLAY_CHARACTER_CODE_9,          // 9
    DISPLAY_CHARACTER_CODE_A_UPCASE,   // A (10)
    DISPLAY_CHARACTER_CODE_B_DOWNCASE, // b (11)
    DISPLAY_CHARACTER_CODE_C_UPCASE,   // C (12)
    DISPLAY_CHARACTER_CODE_D_DOWNCASE, // d (13)
    DISPLAY_CHARACTER_CODE_E_UPCASE,   // E (14)
    DISPLAY_CHARACTER_CODE_F_UPCASE,   // F (15)
};

enum DisplayReturnCode display_api_init(const display_segment_set_state_callback set, const display_segment_toggle_state_callback toggle) {
    if (set == NULL || toggle == NULL) {
        return DISPLAY_RC_NULL_POINTER;
    }
    memset(&display_handler, 0U, sizeof(display_handler));
    display_handler.set = set;
    display_handler.toggle = toggle;
    tdsr0760_api_init(&display_handler.tdsr0760);
    return DISPLAY_RC_OK;
}

enum DisplayCharacterCode display_api_get_code_from_hex_digit(const uint8_t digit) {

    constexpr uint8_t hex_digit_max = 0xF;

    if (digit > hex_digit_max) {
        return DISPLAY_CHARACTER_CODE_SPACE;
    }
    return display_digit_codes[digit];
}

enum DisplayCharacterCode display_api_get_code_from_character(const char symbol, const bool prefer_upcase) {
    switch (symbol) {
        // Symbols
        case ' ':
            return DISPLAY_CHARACTER_CODE_SPACE;
        case '.':
            return DISPLAY_CHARACTER_CODE_DOT;
        case '-':
            return DISPLAY_CHARACTER_CODE_HYPEN;
        case '_':
            return DISPLAY_CHARACTER_CODE_UNDERSCORE;
        default:
            break;
    }
    if ((int)symbol >= '0' && (int)symbol <= '9') {
        return display_digit_codes[(int)symbol - '0'];
    }
    if ((int)symbol >= 'a' && (int)symbol <= 'z' && !prefer_upcase) {
        return display_lower_codes[(int)symbol - 'a'];
    }
    if ((int)symbol >= 'A' && (int)symbol <= 'Z') {
        return display_upper_codes[(int)symbol - 'A'];
    }
    if ((int)symbol >= 'a' && (int)symbol <= 'z') {
        return display_upper_codes[(int)symbol - 'a'];
    }

    return DISPLAY_CHARACTER_CODE_SPACE;
}

enum DisplaySegmentStatus display_api_get_segment(const enum DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT) {
        return DISPLAY_SEGMENT_STATUS_UNKNOWN;
    }
    return (enum DisplaySegmentStatus)tdsr0760_api_get_segment(&display_handler.tdsr0760, (enum Tdsr0760Segment)segment);
}

enum DisplayReturnCode display_api_set_segment(const enum DisplaySegment segment, const enum DisplaySegmentStatus status) {
    if (segment >= DISPLAY_SEGMENT_COUNT || segment < 0) {
        return DISPLAY_RC_INVALID_SEGMENT;
    }
    if (status == DISPLAY_SEGMENT_STATUS_UNKNOWN) {
        return DISPLAY_RC_INVALID_STATUS;
    }
    const enum Tdsr0760ReturnCode code = tdsr0760_api_set_segment(
        &display_handler.tdsr0760,
        (enum Tdsr0760Segment)segment,
        (enum Tdsr0760SegmentStatus)status);
    if (code != TDSR0760_RC_OK) {
        return DISPLAY_RC_DRIVER_ERROR;
    }
    display_handler.set(segment, status);
    return DISPLAY_RC_OK;
}

enum DisplayReturnCode display_api_toggle_segment(const enum DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT || segment < 0) {
        return DISPLAY_RC_INVALID_SEGMENT;
    }
    const enum Tdsr0760ReturnCode code = tdsr0760_api_toggle_segment(&display_handler.tdsr0760, (enum Tdsr0760Segment)segment);
    if (code != TDSR0760_RC_OK) {
        return DISPLAY_RC_DRIVER_ERROR;
    }
    const enum Tdsr0760SegmentStatus status = tdsr0760_api_get_segment(&display_handler.tdsr0760, (enum Tdsr0760Segment)segment);
    if (status == TDSR0760_SEGMENT_STATUS_UNKNOWN) {
        return DISPLAY_RC_INVALID_STATUS;
    }
    display_handler.set(segment, (const enum DisplaySegmentStatus)status);
    return DISPLAY_RC_OK;
}

enum DisplayReturnCode display_api_set_segment_all(const bit_flag8_t bits) {
    enum DisplayReturnCode code = DISPLAY_RC_OK;
    for (enum Tdsr0760Segment segment = 0U; segment < TDSR0760_SEGMENT_COUNT; ++segment) {
        const enum Tdsr0760SegmentStatus status = EAGLETRT_API_BIT_GET(bits, segment) ? TDSR0760_SEGMENT_STATUS_ON : TDSR0760_SEGMENT_STATUS_OFF;
        const enum Tdsr0760ReturnCode ret = tdsr0760_api_set_segment(
            &display_handler.tdsr0760,
            segment,
            status);
        if (ret == TDSR0760_RC_OK) {
            display_handler.set((enum DisplaySegment)segment, (enum DisplaySegmentStatus)status);
        } else {
            code = DISPLAY_RC_DRIVER_ERROR;
        }
    }
    return code;
}

enum DisplayReturnCode display_api_set_digit(const uint8_t digit) {

    constexpr uint8_t hex_digit_max = 0xF;

    if (digit > hex_digit_max) {
        return DISPLAY_RC_INVALID_CHARACTER;
    }
    const enum DisplayCharacterCode code = display_api_get_code_from_hex_digit(digit);
    return display_api_set_segment_all(code);
}

enum DisplayReturnCode display_api_set_character(
    const char symbol,
    const bool prefer_upcase) {
    const enum DisplayCharacterCode code = display_api_get_code_from_character(symbol, prefer_upcase);
    return display_api_set_segment_all(code);
}

enum DisplayReturnCode display_api_run_animation(
    const enum DisplaySegmentBit *const animation,
    const size_t size,
    ticks_t ticks_per_frame,
    const ticks_t ticks) {
    if (animation == NULL) {
        return DISPLAY_RC_NULL_POINTER;
    }
    if (ticks_per_frame <= 0U) {
        ticks_per_frame = 1U;
    }
    // Display a step of the animation based on the current time
    const size_t frame = (ticks / ticks_per_frame) % size;
    return display_api_set_segment_all(animation[frame]);
}

enum DisplayReturnCode display_api_run_animation_string(const char *const string, const size_t size, ticks_t ticks_per_frame, const ticks_t ticks) {
    if (string == NULL) {
        return DISPLAY_RC_NULL_POINTER;
    }
    if (ticks_per_frame <= 0U) {
        ticks_per_frame = 1U;
    }

    // Display a step of the animation based on the current time
    const size_t frame = ticks / ticks_per_frame;
    // const size_t intraframe = ticks / (ticks_per_frame / intraframe_count);
    const size_t letter = frame % size;
    // const size_t j = intraframe % intraframe_count;
    return display_api_set_character(string[letter], true);
}

#ifdef CONF_DISPLAY_STRINGS_ENABLE

EAGLETRT_STATIC char *display_api_module_name = "display";

EAGLETRT_STATIC char *display_return_code_name[] = {
    [DISPLAY_RC_OK] = "ok",
    [DISPLAY_RC_NULL_POINTER] = "null pointer",
    [DISPLAY_RC_INVALID_SEGMENT] = "invalid segment",
    [DISPLAY_RC_INVALID_STATUS] = "invalid status",
    [DISPLAY_RC_INVALID_CHARACTER] = "invalid character",
    [DISPLAY_RC_DRIVER_ERROR] = "driver error"
};

EAGLETRT_STATIC char *display_api_return_code_description[] = {
    [DISPLAY_RC_OK] = "executed succefully",
    [DISPLAY_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [DISPLAY_RC_INVALID_SEGMENT] = "the selected segment does not exist",
    [DISPLAY_RC_INVALID_STATUS] = "the given status is not valid",
    [DISPLAY_RC_INVALID_CHARACTER] = "the character cannot be displayed",
    [DISPLAY_RC_DRIVER_ERROR] = "error caused by the driver"
};

#endif // CONF_DISPLAY_STRINGS_ENABLE

#endif // CONF_DISPLAY_MODULE_ENABLE
