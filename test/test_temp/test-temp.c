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

/* --- temp_api_init --- */

void test_temp_api_init_ok(void) {
    memset(&temp_handler, 0xFFU, sizeof(temp_handler));

    enum TempReturnCode rc = temp_api_init();

    struct TempHandler expected = { 0 };
    TEST_ASSERT_EQUAL_MESSAGE(TEMP_RC_OK, rc, "temp_api_init() failed to return TEMP_RC_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, &temp_handler, sizeof(expected), "temp_api_init() did not zero-initialize the handler");
}

static void prv_fill_info(celsius_t min, celsius_t max, celsius_t average) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        temp_api_cellboard_temperature_info_handle(id, min, max, average);
    }
}

/* --- temp_api_cellboard_temperature_info_handle --- */

void test_temp_api_cellboard_temperature_info_handle_ok(void) {
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_1, 1.0F, 2.0F, 3.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, temp_handler.min[CELLBOARD_ID_1], "temp_api_cellboard_temperature_info_handle() stored wrong min");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 2.0F, temp_handler.max[CELLBOARD_ID_1], "temp_api_cellboard_temperature_info_handle() stored wrong max");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 3.0F, temp_handler.average[CELLBOARD_ID_1], "temp_api_cellboard_temperature_info_handle() stored wrong average");
}

void test_temp_api_cellboard_temperature_info_handle_invalid_cellboard(void) {
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_COUNT, 1.0F, 2.0F, 3.0F);

    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.min[id], "temp_api_cellboard_temperature_info_handle() should ignore invalid cellboard id");
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.max[id], "temp_api_cellboard_temperature_info_handle() should ignore invalid cellboard id");
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.average[id], "temp_api_cellboard_temperature_info_handle() should ignore invalid cellboard id");
    }
}

/* --- temp_api_set_value --- */

void test_temp_api_set_value_ok(void) {
    temp_api_set_value(CELLBOARD_ID_1, 3U, 25.5F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 25.5F, temp_handler.temperatures[CELLBOARD_ID_1][3U], "temp_api_set_value() did not store the value");
}

void test_temp_api_set_value_invalid_cellboard(void) {
    temp_api_set_value(CELLBOARD_ID_COUNT, 0U, 25.5F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[0U][0U], "temp_api_set_value() should ignore invalid cellboard id");
}

void test_temp_api_set_value_invalid_index(void) {
    temp_api_set_value(CELLBOARD_ID_0, CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT, 25.5F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[0U][0U], "temp_api_set_value() should ignore invalid index");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 0.0F, temp_handler.temperatures[1U][0U], "temp_api_set_value() wrote out of the cellboard row");
}

/* --- temp_api_get_values --- */

void test_temp_api_get_values(void) {
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.temperatures, temp_api_get_values(), "temp_api_get_values() returned the wrong pointer");
}

/* --- temp_api_get_min --- */

void test_temp_api_get_min_single_low(void) {
    prv_fill_info(50.0F, 50.0F, 50.0F);
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_1, -10.0F, 50.0F, 50.0F);

    celsius_t min = temp_api_get_min();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, -10.0F, min, "temp_api_get_min() failed to find minimum value");
}

/* --- temp_api_get_max --- */

void test_temp_api_get_max_single_high(void) {
    prv_fill_info(20.0F, 20.0F, 20.0F);
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_0, 20.0F, 80.0F, 20.0F);

    celsius_t max = temp_api_get_max();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 80.0F, max, "temp_api_get_max() failed to find maximum value");
}

/* --- temp_api_get_avg --- */

void test_temp_api_get_avg(void) {
    prv_fill_info(20.0F, 20.0F, 20.0F);

    celsius_t avg = temp_api_get_avg();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 20.0F, avg, "temp_api_get_avg() failed");
}

void test_temp_api_get_avg_different_values(void) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        temp_api_cellboard_temperature_info_handle(id, 0.0F, 0.0F, (celsius_t)id);
    }
    celsius_t expected = 0.0F;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        expected += (celsius_t)id;
    }
    expected /= (celsius_t)CELLBOARD_COUNT;

    celsius_t avg = temp_api_get_avg();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, expected, avg, "temp_api_get_avg() should average the cellboard averages");
}

/* --- temp_api_get_cells_temperature_info_canlib_payload --- */

void test_temp_api_get_cells_temperature_info_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_info(20.0F, 20.0F, 20.0F);
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_0, 5.0F, 20.0F, 20.0F);
    temp_api_cellboard_temperature_info_handle(CELLBOARD_ID_1, 20.0F, 60.0F, 20.0F);

    const union CanPrimaryMessages *payload = temp_api_get_cells_temperature_info_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cells_temperature_info_canlib_payload() should not return NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_temperature_info, payload, "temp_api_get_cells_temperature_info_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsacmainboardtemperatureinfo, size, "temp_api_get_cells_temperature_info_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsacmainboardtemperatureinfo.min, "wrong min");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 60.0F, payload->tsacmainboardtemperatureinfo.max, "wrong max");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.1F, temp_api_get_avg(), payload->tsacmainboardtemperatureinfo.average, "wrong average");
}

void test_temp_api_get_cells_temperature_info_canlib_payload_null_size(void) {
    prv_fill_info(20.0F, 20.0F, 20.0F);

    const union CanPrimaryMessages *payload = temp_api_get_cells_temperature_info_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cells_temperature_info_canlib_payload() should not return NULL when byte_size is NULL");
}

/* --- temp_api_get_cellboardN_temperature_canlib_payload --- */

/*
 * Each call sends the next group of 5 cells: the group counter starts from 0 and the
 * first call after the initialization sends group 1, the tenth wraps back to group 0.
 */
