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
#include "volt-api.h"
#include <string.h>

extern struct BalHandler balancing_handler;
extern struct VoltHandler volt_handler;

void prv_bal_api_timeout(void);

// --- prv_bal_timeout ---

void test_prv_bal_api_timeout_sets_stop_event() {
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    prv_bal_api_timeout();

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_STOP, balancing_handler.event.type, "prv_bal_api_timeout() should set event type to BALANCING_STOP");
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
    primary_hv_balancing_status_converted_t expected_payload = {
        .status = payload.status,
        .cellboard_id = payload.cellboard_id,
        .discharging_cell_0 = payload.discharging_cell_0,
        .discharging_cell_1 = payload.discharging_cell_1,
        .discharging_cell_2 = payload.discharging_cell_2,
        .discharging_cell_3 = payload.discharging_cell_3,
        .discharging_cell_4 = payload.discharging_cell_4,
        .discharging_cell_5 = payload.discharging_cell_5,
        .discharging_cell_6 = payload.discharging_cell_6,
        .discharging_cell_7 = payload.discharging_cell_7,
        .discharging_cell_8 = payload.discharging_cell_8,
        .discharging_cell_9 = payload.discharging_cell_9,
        .discharging_cell_10 = payload.discharging_cell_10,
        .discharging_cell_11 = payload.discharging_cell_11,
        .discharging_cell_12 = payload.discharging_cell_12,
        .discharging_cell_13 = payload.discharging_cell_13,
        .discharging_cell_14 = payload.discharging_cell_14,
        .discharging_cell_15 = payload.discharging_cell_15,
        .discharging_cell_16 = payload.discharging_cell_16,
        .discharging_cell_17 = payload.discharging_cell_17,
        .discharging_cell_18 = payload.discharging_cell_18,
        .discharging_cell_19 = payload.discharging_cell_19,
        .discharging_cell_20 = payload.discharging_cell_20,
        .discharging_cell_21 = payload.discharging_cell_21,
        .discharging_cell_22 = payload.discharging_cell_22,
        .discharging_cell_23 = payload.discharging_cell_23,
    };

    bal_api_cellboard_balancing_status_handle(&payload);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_payload, &balancing_handler.status_can_payload, sizeof(payload), "canlib payload content do not match");
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
    volt_api_init();
    bal_api_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_prv_bal_api_timeout_sets_stop_event);
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
