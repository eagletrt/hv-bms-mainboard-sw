#include "unity.h"
#include "temp.h"

#include <string.h>

extern _TempHandler htemp;

static void _temp_fill_all(const celsius_t value) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            htemp.temperatures[i][j] = value;
        }
    }
}

void test_temp_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(TEMP_OK, temp_init(), "temp_init() failed to return TEMP_OK");
}

void test_temp_init_indices_zero(void) {
    (void)temp_init();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)htemp.offset, "Offset should initialize to 0");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)htemp.cellboard_id, "Cellboard ID should initialize to 0");
}

void test_temp_get_values_pointer(void) {
    const cells_temp_t *values = temp_get_values();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&htemp.temperatures, values, "temp_get_values() pointer mismatch");
}

void test_temp_get_min_max_sum_avg_uniform(void) {
    const celsius_t v = 10.0f;
    const celsius_t expected_sum = v * (celsius_t)CELLBOARD_TEMP_SENSOR_COUNT;

    _temp_fill_all(v);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, temp_get_min(), "temp_get_min() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, temp_get_max(), "temp_get_max() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, expected_sum, temp_get_sum(), "temp_get_sum() mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, v, temp_get_avg(), "temp_get_avg() mismatch");
}

void test_temp_cells_temperature_handle_null_payload(void) {
    htemp.temperatures[0][0] = 12.0f;
    temp_cells_temperature_handle(NULL);
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 12.0f, htemp.temperatures[0][0], "NULL payload should not modify temperatures");
}

void test_temp_cells_temperature_handle_invalid_cellboard_id(void) {
    bms_cellboard_cells_temperature_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    htemp.temperatures[0][0] = 22.0f;
    payload.cellboard_id = (uint8_t)CELLBOARD_ID_COUNT; /* invalid */
    payload.offset = 0U;
    payload.temperature_0 = 1.0f;
    payload.temperature_1 = 2.0f;
    payload.temperature_2 = 3.0f;
    payload.temperature_3 = 4.0f;

    temp_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 22.0f, htemp.temperatures[0][0], "Invalid cellboard_id should be ignored");
}

void test_temp_cells_temperature_handle_invalid_offset(void) {
    bms_cellboard_cells_temperature_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    htemp.temperatures[0][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - 1U] = 33.0f;

    payload.cellboard_id = 0U;
    payload.offset = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - 3U; /* +4 exceeds segment size */
    payload.temperature_0 = 1.0f;
    payload.temperature_1 = 2.0f;
    payload.temperature_2 = 3.0f;
    payload.temperature_3 = 4.0f;

    temp_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
        0.0001f,
        33.0f,
        htemp.temperatures[0][CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - 1U],
        "Out-of-bounds offset should be ignored");
}

void test_temp_cells_temperature_handle_valid_updates_four_values(void) {
    bms_cellboard_cells_temperature_converted_t payload;
    memset(&payload, 0, sizeof(payload));

    payload.cellboard_id = 0U;
    payload.offset = 8U;
    payload.temperature_0 = 11.1f;
    payload.temperature_1 = 22.2f;
    payload.temperature_2 = 33.3f;
    payload.temperature_3 = 44.4f;

    temp_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 11.1f, htemp.temperatures[0][8U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 22.2f, htemp.temperatures[0][9U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 33.3f, htemp.temperatures[0][10U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 44.4f, htemp.temperatures[0][11U]);
}

void test_temp_get_cells_temperature_canlib_payload_pointer_size_content_and_increment(void) {
    size_t byte_size = 0U;

    htemp.temperatures[0][0] = 1.0f;
    htemp.temperatures[0][1] = 2.0f;
    htemp.temperatures[0][2] = 3.0f;
    htemp.temperatures[0][3] = 4.0f;
    htemp.temperatures[0][4] = 5.0f;
    htemp.temperatures[0][5] = 6.0f;
    htemp.temperatures[0][6] = 7.0f;
    htemp.temperatures[0][7] = 8.0f;

    primary_hv_cells_temperature_converted_t *p0 = temp_get_cells_temperature_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&htemp.temp_can_payload, p0, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(htemp.temp_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)p0->cellboard_id, "cellboard_id mismatch (first frame)");
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, p0->temperature_0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 2.0f, p0->temperature_1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 3.0f, p0->temperature_2);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 4.0f, p0->temperature_3);
    TEST_ASSERT_EQUAL_INT32(63, p0->temperature_id_0);
    TEST_ASSERT_EQUAL_INT32(65, p0->temperature_id_1);
    TEST_ASSERT_EQUAL_INT32(15, p0->temperature_id_2);
    TEST_ASSERT_EQUAL_INT32(61, p0->temperature_id_3);

    primary_hv_cells_temperature_converted_t *p1 = temp_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_PTR(&htemp.temp_can_payload, p1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 5.0f, p1->temperature_0);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 6.0f, p1->temperature_1);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 7.0f, p1->temperature_2);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 8.0f, p1->temperature_3);
    TEST_ASSERT_EQUAL_INT32(6, p1->temperature_id_0);
    TEST_ASSERT_EQUAL_INT32(39, p1->temperature_id_1);
    TEST_ASSERT_EQUAL_INT32(46, p1->temperature_id_2);
    TEST_ASSERT_EQUAL_INT32(7, p1->temperature_id_3);
}

void test_temp_get_cells_temperature_canlib_payload_wrap_indices(void) {
    htemp.cellboard_id = CELLBOARD_ID_COUNT - 1U;
    htemp.offset = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - TEMP_NUM_TEMP_CAN_MESSAGE;

    primary_hv_cells_temperature_converted_t *p = temp_get_cells_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)(CELLBOARD_ID_COUNT - 1U), (uint32_t)p->cellboard_id, "Last-frame cellboard mismatch");

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)htemp.offset, "Offset should wrap to 0");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, (uint32_t)htemp.cellboard_id, "Cellboard ID should wrap to 0");
}

void test_temp_get_cells_temperature_stats_canlib_payload_pointer_size_content(void) {
    size_t byte_size = 0U;
    _temp_fill_all(25.0f);

    primary_hv_cells_temp_stats_converted_t *payload = temp_get_cells_temperature_stats_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&htemp.temp_stats_can_payload, payload, "Stats payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(htemp.temp_stats_can_payload), (uint32_t)byte_size, "Stats payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.0f, payload->max);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.0f, payload->min);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.0f, payload->avg);
}

#ifdef TEMP_TESTS

void setUp(void) {
    (void)temp_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_temp_init_ok);
    RUN_TEST(test_temp_init_indices_zero);
    RUN_TEST(test_temp_get_values_pointer);
    RUN_TEST(test_temp_get_min_max_sum_avg_uniform);
    RUN_TEST(test_temp_cells_temperature_handle_null_payload);
    RUN_TEST(test_temp_cells_temperature_handle_invalid_cellboard_id);
    RUN_TEST(test_temp_cells_temperature_handle_invalid_offset);
    RUN_TEST(test_temp_cells_temperature_handle_valid_updates_four_values);
    RUN_TEST(test_temp_get_cells_temperature_canlib_payload_pointer_size_content_and_increment);
    RUN_TEST(test_temp_get_cells_temperature_canlib_payload_wrap_indices);
    RUN_TEST(test_temp_get_cells_temperature_stats_canlib_payload_pointer_size_content);
    return UNITY_END();
}

#endif // TEMP_TESTS