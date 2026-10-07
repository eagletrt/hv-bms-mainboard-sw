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

#include "can-primary.h"

extern struct VoltHandler volt_handler;

static void prv_fill_info(volt_t min, volt_t max, volt_t average, volt_t sum) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        volt_api_cellboard_voltage_info_handle(id, min, max, average, sum);
    }
}

/* --- volt_api_init --- */

void test_volt_api_init_ok(void) {
    memset(&volt_handler, 0x00U, sizeof(volt_handler));

    enum VoltReturnCode rc = volt_api_init();

    TEST_ASSERT_EQUAL_MESSAGE(VOLT_RC_OK, rc, "volt_api_init() failed to return VOLT_RC_OK");
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_SERIES_COUNT; ++j) {
            TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, VOLT_MAX_V, volt_handler.voltages[i][j], "volt_api_init() did not set all voltages to VOLT_MAX_V");
        }
    }
}

/* --- volt_api_set_value --- */

void test_volt_api_set_value_ok(void) {
    volt_api_set_value(CELLBOARD_ID_1, 3U, 3.65F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.65F, volt_handler.voltages[CELLBOARD_ID_1][3U], "volt_api_set_value() did not store the value");
}

void test_volt_api_set_value_invalid_cellboard(void) {
    volt_api_set_value(CELLBOARD_ID_COUNT, 0U, 3.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, VOLT_MAX_V, volt_handler.voltages[0U][0U], "volt_api_set_value() should ignore invalid cellboard id");
}

void test_volt_api_set_value_invalid_index(void) {
    volt_api_set_value(CELLBOARD_ID_0, CELLBOARD_SEGMENT_SERIES_COUNT, 3.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, VOLT_MAX_V, volt_handler.voltages[0U][0U], "volt_api_set_value() should ignore invalid index");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, VOLT_MAX_V, volt_handler.voltages[1U][0U], "volt_api_set_value() wrote out of the cellboard row");
}

/* --- volt_api_cellboard_voltage_info_handle --- */

void test_volt_api_cellboard_voltage_info_handle_ok(void) {
    volt_api_cellboard_voltage_info_handle(CELLBOARD_ID_2, 3.0F, 4.0F, 3.5F, 84.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.0F, volt_handler.min[CELLBOARD_ID_2], "wrong min");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 4.0F, volt_handler.max[CELLBOARD_ID_2], "wrong max");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.5F, volt_handler.average[CELLBOARD_ID_2], "wrong average");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 84.0F, volt_handler.sum[CELLBOARD_ID_2], "wrong sum");
}

void test_volt_api_cellboard_voltage_info_handle_invalid_cellboard(void) {
    struct VoltHandler before = volt_handler;

    volt_api_cellboard_voltage_info_handle(CELLBOARD_ID_COUNT, 1.0F, 2.0F, 3.0F, 4.0F);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&before, &volt_handler, sizeof(volt_handler), "volt_api_cellboard_voltage_info_handle() should not modify state for invalid cellboard id");
}

/* --- volt_api_get_min --- */

void test_volt_api_get_min_single_low(void) {
    prv_fill_info(4.0F, 4.1F, 4.05F, 97.2F);
    volt_handler.min[CELLBOARD_ID_COUNT - 1U] = 2.5F;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 2.5F, volt_api_get_min(), "volt_api_get_min() failed to find minimum value");
}

void test_volt_api_get_min_first_cellboard(void) {
    prv_fill_info(4.0F, 4.1F, 4.05F, 97.2F);
    volt_handler.min[0U] = 2.5F;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 2.5F, volt_api_get_min(), "volt_api_get_min() ignored the first cellboard");
}

/* --- volt_api_get_max --- */

void test_volt_api_get_max_single_high(void) {
    prv_fill_info(3.0F, 3.5F, 3.2F, 76.8F);
    volt_handler.max[CELLBOARD_ID_COUNT - 1U] = 4.2F;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 4.2F, volt_api_get_max(), "volt_api_get_max() failed to find maximum value");
}

void test_volt_api_get_max_first_cellboard(void) {
    prv_fill_info(3.0F, 3.5F, 3.2F, 76.8F);
    volt_handler.max[0U] = 4.2F;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 4.2F, volt_api_get_max(), "volt_api_get_max() ignored the first cellboard");
}

/* --- volt_api_get_sum --- */

void test_volt_api_get_sum(void) {
    prv_fill_info(1.0F, 1.0F, 1.0F, 10.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01F, 10.0F * (float)CELLBOARD_COUNT, volt_api_get_sum(), "volt_api_get_sum() returned incorrect sum");
}

/* --- volt_api_get_avg --- */

void test_volt_api_get_avg(void) {
    prv_fill_info(3.0F, 4.0F, 3.7F, 100.0F);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 3.7F, volt_api_get_avg(), "volt_api_get_avg() failed");
}

