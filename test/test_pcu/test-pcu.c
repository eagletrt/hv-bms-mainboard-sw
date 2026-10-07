/*!
 * \file test-pcu.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#include "unity.h"
#include "pcu-api.h"
#include "fsm.h"
#include "fff.h"
#include "timebase.h"
#include "internal-voltage-api.h"
DEFINE_FFF_GLOBALS;

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern struct PcuHandler pcu_handler;
extern struct InternalVoltageHandler internal_volt_handler;

extern void prv_pcu_api_airn_timeout(void);
extern void prv_pcu_api_precharge_timeout(void);
extern void prv_pcu_api_airp_timeout(void);
extern void prv_pcu_api_ecu_timeout(void);

FAKE_VOID_FUNC(pcu_set, const enum PcuPin, const enum PcuPinStatus);
FAKE_VOID_FUNC(pcu_toggle, const enum PcuPin);

void test_pcu_init_null_set_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(PCU_RC_NULL_POINTER, pcu_api_init(NULL, pcu_toggle), "pcu_init should return PCU_NULL_POINTER when set callback is NULL");
}

void test_pcu_init_null_toggle_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(PCU_RC_NULL_POINTER, pcu_api_init(pcu_set, NULL), "pcu_init should return PCU_NULL_POINTER when toggle callback is NULL");
}

void test_pcu_init_ok(void) {

    enum PcuReturnCode ret = pcu_api_init(pcu_set, pcu_toggle);

    TEST_ASSERT_EQUAL_MESSAGE(PCU_RC_OK, ret, "pcu_api_init failed to return PCU_RC_OK");
    TEST_ASSERT_EQUAL_MESSAGE(pcu_set, pcu_handler.set, "the set function should be initialized");
    TEST_ASSERT_EQUAL_MESSAGE(pcu_toggle, pcu_handler.toggle, "the toggle function should be initialized");
    TEST_ASSERT_EQUAL_MESSAGE(FSM_EVENT_TYPE_IGNORED, pcu_handler.event.type, "the toggle function should be initialized");

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(4U, pcu_set_fake.call_count, "pcu_init should reset  pins");

    enum PcuPin expected_pins[] = { PCU_PIN_AIR_NEGATIVE, PCU_PIN_PRECHARGE, PCU_PIN_AIR_POSITIVE, PCU_PIN_AMS };
    enum PcuPinStatus expected_status[] = { PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH };

    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected_status, pcu_set_fake.arg1_history, 4, "Wrong init calls");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected_pins, pcu_set_fake.arg0_history, 4, "Wrong init calls");
}

void test_pcu_reset_all_sets_default_pins_high(void) {

    pcu_api_reset_all();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(4U, pcu_set_fake.call_count, "pcu_reset_all should write 4 pins");

    enum PcuPin expected_pins[] = { PCU_PIN_AIR_NEGATIVE, PCU_PIN_PRECHARGE, PCU_PIN_AIR_POSITIVE, PCU_PIN_AMS };
    enum PcuPinStatus expected_status[] = { PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH, PCU_PIN_STATUS_HIGH };

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_pins, pcu_set_fake.arg0_history, 4, "Wrong reset calls");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_status, pcu_set_fake.arg1_history, 4, "Wrong reset calls");
}

void test_pcu_airn_open_sets_high(void) {

    pcu_api_airn_open();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AIR_NEGATIVE, pcu_set_fake.arg0_history[0], "pcu_airn_open should set AIR- pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_HIGH, pcu_set_fake.arg1_history[0], "pcu_airn_open should set AIR- pin high");
}

void test_pcu_airn_close_sets_low(void) {

    pcu_api_airn_close();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AIR_NEGATIVE, pcu_set_fake.arg0_history[0], "pcu_airn_close should set AIR- pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_LOW, pcu_set_fake.arg1_history[0], "pcu_airn_close should set AIR- pin low");
}

void test_pcu_airp_open_sets_high(void) {

    pcu_api_airp_open();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AIR_POSITIVE, pcu_set_fake.arg0_history[0], "pcu_airp_open should set AIR+ pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_HIGH, pcu_set_fake.arg1_history[0], "pcu_airp_open should set AIR+ pin high");
}

void test_pcu_airp_close_sets_low(void) {

    pcu_api_airp_close();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AIR_POSITIVE, pcu_set_fake.arg0_history[0], "pcu_airp_close should set AIR+ pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_LOW, pcu_set_fake.arg1_history[0], "pcu_airp_close should set AIR+ pin low");
}

void test_pcu_precharge_start_sets_low(void) {

    pcu_api_precharge_start();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_PRECHARGE, pcu_set_fake.arg0_history[0], "pcu_precharge_start should set PRECHARGE pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_LOW, pcu_set_fake.arg1_history[0], "pcu_precharge_start should set PRECHARGE pin low");
}

void test_pcu_precharge_stop_sets_high(void) {

    pcu_api_precharge_stop();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_PRECHARGE, pcu_set_fake.arg0_history[0], "pcu_precharge_stop should set PRECHARGE pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_HIGH, pcu_set_fake.arg1_history[0], "pcu_precharge_stop should set PRECHARGE pin high");
}

void test_pcu_ams_activate_sets_low(void) {

    pcu_api_ams_activate();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AMS, pcu_set_fake.arg0_history[0], "pcu_ams_activate should set AMS pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_LOW, pcu_set_fake.arg1_history[0], "pcu_ams_activate should set AMS pin low");
}

void test_pcu_ams_deactivate_sets_high(void) {

    pcu_api_ams_deactivate();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_AMS, pcu_set_fake.arg0_history[0], "pcu_ams_deactivate should set AMS pin");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(PCU_PIN_STATUS_HIGH, pcu_set_fake.arg1_history[0], "pcu_ams_deactivate should set AMS pin high");
}

void test_prv_pcu_api_airn_timeout_sets_event_type(void) {
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    prv_pcu_api_airn_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_AIRN_TIMEOUT,
        pcu_handler.timeout_event.type,
        "AIR- timeout should set AIRN_TIMEOUT event");
}

void test_prv_pcu_api_precharge_timeout_sets_event_type(void) {
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    prv_pcu_api_precharge_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_PRECHARGE_TIMEOUT,
        pcu_handler.timeout_event.type,
        "Precharge timeout should set PRECHARGE_TIMEOUT event");
}

void test_prv_pcu_api_airp_timeout_sets_event_type(void) {
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    prv_pcu_api_airp_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_AIRP_TIMEOUT,
        pcu_handler.timeout_event.type,
        "AIR+ timeout should set AIRP_TIMEOUT event");
}

void test_pcu_get_precharge_percentage_zero_batt(void) {
    internal_volt_handler.ts = 100;
    internal_volt_handler.pack = 0;

    TEST_ASSERT_EQUAL_MESSAGE(
        0,
        pcu_api_get_precharge_percentage(),
        "Precharge percentage should be 0 when battery voltage is 0 to avoid division by zero");
}

void test_pcu_bms_set_handle_on(void) {
    pcu_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    pcu_api_bms_set_handle(true);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_ON,
        pcu_handler.event.type,
        "tson=true should map to TS_ON");
}

void test_pcu_bms_set_handle_off(void) {
    pcu_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    pcu_api_bms_set_handle(false);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_OFF,
        pcu_handler.event.type,
        "tson=false should map to TS_OFF");
}

void test_prv_pcu_api_ecu_timeout_sets_event_type(void) {
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    prv_pcu_api_ecu_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_ECU_TIMEOUT,
        pcu_handler.timeout_event.type,
        "ECU timeout should set ECU_TIMEOUT event");
}

void test_pcu_ecu_fsm_handle_not_running_stays_stopped(void) {
    pcu_api_ecu_fsm_handle();

    TEST_ASSERT_FALSE_MESSAGE(pcu_handler.ecu_watchdog.running, "ECU watchdog should not be started by the handle when it is not running");
}

void test_pcu_ecu_fsm_handle_running_stays_running(void) {
    watchdog_restart(&pcu_handler.ecu_watchdog);

    pcu_api_ecu_fsm_handle();

    TEST_ASSERT_TRUE_MESSAGE(pcu_handler.ecu_watchdog.running, "ECU watchdog should keep running after a reset");
    TEST_ASSERT_FALSE_MESSAGE(pcu_handler.ecu_watchdog.timed_out, "ECU watchdog should not be timed out after a reset");
}

void test_pcu_ecu_fsm_handle_timed_out_restarts(void) {
    watchdog_restart(&pcu_handler.ecu_watchdog);
    pcu_handler.ecu_watchdog.timed_out = true;

    pcu_api_ecu_fsm_handle();

    TEST_ASSERT_TRUE_MESSAGE(pcu_handler.ecu_watchdog.running, "ECU watchdog should be restarted after a timeout");
    TEST_ASSERT_FALSE_MESSAGE(pcu_handler.ecu_watchdog.timed_out, "ECU watchdog timeout flag should be cleared after the restart");
}

void setUp(void) {
    timebase_init(500U);

    (void)pcu_api_init(pcu_set, pcu_toggle);
    RESET_FAKE(pcu_set);
    RESET_FAKE(pcu_toggle);
    FFF_RESET_HISTORY();
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pcu_init_null_set_callback);
    RUN_TEST(test_pcu_init_null_toggle_callback);
    RUN_TEST(test_pcu_init_ok);
    RUN_TEST(test_pcu_reset_all_sets_default_pins_high);
    RUN_TEST(test_pcu_airn_open_sets_high);
    RUN_TEST(test_pcu_airn_close_sets_low);
    RUN_TEST(test_pcu_airp_open_sets_high);
    RUN_TEST(test_pcu_airp_close_sets_low);
    RUN_TEST(test_pcu_precharge_start_sets_low);
    RUN_TEST(test_pcu_precharge_stop_sets_high);
    RUN_TEST(test_pcu_ams_activate_sets_low);
    RUN_TEST(test_pcu_ams_deactivate_sets_high);
    RUN_TEST(test_prv_pcu_api_airn_timeout_sets_event_type);
    RUN_TEST(test_prv_pcu_api_precharge_timeout_sets_event_type);
    RUN_TEST(test_prv_pcu_api_airp_timeout_sets_event_type);
    RUN_TEST(test_pcu_get_precharge_percentage_zero_batt);
    RUN_TEST(test_pcu_bms_set_handle_on);
    RUN_TEST(test_pcu_bms_set_handle_off);
    RUN_TEST(test_prv_pcu_api_ecu_timeout_sets_event_type);
    RUN_TEST(test_pcu_ecu_fsm_handle_not_running_stays_stopped);
    RUN_TEST(test_pcu_ecu_fsm_handle_running_stays_running);
    RUN_TEST(test_pcu_ecu_fsm_handle_timed_out_restarts);
    return UNITY_END();
}
