#include "unity.h"
#include "cooling-temp.h"

#include <math.h>
#include <string.h>

extern _CoolingTempHandler hcoolingtemp;
extern celsius_t _cooling_temp_volt_to_celsius(volt_t value);

static celsius_t _expected_cooling_temp_from_volt(volt_t value) {
    if (value < COOLING_TEMP_MIN_LIMIT_V)
        value = COOLING_TEMP_MIN_LIMIT_V;
    if (value > COOLING_TEMP_MAX_LIMIT_V)
        value = COOLING_TEMP_MAX_LIMIT_V;

    const double v = value;
    const double v2 = v * v;
    const double v3 = v2 * v;
    const double v4 = v2 * v2;
    const double v5 = v4 * v;
    const double v6 = v3 * v3;

    return COOLING_TEMP_COEFF_0 +
           COOLING_TEMP_COEFF_1 * v +
           COOLING_TEMP_COEFF_2 * v2 +
           COOLING_TEMP_COEFF_3 * v3 +
           COOLING_TEMP_COEFF_4 * v4 +
           COOLING_TEMP_COEFF_5 * v5 +
           COOLING_TEMP_COEFF_6 * v6;
}

static void _fill_all_temps(const celsius_t value) {
    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        hcoolingtemp.temperatures[i] = value;
    }
}

void test_cooling_temp_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_OK, cooling_temp_init(), "cooling_temp_init() should return COOLING_TEMP_OK");
}

void test_cooling_temp_init_zeroes_temperatures(void) {
    _fill_all_temps(42.0f);

    (void)cooling_temp_init();

    for (size_t i = 0U; i < COOLING_TEMP_COUNT; ++i) {
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.0f, hcoolingtemp.temperatures[i], "Temperature should be zero after init");
    }
}

void test_cooling_temp_get_values_pointer(void) {
    const cooling_temp_t *values = cooling_temp_get_values();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hcoolingtemp.temperatures, values, "cooling_temp_get_values() pointer mismatch");
}

void test_cooling_temp_update_value_ok(void) {
    const size_t idx = COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3;

    TEST_ASSERT_EQUAL_MESSAGE(COOLING_TEMP_OK, cooling_temp_update_value(idx, 23.5f), "Valid update should return COOLING_TEMP_OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 23.5f, hcoolingtemp.temperatures[idx], "Updated value mismatch");
}

void test_cooling_temp_update_value_out_of_bounds(void) {
    hcoolingtemp.temperatures[0] = 11.0f;

    TEST_ASSERT_EQUAL_MESSAGE(
        COOLING_TEMP_OUT_OF_BOUNDS,
        cooling_temp_update_value(COOLING_TEMP_COUNT, 33.3f),
        "Invalid index should return COOLING_TEMP_OUT_OF_BOUNDS");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 11.0f, hcoolingtemp.temperatures[0], "Out-of-bounds update should not alter valid entries");
}

void test_cooling_temp_volt_to_celsius_nominal(void) {
    const volt_t in = 1.5f;
    const celsius_t expected = _expected_cooling_temp_from_volt(in);
    const celsius_t actual = _cooling_temp_volt_to_celsius(in);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected, actual, "Polynomial conversion mismatch");
}

void test_cooling_temp_volt_to_celsius_clamp_low(void) {
    const celsius_t expected = _expected_cooling_temp_from_volt(COOLING_TEMP_MIN_LIMIT_V);
    const celsius_t actual = _cooling_temp_volt_to_celsius(-99.0f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected, actual, "Low clamp conversion mismatch");
}

void test_cooling_temp_volt_to_celsius_clamp_high(void) {
    const celsius_t expected = _expected_cooling_temp_from_volt(COOLING_TEMP_MAX_LIMIT_V);
    const celsius_t actual = _cooling_temp_volt_to_celsius(99.0f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected, actual, "High clamp conversion mismatch");
}

