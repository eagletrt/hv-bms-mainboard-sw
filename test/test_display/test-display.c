/*!
 * \file test-display.c
 * \date 2026-06-15
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Display API tests
 */

#include "display.h"
#include "mainboard-def.h"
#include "unity.h"
#include "display-api.h"
#include <string.h>
#include <fff.h>
#include <stdio.h>
DEFINE_FFF_GLOBALS;

extern struct DisplayHandler display_handler;

FAKE_VOID_FUNC(display_set, const enum DisplaySegment, const enum DisplaySegmentStatus);
FAKE_VOID_FUNC(display_toggle, const enum DisplaySegment);

void test_display_init_ok(void) {
    enum DisplayReturnCode rc = display_api_init(display_set, display_toggle);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_init should return DISPLAY_RC_OK");
    TEST_ASSERT_NOT_NULL_MESSAGE(display_handler.set, "display_handler.set should not be NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(display_handler.toggle, "display_handler.toggle should not be NULL");
}

void test_display_init_null_pointer_set(void) {
    enum DisplayReturnCode rc = display_api_init(NULL, display_toggle);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_NULL_POINTER, rc, "display_api_init should return DISPLAY_RC_NULL_POINTER when set is NULL");
}

void test_display_init_null_pointer_toggle(void) {
    enum DisplayReturnCode rc = display_api_init(display_set, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_NULL_POINTER, rc, "display_api_init should return DISPLAY_RC_NULL_POINTER when toggle is NULL");
}

void test_display_get_code_from_hex_digit_valid(void) {

    enum DisplayCharacterCode actual_codes[16];

    for (uint8_t digit = 0; digit <= 15; ++digit) {
        actual_codes[digit] = display_api_get_code_from_hex_digit(digit);
    }

    int space_count = 0;
    for (int i = 0; i < 16; ++i) {
        if (actual_codes[i] == DISPLAY_CHARACTER_CODE_SPACE) {
            ++space_count;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, space_count, "All valid hex digits (0-15) should map to a non-SPACE character code");
}

void test_display_get_code_from_hex_digit_invalid(void) {
    enum DisplayCharacterCode code = display_api_get_code_from_hex_digit(16);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_CHARACTER_CODE_SPACE, code, "Failed for invalid digit: 16");
}

void test_display_get_code_from_character_valid(void) {
    const char *valid_chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_";
    const size_t len = strlen(valid_chars);

    enum DisplayCharacterCode actual_codes[len];
    for (size_t i = 0; i < len; ++i) {
        actual_codes[i] = display_api_get_code_from_character(valid_chars[i], true);
    }

    int space_count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (actual_codes[i] == DISPLAY_CHARACTER_CODE_SPACE) {
            ++space_count;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, space_count, "All valid characters should map to a non-SPACE character code");
}

void test_display_get_code_from_character_invalid(void) {
    const char *invalid_chars = "!@#^&*()+=[]{}|;:,<>/?`~ ";
    const size_t len = strlen(invalid_chars);

    enum DisplayCharacterCode actual_codes[len];
    for (size_t i = 0; i < len; ++i) {
        actual_codes[i] = display_api_get_code_from_character(invalid_chars[i], true);
    }

    TEST_ASSERT_EACH_EQUAL_INT_MESSAGE(DISPLAY_CHARACTER_CODE_SPACE, actual_codes, len, "All invalid characters should map to DISPLAY_CHARACTER_CODE_SPACE");
}

void test_display_get_segment_invalid(void) {
    enum DisplaySegmentStatus status = display_api_get_segment(DISPLAY_SEGMENT_INVALID);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_STATUS_UNKNOWN, status, "display_api_get_segment should return DISPLAY_SEGMENT_STATUS_UNKNOWN for invalid segment");
}

void test_display_set_segment_invalid_segment(void) {
    enum DisplayReturnCode rc = display_api_set_segment(DISPLAY_SEGMENT_INVALID, DISPLAY_SEGMENT_STATUS_ON);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_INVALID_SEGMENT, rc, "display_api_set_segment should return DISPLAY_RC_INVALID_SEGMENT for invalid segment");
}

