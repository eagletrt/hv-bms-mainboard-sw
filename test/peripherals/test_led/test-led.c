#include "unity.h"
#include "led.h"
#include <fff.h>
DEFINE_FFF_GLOBALS;

#include <stdint.h>

FAKE_VOID_FUNC(_led_set, const LedId, const LedStatus);
FAKE_VOID_FUNC(_led_toggle, const LedId);

void test_led_init_null_set_callback() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_NULL_POINTER,
        led_init(NULL, _led_toggle),
        "led_init() should fail with NULL set callback");
}

void test_led_init_null_toggle_callback() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_NULL_POINTER,
        led_init(_led_set, NULL),
        "led_init() should fail with NULL toggle callback");
}

void test_led_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_OK,
        led_init(_led_set, _led_toggle),
        "led_init() failed to return LED_OK");
}

void test_led_set_status_invalid_id() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_INVALID_ID,
        led_set_status((LedId)LED_ID_COUNT, LED_STATUS_ON),
        "led_set_status() should fail with invalid LED id");
}

void test_led_set_status_invalid_status() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_INVALID_STATUS,
        led_set_status(LED_ID_1, (LedStatus)LED_STATUS_COUNT),
        "led_set_status() should fail with invalid LED status");
}

void test_led_set_status_ok_calls_set_callback() {
    RESET_FAKE(_led_set);
    RESET_FAKE(_led_toggle);
    FFF_RESET_HISTORY();

    LedReturnCode rc = led_set_status(LED_ID_2, LED_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(LED_OK, rc, "led_set_status() should return LED_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, _led_set_fake.call_count, "Set callback should be called once");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_2, _led_set_fake.arg0_val, "Set callback LED id mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(LED_STATUS_ON, _led_set_fake.arg1_val, "Set callback status mismatch");
}

void test_led_toggle_status_invalid_id() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_INVALID_ID,
        led_toggle_status((LedId)LED_ID_COUNT),
        "led_toggle_status() should fail with invalid LED id");
}

void test_led_toggle_status_ok_calls_toggle_callback() {
    RESET_FAKE(_led_set);
    RESET_FAKE(_led_toggle);
    FFF_RESET_HISTORY();

    LedReturnCode rc = led_toggle_status(LED_ID_1);

    TEST_ASSERT_EQUAL_MESSAGE(LED_OK, rc, "led_toggle_status() should return LED_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, _led_toggle_fake.call_count, "Toggle callback should be called once");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_1, _led_toggle_fake.arg0_val, "Toggle callback LED id mismatch");
}

#ifdef LED_TESTS

void setUp(void) {
    RESET_FAKE(_led_set);
    RESET_FAKE(_led_toggle);
    FFF_RESET_HISTORY();

    (void)led_init(_led_set, _led_toggle);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_led_init_null_set_callback);
    RUN_TEST(test_led_init_null_toggle_callback);
    RUN_TEST(test_led_init_ok);
    RUN_TEST(test_led_set_status_invalid_id);
    RUN_TEST(test_led_set_status_invalid_status);
    RUN_TEST(test_led_set_status_ok_calls_set_callback);
    RUN_TEST(test_led_toggle_status_invalid_id);
    RUN_TEST(test_led_toggle_status_ok_calls_toggle_callback);
    return UNITY_END();
}

#endif // LED_TESTS
