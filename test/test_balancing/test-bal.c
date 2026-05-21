/*!
 * \file test-bal.c
 * \date 2024-08-02
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Test functions for the balancing module
 */

#include "unity.h"
#include "bal-api.h"
#include "timebase.h"
#include "volt.h"
#include <string.h>

extern struct BalHandler balancing_handler;
extern _VoltHandler volt_handler;

void prv_bal_timeout(void);

// --- prv_bal_timeout ---

void test_prv_bal_timeout_sets_stop_event() {
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    prv_bal_timeout();

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_STOP, balancing_handler.event.type, "prv_bal_timeout() should set event type to BALANCING_STOP");
}

// --- bal_api_init ---

void test_bal_api_init_ok() {
    memset(&balancing_handler, 0xFFU, sizeof(balancing_handler));

    const enum BalReturnCode rc = bal_api_init();

    TEST_ASSERT_EQUAL_MESSAGE(BAL_RC_OK, rc, "bal_api_init() failed to return BAL_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_init() should set default event to FSM_EVENT_TYPE_IGNORED");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MAX_V, balancing_handler.params.target, "bal_api_init() should set default target to BAL_TARGET_MAX_V");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, balancing_handler.params.threshold, "bal_api_init() should set default threshold to BAL_THRESHOLD_MAX_V");

    TEST_ASSERT_FALSE_MESSAGE(balancing_handler.set_status_can_payload.start, "bal_api_init() should set start to false in can payload");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MAX_V, balancing_handler.set_status_can_payload.target, "bal_api_init() should set default target in can payload");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, balancing_handler.set_status_can_payload.threshold, "bal_api_init() should set default threshold in can payload");

    TEST_ASSERT_FALSE_MESSAGE(balancing_handler.active, "bal_api_init() should set active to false");
}

// --- bal_api_start ---

void test_bal_api_start_already_active_returns_ok() {
    balancing_handler.active = true;

    const enum BalReturnCode rc = bal_api_start();

    TEST_ASSERT_EQUAL_MESSAGE(BAL_RC_OK, rc, "bal_api_start() should return BAL_RC_OK when already active");
}

void test_bal_api_start_already_active_stays_active() {
    balancing_handler.active = true;

    const enum BalReturnCode rc = bal_api_start();

    TEST_ASSERT_EQUAL_MESSAGE(BAL_RC_OK, rc, "bal_api_start() should return BAL_RC_OK when already active");
    TEST_ASSERT_TRUE_MESSAGE(balancing_handler.active, "bal_api_start() should leave active unchanged when already active");
}

// --- bal_api_stop ---

void test_bal_api_stop_not_active_returns_ok() {
    balancing_handler.active = false;

    const enum BalReturnCode rc = bal_api_stop();

    TEST_ASSERT_EQUAL_MESSAGE(BAL_RC_OK, rc, "bal_api_stop() should return BAL_RC_OK when not active");
}

void test_bal_api_stop_not_active_stays_inactive() {
    balancing_handler.active = false;

    const enum BalReturnCode rc = bal_api_stop();

    TEST_ASSERT_EQUAL_MESSAGE(BAL_RC_OK, rc, "bal_api_stop() should return BAL_RC_OK when not active");
    TEST_ASSERT_FALSE_MESSAGE(balancing_handler.active, "bal_api_stop() should leave active unchanged when already inactive");
}

// --- bal_api_set_balancing_state_from_steering_wheel_handle ---

void test_bal_api_set_steering_wheel_null_payload() {
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    bal_api_set_balancing_state_from_steering_wheel_handle(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_from_steering_wheel_handle() should not change event on NULL payload");
}

void test_bal_api_set_steering_wheel_stop_when_inactive_ignored() {
    balancing_handler.active = false;
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;
    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 0,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_from_steering_wheel_handle() should ignore stop when not active");
}

void test_bal_api_set_steering_wheel_triggers_start_event() {
    balancing_handler.active = false;
    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 1,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_START, balancing_handler.event.type, "bal_api_set_balancing_state_from_steering_wheel_handle() should set BALANCING_START event");
}

