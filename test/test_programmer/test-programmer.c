/*!
 * \file test-programmer.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tests for the Programmer API
 */

#include "unity.h"
#include "programmer-api.h"
#include "fsm.h"
#include "fff.h"
#include "timebase.h"
DEFINE_FFF_GLOBALS;

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern struct ProgrammerHandler programmer_handler;
extern _FsmHandler hfsm;

extern bool prv_programmer_cellboard_ready_all(void);
extern void prv_programmer_flash_timeout(void);
extern void prv_programmer_flash_stop(void);
extern void prv_programmer_flash_reset_flags(void);

FAKE_VOID_FUNC(system_reset);

// programmer_api_init

void test_programmer_init_null_reset_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(PROGRAMMER_RC_NULL_POINTER, programmer_api_init(NULL), "programmer_api_init should return PROGRAMMER_RC_NULL_POINTER when reset is NULL");
}

void test_programmer_init_ok(void) {

    struct ProgrammerHandler expected = {
        .reset = system_reset,
        .flash_request = false,
        .flashing = false,
        .flash_stop = false,
        .cellboard_ready = 0U,
        .flash_event = { .type = FSM_EVENT_TYPE_FLASH_REQUEST },
        .programmer_can_payload = { .ready = false },
        .target = MAINBOARD_ID
    };
    watchdog_init(&expected.watchdog, TIMEBASE_TIME_TO_TICKS(PROGRAMMER_FLASH_TIMEOUT_MS, timebase_get_resolution()), prv_programmer_flash_timeout);

    enum ProgrammerReturnCode ret = programmer_api_init(system_reset);

    TEST_ASSERT_EQUAL_MESSAGE(PROGRAMMER_RC_OK, ret, "programmer_api_init should return PROGRAMMER_RC_OK on successful initialization");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, &programmer_handler, sizeof(expected), "programmer_handler should be initialized correctly");
}

// prv_programmer_flash_timeout

void test_prv_flash_timeout_resets_all_flags(void) {
    programmer_handler.flash_request = true;
    programmer_handler.flashing = true;
    programmer_handler.flash_stop = true;
    programmer_handler.cellboard_ready = 0xFFU;

    prv_programmer_flash_timeout();

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_request, "flash_request should be false after timeout");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should be false after timeout");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_stop, "flash_stop should be false after timeout");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "cellboard_ready should be 0 after timeout");
}

// prv_programmer_flash_stop

void test_prv_flash_stop_sets_stop_flag_and_clears_others(void) {
    programmer_handler.flash_request = true;
    programmer_handler.flashing = true;
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = 0xFFU;

    prv_programmer_flash_stop();

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_request, "flash_request should be false after stop");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should be false after stop");
    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flash_stop, "flash_stop should be true after stop");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "cellboard_ready should be 0 after stop");
}

// prv_programmer_flash_reset_flags

void test_prv_flash_reset_flags_clears_all(void) {
    programmer_handler.flash_request = true;
    programmer_handler.flashing = true;
    programmer_handler.flash_stop = true;
    programmer_handler.cellboard_ready = 0xFFU;

    prv_programmer_flash_reset_flags();

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_request, "flash_request should be false after reset");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should be false after reset");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_stop, "flash_stop should be false after reset");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "cellboard_ready should be 0 after reset");
}

// prv_programmer_cellboard_ready_all

void test_prv_cellboard_ready_all_false_when_not_all_set(void) {
    programmer_handler.cellboard_ready = 0U;

    TEST_ASSERT_FALSE_MESSAGE(prv_programmer_cellboard_ready_all(), "should return false when no cellboards are ready");
}

void test_prv_cellboard_ready_all_true_when_mask_satisfied(void) {
    programmer_handler.cellboard_ready = PROGRAMMER_CELLBOARD_READY_MASK;

    TEST_ASSERT_TRUE_MESSAGE(prv_programmer_cellboard_ready_all(), "should return true when all cellboard bits are set");
}

// programmer_api_flash_request_handle

void test_flash_request_handle_null_payload(void) {
    programmer_handler.flash_request = false;

    programmer_api_flash_request_handle(NULL);

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_request, "flash_request should remain false on NULL payload");
}

void test_flash_request_handle_already_pending(void) {
    programmer_handler.flash_request = true;
    primary_hv_flash_request_converted_t payload = { .mainboard = true };

    programmer_api_flash_request_handle(&payload);

    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flash_request, "flash_request should remain true when already pending");
    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should not change when already pending");
}

