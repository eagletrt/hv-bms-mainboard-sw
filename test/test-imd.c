#include "unity.h"
#include "mainboard-def.h"
#include "ir1553204.h"
#include "imd.h"
#include <string.h>
#include <stdbool.h>

extern _ImdHandler himd;

/* ------------------------- IR1553204 tests ------------------------- */

void test_ir1553204_init_ok() {
    Ir1553204Handler h;
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_init(&h), "ir1553204_init() should return IR1553204_OK");
}

void test_ir1553204_init_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_init(NULL), "ir1553204_init(NULL) should return IR1553204_NULL_POINTER");
}

void test_ir1553204_getters_default_zero_after_init() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, ir1553204_get_frequency(&h), "Default frequency should be 0");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, ir1553204_get_duty_cycle(&h), "Default duty cycle should be 0");
}

void test_ir1553204_set_get_frequency() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_set_frequency(&h, 23.0f), "set_frequency should return OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 23.0f, ir1553204_get_frequency(&h), "Frequency should match set value");
}

void test_ir1553204_set_get_duty_cycle() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_OK, ir1553204_set_duty_cycle(&h, 0.37f), "set_duty_cycle should return OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.37f, ir1553204_get_duty_cycle(&h), "Duty cycle should match set value");
}

void test_ir1553204_setters_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_set_frequency(NULL, 10.0f), "set_frequency(NULL, ...) should return NULL_POINTER");
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_NULL_POINTER, ir1553204_set_duty_cycle(NULL, 0.5f), "set_duty_cycle(NULL, ...) should return NULL_POINTER");
}

void test_ir1553204_get_period_valid() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    ir1553204_set_frequency(&h, 20.0f); /* 1000/20 = 50 ms */
    TEST_ASSERT_EQUAL_MESSAGE(50U, ir1553204_get_period(&h), "Period should be 50 ms for 20 Hz");
}

void test_ir1553204_get_period_zero_frequency() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    TEST_ASSERT_EQUAL_MESSAGE(0U, ir1553204_get_period(&h), "Period should be 0 when frequency is 0");
    TEST_ASSERT_EQUAL_MESSAGE(0U, ir1553204_get_period(NULL), "Period should be 0 on NULL handler");
}

void test_ir1553204_get_status_normal() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    ir1553204_set_frequency(&h, 10.0f); /* round(10/10)=1 => NORMAL */
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_NORMAL, ir1553204_get_status(&h), "Status should be NORMAL for 10 Hz");
}

void test_ir1553204_get_status_unknown_out_of_range() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    ir1553204_set_frequency(&h, 70.0f); /* round(70/10)=7 >= COUNT(6) => UNKNOWN */
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_UNKNOWN, ir1553204_get_status(&h), "Status should be UNKNOWN out of range");
}

void test_ir1553204_get_status_rounding_behavior() {
    Ir1553204Handler h;
    ir1553204_init(&h);
    ir1553204_set_frequency(&h, 14.9f); /* round(1.49)=1 => NORMAL */
    TEST_ASSERT_EQUAL_MESSAGE(IR1553204_STATUS_NORMAL, ir1553204_get_status(&h), "Rounding should map 14.9Hz to NORMAL");
}

/* ---------------------------- IMD tests ----------------------------- */

static bool g_imd_start_called = false;

static void imd_start_mock(void) {
    g_imd_start_called = true;
}

void test_imd_init_ok_and_start_called() {
    g_imd_start_called = false;
    TEST_ASSERT_EQUAL_MESSAGE(IMD_OK, imd_init(imd_start_mock), "imd_init() should return IMD_OK");
    TEST_ASSERT_TRUE_MESSAGE(g_imd_start_called, "Start callback should be called by imd_init()");
}

void test_imd_update_invalid_period_count() {
    (void)imd_init(imd_start_mock);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_INVALID_DATA, imd_update(1000U, 0U, 0U), "imd_update should reject period_count=0");
}

void test_imd_update_sets_frequency_and_duty_cycle() {
    (void)imd_init(imd_start_mock);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_OK, imd_update(1000U, 100U, 40U), "imd_update should return IMD_OK for valid input");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 10.0f, imd_get_frequency(), "Frequency should be computed as source/period_count");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.4f, imd_get_duty_cycle(), "Duty cycle should be computed as high/period_count");
}

void test_imd_get_status_normal_after_update() {
    (void)imd_init(imd_start_mock);
    (void)imd_update(1000U, 100U, 50U); /* freq=10 => NORMAL */
    TEST_ASSERT_EQUAL_MESSAGE(IMD_STATUS_NORMAL, imd_get_status(), "IMD status should be NORMAL after 10Hz update");
}

void test_imd_get_period_after_update() {
    (void)imd_init(imd_start_mock);
    (void)imd_update(1000U, 50U, 25U); /* freq=20 => period=50ms */
    TEST_ASSERT_EQUAL_MESSAGE(50U, imd_get_period(), "IMD period should be 50ms for 20Hz");
}

void test_imd_get_status_canlib_payload_pointer_and_size() {
    size_t byte_size = 0U;
    (void)imd_init(imd_start_mock);
    primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(&himd.status_can_payload, payload, "Payload pointer should match internal handler payload");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(himd.status_can_payload), byte_size, "Payload byte_size mismatch");
}

void test_imd_get_status_canlib_payload_content_mapping() {
    size_t byte_size = 0U;
    (void)imd_init(imd_start_mock);
    (void)imd_update(1000U, 100U, 30U); /* freq=10 => IMD_STATUS_NORMAL */

    primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);
    (void)byte_size;

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Payload pointer should not be NULL");
    TEST_ASSERT_EQUAL_MESSAGE((int)(IMD_STATUS_NORMAL + 1), (int)payload->status, "CAN status should be IMD status + 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 10.0f, payload->frequency, "CAN payload frequency mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.3f, payload->duty_cycle, "CAN payload duty cycle mismatch");
}


/* -------------------------- Unity runner ---------------------------- */

#ifdef IMD_TESTS

void setUp() {
    g_imd_start_called = false;
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
    RUN_TEST(test_ir1553204_get_status_normal);
    RUN_TEST(test_ir1553204_get_status_unknown_out_of_range);
    RUN_TEST(test_ir1553204_get_status_rounding_behavior);

    RUN_TEST(test_imd_init_ok_and_start_called);
    RUN_TEST(test_imd_update_invalid_period_count);
    RUN_TEST(test_imd_update_sets_frequency_and_duty_cycle);
    RUN_TEST(test_imd_get_status_normal_after_update);
    RUN_TEST(test_imd_get_period_after_update);
    RUN_TEST(test_imd_get_status_canlib_payload_pointer_and_size);
    RUN_TEST(test_imd_get_status_canlib_payload_content_mapping);

    return UNITY_END();
}

#endif // IMD_TESTS