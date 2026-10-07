/*!
 * \file test-imd.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief IMD tests
 */

#include "feedback-api.h"
#include "imd-api.h"
#include "unity.h"

#include <fff.h>
DEFINE_FFF_GLOBALS;

extern struct ImdHandler imd_handler;

FAKE_VOID_FUNC(start_pwm_conversion);

void test_imd_init_null_pointer(void) {
    enum ImdReturnCode code = imd_api_init(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_NULL_POINTER, code, "imd_api_init should return IMD_RC_NULL_POINTER when given a NULL pointer");
}

void test_imd_init_ok(void) {
    enum ImdReturnCode code = imd_api_init(start_pwm_conversion);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_OK, code, "imd_api_init should return IMD_RC_OK when given a valid pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(start_pwm_conversion, imd_handler.start, "imd_handler.start should be set to the given callback function");
}

void test_imd_update_invalid_data(void) {
    enum ImdReturnCode code = imd_api_update(1000, 0, 500);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_INVALID_DATA, code, "imd_api_update should return IMD_RC_INVALID_DATA when period_count is 0");
}

void test_imd_update_ok(void) {
    enum ImdReturnCode code = imd_api_update(1000, 100, 50);
    TEST_ASSERT_EQUAL_MESSAGE(IMD_RC_OK, code, "imd_api_update should return IMD_RC_OK when given valid parameters");

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(10.0f, imd_api_get_frequency(), "imd_api_update should set the frequency correctly");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.5f, imd_api_get_duty_cycle(), "imd_api_update should set the duty cycle correctly");
}

static void assert_payload_matches(const union CanPrimaryMessages *payload) {
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "imd_api_get_canlib_payload should not return NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&imd_handler.libcan_message_imd, payload, "imd_api_get_canlib_payload should return the handler message");

    TEST_ASSERT_EQUAL_MESSAGE(imd_api_get_status(), payload->tsacmainboardimd.status, "imd_api_get_canlib_payload should set the status correctly");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(imd_api_get_frequency(), payload->tsacmainboardimd.frequency, "imd_api_get_canlib_payload should set the frequency correctly");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(imd_api_get_duty_cycle(), payload->tsacmainboardimd.dutycycle, "imd_api_get_canlib_payload should set the duty cycle correctly");
    TEST_ASSERT_EQUAL_MESSAGE(feedback_api_get_status(FEEDBACK_ID_IMD_OK) == FEEDBACK_STATUS_HIGH, payload->tsacmainboardimd.ok, "imd_api_get_canlib_payload should set the ok flag correctly");
}

void test_imd_get_canlib_payload_null(void) {
    assert_payload_matches(imd_api_get_canlib_payload(NULL));
}

void test_imd_get_canlib_payload_byte_size(void) {
    size_t byte_size = 0;
    const union CanPrimaryMessages *payload = imd_api_get_canlib_payload(&byte_size);
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsacmainboardimd, byte_size, "imd_api_get_canlib_payload should set the correct byte size");
    assert_payload_matches(payload);
}

void test_imd_get_canlib_payload_after_update(void) {
    TEST_ASSERT_EQUAL(IMD_RC_OK, imd_api_update(1000, 100, 50));
    const union CanPrimaryMessages *payload = imd_api_get_canlib_payload(NULL);
    assert_payload_matches(payload);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(10.0f, payload->tsacmainboardimd.frequency, "payload frequency should reflect the update");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.5f, payload->tsacmainboardimd.dutycycle, "payload duty cycle should reflect the update");
}

void setUp() {
    RESET_FAKE(start_pwm_conversion);
    imd_api_init(start_pwm_conversion);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_imd_init_null_pointer);
    RUN_TEST(test_imd_init_ok);
    RUN_TEST(test_imd_update_invalid_data);
    RUN_TEST(test_imd_update_ok);
    RUN_TEST(test_imd_get_canlib_payload_null);
    RUN_TEST(test_imd_get_canlib_payload_byte_size);
    RUN_TEST(test_imd_get_canlib_payload_after_update);

    return UNITY_END();
}