void test_cooling_temp_notify_conversion_complete_updates_value(void) {
    const size_t idx = COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE;
    const volt_t raw = 1.8f;
    const celsius_t expected = _expected_cooling_temp_from_volt(raw);

    TEST_ASSERT_EQUAL_MESSAGE(
        COOLING_TEMP_OK,
        cooling_temp_notify_conversion_complete(idx, raw),
        "notify_conversion_complete should return COOLING_TEMP_OK");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected, hcoolingtemp.temperatures[idx], "Converted temperature mismatch");
}

void test_cooling_temp_notify_conversion_complete_invalid_index_current_behavior(void) {
    hcoolingtemp.temperatures[0] = 7.0f;

    TEST_ASSERT_EQUAL_MESSAGE(
        COOLING_TEMP_OK,
        cooling_temp_notify_conversion_complete(COOLING_TEMP_COUNT, 1.2f),
        "Current implementation returns COOLING_TEMP_OK even for invalid index");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 7.0f, hcoolingtemp.temperatures[0], "Invalid index should not change valid entries");
}

void test_cooling_temp_get_min_max_sum_avg(void) {
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE] = 10.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1] = 20.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2] = 30.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3] = 40.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4] = 50.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5] = 15.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6] = 25.0f;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 10.0f, cooling_temp_get_min(), "Min mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 50.0f, cooling_temp_get_max(), "Max mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 190.0f, cooling_temp_get_sum(), "Sum mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, (190.0f / 7.0f), cooling_temp_get_avg(), "Avg mismatch");
}

void test_cooling_temp_get_temperatures_canlib_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;

    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_INLET_LIQUID_TEMPERATURE] = 11.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_1] = 21.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_2] = 22.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_3] = 23.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_4] = 24.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_5] = 25.0f;
    hcoolingtemp.temperatures[COOLING_TEMP_INDEX_OUTLET_LIQUID_TEMPERATURE_6] = 26.0f;

    primary_hv_cooling_temperature_converted_t *payload = cooling_temp_get_temperatures_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hcoolingtemp.cooling_temp_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hcoolingtemp.cooling_temp_can_payload), (uint32_t)byte_size, "Payload size mismatch");

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 11.0f, payload->inlet);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 21.0f, payload->outlet_0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 22.0f, payload->outlet_1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 23.0f, payload->outlet_2);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 24.0f, payload->outlet_3);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.0f, payload->outlet_4);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 26.0f, payload->outlet_5);
}

void test_cooling_temp_get_temperatures_canlib_payload_null_size_pointer(void) {
    primary_hv_cooling_temperature_converted_t *payload = cooling_temp_get_temperatures_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hcoolingtemp.cooling_temp_can_payload, payload, "Payload pointer should be valid when byte_size is NULL");
}

#ifdef COOLING_TEMP_TESTS

void setUp(void) {
    (void)cooling_temp_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_cooling_temp_init_ok);
    RUN_TEST(test_cooling_temp_init_zeroes_temperatures);
    RUN_TEST(test_cooling_temp_get_values_pointer);
    RUN_TEST(test_cooling_temp_update_value_ok);
    RUN_TEST(test_cooling_temp_update_value_out_of_bounds);
    RUN_TEST(test_cooling_temp_volt_to_celsius_nominal);
    RUN_TEST(test_cooling_temp_volt_to_celsius_clamp_low);
    RUN_TEST(test_cooling_temp_volt_to_celsius_clamp_high);
    RUN_TEST(test_cooling_temp_notify_conversion_complete_updates_value);
    RUN_TEST(test_cooling_temp_notify_conversion_complete_invalid_index_current_behavior);
    RUN_TEST(test_cooling_temp_get_min_max_sum_avg);
    RUN_TEST(test_cooling_temp_get_temperatures_canlib_payload_pointer_size_and_content);
    RUN_TEST(test_cooling_temp_get_temperatures_canlib_payload_null_size_pointer);
    return UNITY_END();
}

#endif // COOLING_TEMP_TESTS