void test_bal_api_set_steering_wheel_triggers_stop_event() {
    bal_api_start();
    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 0,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_STOP, balancing_handler.event.type, "bal_api_set_balancing_state_from_steering_wheel_handle() should set BALANCING_STOP event");
}

void test_bal_api_set_steering_wheel_clamps_target_to_min() {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id)
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            volt_handler.voltages[id][i] = BAL_TARGET_MIN_V - 1.0f;

    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 1,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MIN_V, balancing_handler.params.target, "bal_api_set_balancing_state_from_steering_wheel_handle() should clamp target to BAL_TARGET_MIN_V");
}

void test_bal_api_set_steering_wheel_clamps_target_to_max() {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id)
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            volt_handler.voltages[id][i] = BAL_TARGET_MAX_V + 1.0f;

    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 1,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MAX_V, balancing_handler.params.target, "bal_api_set_balancing_state_from_steering_wheel_handle() should clamp target to BAL_TARGET_MAX_V");
}

void test_bal_api_set_steering_wheel_clamps_threshold_to_min() {
    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 1,
        .threshold = BAL_THRESHOLD_MIN_V - 1.0f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MIN_V, balancing_handler.params.threshold, "bal_api_set_balancing_state_from_steering_wheel_handle() should clamp threshold to BAL_THRESHOLD_MIN_V");
}

void test_bal_api_set_steering_wheel_clamps_threshold_to_max() {
    primary_hv_set_balancing_status_steering_wheel_converted_t payload = {
        .status = 1,
        .threshold = BAL_THRESHOLD_MAX_V + 1.0f,
    };

    bal_api_set_balancing_state_from_steering_wheel_handle(&payload);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, balancing_handler.params.threshold, "bal_api_set_balancing_state_from_steering_wheel_handle() should clamp threshold to BAL_THRESHOLD_MAX_V");
}

// --- bal_api_set_balancing_state_from_handcart_handle ---

void test_bal_api_set_handcart_null_payload() {
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    bal_api_set_balancing_state_from_handcart_handle(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_from_handcart_handle() should not change event on NULL payload");
}

void test_bal_api_set_handcart_stop_when_inactive_ignored() {
    balancing_handler.active = false;
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;
    primary_hv_set_balancing_status_handcart_converted_t payload = {
        .status = 0,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_from_handcart_handle() should ignore stop when not active");
}

void test_bal_api_set_handcart_triggers_start_event() {
    balancing_handler.active = false;
    primary_hv_set_balancing_status_handcart_converted_t payload = {
        .status = 1,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_START, balancing_handler.event.type, "bal_api_set_balancing_state_from_handcart_handle() should set BALANCING_START event");
}

void test_bal_api_set_handcart_triggers_stop_event() {
    bal_api_start();
    primary_hv_set_balancing_status_handcart_converted_t payload = {
        .status = 0,
        .threshold = 0.01f,
    };

    bal_api_set_balancing_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_STOP, balancing_handler.event.type, "bal_api_set_balancing_state_from_handcart_handle() should set BALANCING_STOP event");
}

// --- bal_api_cellboard_balancing_status_handle ---
// TODO: these tests will be changed see issue #29

void test_bal_api_cellboard_balancing_status_handle_null_payload() {
    memset(&balancing_handler.status_can_payload, 0U, sizeof(balancing_handler.status_can_payload));

    bal_api_cellboard_balancing_status_handle(NULL);

    primary_hv_balancing_status_converted_t expected = { 0 };
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, &balancing_handler.status_can_payload, sizeof(expected), "bal_api_cellboard_balancing_status_handle() should not modify payload on NULL input");
}

