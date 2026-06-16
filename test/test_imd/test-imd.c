/*!
 * \file test-imd.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief IMD tests
 */

#include "imd-api.h"
#include "unity.h"

#include <fff.h>
DEFINE_FFF_GLOBALS;

extern struct ImdHandler imd_handler;

FAKE_VOID_FUNC(start_pwm_conversion);

void test_imd_init_null_pointer(void) {
    enum ImdReturnCode code = imd_init(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_NULL_POINTER, code, "imd_init should return IMD_RC_NULL_POINTER when given a NULL pointer");
}

void test_imd_init_ok(void) {
    enum ImdReturnCode code = imd_init(start_pwm_conversion);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_OK, code, "imd_init should return IMD_RC_OK when given a valid pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(start_pwm_conversion, imd_handler.start, "imd_handler.start should be set to the given callback function");
}

void test_imd_update_invalid_data(void) {
    enum ImdReturnCode code = imd_update(1000, 0, 500);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_INVALID_DATA, code, "imd_update should return IMD_RC_INVALID_DATA when period_count is 0");
}

void test_imd_update_ok(void) {
    enum ImdReturnCode code = imd_update(1000, 100, 50);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_OK, code, "imd_update should return IMD_RC_OK when given valid parameters");

    TEST_ASSERT_EQUAL_MESSAGE(10, ir1553204_api_get_frequency(&imd_handler.ir1153204), "imd_update should set the frequency correctly");
    TEST_ASSERT_EQUAL_MESSAGE(0.5, ir1553204_api_get_duty_cycle(&imd_handler.ir1153204), "imd_update should set the duty cycle correctly");
}

void test_imd_get_status_canlib_payload_null(void) {
    const primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "imd_get_status_canlib_payload should not return NULL");

    TEST_ASSERT_EQUAL_MESSAGE((primary_hv_imd_status_status)(imd_get_status() + 1U), payload->status, "imd_get_status_canlib_payload should set the status correctly");
    TEST_ASSERT_EQUAL_MESSAGE(imd_get_frequency(), payload->frequency, "imd_get_status_canlib_payload should set the frequency correctly");
    TEST_ASSERT_EQUAL_MESSAGE(imd_get_duty_cycle(), payload->duty_cycle, "imd_get_status_canlib_payload should set the duty cycle correctly");
}

void test_imd_get_status_canlib_payload_byte_size(void) {
    size_t byte_size = 0;
    const primary_hv_imd_status_converted_t *payload = imd_get_status_canlib_payload(&byte_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "imd_get_status_canlib_payload should not return NULL");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(primary_hv_imd_status_converted_t), byte_size, "imd_get_status_canlib_payload should set the correct byte size");

    TEST_ASSERT_EQUAL_MESSAGE((primary_hv_imd_status_status)(imd_get_status() + 1U), payload->status, "imd_get_status_canlib_payload should set the status correctly");
    TEST_ASSERT_EQUAL_MESSAGE(imd_get_frequency(), payload->frequency, "imd_get_status_canlib_payload should set the frequency correctly");
    TEST_ASSERT_EQUAL_MESSAGE(imd_get_duty_cycle(), payload->duty_cycle, "imd_get_status_canlib_payload should set the duty cycle correctly");
}

void setUp() {
    RESET_FAKE(start_pwm_conversion);
    imd_init(start_pwm_conversion);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_imd_init_null_pointer);
    RUN_TEST(test_imd_init_ok);
    RUN_TEST(test_imd_update_invalid_data);
    RUN_TEST(test_imd_update_ok);
    RUN_TEST(test_imd_get_status_canlib_payload_null);
    RUN_TEST(test_imd_get_status_canlib_payload_byte_size);

    return UNITY_END();
}
