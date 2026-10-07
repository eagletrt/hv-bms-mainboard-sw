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

#include "eagletrt-api.h"

extern struct BalHandler balancing_handler;

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

    const struct CanBmsTsacmainboardbalancingset *payload = &balancing_handler.libcan_message_balancing_set.tsacmainboardbalancingset;
    TEST_ASSERT_EQUAL_MESSAGE(0U, payload->start, "bal_api_init() should set start to false in can payload");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MAX_V, payload->target, "bal_api_init() should set default target in can payload");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, payload->threshold, "bal_api_init() should set default threshold in can payload");

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

// --- bal_api_set_balancing_state_handle ---

static void prv_fill_min_voltage(const volt_t min) {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        volt_api_cellboard_voltage_info_handle(id, min, min, min, min);
    }
}

void test_bal_api_set_state_stop_when_inactive_ignored(void) {
    balancing_handler.active = false;
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    bal_api_set_balancing_state_handle(false, 0.01f);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_handle() should ignore stop when not active");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, balancing_handler.params.threshold, "bal_api_set_balancing_state_handle() should not update parameters when ignoring stop");
}

void test_bal_api_set_state_triggers_start_event(void) {
    balancing_handler.active = false;

    bal_api_set_balancing_state_handle(true, 0.01f);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_START, balancing_handler.event.type, "bal_api_set_balancing_state_handle() should set BALANCING_START event");
}

void test_bal_api_set_state_triggers_stop_event(void) {
    bal_api_start();

    bal_api_set_balancing_state_handle(false, 0.01f);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_BALANCING_STOP, balancing_handler.event.type, "bal_api_set_balancing_state_handle() should set BALANCING_STOP event");
}

void test_bal_api_set_state_start_when_active_no_event(void) {
    bal_api_start();
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    bal_api_set_balancing_state_handle(true, 0.01f);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, balancing_handler.event.type, "bal_api_set_balancing_state_handle() should not trigger an event when the state does not change");
}

void test_bal_api_set_state_sets_target_and_threshold(void) {
    prv_fill_min_voltage(3.5f);

    bal_api_set_balancing_state_handle(true, 0.01f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 3.5f, balancing_handler.params.target, "bal_api_set_balancing_state_handle() should use the minimum voltage as target");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.01f, balancing_handler.params.threshold, "bal_api_set_balancing_state_handle() should store the given threshold");
}

void test_bal_api_set_state_clamps_target_to_min(void) {
    prv_fill_min_voltage(BAL_TARGET_MIN_V - 1.0f);

    bal_api_set_balancing_state_handle(true, 0.01f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MIN_V, balancing_handler.params.target, "bal_api_set_balancing_state_handle() should clamp target to BAL_TARGET_MIN_V");
}

void test_bal_api_set_state_clamps_target_to_max(void) {
    prv_fill_min_voltage(BAL_TARGET_MAX_V + 1.0f);

    bal_api_set_balancing_state_handle(true, 0.01f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_TARGET_MAX_V, balancing_handler.params.target, "bal_api_set_balancing_state_handle() should clamp target to BAL_TARGET_MAX_V");
}

void test_bal_api_set_state_clamps_threshold_to_min(void) {
    bal_api_set_balancing_state_handle(true, BAL_THRESHOLD_MIN_V - 1.0f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MIN_V, balancing_handler.params.threshold, "bal_api_set_balancing_state_handle() should clamp threshold to BAL_THRESHOLD_MIN_V");
}

void test_bal_api_set_state_clamps_threshold_to_max(void) {
    bal_api_set_balancing_state_handle(true, BAL_THRESHOLD_MAX_V + 1.0f);

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, BAL_THRESHOLD_MAX_V, balancing_handler.params.threshold, "bal_api_set_balancing_state_handle() should clamp threshold to BAL_THRESHOLD_MAX_V");
}

// --- bal_api_cellboard_balancing_handle ---

