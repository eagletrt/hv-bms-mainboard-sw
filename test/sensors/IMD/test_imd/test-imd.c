#include "unity.h"
#include "mainboard-def.h"
#include "ir1553204.h"
#include "imd.h"
#include <string.h>
#include <stdbool.h>
#include <fff.h>
DEFINE_FFF_GLOBALS;

extern _ImdHandler himd;

/* ---------------------------- IMD tests ----------------------------- */

FAKE_VOID_FUNC(imd_start_mock);

void test_imd_init_ok() {
    RESET_FAKE(imd_start_mock);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_OK, imd_init(imd_start_mock), "imd_init() should return IMD_OK");
    TEST_ASSERT_TRUE_MESSAGE(imd_start_mock_fake.call_count > 0, "Start callback should be called by imd_init()");
}

void test_imd_update_invalid_period_count() {
    TEST_ASSERT_EQUAL_MESSAGE(IMD_INVALID_DATA, imd_update(1000U, 0U, 0U), "imd_update should reject period_count=0");
}

void test_imd_update_sets_frequency_and_duty_cycle() {
    TEST_ASSERT_EQUAL_MESSAGE(IMD_OK, imd_update(1000U, 100U, 40U), "imd_update should return IMD_OK for valid input");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 10.0f, imd_get_frequency(), "Frequency should be computed as source/period_count");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.4f, imd_get_duty_cycle(), "Duty cycle should be computed as high/period_count");
}

// these tests are redundant with ir1553204 tests but better to be safe than sorry

void test_imd_get_status_normal_after_update() {
    (void)imd_update(1000U, 100U, 50U); /* freq=10 => NORMAL */
    TEST_ASSERT_EQUAL_MESSAGE(IMD_STATUS_NORMAL, imd_get_status(), "IMD status should be NORMAL after 10Hz update");
}

void test_imd_get_status_under_voltage_after_update() {
    (void)imd_update(2000U, 100U, 50U); /* freq=20 => UNDER_VOLTAGE */
    TEST_ASSERT_EQUAL_MESSAGE(IMD_STATUS_UNDER_VOLTAGE, imd_get_status(), "IMD status should be UNDER_VOLTAGE after 20Hz update");
}

void test_imd_get_period_after_update() {
    (void)imd_update(1000U, 50U, 25U); /* freq=20 => period=50ms */
    TEST_ASSERT_EQUAL_MESSAGE(50U, imd_get_period(), "IMD period should be 50ms for 20Hz");
}

void test_imd_get_status_canlib_payload_pointer_and_size() {
    size_t byte_size = 0U;
    primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(&himd.status_can_payload, payload, "Payload pointer should match internal handler payload");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(himd.status_can_payload), byte_size, "Payload byte_size mismatch");
}

void test_imd_get_status_canlib_payload_NULL_size_ptr_ok() {
    size_t byte_size = 0U;
    primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Payload pointer should not be NULL when byte_size is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(himd.status_can_payload), byte_size, "Byte size should be sizeof(himd.status_can_payload) when byte_size is NULL");
}

void test_imd_get_status_canlib_payload_content_mapping() {
    size_t byte_size = 0U;

    (void)imd_update(1000U, 100U, 30U); /* freq=10 => IMD_STATUS_NORMAL */

    primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);
    (void)byte_size;

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Payload pointer should not be NULL");

    // MISMATCH BETWEEN CANLIB STATUS AND IMD STATUS ON PURPOSE TO DETECT WRONG MAPPING
    TEST_ASSERT_EQUAL_MESSAGE((int)(IMD_STATUS_NORMAL + 1), (int)payload->status, "CAN status should be IMD status + 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 10.0f, payload->frequency, "CAN payload frequency mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.3f, payload->duty_cycle, "CAN payload duty cycle mismatch");
}

#ifdef IMD_TESTS

void setUp() {
    RESET_FAKE(imd_start_mock);
    (void)imd_init(imd_start_mock);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_imd_init_ok);
    RUN_TEST(test_imd_update_invalid_period_count);
    RUN_TEST(test_imd_update_sets_frequency_and_duty_cycle);
    RUN_TEST(test_imd_get_status_normal_after_update);
    RUN_TEST(test_imd_get_status_under_voltage_after_update);
    RUN_TEST(test_imd_get_period_after_update);
    RUN_TEST(test_imd_get_status_canlib_payload_pointer_and_size);
    RUN_TEST(test_imd_get_status_canlib_payload_NULL_size_ptr_ok);
    RUN_TEST(test_imd_get_status_canlib_payload_content_mapping);

    return UNITY_END();
}

#endif // IMD_TESTS