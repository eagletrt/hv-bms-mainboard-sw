#include "unity.h"
#include "display.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* ---------- callback spies ---------- */

static uint32_t g_set_called = 0U;
static uint32_t g_toggle_called = 0U;
static DisplaySegment g_last_set_segment = (DisplaySegment)0U;
static DisplaySegmentStatus g_last_set_status = DISPLAY_SEGMENT_STATUS_UNKNOWN;
static DisplaySegment g_last_toggle_segment = (DisplaySegment)0U;

static void _display_set_spy(const DisplaySegment segment, const DisplaySegmentStatus status) {
    g_set_called++;
    g_last_set_segment = segment;
    g_last_set_status = status;
}

static void _display_toggle_spy(const DisplaySegment segment) {
    g_toggle_called++;
    g_last_toggle_segment = segment;
}

static void _display_reset_spies(void) {
    g_set_called = 0U;
    g_toggle_called = 0U;
    g_last_set_segment = (DisplaySegment)0U;
    g_last_set_status = DISPLAY_SEGMENT_STATUS_UNKNOWN;
    g_last_toggle_segment = (DisplaySegment)0U;
}

/* ---------- tests ---------- */

void test_display_init_null_set_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_NULL_POINTER,
        display_init(NULL, _display_toggle_spy),
        "display_init() should fail with NULL set callback");
}

void test_display_init_null_toggle_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_NULL_POINTER,
        display_init(_display_set_spy, NULL),
        "display_init() should fail with NULL toggle callback");
}

void test_display_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_OK,
        display_init(_display_set_spy, _display_toggle_spy),
        "display_init() failed to return DISPLAY_OK");
}

void test_display_get_code_from_hex_digit_zero(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_CHARACTER_CODE_0,
        display_get_code_from_hex_digit(0U),
        "Hex digit 0 should map to code 0");
}

void test_display_get_code_from_hex_digit_fifteen(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_CHARACTER_CODE_F_UPCASE,
        display_get_code_from_hex_digit(15U),
        "Hex digit 15 should map to code F");
}

void test_display_get_code_from_hex_digit_invalid(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_CHARACTER_CODE_SPACE,
        display_get_code_from_hex_digit(16U),
        "Invalid hex digit should map to SPACE");
}

void test_display_get_code_from_character_symbols(void) {
    TEST_ASSERT_EQUAL(DISPLAY_CHARACTER_CODE_SPACE, display_get_code_from_character(' ', true, true));
    TEST_ASSERT_EQUAL(DISPLAY_CHARACTER_CODE_DOT, display_get_code_from_character('.', true, true));
    TEST_ASSERT_EQUAL(DISPLAY_CHARACTER_CODE_HYPEN, display_get_code_from_character('-', true, true));
    TEST_ASSERT_EQUAL(DISPLAY_CHARACTER_CODE_UNDERSCORE, display_get_code_from_character('_', true, true));
}

void test_display_get_code_from_character_case_preference(void) {
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_A_UPCASE,
        display_get_code_from_character('A', true, true));
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_A_DOWNCASE,
        display_get_code_from_character('A', true, false));
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_A_DOWNCASE,
        display_get_code_from_character('a', true, false));
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_A_UPCASE,
        display_get_code_from_character('a', true, true));
}

void test_display_get_code_from_character_unsupported_branch_returns_space(void) {
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_SPACE,
        display_get_code_from_character('B', false, true));
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_SPACE,
        display_get_code_from_character('e', false, true));
    TEST_ASSERT_EQUAL(
        DISPLAY_CHARACTER_CODE_SPACE,
        display_get_code_from_character('@', true, true));
}

void test_display_get_segment_invalid_segment(void) {
    const DisplaySegment invalid = (DisplaySegment)DISPLAY_SEGMENT_COUNT;
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_SEGMENT_STATUS_UNKNOWN,
        display_get_segment(invalid),
        "Invalid segment should return UNKNOWN status");
}

void test_display_set_segment_invalid_segment(void) {
    const DisplaySegment invalid = (DisplaySegment)DISPLAY_SEGMENT_COUNT;
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_INVALID_SEGMENT,
        display_set_segment(invalid, DISPLAY_SEGMENT_STATUS_ON),
        "Invalid segment should return DISPLAY_INVALID_SEGMENT");
}

void test_display_set_segment_invalid_status(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_INVALID_STATUS,
        display_set_segment((DisplaySegment)0U, DISPLAY_SEGMENT_STATUS_UNKNOWN),
        "Unknown status should return DISPLAY_INVALID_STATUS");
}

