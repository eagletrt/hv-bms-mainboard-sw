/*!
 * \file test_cooling_temp.c
 * \date 2024-09-24
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Test functions for the cooling-temp module
 */

#include "unity.h"
#include "cooling-temp-api.h"
#include "error.h"
#include "string.h"

extern struct CoolingTempHandler cooling_temp_handler;

celsius_t prv_cooling_temp_volt_to_celsius(volt_t value);

void test_prv_cooling_temp_volt_to_celsius_midrange() {
    volt_t input = 2.0f;
    celsius_t expected = 14.02773f;

    celsius_t result = prv_cooling_temp_volt_to_celsius(input);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, result, "prv_cooling_temp_volt_to_celsius() failed for midrange input 2.0V");
}

void test_prv_cooling_temp_volt_to_celsius_at_min_limit() {
    volt_t input = COOLING_TEMP_MIN_LIMIT_V;
    celsius_t expected = COOLING_TEMP_COEFF_0;

    celsius_t result = prv_cooling_temp_volt_to_celsius(input);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, result, "prv_cooling_temp_volt_to_celsius() failed at minimum limit");
}

void test_prv_cooling_temp_volt_to_celsius_below_min_clamped() {
    volt_t input = COOLING_TEMP_MIN_LIMIT_V - 1.0f;
    volt_t clamped = COOLING_TEMP_MIN_LIMIT_V;
    celsius_t expected = prv_cooling_temp_volt_to_celsius(clamped);

    celsius_t result = prv_cooling_temp_volt_to_celsius(input);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, result, "prv_cooling_temp_volt_to_celsius() should clamp values below minimum");
}

void test_prv_cooling_temp_volt_to_celsius_above_max_clamped() {
    volt_t input = COOLING_TEMP_MAX_LIMIT_V + 1.0f;
    volt_t clamped = COOLING_TEMP_MAX_LIMIT_V;
    celsius_t expected = prv_cooling_temp_volt_to_celsius(clamped);

    celsius_t result = prv_cooling_temp_volt_to_celsius(input);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, result, "prv_cooling_temp_volt_to_celsius() should clamp values above maximum");
}

void test_cooling_temp_init_ok() {
    memset(&cooling_temp_handler, 0xFFU, sizeof(cooling_temp_handler));

    const enum CoolingTempReturnCode rc = cooling_temp_api_init();

    struct CoolingTempHandler expected = { 0 };
    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_RC_OK, rc, "cooling_temp_api_init() failed to return COOLING_TEMP_RC_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, &cooling_temp_handler, sizeof(expected), "cooling_temp_api_init() did not zero-initialize the handler");
}

void test_cooling_temp_notify_conversion_complete_ok() {
    volt_t raw = 2.0f;
    celsius_t expected = prv_cooling_temp_volt_to_celsius(raw);

    const enum CoolingTempReturnCode rc = cooling_temp_api_notify_conversion_complete(0U, raw);

    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_RC_OK, rc, "cooling_temp_api_notify_conversion_complete() failed to return COOLING_TEMP_RC_OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, cooling_temp_handler.temperatures[0U], "cooling_temp_api_notify_conversion_complete() stored incorrect converted temperature");
}

void test_cooling_temp_notify_conversion_complete_out_of_bounds() {
    const enum CoolingTempReturnCode rc = cooling_temp_api_notify_conversion_complete(COOLING_TEMP_COUNT, 2.0f);

    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_RC_OUT_OF_BOUNDS, rc, "cooling_temp_api_notify_conversion_complete() should return COOLING_TEMP_RC_OUT_OF_BOUNDS for invalid index");
}

void test_cooling_temp_update_value_ok() {
    celsius_t expected = 37.5f;

    const enum CoolingTempReturnCode rc = cooling_temp_api_update_value(0U, expected);

    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_RC_OK, rc, "cooling_temp_api_update_value() failed to return COOLING_TEMP_RC_OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, expected, cooling_temp_handler.temperatures[0U], "cooling_temp_api_update_value() did not store the value correctly");
}

