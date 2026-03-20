#include "unity.h"
#include "mainboard-def.h"
#include "ir1553204.h"
#include "imd.h"
#include <string.h>
#include <stdbool.h>

Ir1553204Handler ir1553204Handler;

void test_ir1553204_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_init(&ir1553204Handler), "ir1553204_init() should return IR1553204_OK");
}

void test_ir1553204_init_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_init(NULL), "ir1553204_init(NULL) should return IR1553204_NULL_POINTER");
}

void test_ir1553204_getters_default_zero_after_init() {
    Ir1553204Handler empty = { 0 };
    ir1553204Handler.frequency = 123.45f; // set to non-zero to verify init resets it
    ir1553204Handler.duty_cycle = 0.67f;  // set to non-zero to verify init resets it

    ir1553204_init(&ir1553204Handler);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&empty, &ir1553204Handler, sizeof(ir1553204Handler), "Default values should be zero");
}

// these test both get and set, as the get is just returning the struct value set by the set it is usless to split them up

void test_ir1553204_set_get_frequency() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_set_frequency(&ir1553204Handler, 23.0f), "set_frequency should return OK");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 23.0f, ir1553204_get_frequency(&ir1553204Handler), "Frequency should match set value");
}

void test_ir1553204_set_get_duty_cycle() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_set_duty_cycle(&ir1553204Handler, 0.37f), "set_duty_cycle should return OK");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.37f, ir1553204_get_duty_cycle(&ir1553204Handler), "Duty cycle should match set value");
}

void test_ir1553204_setters_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_set_frequency(NULL, 10.0f), "set_frequency(NULL, ...) should return NULL_POINTER");
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_set_duty_cycle(NULL, 0.5f), "set_duty_cycle(NULL, ...) should return NULL_POINTER");
}

void test_ir1553204_get_period_valid() {
    ir1553204Handler.frequency = 20.0f; /* 1000/20 = 50 ms */

    TEST_ASSERT_EQUAL_MESSAGE(50U, ir1553204_get_period(&ir1553204Handler), "Period should be 50 ms for 20 Hz");
}

void test_ir1553204_get_period_zero_frequency() {
    TEST_ASSERT_EQUAL_MESSAGE(0U, ir1553204_get_period(&ir1553204Handler), "Period should be 0 when frequency is 0");
}

void test_ir1553204_get_period_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(0U, ir1553204_get_period(NULL), "get_period(NULL) should return 0");
}

void test_ir1553204_get_status_normal() {
    ir1553204Handler.frequency = 10.0f; /* round(10/10)=1 => NORMAL */

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_NORMAL, ir1553204_get_status(&ir1553204Handler), "Status should be NORMAL for 10 Hz");
}

void test_ir1553204_get_status_unknown_out_of_range() {
    ir1553204Handler.frequency = 42949672950; /* round(70/10)=7 >= COUNT(6) => UNKNOWN */

    //casting roundf(handler->frequency / 10.f) to (uint32_t) causes it to wrap around to 0

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_UNKNOWN, ir1553204_get_status(&ir1553204Handler), "Status should be UNKNOWN out of range");
}

void test_ir1553204_get_status_rounding_behavior() {
    ir1553204Handler.frequency = 14.9f; /* round(1.49)=1 => NORMAL */

    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_NORMAL, ir1553204_get_status(&ir1553204Handler), "Rounding should map 14.9Hz to NORMAL");
}

void setUp() {
    ir1553204_init(&ir1553204Handler);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_ir1553204_init_ok);
    RUN_TEST(test_ir1553204_init_null_pointer);
    RUN_TEST(test_ir1553204_getters_default_zero_after_init);
    RUN_TEST(test_ir1553204_set_get_frequency);
    RUN_TEST(test_ir1553204_set_get_duty_cycle);
    RUN_TEST(test_ir1553204_setters_null_pointer);
    RUN_TEST(test_ir1553204_get_period_valid);
    RUN_TEST(test_ir1553204_get_period_zero_frequency);
    RUN_TEST(test_ir1553204_get_period_null_pointer);
    RUN_TEST(test_ir1553204_get_status_normal);
    RUN_TEST(test_ir1553204_get_status_unknown_out_of_range);
    RUN_TEST(test_ir1553204_get_status_rounding_behavior);

    return UNITY_END();
}
