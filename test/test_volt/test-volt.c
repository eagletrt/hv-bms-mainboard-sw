/*!
 * \file test-volt.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Voltage measurement and control
 */
#include "unity.h"
#include "volt-api.h"
#include <string.h>

extern struct VoltHandler volt_handler;

void prv_volt_check_value(CellboardId cellboard_id, size_t offset, volt_t value);

/* --- volt_api_init --- */

void test_volt_api_init_ok(void) {
    memset(&volt_handler, 0x00U, sizeof(volt_handler));

    enum VoltReturnCode rc = volt_api_init();

    TEST_ASSERT_EQUAL_MESSAGE(VOLT_RC_OK, rc, "volt_api_init() failed to return VOLT_RC_OK");
    TEST_ASSERT_EACH_EQUAL_FLOAT_MESSAGE(VOLT_MAX_V, volt_handler.voltages, sizeof(volt_handler.voltages) / sizeof(volt_handler.voltages[0][0]), "volt_api_init() did not set all voltages to VOLT_MAX_V");
}

/* --- volt_api_get_min --- */

void test_volt_api_get_min_single_low(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 4.0F;
        }
    }
    volt_handler.voltages[1U][3U] = 2.5F;

    volt_t min = volt_api_get_min();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 2.5F, min, "volt_api_get_min() failed to find minimum value");
}

/* --- volt_api_get_max --- */

void test_volt_api_get_max_single_high(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 3.5F;
        }
    }
    volt_handler.voltages[0U][0U] = 4.2F;

    volt_t max = volt_api_get_max();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 4.2F, max, "volt_api_get_max() failed to find maximum value");
}

/* --- volt_api_get_sum --- */

void test_volt_api_get_sum(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 1.0F;
        }
    }

    volt_t sum = volt_api_get_sum();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, (float)CELLBOARD_SERIES_COUNT, sum, "volt_api_get_sum() returned incorrect sum");
}

/* --- volt_api_get_avg --- */

void test_volt_api_get_avg(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 3.7F;
        }
    }

    volt_t avg = volt_api_get_avg();

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.7F, avg, "volt_api_get_avg() failed");
}

/* --- volt_api_cells_voltage_handle --- */

void test_volt_api_cells_voltage_handle_null_payload(void) {
    volt_t before = volt_handler.voltages[0U][0U];

    volt_api_cells_voltage_handle(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, before, volt_handler.voltages[0U][0U], "volt_api_cells_voltage_handle() should not modify state on NULL payload");
}

void test_volt_api_cells_voltage_handle_invalid_cellboard_id(void) {
    volt_t before = volt_handler.voltages[0U][0U];
    bms_cellboard_cells_voltage_converted_t payload = {
        .cellboard_id = CELLBOARD_ID_COUNT,
        .offset = 0U,
        .voltage_0 = 3.0F,
        .voltage_1 = 3.1F,
        .voltage_2 = 3.2F,
    };

    volt_api_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, before, volt_handler.voltages[0U][0U], "volt_api_cells_voltage_handle() should not modify state for invalid cellboard id");
}

void test_volt_api_cells_voltage_handle_offset_overflow(void) {
    volt_t before = volt_handler.voltages[0U][0U];
    bms_cellboard_cells_voltage_converted_t payload = {
        .cellboard_id = 0U,
        .offset = CELLBOARD_SEGMENT_SERIES_COUNT - 1U,
        .voltage_0 = 3.0F,
        .voltage_1 = 3.1F,
        .voltage_2 = 3.2F,
    };

    volt_api_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, before, volt_handler.voltages[0U][0U], "volt_api_cells_voltage_handle() should not modify state when offset + size exceeds bounds");
}

void test_volt_api_cells_voltage_handle_ok(void) {
    bms_cellboard_cells_voltage_converted_t payload = {
        .cellboard_id = 0U,
        .offset = 0U,
        .voltage_0 = 3.6F,
        .voltage_1 = 3.7F,
        .voltage_2 = 3.8F,
    };

    volt_api_cells_voltage_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.6F, volt_handler.voltages[0U][0U], "volt_api_cells_voltage_handle() stored wrong voltage_0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.7F, volt_handler.voltages[0U][1U], "volt_api_cells_voltage_handle() stored wrong voltage_1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.8F, volt_handler.voltages[0U][2U], "volt_api_cells_voltage_handle() stored wrong voltage_2");
}

/* --- volt_api_get_cells_voltage_canlib_payload --- */

void test_volt_api_get_cells_voltage_canlib_payload_size(void) {
    size_t size = 0U;

    (void)volt_api_get_cells_voltage_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(volt_handler.volt_can_payload), size, "volt_api_get_cells_voltage_canlib_payload() returned incorrect byte size");
}