void test_volt_api_get_avg_different_cellboards(void) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        volt_handler.average[id] = (id % 2U == 0U) ? 3.0F : 4.0F;
    }
    float expected = 0.0F;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        expected += volt_handler.average[id];
    }
    expected /= (float)CELLBOARD_ID_COUNT;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001F, expected, volt_api_get_avg(), "volt_api_get_avg() returned wrong weighted average");
}

/* --- volt_api_get_cellboardN_voltage_canlib_payload --- */

/* Fill a cellboard row with a ramp: cell i (0-based) = 3.0 V + i * 0.01 V */
static void prv_fill_ramp(CellboardId id) {
    for (size_t cell = 0U; cell < CELLBOARD_SEGMENT_SERIES_COUNT; ++cell) {
        volt_handler.voltages[id][cell] = 3.0F + ((float)cell * 0.01F);
    }
}

void test_volt_api_get_cellboard1_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_0);

    union CanPrimaryMessages *msg = volt_api_get_cellboard1_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard1voltage *payload = &msg->tsaccellboard1voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard1, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard1voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard1_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard1_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard1_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard2_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_1);

    union CanPrimaryMessages *msg = volt_api_get_cellboard2_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard2voltage *payload = &msg->tsaccellboard2voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard2, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard2voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard2_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard2_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard2_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard3_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_2);

    union CanPrimaryMessages *msg = volt_api_get_cellboard3_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard3voltage *payload = &msg->tsaccellboard3voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard3, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard3voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard3_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard3_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard3_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard4_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_3);

    union CanPrimaryMessages *msg = volt_api_get_cellboard4_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard4voltage *payload = &msg->tsaccellboard4voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard4, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard4voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard4_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard4_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard4_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard5_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_4);

    union CanPrimaryMessages *msg = volt_api_get_cellboard5_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard5voltage *payload = &msg->tsaccellboard5voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard5, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard5voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard5_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard5_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard5_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard6_voltage_canlib_payload(void) {
    size_t size = 0U;
    prv_fill_ramp(CELLBOARD_ID_5);

    union CanPrimaryMessages *msg = volt_api_get_cellboard6_voltage_canlib_payload(&size);
    struct CanPrimaryTsaccellboard6voltage *payload = &msg->tsaccellboard6voltage;

    TEST_ASSERT_EQUAL_PTR(&volt_handler.libcan_message_cellboard6, msg);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsaccellboard6voltage, size);
    TEST_ASSERT_EQUAL_UINT8(1U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.06F, payload->group_payload.mux_1.cell7);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.11F, payload->group_payload.mux_1.cell12);

    (void)volt_api_get_cellboard6_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(2U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.12F, payload->group_payload.mux_2.cell13);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.17F, payload->group_payload.mux_2.cell18);

    (void)volt_api_get_cellboard6_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(3U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.18F, payload->group_payload.mux_3.cell19);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.23F, payload->group_payload.mux_3.cell24);

    (void)volt_api_get_cellboard6_voltage_canlib_payload(NULL);
    TEST_ASSERT_EQUAL_UINT8(0U, payload->group);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.00F, payload->group_payload.mux_0.cell1);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.05F, payload->group_payload.mux_0.cell6);
}

void test_volt_api_get_cellboard_voltage_canlib_payload_null_size(void) {
    TEST_ASSERT_NOT_NULL(volt_api_get_cellboard1_voltage_canlib_payload(NULL));
}

void setUp(void) {
    (void)volt_api_init();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_volt_api_init_ok);

    RUN_TEST(test_volt_api_set_value_ok);
    RUN_TEST(test_volt_api_set_value_invalid_cellboard);
    RUN_TEST(test_volt_api_set_value_invalid_index);

    RUN_TEST(test_volt_api_cellboard_voltage_info_handle_ok);
    RUN_TEST(test_volt_api_cellboard_voltage_info_handle_invalid_cellboard);

    RUN_TEST(test_volt_api_get_min_single_low);
    RUN_TEST(test_volt_api_get_min_first_cellboard);
    RUN_TEST(test_volt_api_get_max_single_high);
    RUN_TEST(test_volt_api_get_max_first_cellboard);
    RUN_TEST(test_volt_api_get_sum);
    RUN_TEST(test_volt_api_get_avg);
    RUN_TEST(test_volt_api_get_avg_different_cellboards);

    RUN_TEST(test_volt_api_get_cellboard1_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard2_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard3_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard4_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard5_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard6_voltage_canlib_payload);
    RUN_TEST(test_volt_api_get_cellboard_voltage_canlib_payload_null_size);
    return UNITY_END();
}
