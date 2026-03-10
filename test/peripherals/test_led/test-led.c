#include "unity.h"
#include "led.h"

#include <stdint.h>

static uint32_t g_set_called = 0U;
static uint32_t g_toggle_called = 0U;
static LedId g_last_set_id = (LedId)0U;
static LedStatus g_last_set_status = (LedStatus)0U;
static LedId g_last_toggle_id = (LedId)0U;

static void _led_set_spy(const LedId led, const LedStatus state) {
    g_set_called++;
    g_last_set_id = led;
    g_last_set_status = state;
}

static void _led_toggle_spy(const LedId led) {
    g_toggle_called++;
    g_last_toggle_id = led;
}

static void _led_reset_spies() {
    g_set_called = 0U;
    g_toggle_called = 0U;
    g_last_set_id = (LedId)0U;
    g_last_set_status = (LedStatus)0U;
    g_last_toggle_id = (LedId)0U;
}

void test_led_init_null_set_callback() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_NULL_POINTER,
        led_init(NULL, _led_toggle_spy),
        "led_init() should fail with NULL set callback");
}

void test_led_init_null_toggle_callback() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_NULL_POINTER,
        led_init(_led_set_spy, NULL),
        "led_init() should fail with NULL toggle callback");
}

void test_led_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_OK,
        led_init(_led_set_spy, _led_toggle_spy),
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
    _led_reset_spies();

    LedReturnCode rc = led_set_status(LED_ID_2, LED_STATUS_ON);

    TEST_ASSERT_EQUAL_MESSAGE(LED_OK, rc, "led_set_status() should return LED_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, g_set_called, "Set callback should be called once");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_2, g_last_set_id, "Set callback LED id mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(LED_STATUS_ON, g_last_set_status, "Set callback status mismatch");
}

void test_led_toggle_status_invalid_id() {
    TEST_ASSERT_EQUAL_MESSAGE(
        LED_INVALID_ID,
        led_toggle_status((LedId)LED_ID_COUNT),
        "led_toggle_status() should fail with invalid LED id");
}

void test_led_toggle_status_ok_calls_toggle_callback() {
    _led_reset_spies();

    LedReturnCode rc = led_toggle_status(LED_ID_1);

    TEST_ASSERT_EQUAL_MESSAGE(LED_OK, rc, "led_toggle_status() should return LED_OK");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, g_toggle_called, "Toggle callback should be called once");
    TEST_ASSERT_EQUAL_MESSAGE(LED_ID_1, g_last_toggle_id, "Toggle callback LED id mismatch");
}

#ifdef LED_TESTS

void setUp(void) {
    _led_reset_spies();
    (void)led_init(_led_set_spy, _led_toggle_spy);
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