void test_display_set_segment_invalid_status(void) {
    enum DisplayReturnCode rc = display_api_set_segment(DISPLAY_SEGMENT_TOP, DISPLAY_SEGMENT_STATUS_UNKNOWN);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_INVALID_STATUS, rc, "display_api_set_segment should return DISPLAY_RC_INVALID_STATUS for invalid status");
}

void test_display_set_segment_ok(void) {
    enum DisplayReturnCode rc = display_api_set_segment(DISPLAY_SEGMENT_TOP, DISPLAY_SEGMENT_STATUS_ON);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_set_segment should return DISPLAY_RC_OK for valid segment and status");
    TEST_ASSERT_TRUE_MESSAGE(display_set_fake.call_count > 0, "Expected call to display_set_fake was not made");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_TOP, display_set_fake.arg0_val, "display_set_fake was called with incorrect segment");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_STATUS_ON, display_set_fake.arg1_val, "display_set_fake was called with incorrect status");
}

void test_display_toggle_segment_invalid_segment(void) {
    enum DisplayReturnCode rc = display_api_toggle_segment(DISPLAY_SEGMENT_INVALID);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_INVALID_SEGMENT, rc, "display_api_toggle_segment should return DISPLAY_RC_INVALID_SEGMENT for invalid segment");
}

void test_display_toggle_segment_ok(void) {
    // Set the segment to a known state
    display_api_set_segment(DISPLAY_SEGMENT_TOP, DISPLAY_SEGMENT_STATUS_OFF);
    // Toggle the segment
    enum DisplayReturnCode rc = display_api_toggle_segment(DISPLAY_SEGMENT_TOP);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_toggle_segment should return DISPLAY_RC_OK for valid segment");
    TEST_ASSERT_TRUE_MESSAGE(display_set_fake.call_count > 0, "Expected call to display_set_fake was not made");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_TOP, display_set_fake.arg0_val, "display_set_fake was called with incorrect segment");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_STATUS_ON, display_set_fake.arg1_val, "display_set_fake was called with incorrect status");
}

void test_display_set_segment_all_ok(void) {
    bit_flag8_t code = DISPLAY_SEGMENT_BIT_TOP | DISPLAY_SEGMENT_BIT_MIDDLE | DISPLAY_SEGMENT_BIT_BOTTOM;

    enum DisplaySegmentStatus expected_status[DISPLAY_SEGMENT_COUNT] = { 1, 0, 0, 1, 0, 0, 1, 0 };
    enum DisplaySegment expected_segment[DISPLAY_SEGMENT_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    enum DisplayReturnCode rc = display_api_set_segment_all(code);

    for (int i = 0; i < display_set_fake.call_count; ++i) {
        printf("Call %d: Segment = %d, Status = %d\n", i, display_set_fake.arg0_history[i], display_set_fake.arg1_history[i]);
    }

    printf("Expected Segments: ");
    for (int i = 0; i < DISPLAY_SEGMENT_COUNT; ++i) {
        printf("%d , %d \n", expected_segment[i], display_set_fake.arg0_history[i]);
    }
    printf("\n");

    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_set_segment_all should return DISPLAY_RC_OK for valid code");
    TEST_ASSERT_TRUE_MESSAGE(display_set_fake.call_count > 0, "Expected call to display_set_fake was not made");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected_segment, display_set_fake.arg0_history, DISPLAY_SEGMENT_COUNT, "Segments do not match expected");
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected_status, display_set_fake.arg1_history, DISPLAY_SEGMENT_COUNT, "Statuses do not match expected");
}

void test_display_set_digit_valid(void) {
    enum DisplayReturnCode actual_rcs[16];
    for (uint8_t digit = 0; digit <= 15; ++digit) {
        actual_rcs[digit] = display_api_set_digit(digit);
    }

    TEST_ASSERT_EACH_EQUAL_INT_MESSAGE(DISPLAY_RC_OK, actual_rcs, 16, "display_api_set_digit should return DISPLAY_RC_OK for all valid digits (0-15)");
}

