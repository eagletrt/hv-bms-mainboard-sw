#include "unity.h"
#include "volt.h"

#include <string.h>

extern _VoltHandler hvolt;

static void _volt_fill_all(const volt_t value) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
            hvolt.voltages[id][i] = value;
        }
    }
}

void test_volt_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(VOLT_OK, volt_init(), "volt_init() failed to return VOLT_OK");
}

void test_volt_init_values_are_max(void) {
    (void)volt_init();
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
            TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
                0.0001f,
                VOLT_MAX_V,
                hvolt.voltages[id][i],
                "Voltage cell not initialized to VOLT_MAX_V");
        }
    }
}

void test_volt_get_values_pointer(void) {
    const cells_voltage_t *values = volt_get_values();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hvolt.voltages, values, "volt_get_values() pointer mismatch");
}

void test_volt_get_min_max_sum_avg_uniform(void) {
    const volt_t v = 3.7f;
    const volt_t expected_sum = v * (volt_t)CELLBOARD_SERIES_COUNT;

    _volt_fill_all(v);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, volt_get_min(), "volt_get_min() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, volt_get_max(), "volt_get_max() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected_sum, volt_get_sum(), "volt_get_sum() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, volt_get_avg(), "volt_get_avg() mismatch");
}

void test_volt_cells_voltage_handle_null_payload(void) {
    hvolt.voltages[0][0] = 3.33f;

    volt_cells_voltage_handle(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 3.33f, hvolt.voltages[0][0], "NULL payload should not change values");
}

void test_volt_cells_voltage_handle_invalid_cellboard_id(void) {
    bms_cellboard_cells_voltage_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    hvolt.voltages[0][0] = 3.33f;
    payload.cellboard_id = (uint8_t)CELLBOARD_ID_COUNT; /* invalid */
    payload.offset = 0U;
    payload.voltage_0 = 1.0f;
    payload.voltage_1 = 2.0f;
    payload.voltage_2 = 3.0f;

    volt_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 3.33f, hvolt.voltages[0][0], "Invalid cellboard_id should be ignored");
}

void test_volt_cells_voltage_handle_invalid_offset(void) {
    bms_cellboard_cells_voltage_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    const size_t last = CELLBOARD_SEGMENT_SERIES_COUNT - 1U;
    hvolt.voltages[0][last] = 4.44f;

    payload.cellboard_id = 0U;
    payload.offset = CELLBOARD_SEGMENT_SERIES_COUNT - 2U; /* offset + 3 out of bounds */
    payload.voltage_0 = 1.0f;
    payload.voltage_1 = 2.0f;
    payload.voltage_2 = 3.0f;

    volt_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 4.44f, hvolt.voltages[0][last], "Out-of-bounds offset should be ignored");
}

void test_volt_cells_voltage_handle_valid_updates_three_values(void) {
    bms_cellboard_cells_voltage_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    payload.cellboard_id = 0U;
    payload.offset = 6U;
    payload.voltage_0 = 3.10f;
    payload.voltage_1 = 3.20f;
    payload.voltage_2 = 3.30f;

    volt_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.10f, hvolt.voltages[0][6U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.20f, hvolt.voltages[0][7U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.30f, hvolt.voltages[0][8U]);
}

void test_volt_get_cells_voltage_canlib_payload_pointer_size_content_and_increment(void) {
    size_t byte_size = 0U;

    hvolt.cellboard_id = 0U;
    hvolt.offset = 0U;

    hvolt.voltages[0][0] = 3.01f;
    hvolt.voltages[0][1] = 3.02f;
    hvolt.voltages[0][2] = 3.03f;
    hvolt.voltages[0][3] = 3.04f;
    hvolt.voltages[0][4] = 3.05f;
    hvolt.voltages[0][5] = 3.06f;

    primary_hv_cells_voltage_converted_t *p0 = volt_get_cells_voltage_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hvolt.volt_can_payload, p0, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hvolt.volt_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)p0->cellboard_id, "cellboard_id mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)p0->offset, "offset mismatch");
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.01f, p0->voltage_0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.02f, p0->voltage_1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.03f, p0->voltage_2);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3U, (uint32_t)hvolt.offset, "Internal offset should increment by 3");

    primary_hv_cells_voltage_converted_t *p1 = volt_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_PTR(&hvolt.volt_can_payload, p1);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3U, (uint32_t)p1->offset, "Second frame offset mismatch");
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.04f, p1->voltage_0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.05f, p1->voltage_1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.06f, p1->voltage_2);
}

void test_volt_get_cells_voltage_canlib_payload_wrap_indices(void) {
    hvolt.cellboard_id = CELLBOARD_ID_COUNT - 1U;
    hvolt.offset = CELLBOARD_SEGMENT_SERIES_COUNT - 3U;

    (void)volt_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)hvolt.offset, "Offset should wrap to 0");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)hvolt.cellboard_id, "Cellboard ID should wrap to 0");
}

void test_volt_get_cells_voltage_stats_canlib_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;

    _volt_fill_all(4.00f);
    hvolt.voltages[0][0] = 3.10f; /* min */
    hvolt.voltages[0][1] = 4.20f; /* max */

    const volt_t expected_sum =
        3.10f + 4.20f + ((volt_t)CELLBOARD_SERIES_COUNT - 2.0f) * 4.00f;
    const volt_t expected_avg = expected_sum / (volt_t)CELLBOARD_SERIES_COUNT;

    primary_hv_cells_voltage_stats_converted_t *payload = volt_get_cells_voltage_stats_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hvolt.volt_stats_can_payload, payload, "Stats payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hvolt.volt_stats_can_payload), (uint32_t)byte_size, "Stats payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 4.20f, payload->max);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.10f, payload->min);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.10f, payload->delta);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected_avg, payload->avg);
}

#ifdef VOLT_TESTS

void setUp(void) {
    (void)volt_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_volt_init_ok);
    RUN_TEST(test_volt_init_values_are_max);
    RUN_TEST(test_volt_get_values_pointer);
    RUN_TEST(test_volt_get_min_max_sum_avg_uniform);
    RUN_TEST(test_volt_cells_voltage_handle_null_payload);
    RUN_TEST(test_volt_cells_voltage_handle_invalid_cellboard_id);
    RUN_TEST(test_volt_cells_voltage_handle_invalid_offset);
    RUN_TEST(test_volt_cells_voltage_handle_valid_updates_three_values);
    RUN_TEST(test_volt_get_cells_voltage_canlib_payload_pointer_size_content_and_increment);
    RUN_TEST(test_volt_get_cells_voltage_canlib_payload_wrap_indices);
    RUN_TEST(test_volt_get_cells_voltage_stats_canlib_payload_pointer_size_and_content);
    return UNITY_END();
}

#endif // VOLT_TESTS