void test_bal_api_cellboard_balancing_handle_ok(void) {
    bal_api_cellboard_balancing_handle(CELLBOARD_ID_2, 0x00A5A5A5U);

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x00A5A5A5U, balancing_handler.discharging_cells[CELLBOARD_ID_2], "bal_api_cellboard_balancing_handle() should store the discharging flags");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0U, balancing_handler.discharging_cells[CELLBOARD_ID_1], "bal_api_cellboard_balancing_handle() should not modify other cellboards");
}

void test_bal_api_cellboard_balancing_handle_invalid_cellboard(void) {
    bal_api_cellboard_balancing_handle(CELLBOARD_ID_COUNT, 0xFFFFFFFFU);

    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        TEST_ASSERT_EQUAL_HEX32_MESSAGE(0U, balancing_handler.discharging_cells[id], "bal_api_cellboard_balancing_handle() should ignore invalid cellboard id");
    }
}

// --- bal_api_get_balancing_set_canlib_payload ---

void test_bal_api_get_balancing_set_canlib_payload_null_size(void) {
    union CanBmsMessages *payload = bal_api_get_balancing_set_canlib_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "bal_api_get_balancing_set_canlib_payload() should not return NULL with NULL size");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&balancing_handler.libcan_message_balancing_set, payload, "bal_api_get_balancing_set_canlib_payload() returned wrong pointer");
}

void test_bal_api_get_balancing_set_canlib_payload_size(void) {
    size_t size = 0U;

    bal_api_get_balancing_set_canlib_payload(&size);

    TEST_ASSERT_EQUAL_MESSAGE(can_bms_byte_size_tsacmainboardbalancingset, size, "bal_api_get_balancing_set_canlib_payload() returned incorrect byte size");
}

void test_bal_api_get_balancing_set_canlib_payload_values(void) {
    balancing_handler.active = true;
    balancing_handler.params.target = 3.6f;
    balancing_handler.params.threshold = 0.01f;

    const union CanBmsMessages *payload = bal_api_get_balancing_set_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(1U, payload->tsacmainboardbalancingset.start, "bal_api_get_balancing_set_canlib_payload() should set start when active");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 3.6f, payload->tsacmainboardbalancingset.target, "bal_api_get_balancing_set_canlib_payload() returned incorrect target");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.01f, payload->tsacmainboardbalancingset.threshold, "bal_api_get_balancing_set_canlib_payload() returned incorrect threshold");
}

void test_bal_api_get_balancing_set_canlib_payload_inactive(void) {
    balancing_handler.active = false;

    const union CanBmsMessages *payload = bal_api_get_balancing_set_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(0U, payload->tsacmainboardbalancingset.start, "bal_api_get_balancing_set_canlib_payload() should clear start when inactive");
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

    RUN_TEST(test_bal_api_set_state_stop_when_inactive_ignored);
    RUN_TEST(test_bal_api_set_state_triggers_start_event);
    RUN_TEST(test_bal_api_set_state_triggers_stop_event);
    RUN_TEST(test_bal_api_set_state_start_when_active_no_event);
    RUN_TEST(test_bal_api_set_state_sets_target_and_threshold);
    RUN_TEST(test_bal_api_set_state_clamps_target_to_min);
    RUN_TEST(test_bal_api_set_state_clamps_target_to_max);
    RUN_TEST(test_bal_api_set_state_clamps_threshold_to_min);
    RUN_TEST(test_bal_api_set_state_clamps_threshold_to_max);
    RUN_TEST(test_bal_api_cellboard_balancing_handle_ok);
    RUN_TEST(test_bal_api_cellboard_balancing_handle_invalid_cellboard);
    RUN_TEST(test_bal_api_get_balancing_set_canlib_payload_null_size);
    RUN_TEST(test_bal_api_get_balancing_set_canlib_payload_size);
    RUN_TEST(test_bal_api_get_balancing_set_canlib_payload_values);
    RUN_TEST(test_bal_api_get_balancing_set_canlib_payload_inactive);
    return UNITY_END();
}