void test_flash_request_handle_wrong_fsm_state(void) {
    hfsm.fsm_state = (FSM_STATE_AIRN_CHECK);
    programmer_handler.flash_request = false;
    primary_hv_flash_request_converted_t payload = { .mainboard = true };

    programmer_api_flash_request_handle(&payload);

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flash_request, "flash_request should remain false in non-IDLE/FATAL state");
}

void test_flash_request_handle_sets_mainboard_target(void) {
    hfsm.fsm_state = (FSM_STATE_IDLE);
    primary_hv_flash_request_converted_t payload = { .mainboard = true };

    programmer_api_flash_request_handle(&payload);

    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flash_request, "flash_request should be true after valid request");
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_ID, programmer_handler.target, "target should be MAINBOARD_ID when mainboard=true");
}

void test_flash_request_handle_sets_cellboard_target(void) {
    hfsm.fsm_state = (FSM_STATE_IDLE);
    primary_hv_flash_request_converted_t payload = { .mainboard = false, .cellboard_id = 2U };

    programmer_api_flash_request_handle(&payload);

    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flash_request, "flash_request should be true after valid request");
    TEST_ASSERT_EQUAL_MESSAGE((CellboardId)2U, programmer_handler.target, "target should match cellboard_id when mainboard=false");
}

// programmer_api_cellboard_flash_response_handle

void test_cellboard_flash_response_handle_null_payload(void) {
    programmer_handler.cellboard_ready = 0U;

    programmer_api_cellboard_flash_response_handle(NULL);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "NULL payload should not modify cellboard_ready");
}

void test_cellboard_flash_response_handle_no_pending_request(void) {
    programmer_handler.flash_request = false;
    programmer_handler.cellboard_ready = 0U;
    bms_cellboard_flash_response_converted_t payload = { .ready = true, .cellboard_id = 0U };

    programmer_api_cellboard_flash_response_handle(&payload);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "should not update cellboard_ready without a pending flash_request");
}

void test_cellboard_flash_response_handle_sets_ready_bit(void) {
    programmer_handler.flash_request = true;
    programmer_handler.cellboard_ready = 0U;
    bms_cellboard_flash_response_converted_t payload = { .ready = true, .cellboard_id = 0U };

    programmer_api_cellboard_flash_response_handle(&payload);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(0U, programmer_handler.cellboard_ready, "cellboard_ready should have bit set for ready cellboard");
}

void test_cellboard_flash_response_handle_clears_ready_bit(void) {
    programmer_handler.flash_request = true;
    programmer_handler.cellboard_ready = PROGRAMMER_CELLBOARD_READY_MASK;
    bms_cellboard_flash_response_converted_t payload = { .ready = false, .cellboard_id = 0U };

    programmer_api_cellboard_flash_response_handle(&payload);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(PROGRAMMER_CELLBOARD_READY_MASK, programmer_handler.cellboard_ready, "cellboard_ready should have bit cleared when ready=false");
}

void test_cellboard_flash_response_handle_invalid_cellboard_id(void) {
    programmer_handler.flash_request = true;
    programmer_handler.cellboard_ready = 0U;
    bms_cellboard_flash_response_converted_t payload = { .ready = true, .cellboard_id = CELLBOARD_COUNT }; // Invalid ID

    programmer_api_cellboard_flash_response_handle(&payload);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, programmer_handler.cellboard_ready, "should not modify cellboard_ready with invalid cellboard_id");
}

// programmer_api_flash_handle

void test_flash_handle_null_payload(void) {
    programmer_handler.flashing = false;

    programmer_api_flash_handle(NULL);

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should remain unchanged on NULL payload");
}

void test_flash_handle_same_state_no_change(void) {
    programmer_handler.flashing = true;
    programmer_handler.flash_request = true;
    hfsm.fsm_state = (FSM_STATE_FLASH);
    primary_hv_flash_converted_t payload = { .start = true };

    programmer_api_flash_handle(&payload);

    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flashing, "flashing should remain true when state matches payload");
}

void test_flash_handle_wrong_fsm_state(void) {
    programmer_handler.flashing = false;
    programmer_handler.flash_request = true;
    hfsm.fsm_state = (FSM_STATE_IDLE);
    primary_hv_flash_converted_t payload = { .start = true };

    programmer_api_flash_handle(&payload);

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should not change in wrong FSM state");
}

void test_flash_handle_start_sets_flashing(void) {
    programmer_handler.flashing = false;
    programmer_handler.flash_request = true;
    hfsm.fsm_state = (FSM_STATE_FLASH);
    primary_hv_flash_converted_t payload = { .start = true };

    programmer_api_flash_handle(&payload);

    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flashing, "flashing should be true after start");
}

