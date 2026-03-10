#include "unity.h"
#include "current.h"
#include "timebase.h"
#include "internal-voltage.h"
#include "error.h"
#include <string.h>

extern _CurrentHandler hcurrent;
extern _InternalVoltageHandler hvolt_int;

void test_current_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(CURRENT_OK, current_init(), "current_init() failed to return CURRENT_OK");
}

void test_current_init_zero_current() {
    current_init();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, current_get_current(), "Current should initialize to 0.0 A");
}

void test_current_init_zero_power() {
    current_init();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, current_get_power(), "Power should initialize to 0.0 kW");
}

void test_current_get_current_value() {
    hcurrent.current = 12.34f;
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(12.34f, current_get_current(), "current_get_current() returned unexpected value");
}

void test_current_get_power_value() {
    hcurrent.current = 10.0f;
    float expected = 10.0f * internal_voltage_get_ts() * 0.001f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, expected, current_get_power(), "current_get_power() returned unexpected value");
}

void test_current_start_sensor_communication_watchdog_ok() {
    current_init();
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_OK, current_start_sensor_communication_watchdog(), "Watchdog start failed");
}

void test_current_handle_null_payload_no_change() {
    hcurrent.current = 5.0f;
    current_handle(NULL);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(5.0f, hcurrent.current, "Current should not change when payload is NULL");
}

void test_current_handle_updates_current() {
    bms_ivt_msg_result_i_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.ivt_result_i = 12345; /* mA */

    current_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 12.345f, hcurrent.current, "Payload conversion mA->A is incorrect");
}

void test_current_get_current_canlib_payload() {
    size_t byte_size = 0U;
    hcurrent.current = 7.89f;

    primary_hv_current_converted_t *payload = current_get_current_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(&hcurrent.current_can_payload, payload, "Returned payload pointer mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(hcurrent.current_can_payload), byte_size, "Returned payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 7.89f, payload->current, "Payload current value mismatch");
}

void test_current_get_power_canlib_payload() {
    size_t byte_size = 0U;
    hcurrent.current = 8.0f;

    float expected = hcurrent.current * internal_voltage_get_ts() * 0.001f;
    primary_hv_power_converted_t *payload = current_get_power_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(&hcurrent.power_can_payload, payload, "Returned payload pointer mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(hcurrent.power_can_payload), byte_size, "Returned payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, expected, payload->power, "Payload power value mismatch");
}

void test_current_error_over_current() {
    bms_ivt_msg_result_i_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.ivt_result_i = (int32_t)((CURRENT_MAX_A + 1.0f) * 1000.0f); /* mA */
    current_handle(&payload);
    TEST_ASSERT_TRUE_MESSAGE(error_get_expired() == 0U, "Over current error should NOT be set");
    current_handle(&payload);
    TEST_ASSERT_TRUE_MESSAGE(error_get_expired() > 0U, "Over current error should be set");
}

void test_current_error_over_power() {
    bms_ivt_msg_result_i_t payload;
    memset(&payload, 0, sizeof(payload));
    hvolt_int.ts = 400.0f;                                                                                            /* V */
    payload.ivt_result_i = (int32_t)((CURRENT_MAX_POWER_KW / (internal_voltage_get_ts() * 0.001f) + 1.0f) * 1000.0f); /* mA */
    current_handle(&payload);
    TEST_ASSERT_TRUE_MESSAGE(error_get_expired() == 0U, "Over power error should NOT be set");
    current_handle(&payload);
    TEST_ASSERT_TRUE_MESSAGE(error_get_expired() > 0U, "Over power error should be set");
}

#ifdef CURRENT_TESTS

void setUp() {
    timebase_init(500U);
    current_init();
    error_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_current_init_ok);
    RUN_TEST(test_current_init_zero_current);
    RUN_TEST(test_current_init_zero_power);
    RUN_TEST(test_current_get_current_value);
    RUN_TEST(test_current_get_power_value);
    RUN_TEST(test_current_start_sensor_communication_watchdog_ok);
    RUN_TEST(test_current_handle_null_payload_no_change);
    RUN_TEST(test_current_handle_updates_current);
    RUN_TEST(test_current_get_current_canlib_payload);
    RUN_TEST(test_current_get_power_canlib_payload);
    RUN_TEST(test_current_error_over_current);
    RUN_TEST(test_current_error_over_power);
    return UNITY_END();
}

#endif /* CURRENT_TESTS */