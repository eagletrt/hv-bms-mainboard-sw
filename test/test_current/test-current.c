/*!
 * \file test_volt.c
 * \date 2026-05-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Test functions for the voltage module
 */

#include "unity.h"
#include "current-api.h"
#include <stddef.h>
#include "internal-voltage-api.h"

extern struct CurrentHandler current_api_handler;
extern struct InternalVoltageHandler internal_volt_handler;

void test_current_api_init_clears_struct(void) {
    current_api_handler.current = 10.f;
    current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.current = 10.f;
    current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.power = 10.f;

    enum CurrentReturnCode rc = current_api_init();

    TEST_ASSERT_EQUAL_MESSAGE(CURRENT_RC_OK, rc, "current_api_init() failed to return CURRENT_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(0, current_api_handler.current, "Expected current value not found");
    TEST_ASSERT_EQUAL_MESSAGE(0, current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.current, "Expected current payload value not found");
    TEST_ASSERT_EQUAL_MESSAGE(0, current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.power, "Expected power payload value not found");
}

void test_current_api_get_current(void) {
    current_api_handler.current = 12.5f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 12.5f, current_api_get_current(), "Expected current value not found");
}

void test_current_api_set_current(void) {
    current_api_set_current(7.5f);
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 7.5f, current_api_handler.current, "Expected current value not stored");
}

void test_current_api_set_current_restarts_timed_out_watchdog(void) {
    watchdog_restart(&current_api_handler.sensor_wdg);
    current_api_handler.sensor_wdg.timed_out = true;

    current_api_set_current(3.f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 3.f, current_api_handler.current, "Expected current value not stored");
    TEST_ASSERT_TRUE_MESSAGE(current_api_handler.sensor_wdg.running, "Sensor watchdog should be restarted after a timeout");
    TEST_ASSERT_FALSE_MESSAGE(current_api_handler.sensor_wdg.timed_out, "Sensor watchdog timeout flag should be cleared after the restart");
}

void test_current_api_get_power(void) {
    current_api_handler.current = 10.f;
    internal_volt_handler.ts = 400.f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 4.f, current_api_get_power(), "Expected power value not found");
}

void test_current_api_start_sensor_communication_watchdog(void) {
    enum WatchdogReturnCode rc = current_api_start_sensor_communication_watchdog();

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, rc, "Expected the watchdog to start");
    TEST_ASSERT_TRUE_MESSAGE(current_api_handler.sensor_wdg.running, "Sensor watchdog should be running");
}

void test_current_api_get_canlib_payload(void) {
    size_t byte_size = 0U;
    current_api_handler.current = 15.f;
    internal_volt_handler.ts = 400.f;

    const union CanPrimaryMessages *payload = current_api_get_canlib_payload(&byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&current_api_handler.libcan_message_current, payload, "Expected the handler message");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(can_primary_byte_size_tsacmainboardcurrentinfo, byte_size, "Expected byte size not found");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 15.f, payload->tsacmainboardcurrentinfo.current, "Expected current value not found in payload");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, current_api_get_power(), payload->tsacmainboardcurrentinfo.power, "Expected power value not found in payload");
}

void test_current_api_get_canlib_payload_null_byte_size(void) {
    const union CanPrimaryMessages *payload = current_api_get_canlib_payload(NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
}

void setUp() {
    current_api_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_current_api_init_clears_struct);
    RUN_TEST(test_current_api_get_current);
    RUN_TEST(test_current_api_set_current);
    RUN_TEST(test_current_api_set_current_restarts_timed_out_watchdog);
    RUN_TEST(test_current_api_get_power);
    RUN_TEST(test_current_api_start_sensor_communication_watchdog);
    RUN_TEST(test_current_api_get_canlib_payload);
    RUN_TEST(test_current_api_get_canlib_payload_null_byte_size);

    return UNITY_END();
}