void test_cooling_temp_update_value_out_of_bounds() {
    const enum CoolingTempReturnCode rc = cooling_temp_api_update_value(COOLING_TEMP_COUNT, 25.0f);

    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_RC_OUT_OF_BOUNDS, rc, "cooling_temp_api_update_value() should return COOLING_TEMP_RC_OUT_OF_BOUNDS for invalid index");
}

void test_cooling_temp_get_min() {
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        cooling_temp_handler.temperatures[i] = 50.0f;
    }
    cooling_temp_handler.temperatures[2U] = 5.0f;

    const celsius_t min = cooling_temp_api_get_min();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 5.0f, min, "cooling_temp_api_get_min() failed to return the minimum temperature");
}

void test_cooling_temp_get_max() {
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        cooling_temp_handler.temperatures[i] = 0.0f;
    }
    cooling_temp_handler.temperatures[4U] = 80.0f;

    const celsius_t max = cooling_temp_api_get_max();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 80.0f, max, "cooling_temp_api_get_max() failed to return the maximum temperature");
}

void test_cooling_temp_get_sum() {
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        cooling_temp_handler.temperatures[i] = 1.0f;
    }

    const celsius_t sum = cooling_temp_api_get_sum();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.1f, (float)COOLING_TEMP_COUNT, sum, "cooling_temp_api_get_sum() failed");
}

void test_cooling_temp_get_avg() {
    // If you think there is a better way to test this please let me know
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; i++) {
        cooling_temp_handler.temperatures[i] = 10.0f;
    }

    const celsius_t avg = cooling_temp_api_get_avg();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, 10.0f, avg, "cooling_temp_api_get_avg() failed");
}

void test_cooling_temp_get_temperatures_canlib_payload_null_size() {
    primary_hv_cooling_temperature_converted_t *payload = cooling_temp_api_get_temperatures_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "cooling_temp_api_get_temperatures_canlib_payload() should not return NULL with NULL size pointer");
}

void test_cooling_temp_get_temperatures_canlib_payload_size() {
    size_t size = 0U;

    (void)cooling_temp_api_get_temperatures_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(cooling_temp_handler.cooling_temp_can_payload), size, "cooling_temp_api_get_temperatures_canlib_payload() returned incorrect byte size");
}

void test_cooling_temp_get_temperatures_canlib_payload_values() {
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE] = 25.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1] = 30.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2] = 31.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3] = 32.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4] = 33.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5] = 34.0f;
    cooling_temp_handler.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6] = 35.0f;

    primary_hv_cooling_temperature_converted_t expected = {
        .inlet = 25.0f,
        .outlet_0 = 30.0f,
        .outlet_1 = 31.0f,
        .outlet_2 = 32.0f,
        .outlet_3 = 33.0f,
        .outlet_4 = 34.0f,
        .outlet_5 = 35.0f,
    };

    size_t size = 0U;
    primary_hv_cooling_temperature_converted_t *payload = cooling_temp_api_get_temperatures_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "cooling_temp_api_get_temperatures_canlib_payload() returned incorrect payload contents");
}

void setUp() {
    cooling_temp_api_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_prv_cooling_temp_volt_to_celsius_midrange);
    RUN_TEST(test_prv_cooling_temp_volt_to_celsius_at_min_limit);
    RUN_TEST(test_prv_cooling_temp_volt_to_celsius_below_min_clamped);
    RUN_TEST(test_prv_cooling_temp_volt_to_celsius_above_max_clamped);
    RUN_TEST(test_cooling_temp_init_ok);
    RUN_TEST(test_cooling_temp_notify_conversion_complete_ok);
    RUN_TEST(test_cooling_temp_notify_conversion_complete_out_of_bounds);
    RUN_TEST(test_cooling_temp_update_value_ok);
    RUN_TEST(test_cooling_temp_update_value_out_of_bounds);
    RUN_TEST(test_cooling_temp_get_min);
    RUN_TEST(test_cooling_temp_get_max);
    RUN_TEST(test_cooling_temp_get_sum);
    RUN_TEST(test_cooling_temp_get_avg);
    RUN_TEST(test_cooling_temp_get_temperatures_canlib_payload_null_size);
    RUN_TEST(test_cooling_temp_get_temperatures_canlib_payload_size);
    RUN_TEST(test_cooling_temp_get_temperatures_canlib_payload_values);
    return UNITY_END();
}