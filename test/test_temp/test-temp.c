/*!
 * \file test-temp.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Temperature measurement and control
 */
#include "unity.h"
#include "temp-api.h"
#include <string.h>

extern struct TempHandler temp_handler;

int32_t prv_temp_cell_position_from_index(size_t index);
void prv_temp_check_value(CellboardId cellboard_id, size_t offset, celsius_t value);

/* --- prv_temp_cell_position_from_index --- */

void test_prv_temp_cell_position_from_index_valid(void) {
    int32_t result = prv_temp_cell_position_from_index(0U);

    TEST_ASSERT_EQUAL_INT32_MESSAGE(63, result, "prv_temp_cell_position_from_index() failed for index 0");
}

void test_prv_temp_cell_position_from_index_out_of_bounds(void) {
    int32_t result = prv_temp_cell_position_from_index(CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT);

    TEST_ASSERT_EQUAL_INT32_MESSAGE(-1, result, "prv_temp_cell_position_from_index() should return -1 for out-of-bounds index");
}

/* --- temp_api_init --- */

void test_temp_api_init_ok(void) {
    memset(&temp_handler, 0xFFU, sizeof(temp_handler));

    enum TempReturnCode rc = temp_api_init();

    struct TempHandler expected = { 0 };
    TEST_ASSERT_EQUAL_MESSAGE(TEMP_RC_OK, rc, "temp_api_init() failed to return TEMP_RC_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, &temp_handler, sizeof(expected), "temp_api_init() did not zero-initialize the handler");
}

/* --- temp_api_get_min --- */

void test_temp_api_get_min_single_low(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            temp_handler.temperatures[i][j] = 50.0F;
        }
    }
    temp_handler.temperatures[1U][3U] = -10.0F;

    celsius_t min = temp_api_get_min();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, -10.0F, min, "temp_api_get_min() failed to find minimum value");
}

/* --- temp_api_get_max --- */

void test_temp_api_get_max_single_high(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            temp_handler.temperatures[i][j] = 20.0F;
        }
    }
    temp_handler.temperatures[0U][0U] = 80.0F;

    celsius_t max = temp_api_get_max();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 80.0F, max, "temp_api_get_max() failed to find maximum value");
}

/* --- temp_api_get_sum --- */

void test_temp_api_get_sum(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            temp_handler.temperatures[i][j] = 1.0F;
        }
    }

    celsius_t sum = temp_api_get_sum();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.1F, (float)CELLBOARD_TEMP_SENSOR_COUNT, sum, "temp_api_get_sum() returned incorrect sum");
}

/* --- temp_api_get_avg --- */

void test_temp_api_get_avg(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            temp_handler.temperatures[i][j] = 20.0F;
        }
    }

    celsius_t avg = temp_api_get_avg();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 20.0F, avg, "temp_api_get_avg() failed");
}

/* --- temp_api_cells_temperature_handle --- */

void test_temp_api_cells_temperature_handle_null_payload(void) {
    temp_api_cells_temperature_handle(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[0U][0U], "temp_api_cells_temperature_handle() should not modify state on NULL payload");
}

void test_temp_api_cells_temperature_handle_invalid_cellboard_id(void) {
    bms_cellboard_cells_temperature_converted_t payload = {
        .cellboard_id = CELLBOARD_ID_COUNT,
        .offset = 0U,
        .temperature_0 = 25.0F,
        .temperature_1 = 26.0F,
        .temperature_2 = 27.0F,
        .temperature_3 = 28.0F,
    };

    temp_api_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[0U][0U], "temp_api_cells_temperature_handle() should not modify state for invalid cellboard id");
}

void test_temp_api_cells_temperature_handle_offset_overflow(void) {
    bms_cellboard_cells_temperature_converted_t payload = {
        .cellboard_id = 0U,
        .offset = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - 1U,
        .temperature_0 = 25.0F,
        .temperature_1 = 26.0F,
        .temperature_2 = 27.0F,
        .temperature_3 = 28.0F,
    };

    temp_api_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[0U][0U], "temp_api_cells_temperature_handle() should not modify state when offset + size exceeds bounds");
}

void test_temp_api_cells_temperature_handle_ok(void) {
    bms_cellboard_cells_temperature_converted_t payload = {
        .cellboard_id = 0U,
        .offset = 0U,
        .temperature_0 = 10.0F,
        .temperature_1 = 20.0F,
        .temperature_2 = 30.0F,
        .temperature_3 = 40.0F,
    };

    temp_api_cells_temperature_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, temp_handler.temperatures[0U][0U], "temp_api_cells_temperature_handle() stored wrong temperature_0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 20.0F, temp_handler.temperatures[0U][1U], "temp_api_cells_temperature_handle() stored wrong temperature_1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 30.0F, temp_handler.temperatures[0U][2U], "temp_api_cells_temperature_handle() stored wrong temperature_2");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 40.0F, temp_handler.temperatures[0U][3U], "temp_api_cells_temperature_handle() stored wrong temperature_3");
}

/* --- temp_api_get_cells_temperature_canlib_payload --- */

void test_temp_api_get_cells_temperature_canlib_payload_size(void) {
    size_t size = 0U;

    (void)temp_api_get_cells_temperature_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(temp_handler.temp_can_payload), size, "temp_api_get_cells_temperature_canlib_payload() returned incorrect byte size");
}

