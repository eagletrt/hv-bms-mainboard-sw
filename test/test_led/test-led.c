/*!
 * \file test-led.c
 * \date 2026-06-15
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief LED tests
 */

#include "led.h"
#include "unity.h"
#include "led-api.h"

#include <fff.h>
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(led_set_state, const enum LedId, const enum LedStatus);
FAKE_VOID_FUNC(led_toggle_state, const enum LedId);

extern struct LedHandler led_handler;

void test_led_init_ok() {
    enum LedReturnCode rc = led_api_init(led_set_state, led_toggle_state);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_OK, rc, "led_api_init() failed to return LED_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(led_set_state, led_handler.set, "led_api_init() did not properly set the set callback");
    TEST_ASSERT_EQUAL_MESSAGE(led_toggle_state, led_handler.toggle, "led_api_init() did not properly set the toggle callback");
}

void test_led_init_null_pointer_set() {
    enum LedReturnCode rc = led_api_init(NULL, led_toggle_state);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_NULL_POINTER, rc, "led_api_init() should return LED_RC_NULL_POINTER if set callback is NULL");
}

void test_led_init_null_pointer_toggle() {
    enum LedReturnCode rc = led_api_init(led_set_state, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_NULL_POINTER, rc, "led_api_init() should return LED_RC_NULL_POINTER if toggle callback is NULL");
}

void test_led_set_status_ok() {
    enum LedReturnCode rc = led_api_set_status(LED_ID_1, LED_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_OK, rc, "led_api_set_status() failed to return LED_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(1, led_set_state_fake.call_count, "led_api_set_status() did not call the set callback");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_1, led_set_state_fake.arg0_val, "led_api_set_status() called set callback with wrong led_id");
    TEST_ASSERT_EQUAL_MESSAGE(LED_STATUS_ON, led_set_state_fake.arg1_val, "led_api_set_status() called set callback with wrong status");
}

void test_led_set_status_invalid_id() {
    enum LedReturnCode rc = led_api_set_status(LED_ID_COUNT, LED_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_INVALID_ID, rc, "led_api_set_status() should return LED_RC_INVALID_ID for invalid led_id");
}

void test_led_set_status_invalid_status() {
    enum LedReturnCode rc = led_api_set_status(LED_ID_1, LED_STATUS_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_INVALID_STATUS, rc, "led_api_set_status() should return LED_RC_INVALID_STATUS for invalid status");
}

void test_led_toggle_status_ok() {
    enum LedReturnCode rc = led_api_toggle_status(LED_ID_1);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_OK, rc, "led_api_toggle_status() failed to return LED_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(1, led_toggle_state_fake.call_count, "led_api_toggle_status() did not call the toggle callback");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_1, led_toggle_state_fake.arg0_val, "led_api_toggle_status() called toggle callback with wrong led_id");
}

void test_led_toggle_status_invalid_id() {
    enum LedReturnCode rc = led_api_toggle_status(LED_ID_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(LED_RC_INVALID_ID, rc, "led_api_toggle_status() should return LED_RC_INVALID_ID for invalid led_id");
}

void setUp() {
    led_set_state_fake.custom_fake = NULL;
    led_toggle_state_fake.custom_fake = NULL;
    RESET_FAKE(led_set_state);
    RESET_FAKE(led_toggle_state);

    led_api_init(led_set_state, led_toggle_state);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_led_init_ok);
    RUN_TEST(test_led_init_null_pointer_set);
    RUN_TEST(test_led_init_null_pointer_toggle);
    RUN_TEST(test_led_set_status_ok);
    RUN_TEST(test_led_set_status_invalid_id);
    RUN_TEST(test_led_set_status_invalid_status);
    RUN_TEST(test_led_toggle_status_ok);
    RUN_TEST(test_led_toggle_status_invalid_id);

    return UNITY_END();
}