void test_bal_api_cellboard_balancing_status_handle_ok() {
    bms_cellboard_balancing_status_converted_t payload = {
        .status = 1,
        .cellboard_id = CELLBOARD_ID_0,
        .discharging_cell_0 = true,
        .discharging_cell_1 = false,
        .discharging_cell_2 = true,
        .discharging_cell_3 = false,
        .discharging_cell_4 = true,
        .discharging_cell_5 = false,
        .discharging_cell_6 = true,
        .discharging_cell_7 = false,
        .discharging_cell_8 = true,
        .discharging_cell_9 = false,
        .discharging_cell_10 = true,
        .discharging_cell_11 = false,
        .discharging_cell_12 = true,
        .discharging_cell_13 = false,
        .discharging_cell_14 = true,
        .discharging_cell_15 = false,
        .discharging_cell_16 = true,
        .discharging_cell_17 = false,
        .discharging_cell_18 = true,
        .discharging_cell_19 = false,
        .discharging_cell_20 = true,
        .discharging_cell_21 = false,
        .discharging_cell_22 = true,
        .discharging_cell_23 = false,
    };

    bal_api_cellboard_balancing_status_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(payload.status, balancing_handler.status_can_payload.status, "failed to forward status");
    TEST_ASSERT_EQUAL_MESSAGE(payload.cellboard_id, balancing_handler.status_can_payload.cellboard_id, "failed to forward cellboard_id");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_0, balancing_handler.status_can_payload.discharging_cell_0, "failed to forward discharging_cell_0");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_1, balancing_handler.status_can_payload.discharging_cell_1, "failed to forward discharging_cell_1");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_2, balancing_handler.status_can_payload.discharging_cell_2, "failed to forward discharging_cell_2");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_3, balancing_handler.status_can_payload.discharging_cell_3, "failed to forward discharging_cell_3");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_4, balancing_handler.status_can_payload.discharging_cell_4, "failed to forward discharging_cell_4");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_5, balancing_handler.status_can_payload.discharging_cell_5, "failed to forward discharging_cell_5");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_6, balancing_handler.status_can_payload.discharging_cell_6, "failed to forward discharging_cell_6");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_7, balancing_handler.status_can_payload.discharging_cell_7, "failed to forward discharging_cell_7");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_8, balancing_handler.status_can_payload.discharging_cell_8, "failed to forward discharging_cell_8");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_9, balancing_handler.status_can_payload.discharging_cell_9, "failed to forward discharging_cell_9");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_10, balancing_handler.status_can_payload.discharging_cell_10, "failed to forward discharging_cell_10");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_11, balancing_handler.status_can_payload.discharging_cell_11, "failed to forward discharging_cell_11");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_12, balancing_handler.status_can_payload.discharging_cell_12, "failed to forward discharging_cell_12");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_13, balancing_handler.status_can_payload.discharging_cell_13, "failed to forward discharging_cell_13");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_14, balancing_handler.status_can_payload.discharging_cell_14, "failed to forward discharging_cell_14");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_15, balancing_handler.status_can_payload.discharging_cell_15, "failed to forward discharging_cell_15");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_16, balancing_handler.status_can_payload.discharging_cell_16, "failed to forward discharging_cell_16");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_17, balancing_handler.status_can_payload.discharging_cell_17, "failed to forward discharging_cell_17");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_18, balancing_handler.status_can_payload.discharging_cell_18, "failed to forward discharging_cell_18");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_19, balancing_handler.status_can_payload.discharging_cell_19, "failed to forward discharging_cell_19");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_20, balancing_handler.status_can_payload.discharging_cell_20, "failed to forward discharging_cell_20");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_21, balancing_handler.status_can_payload.discharging_cell_21, "failed to forward discharging_cell_21");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_22, balancing_handler.status_can_payload.discharging_cell_22, "failed to forward discharging_cell_22");
    TEST_ASSERT_EQUAL_MESSAGE(payload.discharging_cell_23, balancing_handler.status_can_payload.discharging_cell_23, "failed to forward discharging_cell_23");
}

// --- bal_api_get_set_status_canlib_payload ---