void test_temp_api_get_cells_temperature_canlib_payload_values(void) {
    temp_handler.cellboard_id = 0U;
    temp_handler.offset = 0U;
    temp_handler.temperatures[0U][0U] = 11.0F;
    temp_handler.temperatures[0U][1U] = 22.0F;
    temp_handler.temperatures[0U][2U] = 33.0F;
    temp_handler.temperatures[0U][3U] = 44.0F;

    primary_hv_cells_temperature_converted_t *payload = temp_api_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 11.0F, payload->temperature_0, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 22.0F, payload->temperature_1, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 33.0F, payload->temperature_2, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_2");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 44.0F, payload->temperature_3, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_3");
}

void test_temp_api_get_cells_temperature_canlib_payload_offset_advances(void) {
    temp_handler.cellboard_id = 0U;
    temp_handler.offset = 0U;

    (void)temp_api_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(TEMP_NUM_TEMP_CAN_MESSAGE, temp_handler.offset, "temp_api_get_cells_temperature_canlib_payload() did not advance offset");
}

void test_temp_api_get_cells_temperature_canlib_payload_offset_wraps(void) {
    temp_handler.cellboard_id = 0U;
    temp_handler.offset = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - TEMP_NUM_TEMP_CAN_MESSAGE;

    (void)temp_api_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, temp_handler.offset, "temp_api_get_cells_temperature_canlib_payload() did not wrap offset to 0");
    TEST_ASSERT_EQUAL_MESSAGE(1U, temp_handler.cellboard_id, "temp_api_get_cells_temperature_canlib_payload() did not advance cellboard_id on offset wrap");
}

void test_temp_api_get_cells_temperature_canlib_payload_cellboard_wraps(void) {
    temp_handler.cellboard_id = CELLBOARD_ID_COUNT - 1U;
    temp_handler.offset = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT - TEMP_NUM_TEMP_CAN_MESSAGE;

    (void)temp_api_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, temp_handler.cellboard_id, "temp_api_get_cells_temperature_canlib_payload() did not wrap cellboard_id to 0");
}

void test_temp_api_get_cells_temperature_canlib_payload_temperature_ids(void) {
    temp_handler.cellboard_id = 0U;
    temp_handler.offset = 0U;

    primary_hv_cells_temperature_converted_t *payload = temp_api_get_cells_temperature_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_INT32_MESSAGE(prv_temp_cell_position_from_index(0U), payload->temperature_id_0, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_id_0");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(prv_temp_cell_position_from_index(1U), payload->temperature_id_1, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_id_1");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(prv_temp_cell_position_from_index(2U), payload->temperature_id_2, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_id_2");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(prv_temp_cell_position_from_index(3U), payload->temperature_id_3, "temp_api_get_cells_temperature_canlib_payload() wrong temperature_id_3");
}

/* --- temp_api_get_cells_temperature_stats_canlib_payload --- */

void test_temp_api_get_cells_temperature_stats_canlib_payload_not_null(void) {
    primary_hv_cells_temp_stats_converted_t *payload = temp_api_get_cells_temperature_stats_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cells_temperature_stats_canlib_payload() should not return NULL");
}

void test_temp_api_get_cells_temperature_stats_canlib_payload_size(void) {
    size_t size = 0U;

    (void)temp_api_get_cells_temperature_stats_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(temp_handler.temp_stats_can_payload), size, "temp_api_get_cells_temperature_stats_canlib_payload() returned incorrect byte size");
}

void test_temp_api_get_cells_temperature_stats_canlib_payload_values(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            temp_handler.temperatures[i][j] = 20.0F;
        }
    }
    temp_handler.temperatures[0U][0U] = 5.0F;
    temp_handler.temperatures[1U][0U] = 60.0F;

    primary_hv_cells_temp_stats_converted_t *payload = temp_api_get_cells_temperature_stats_canlib_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->min, "temp_api_get_cells_temperature_stats_canlib_payload() wrong min");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 60.0F, payload->max, "temp_api_get_cells_temperature_stats_canlib_payload() wrong max");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.1F, temp_api_get_avg(), payload->avg, "temp_api_get_cells_temperature_stats_canlib_payload() wrong avg");
}

void setUp(void) {
    (void)temp_api_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_prv_temp_cell_position_from_index_valid);
    RUN_TEST(test_prv_temp_cell_position_from_index_out_of_bounds);

    RUN_TEST(test_temp_api_init_ok);

    RUN_TEST(test_temp_api_get_min_single_low);

    RUN_TEST(test_temp_api_get_max_single_high);
    RUN_TEST(test_temp_api_get_sum);

    RUN_TEST(test_temp_api_get_avg);

    RUN_TEST(test_temp_api_cells_temperature_handle_null_payload);
    RUN_TEST(test_temp_api_cells_temperature_handle_invalid_cellboard_id);
    RUN_TEST(test_temp_api_cells_temperature_handle_offset_overflow);
    RUN_TEST(test_temp_api_cells_temperature_handle_ok);

    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_size);
    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_values);
    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_offset_advances);
    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_offset_wraps);
    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_cellboard_wraps);
    RUN_TEST(test_temp_api_get_cells_temperature_canlib_payload_temperature_ids);

    RUN_TEST(test_temp_api_get_cells_temperature_stats_canlib_payload_not_null);
    RUN_TEST(test_temp_api_get_cells_temperature_stats_canlib_payload_size);
    RUN_TEST(test_temp_api_get_cells_temperature_stats_canlib_payload_values);
    return UNITY_END();
}