void test_flash_handle_stop_clears_flashing(void) {
    programmer_handler.flashing = true;
    programmer_handler.flash_request = true;
    hfsm.fsm_state = (FSM_STATE_FLASH);
    primary_hv_flash_converted_t payload = { .start = false };

    programmer_api_flash_handle(&payload);

    TEST_ASSERT_FALSE_MESSAGE(programmer_handler.flashing, "flashing should be false after stop");
    TEST_ASSERT_TRUE_MESSAGE(programmer_handler.flash_stop, "flash_stop should be true after stop");
}

// programmer_api_routine

void test_routine_returns_ok_when_flash_stop(void) {
    programmer_handler.flash_stop = true;

    enum ProgrammerReturnCode ret = programmer_api_routine();

    TEST_ASSERT_EQUAL_MESSAGE(PROGRAMMER_RC_OK, ret, "routine should return PROGRAMMER_RC_OK when flash_stop is set");
}

void test_routine_returns_busy_when_cellboards_not_ready(void) {
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = 0U;

    enum ProgrammerReturnCode ret = programmer_api_routine();

    TEST_ASSERT_EQUAL_MESSAGE(PROGRAMMER_RC_BUSY, ret, "routine should return PROGRAMMER_RC_BUSY when cellboards not ready");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, system_reset_fake.call_count, "reset should not be called when cellboards not ready");
}

void test_routine_calls_reset_when_mainboard_target(void) {
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = PROGRAMMER_CELLBOARD_READY_MASK;
    programmer_handler.target = MAINBOARD_ID;

    (void)programmer_api_routine();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, system_reset_fake.call_count, "reset should be called when target is mainboard");
}

void test_routine_does_not_call_reset_for_cellboard_target(void) {
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = PROGRAMMER_CELLBOARD_READY_MASK;
    programmer_handler.target = (CellboardId)0U;

    enum ProgrammerReturnCode ret = programmer_api_routine();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, system_reset_fake.call_count, "reset should not be called when target is a cellboard");
    TEST_ASSERT_EQUAL_MESSAGE(PROGRAMMER_RC_BUSY, ret, "routine should return PROGRAMMER_RC_BUSY when target is a cellboard");
}

void setUp(void) {
    timebase_init(500U);

    (void)programmer_api_init(system_reset);
    RESET_FAKE(system_reset);
    FFF_RESET_HISTORY();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_programmer_init_null_reset_callback);
    RUN_TEST(test_programmer_init_ok);
    RUN_TEST(test_prv_flash_timeout_resets_all_flags);
    RUN_TEST(test_prv_flash_stop_sets_stop_flag_and_clears_others);
    RUN_TEST(test_prv_flash_reset_flags_clears_all);
    RUN_TEST(test_prv_cellboard_ready_all_false_when_not_all_set);
    RUN_TEST(test_prv_cellboard_ready_all_true_when_mask_satisfied);
    RUN_TEST(test_flash_request_handle_null_payload);
    RUN_TEST(test_flash_request_handle_already_pending);
    RUN_TEST(test_flash_request_handle_wrong_fsm_state);
    RUN_TEST(test_flash_request_handle_sets_mainboard_target);
    RUN_TEST(test_flash_request_handle_sets_cellboard_target);
    RUN_TEST(test_cellboard_flash_response_handle_null_payload);
    RUN_TEST(test_cellboard_flash_response_handle_no_pending_request);
    RUN_TEST(test_cellboard_flash_response_handle_sets_ready_bit);
    RUN_TEST(test_cellboard_flash_response_handle_clears_ready_bit);
    RUN_TEST(test_cellboard_flash_response_handle_invalid_cellboard_id);
    RUN_TEST(test_flash_handle_null_payload);
    RUN_TEST(test_flash_handle_same_state_no_change);
    RUN_TEST(test_flash_handle_wrong_fsm_state);
    RUN_TEST(test_flash_handle_start_sets_flashing);
    RUN_TEST(test_flash_handle_stop_clears_flashing);
    RUN_TEST(test_routine_returns_ok_when_flash_stop);
    RUN_TEST(test_routine_returns_busy_when_cellboards_not_ready);
    RUN_TEST(test_routine_calls_reset_when_mainboard_target);
    RUN_TEST(test_routine_does_not_call_reset_for_cellboard_target);
    return UNITY_END();
}