void test_volt_api_get_cells_voltage_canlib_payload_values(void) {
    volt_handler.cellboard_id = 0U;
    volt_handler.offset = 0U;
    volt_handler.voltages[0U][0U] = 3.6F;
    volt_handler.voltages[0U][1U] = 3.7F;
    volt_handler.voltages[0U][2U] = 3.8F;

    primary_hv_cells_voltage_converted_t *payload = volt_api_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.6F, payload->voltage_0, "volt_api_get_cells_voltage_canlib_payload() wrong voltage_0");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.7F, payload->voltage_1, "volt_api_get_cells_voltage_canlib_payload() wrong voltage_1");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.8F, payload->voltage_2, "volt_api_get_cells_voltage_canlib_payload() wrong voltage_2");
}

void test_volt_api_get_cells_voltage_canlib_payload_offset_advances(void) {
    volt_handler.cellboard_id = 0U;
    volt_handler.offset = 0U;

    (void)volt_api_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(3U, volt_handler.offset, "volt_api_get_cells_voltage_canlib_payload() did not advance offset");
}

void test_volt_api_get_cells_voltage_canlib_payload_offset_wraps(void) {
    volt_handler.cellboard_id = 0U;
    volt_handler.offset = CELLBOARD_SEGMENT_SERIES_COUNT - 3U;

    (void)volt_api_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, volt_handler.offset, "volt_api_get_cells_voltage_canlib_payload() did not wrap offset to 0");
}

void test_volt_api_get_cells_voltage_canlib_payload_cellboard_advances_on_wrap(void) {
    volt_handler.cellboard_id = 0U;
    volt_handler.offset = CELLBOARD_SEGMENT_SERIES_COUNT - 3U;

    (void)volt_api_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(1U, volt_handler.cellboard_id, "volt_api_get_cells_voltage_canlib_payload() did not advance cellboard_id on offset wrap");
}

void test_volt_api_get_cells_voltage_canlib_payload_cellboard_wraps(void) {
    volt_handler.cellboard_id = CELLBOARD_ID_COUNT - 1U;
    volt_handler.offset = CELLBOARD_SEGMENT_SERIES_COUNT - 3U;

    (void)volt_api_get_cells_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, volt_handler.cellboard_id, "volt_api_get_cells_voltage_canlib_payload() did not wrap cellboard_id to 0");
}

/* --- volt_api_get_cells_voltage_stats_canlib_payload --- */

void test_volt_api_get_cells_voltage_stats_canlib_payload_size(void) {
    size_t size = 0U;

    (void)volt_api_get_cells_voltage_stats_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(volt_handler.volt_stats_can_payload), size, "volt_api_get_cells_voltage_stats_canlib_payload() returned incorrect byte size");
}

void test_volt_api_get_cells_voltage_stats_canlib_payload_values(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 3.7F;
        }
    }
    volt_handler.voltages[0U][0U] = 3.0F;
    volt_handler.voltages[1U][0U] = 4.2F;

    primary_hv_cells_voltage_stats_converted_t *payload = volt_api_get_cells_voltage_stats_canlib_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.0F, payload->min, "volt_api_get_cells_voltage_stats_canlib_payload() wrong min");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 4.2F, payload->max, "volt_api_get_cells_voltage_stats_canlib_payload() wrong max");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 1.2F, payload->delta, "volt_api_get_cells_voltage_stats_canlib_payload() wrong delta");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, volt_api_get_avg(), payload->avg, "volt_api_get_cells_voltage_stats_canlib_payload() wrong avg");
}

void test_volt_api_get_cells_voltage_stats_canlib_payload_delta_zero_when_all_equal(void) {
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            volt_handler.voltages[i][j] = 3.7F;
        }
    }

    primary_hv_cells_voltage_stats_converted_t *payload = volt_api_get_cells_voltage_stats_canlib_payload(NULL);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 0.0F, payload->delta, "volt_api_get_cells_voltage_stats_canlib_payload() delta should be 0 when all voltages are equal");
}

void setUp(void) {
    (void)volt_api_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_volt_api_init_ok);

    RUN_TEST(test_volt_api_get_min_single_low);

    RUN_TEST(test_volt_api_get_max_single_high);

    RUN_TEST(test_volt_api_get_sum);

    RUN_TEST(test_volt_api_get_avg);

    RUN_TEST(test_volt_api_cells_voltage_handle_null_payload);
    RUN_TEST(test_volt_api_cells_voltage_handle_invalid_cellboard_id);
    RUN_TEST(test_volt_api_cells_voltage_handle_offset_overflow);
    RUN_TEST(test_volt_api_cells_voltage_handle_ok);

    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_size);
    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_values);
    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_offset_advances);
    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_offset_wraps);
    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_cellboard_advances_on_wrap);
    RUN_TEST(test_volt_api_get_cells_voltage_canlib_payload_cellboard_wraps);

    RUN_TEST(test_volt_api_get_cells_voltage_stats_canlib_payload_size);
    RUN_TEST(test_volt_api_get_cells_voltage_stats_canlib_payload_values);
    RUN_TEST(test_volt_api_get_cells_voltage_stats_canlib_payload_delta_zero_when_all_equal);
    return UNITY_END();
}