void test_bal_api_get_set_status_canlib_payload_null_size() {
    bms_cellboard_set_balancing_status_converted_t *payload = bal_api_get_set_status_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "bal_api_get_set_status_canlib_payload() should not return NULL with NULL size");
}

void test_bal_api_get_set_status_canlib_payload_size() {
    size_t size = 0U;

    bal_api_get_set_status_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(balancing_handler.set_status_can_payload), size, "bal_api_get_set_status_canlib_payload() returned incorrect byte size");
}

void test_bal_api_get_set_status_canlib_payload_values() {

    balancing_handler.active = true;
    balancing_handler.params.target = 3.6f;
    balancing_handler.params.threshold = 0.01f;

    bms_cellboard_set_balancing_status_converted_t expected = {
        .start = true,
        .target = 3.6f,
        .threshold = 0.01f,
    };

    size_t size = 0U;
    bms_cellboard_set_balancing_status_converted_t *payload = bal_api_get_set_status_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "bal_api_get_set_status_canlib_payload() returned incorrect payload contents");
}

// --- bal_api_get_status_canlib_payload ---

void test_bal_api_get_status_canlib_payload_null_size() {
    primary_hv_balancing_status_converted_t *payload = bal_api_get_status_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "bal_api_get_status_canlib_payload() should not return NULL with NULL size");
}

void test_bal_api_get_status_canlib_payload_size() {
    size_t size = 0U;

    bal_api_get_status_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(sizeof(balancing_handler.status_can_payload), size, "bal_api_get_status_canlib_payload() returned incorrect byte size");
}

void test_bal_api_get_status_canlib_payload_returns_correct_pointer() {
    primary_hv_balancing_status_converted_t *payload = bal_api_get_status_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&balancing_handler.status_can_payload, payload, "bal_api_get_status_canlib_payload() returned wrong pointer");
}

void setUp() {
    timebase_init(1U);
    volt_init();
    bal_api_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_prv_bal_timeout_sets_stop_event);
    RUN_TEST(test_bal_api_init_ok);

    RUN_TEST(test_bal_api_start_already_active_returns_ok);
    RUN_TEST(test_bal_api_start_already_active_stays_active);

    RUN_TEST(test_bal_api_stop_not_active_returns_ok);
    RUN_TEST(test_bal_api_stop_not_active_stays_inactive);

    RUN_TEST(test_bal_api_set_steering_wheel_null_payload);
    RUN_TEST(test_bal_api_set_steering_wheel_stop_when_inactive_ignored);
    RUN_TEST(test_bal_api_set_steering_wheel_triggers_start_event);
    RUN_TEST(test_bal_api_set_steering_wheel_triggers_stop_event);

    RUN_TEST(test_bal_api_set_steering_wheel_clamps_target_to_min);
    RUN_TEST(test_bal_api_set_steering_wheel_clamps_target_to_max);
    RUN_TEST(test_bal_api_set_steering_wheel_clamps_threshold_to_min);
    RUN_TEST(test_bal_api_set_steering_wheel_clamps_threshold_to_max);

    RUN_TEST(test_bal_api_set_handcart_null_payload);
    RUN_TEST(test_bal_api_set_handcart_stop_when_inactive_ignored);
    RUN_TEST(test_bal_api_set_handcart_triggers_start_event);
    RUN_TEST(test_bal_api_set_handcart_triggers_stop_event);

    RUN_TEST(test_bal_api_cellboard_balancing_status_handle_null_payload);
    RUN_TEST(test_bal_api_cellboard_balancing_status_handle_ok);

    RUN_TEST(test_bal_api_get_set_status_canlib_payload_null_size);
    RUN_TEST(test_bal_api_get_set_status_canlib_payload_size);
    RUN_TEST(test_bal_api_get_set_status_canlib_payload_values);

    RUN_TEST(test_bal_api_get_status_canlib_payload_null_size);
    RUN_TEST(test_bal_api_get_status_canlib_payload_size);
    RUN_TEST(test_bal_api_get_status_canlib_payload_returns_correct_pointer);
    return UNITY_END();
}