void test_display_set_digit_invalid(void) {
    enum DisplayReturnCode rc = display_api_set_digit(16);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_INVALID_CHARACTER, rc, "display_api_set_digit should return DISPLAY_RC_INVALID_CHARACTER for invalid digit");
}

void test_display_set_character_valid(void) {
    const char *valid_chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_";
    const size_t len = strlen(valid_chars);

    enum DisplayReturnCode actual_rcs[len];
    for (size_t i = 0; i < len; ++i) {
        actual_rcs[i] = display_api_set_character(valid_chars[i], true);
    }

    TEST_ASSERT_EACH_EQUAL_INT_MESSAGE(DISPLAY_RC_OK, actual_rcs, len, "display_api_set_character should return DISPLAY_RC_OK for all valid characters");
}

void test_display_set_character_invalid(void) {
    enum DisplayReturnCode rc = display_api_set_character('!', true);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_CHARACTER_CODE_SPACE, rc, "display_api_set_character should return DISPLAY_CHARACTER_CODE_SPACE for invalid character");
}

void test_display_run_animation_null_pointer(void) {
    enum DisplayReturnCode rc = display_api_run_animation(NULL, 1, 100, 100);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_NULL_POINTER, rc, "display_api_run_animation should return DISPLAY_RC_NULL_POINTER when animation is NULL");
}

void test_display_run_animation_ok(void) {
    enum DisplaySegmentBit animation[] = { DISPLAY_SEGMENT_BIT_TOP, DISPLAY_SEGMENT_BIT_MIDDLE, DISPLAY_SEGMENT_BIT_BOTTOM };
    enum DisplayReturnCode rc = display_api_run_animation(animation, sizeof(animation) / sizeof(animation[0]), 100, 100);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_run_animation should return DISPLAY_RC_OK for valid animation");
}

void test_display_run_animation_string_null_pointer(void) {
    enum DisplayReturnCode rc = display_api_run_animation(NULL, 0, 100, 100);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_NULL_POINTER, rc, "display_api_run_animation should return DISPLAY_RC_NULL_POINTER when animation is NULL");
}

void test_display_run_animation_string_ok(void) {
    const char *animation = "TOP MIDDLE BOTTOM";
    enum DisplayReturnCode rc = display_api_run_animation((const enum DisplaySegmentBit *)animation, strlen(animation), 100, 100);
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_RC_OK, rc, "display_api_run_animation should return DISPLAY_RC_OK for valid animation string");
}

void setUp() {
    RESET_FAKE(display_set);
    RESET_FAKE(display_toggle);
    display_api_init(display_set, display_toggle);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_display_init_ok);
    RUN_TEST(test_display_init_null_pointer_set);
    RUN_TEST(test_display_init_null_pointer_toggle);
    RUN_TEST(test_display_get_code_from_hex_digit_valid);
    RUN_TEST(test_display_get_code_from_hex_digit_invalid);
    RUN_TEST(test_display_get_code_from_character_valid);
    RUN_TEST(test_display_get_code_from_character_invalid);
    RUN_TEST(test_display_get_segment_invalid);
    RUN_TEST(test_display_set_segment_invalid_segment);
    RUN_TEST(test_display_set_segment_invalid_status);
    RUN_TEST(test_display_set_segment_ok);
    RUN_TEST(test_display_toggle_segment_invalid_segment);
    RUN_TEST(test_display_toggle_segment_ok);
    RUN_TEST(test_display_set_segment_all_ok);
    RUN_TEST(test_display_set_digit_valid);
    RUN_TEST(test_display_set_digit_invalid);
    RUN_TEST(test_display_set_character_valid);
    RUN_TEST(test_display_set_character_invalid);
    RUN_TEST(test_display_run_animation_null_pointer);
    RUN_TEST(test_display_run_animation_ok);

    return UNITY_END();
}