void test_display_set_segment_ok(void) {
    const DisplaySegment s = (DisplaySegment)0U;

    DisplayReturnCode rc = display_set_segment(s, DISPLAY_SEGMENT_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_OK, rc, "display_set_segment() should return DISPLAY_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, g_set_called, "Set callback should be called exactly once");
    TEST_ASSERT_EQUAL_MESSAGE(s, g_last_set_segment, "Set callback segment mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_STATUS_ON, g_last_set_status, "Set callback status mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_SEGMENT_STATUS_ON, display_get_segment(s), "Segment state should be ON");
}

void test_display_toggle_segment_invalid_segment(void) {
    const DisplaySegment invalid = (DisplaySegment)DISPLAY_SEGMENT_COUNT;
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_INVALID_SEGMENT,
        display_toggle_segment(invalid),
        "Invalid segment should return DISPLAY_INVALID_SEGMENT");
}

void test_display_toggle_segment_ok(void) {
    const DisplaySegment s = (DisplaySegment)0U;
    (void)display_set_segment(s, DISPLAY_SEGMENT_STATUS_OFF);
    _display_reset_spies();

    DisplayReturnCode rc = display_toggle_segment(s);

    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_OK, rc, "display_toggle_segment() should return DISPLAY_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, g_set_called, "Set callback should be called once after toggle");
}

void test_display_set_segment_all_ok(void) {
    _display_reset_spies();

    DisplayReturnCode rc = display_set_segment_all((bit_flag8_t)0xFFU);

    TEST_ASSERT_EQUAL_MESSAGE(DISPLAY_OK, rc, "display_set_segment_all() should return DISPLAY_OK");
    TEST_ASSERT_TRUE_MESSAGE(g_set_called > 0U, "Set callback should be called for at least one segment");
}

void test_display_set_digit_invalid(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_INVALID_CHARACTER,
        display_set_digit(0x10U),
        "Digit > 0x0F should return DISPLAY_INVALID_CHARACTER");
}

void test_display_set_digit_valid(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_OK,
        display_set_digit(0x0AU),
        "Valid hex digit should return DISPLAY_OK");
}

void test_display_set_character_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_OK,
        display_set_character('A', true, true),
        "display_set_character() should return DISPLAY_OK");
}

void test_display_run_animation_null_pointer(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_NULL_POINTER,
        display_run_animation(NULL, 3U, 1U, 0U),
        "NULL animation pointer should return DISPLAY_NULL_POINTER");
}

void test_display_run_animation_ok_with_zero_ticks_per_frame(void) {
    const DisplaySegmentBit anim[] = {
        (DisplaySegmentBit)DISPLAY_CHARACTER_CODE_1,
        (DisplaySegmentBit)DISPLAY_CHARACTER_CODE_2,
        (DisplaySegmentBit)DISPLAY_CHARACTER_CODE_3
    };

    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_OK,
        display_run_animation(anim, 3U, 0U, 5U),
        "display_run_animation() should handle ticks_per_frame=0 and return DISPLAY_OK");
}

void test_display_run_animation_string_null_pointer(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_NULL_POINTER,
        display_run_animation_string(NULL, 3U, 1U, 0U),
        "NULL string pointer should return DISPLAY_NULL_POINTER");
}

void test_display_run_animation_string_ok_with_zero_ticks_per_frame(void) {
    const char str[] = "ABC";

    TEST_ASSERT_EQUAL_MESSAGE(
        DISPLAY_OK,
        display_run_animation_string(str, 3U, 0U, 7U),
        "display_run_animation_string() should handle ticks_per_frame=0 and return DISPLAY_OK");
}

#ifdef DISPLAY_TESTS

void setUp(void) {
    _display_reset_spies();
    (void)display_init(_display_set_spy, _display_toggle_spy);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_display_init_null_set_callback);
    RUN_TEST(test_display_init_null_toggle_callback);
    RUN_TEST(test_display_init_ok);
    RUN_TEST(test_display_get_code_from_hex_digit_zero);
    RUN_TEST(test_display_get_code_from_hex_digit_fifteen);
    RUN_TEST(test_display_get_code_from_hex_digit_invalid);
    RUN_TEST(test_display_get_code_from_character_symbols);
    RUN_TEST(test_display_get_code_from_character_case_preference);
    RUN_TEST(test_display_get_code_from_character_unsupported_branch_returns_space);
    RUN_TEST(test_display_get_segment_invalid_segment);
    RUN_TEST(test_display_set_segment_invalid_segment);
    RUN_TEST(test_display_set_segment_invalid_status);
    RUN_TEST(test_display_set_segment_ok);
    RUN_TEST(test_display_toggle_segment_invalid_segment);
    RUN_TEST(test_display_toggle_segment_ok);
    RUN_TEST(test_display_set_segment_all_ok);
    RUN_TEST(test_display_set_digit_invalid);
    RUN_TEST(test_display_set_digit_valid);
    RUN_TEST(test_display_set_character_ok);
    RUN_TEST(test_display_run_animation_null_pointer);
    RUN_TEST(test_display_run_animation_ok_with_zero_ticks_per_frame);
    RUN_TEST(test_display_run_animation_string_null_pointer);
    RUN_TEST(test_display_run_animation_string_ok_with_zero_ticks_per_frame);
    return UNITY_END();
}

#endif /* DISPLAY_TESTS */