void test_temp_api_get_cellboard1_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_0][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard1_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard1_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard1, payload, "temp_api_get_cellboard1_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard1temperature, size, "temp_api_get_cellboard1_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard1temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard1temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard1temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard1temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard1temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard1temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard1_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard1temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard1temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard1temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard1temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard1_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard1temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard1temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard1temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void test_temp_api_get_cellboard2_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_1][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard2_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard2_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard2, payload, "temp_api_get_cellboard2_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard2temperature, size, "temp_api_get_cellboard2_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard2temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard2temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard2temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard2temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard2temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard2temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard2_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard2temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard2temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard2temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard2temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard2_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard2temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard2temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard2temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void test_temp_api_get_cellboard3_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_2][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard3_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard3_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard3, payload, "temp_api_get_cellboard3_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard3temperature, size, "temp_api_get_cellboard3_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard3temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard3temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard3temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard3temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard3temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard3temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard3_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard3temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard3temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard3temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard3temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard3_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard3temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard3temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard3temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void test_temp_api_get_cellboard4_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_3][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard4_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard4_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard4, payload, "temp_api_get_cellboard4_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard4temperature, size, "temp_api_get_cellboard4_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard4temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard4temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard4temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard4temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard4temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard4temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard4_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard4temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard4temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard4temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard4temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard4_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard4temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard4temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard4temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void test_temp_api_get_cellboard5_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_4][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard5_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard5_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard5, payload, "temp_api_get_cellboard5_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard5temperature, size, "temp_api_get_cellboard5_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard5temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard5temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard5temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard5temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard5temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard5temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard5_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard5temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard5temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard5temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard5temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard5_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard5temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard5temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard5temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void test_temp_api_get_cellboard6_temperature_canlib_payload(void) {
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temp_handler.temperatures[CELLBOARD_ID_5][i] = (celsius_t)(i + 1U);
    }
    size_t size = 0U;

    const union CanPrimaryMessages *payload = temp_api_get_cellboard6_temperature_canlib_payload(&size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "temp_api_get_cellboard6_temperature_canlib_payload() returned NULL");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&temp_handler.libcan_message_cellboard6, payload, "temp_api_get_cellboard6_temperature_canlib_payload() returned the wrong pointer");
    TEST_ASSERT_EQUAL_MESSAGE(can_primary_byte_size_tsaccellboard6temperature, size, "temp_api_get_cellboard6_temperature_canlib_payload() returned incorrect byte size");
    TEST_ASSERT_EQUAL_MESSAGE(1, payload->tsaccellboard6temperature.group, "first group should be 1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 6.0F, payload->tsaccellboard6temperature.group_payload.mux_1.cell6, "wrong cell6");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 7.0F, payload->tsaccellboard6temperature.group_payload.mux_1.cell7, "wrong cell7");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 8.0F, payload->tsaccellboard6temperature.group_payload.mux_1.cell8, "wrong cell8");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 9.0F, payload->tsaccellboard6temperature.group_payload.mux_1.cell9, "wrong cell9");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F, payload->tsaccellboard6temperature.group_payload.mux_1.cell10, "wrong cell10");

    for (size_t call = 2U; call <= 9U; ++call) {
        payload = temp_api_get_cellboard6_temperature_canlib_payload(NULL);
        TEST_ASSERT_EQUAL_MESSAGE(call, payload->tsaccellboard6temperature.group, "group should increase on each call");
    }
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 46.0F, payload->tsaccellboard6temperature.group_payload.mux_9.cell46, "wrong cell46");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 47.0F, payload->tsaccellboard6temperature.group_payload.mux_9.cell47, "wrong cell47");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 48.0F, payload->tsaccellboard6temperature.group_payload.mux_9.cell48, "wrong cell48");

    payload = temp_api_get_cellboard6_temperature_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(0, payload->tsaccellboard6temperature.group, "group should wrap to 0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 1.0F, payload->tsaccellboard6temperature.group_payload.mux_0.cell1, "wrong cell1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 5.0F, payload->tsaccellboard6temperature.group_payload.mux_0.cell5, "wrong cell5");
}

void setUp(void) {
    (void)temp_api_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_temp_api_init_ok);

    RUN_TEST(test_temp_api_cellboard_temperature_info_handle_ok);
    RUN_TEST(test_temp_api_cellboard_temperature_info_handle_invalid_cellboard);

    RUN_TEST(test_temp_api_set_value_ok);
    RUN_TEST(test_temp_api_set_value_invalid_cellboard);
    RUN_TEST(test_temp_api_set_value_invalid_index);
    RUN_TEST(test_temp_api_get_values);

    RUN_TEST(test_temp_api_get_min_single_low);
    RUN_TEST(test_temp_api_get_max_single_high);
    RUN_TEST(test_temp_api_get_avg);
    RUN_TEST(test_temp_api_get_avg_different_values);

    RUN_TEST(test_temp_api_get_cells_temperature_info_canlib_payload);
    RUN_TEST(test_temp_api_get_cells_temperature_info_canlib_payload_null_size);

    RUN_TEST(test_temp_api_get_cellboard1_temperature_canlib_payload);
    RUN_TEST(test_temp_api_get_cellboard2_temperature_canlib_payload);
    RUN_TEST(test_temp_api_get_cellboard3_temperature_canlib_payload);
    RUN_TEST(test_temp_api_get_cellboard4_temperature_canlib_payload);
    RUN_TEST(test_temp_api_get_cellboard5_temperature_canlib_payload);
    RUN_TEST(test_temp_api_get_cellboard6_temperature_canlib_payload);
    return UNITY_END();
}
