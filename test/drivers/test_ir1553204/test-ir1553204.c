/*!
 * \file test-ir1553204.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief IR155-3204 insulation monitoring device tests
 */

#include "ir1553204-api.h"

#include "unity.h"
#include <string.h>

static struct Ir1553204Handler ir1553204_handler;

void test_ir1553204_api_init_ok() {
    struct Ir1553204Handler expected_handler;
    memset(&expected_handler, 0U, sizeof(expected_handler));

    enum Ir1553204ReturnCode rc = ir1553204_api_init(&ir1553204_handler);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_RC_OK, rc, "ir1553204_api_init() failed to return IR1553204_RC_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &ir1553204_handler, sizeof(expected_handler), "ir1553204_api_init() did not properly initialize the handler");
}

void test_ir1553204_api_init_null_pointer() {
    enum Ir1553204ReturnCode rc = ir1553204_api_init(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_RC_NULL_POINTER, rc, "ir1553204_api_init() should return IR1553204_RC_NULL_POINTER if given a NULL pointer");
}

void test_ir1553204_api_get_duty_cycle_null_pointer() {
    precise_percentage_t duty_cycle = ir1553204_api_get_duty_cycle(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0.F, duty_cycle, "ir1553204_api_get_duty_cycle() should return 0 if given a NULL pointer");
}

void test_ir1553204_api_set_duty_cycle_null_pointer() {
    enum Ir1553204ReturnCode rc = ir1553204_api_set_duty_cycle(NULL, 0.5F);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_RC_NULL_POINTER, rc, "ir1553204_api_set_duty_cycle() should return IR1553204_RC_NULL_POINTER if given a NULL pointer");
}

void test_ir1553204_api_get_frequency_null_pointer() {
    hertz_t frequency = ir1553204_api_get_frequency(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0.F, frequency, "ir1553204_api_get_frequency() should return 0 if given a NULL pointer");
}

void test_ir1553204_api_set_frequency_null_pointer() {
    enum Ir1553204ReturnCode rc = ir1553204_api_set_frequency(NULL, 100.F);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_RC_NULL_POINTER, rc, "ir1553204_api_set_frequency() should return IR1553204_RC_NULL_POINTER if given a NULL pointer");
}

void test_ir1553204_api_get_period_null_pointer() {
    milliseconds_t period = ir1553204_api_get_period(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, period, "ir1553204_api_get_period() should return 0 if given a NULL pointer");
}

void test_ir1553204_api_get_period_zero_frequency() {
    struct Ir1553204Handler handler = { .frequency = 0.F };

    milliseconds_t period = ir1553204_api_get_period(&handler);

    TEST_ASSERT_EQUAL_MESSAGE(0U, period, "ir1553204_api_get_period() should return 0 if frequency is 0");
}

void test_ir1553204_api_get_period_ok() {
    struct Ir1553204Handler handler = { .frequency = 100.F };

    milliseconds_t period = ir1553204_api_get_period(&handler);

    TEST_ASSERT_EQUAL_MESSAGE(10U, period, "ir1553204_api_get_period() returned incorrect period");
}

void test_ir1553204_api_get_status_null_pointer() {
    enum Ir1553204Status status = ir1553204_api_get_status(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_UNKNOWN, status, "ir1553204_api_get_status() should return IR1553204_STATUS_UNKNOWN if given a NULL pointer");
}

void test_ir1553204_api_get_status_ok() {
    struct Ir1553204Handler handler = { .frequency = 25.F };

    enum Ir1553204Status status = ir1553204_api_get_status(&handler);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_START_MEASURE, status, "ir1553204_api_get_status() returned incorrect status");
}

void test_ir1553204_api_get_status_unknown() {
    struct Ir1553204Handler handler = { .frequency = 1000.F };

    enum Ir1553204Status status = ir1553204_api_get_status(&handler);

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_UNKNOWN, status, "ir1553204_api_get_status() should return IR1553204_STATUS_UNKNOWN if frequency is out of range");
}

void setUp() {

    ir1553204_api_init(&ir1553204_handler);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_ir1553204_api_init_ok);
    RUN_TEST(test_ir1553204_api_init_null_pointer);
    RUN_TEST(test_ir1553204_api_get_duty_cycle_null_pointer);
    RUN_TEST(test_ir1553204_api_set_duty_cycle_null_pointer);
    RUN_TEST(test_ir1553204_api_get_frequency_null_pointer);
    RUN_TEST(test_ir1553204_api_set_frequency_null_pointer);
    RUN_TEST(test_ir1553204_api_get_period_null_pointer);
    RUN_TEST(test_ir1553204_api_get_period_zero_frequency);
    RUN_TEST(test_ir1553204_api_get_period_ok);
    RUN_TEST(test_ir1553204_api_get_status_null_pointer);
    RUN_TEST(test_ir1553204_api_get_status_ok);
    RUN_TEST(test_ir1553204_api_get_status_unknown);

    return UNITY_END();
}
