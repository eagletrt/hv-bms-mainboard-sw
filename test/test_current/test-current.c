/*!
 * \file test_volt.c
 * \date 2026-05-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Test functions for the voltage module
 */

#include "unity.h"
#include "current-api.h"
#include "error.h"
#include <stddef.h>
#include "internal-voltage.h"

extern struct CurrentHandler current_handler;

extern _InternalVoltageHandler internal_volt_handler;

void test_current_get_power(void) {
    current_handler.current = 10.f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 0.0f, current_get_power(), "Expected zero power value");
    internal_volt_handler.ts = 400.f;
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 4.f, current_get_power(), "Expected power value not found");
}

void test_current_handle_null_pointer(void) {
    // If the test doesn't crash, then it means that it works.
    current_handle(NULL);
}

void test_current_handle_sets_current(void) {
    bms_ivt_msg_result_i_t payload = {
        .ivt_result_i = 1000
    };
    current_handle(&payload);
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 1.f, current_handler.current, "Expected current value not found");
}

void test_current_get_current_canlib_payload(void) {
    size_t byte_size = 0U;
    current_handler.current = 15.f;
    primary_hv_current_converted_t *payload = current_get_current_canlib_payload(&byte_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(current_handler.current_can_payload), byte_size, "Expected byte size not found");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, current_handler.current, payload->current, "Expected current value not found in payload");
}

void test_current_get_current_canlib_payload_null_byte_size(void) {
    primary_hv_current_converted_t *payload = current_get_current_canlib_payload(NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
}

void test_current_get_power_canlib_payload(void) {
    size_t byte_size = 0U;
    current_handler.current = 20.f;
    internal_volt_handler.ts = 400.f;
    primary_hv_power_converted_t *payload = current_get_power_canlib_payload(&byte_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(current_handler.power_can_payload), byte_size, "Expected byte size not found");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, current_get_power(), payload->power, "Expected power value not found in payload");
}

void test_current_get_power_canlib_payload_null_byte_size(void) {
    primary_hv_power_converted_t *payload = current_get_power_canlib_payload(NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Expected non null pointer");
}

void setUp() {
    current_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_current_get_power);
    RUN_TEST(test_current_handle_null_pointer);
    RUN_TEST(test_current_handle_sets_current);
    RUN_TEST(test_current_get_current_canlib_payload);
    RUN_TEST(test_current_get_current_canlib_payload_null_byte_size);
    RUN_TEST(test_current_get_power_canlib_payload);
    RUN_TEST(test_current_get_power_canlib_payload_null_byte_size);

    return UNITY_END();
}