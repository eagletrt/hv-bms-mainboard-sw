/*!
 * \file test-tdsr0760.c
 * \date 2026-06-15
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief TDSR0760 tests
 */

#include "tdsr0760.h"
#include "unity.h"
#include "tdsr0760-api.h"
#include <string.h>

static struct Tdsr0760Handler tdsr0760_handler;

void test_tdsr0760_api_init_ok() {
    struct Tdsr0760Handler expected_handler;
    memset(&expected_handler, 0U, sizeof(expected_handler));
    for (size_t i = 0U; i < TDSR0760_SEGMENT_COUNT; ++i) {
        expected_handler.segments[i] = TDSR0760_SEGMENT_STATUS_UNKNOWN;
    }

    enum Tdsr0760ReturnCode rc = tdsr0760_api_init(&tdsr0760_handler);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_OK, rc, "tdsr0760_api_init() failed to return TDSR0760_RC_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &tdsr0760_handler, sizeof(expected_handler), "tdsr0760_api_init() did not properly initialize the handler");
}

void test_tdsr0760_api_init_null_pointer() {
    enum Tdsr0760ReturnCode rc = tdsr0760_api_init(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_NULL_POINTER, rc, "tdsr0760_api_init() should return TDSR0760_RC_NULL_POINTER if handler pointer is NULL");
}

void test_tdsr0760_api_get_segment_null_handler() {
    enum Tdsr0760SegmentStatus status = tdsr0760_api_get_segment(NULL, 0U);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_SEGMENT_STATUS_UNKNOWN, status, "tdsr0760_api_get_segment() should return TDSR0760_SEGMENT_STATUS_UNKNOWN if handler pointer is NULL");
}

void test_tdsr0760_api_get_segment_invalid_segment() {
    enum Tdsr0760SegmentStatus status = tdsr0760_api_get_segment(&tdsr0760_handler, TDSR0760_SEGMENT_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_SEGMENT_STATUS_UNKNOWN, status, "tdsr0760_api_get_segment() should return TDSR0760_SEGMENT_STATUS_UNKNOWN if segment is invalid");
}

void test_tdsr0760_api_set_segment_null_handler() {
    enum Tdsr0760ReturnCode rc = tdsr0760_api_set_segment(NULL, 0U, TDSR0760_SEGMENT_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_NULL_POINTER, rc, "tdsr0760_api_set_segment() should return TDSR0760_RC_NULL_POINTER if handler pointer is NULL");
}

void test_tdsr0760_api_set_segment_invalid_segment() {
    enum Tdsr0760ReturnCode rc = tdsr0760_api_set_segment(&tdsr0760_handler, TDSR0760_SEGMENT_COUNT, TDSR0760_SEGMENT_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_INVALID_SEGMENT, rc, "tdsr0760_api_set_segment() should return TDSR0760_RC_INVALID_SEGMENT if segment is invalid");
}

void test_tdsr0760_api_toggle_segment_null_handler() {
    enum Tdsr0760ReturnCode rc = tdsr0760_api_toggle_segment(NULL, 0U);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_NULL_POINTER, rc, "tdsr0760_api_toggle_segment() should return TDSR0760_RC_NULL_POINTER if handler pointer is NULL");
}

void test_tdsr0760_api_toggle_segment_invalid_segment() {
    enum Tdsr0760ReturnCode rc = tdsr0760_api_toggle_segment(&tdsr0760_handler, TDSR0760_SEGMENT_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_INVALID_SEGMENT, rc, "tdsr0760_api_toggle_segment() should return TDSR0760_RC_INVALID_SEGMENT if segment is invalid");
}

void test_tdsr0760_api_toggles_from_off_to_on() {

    tdsr0760_handler.segments[0U] = TDSR0760_SEGMENT_STATUS_OFF;

    enum Tdsr0760ReturnCode rc;
    rc = tdsr0760_api_toggle_segment(&tdsr0760_handler, 0U);
    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_OK, rc, "tdsr0760_api_toggle_segment() failed to toggle segment from OFF to ON");

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_SEGMENT_STATUS_ON, tdsr0760_handler.segments[0U], "tdsr0760_api_toggle_segment() did not toggle segment from OFF to ON");
}

void test_tdsr0760_api_toggles_from_on_to_off() {

    tdsr0760_handler.segments[0U] = TDSR0760_SEGMENT_STATUS_ON;

    enum Tdsr0760ReturnCode rc;
    rc = tdsr0760_api_toggle_segment(&tdsr0760_handler, 0U);
    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_RC_OK, rc, "tdsr0760_api_toggle_segment() failed to toggle segment from ON to OFF");

    TEST_ASSERT_EQUAL_MESSAGE(TDSR0760_SEGMENT_STATUS_OFF, tdsr0760_handler.segments[0U], "tdsr0760_api_toggle_segment() did not toggle segment from ON to OFF");
}

void setUp() {
    tdsr0760_api_init(&tdsr0760_handler);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_tdsr0760_api_init_ok);
    RUN_TEST(test_tdsr0760_api_init_null_pointer);
    RUN_TEST(test_tdsr0760_api_get_segment_null_handler);
    RUN_TEST(test_tdsr0760_api_get_segment_invalid_segment);
    RUN_TEST(test_tdsr0760_api_set_segment_null_handler);
    RUN_TEST(test_tdsr0760_api_set_segment_invalid_segment);
    RUN_TEST(test_tdsr0760_api_toggle_segment_null_handler);
    RUN_TEST(test_tdsr0760_api_toggle_segment_invalid_segment);
    RUN_TEST(test_tdsr0760_api_toggles_from_off_to_on);
    RUN_TEST(test_tdsr0760_api_toggles_from_on_to_off);

    return UNITY_END();
}
