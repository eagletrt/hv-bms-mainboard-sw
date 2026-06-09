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
#include "internal-voltage.h"
DEFINE_FFF_GLOBALS;

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern struct PcuHandler pcu_handler;
extern _InternalVoltageHandler internal_volt_handler;

extern void prv_pcu_api_airn_timeout(void);
extern void prv_pcu_api_precharge_timeout(void);
extern void prv_pcu_api_airp_timeout(void);

FAKE_VOID_FUNC(pcu_set, const enum PcuPin, const enum PcuPinStatus);
FAKE_VOID_FUNC(pcu_toggle, const enum PcuPin);

static inline void assert_set_call(const uint32_t idx, const enum PcuPin pin, const enum PcuPinStatus status, const char *msg) {
    TEST_ASSERT_EQUAL_MESSAGE(pin, pcu_set_fake.arg0_history[idx], msg);
    TEST_ASSERT_EQUAL_MESSAGE(status, pcu_set_fake.arg1_history[idx], msg);
}

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
    assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "AIR- default state mismatch");
    assert_set_call(1U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "Precharge default state mismatch");
    assert_set_call(2U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "AIR+ default state mismatch");
    assert_set_call(3U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "AMS default state mismatch");
}

void test_pcu_reset_all_sets_default_pins_high(void) {

    pcu_api_reset_all();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(4U, pcu_set_fake.call_count, "pcu_reset_all should write 4 pins");
    assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "AIR- reset mismatch");
    assert_set_call(1U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "Precharge reset mismatch");
    assert_set_call(2U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "AIR+ reset mismatch");
    assert_set_call(3U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "AMS reset mismatch");
}

void test_pcu_airn_open_sets_high(void) {

    pcu_api_airn_open();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "pcu_airn_open mismatch");
}

void test_pcu_airn_close_sets_low(void) {

    pcu_api_airn_close();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_LOW, "pcu_airn_close mismatch");
}

void test_pcu_airp_open_sets_high(void) {

    pcu_api_airp_open();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "pcu_airp_open mismatch");
}

void test_pcu_airp_close_sets_low(void) {

    pcu_api_airp_close();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW, "pcu_airp_close mismatch");
}

void test_pcu_precharge_start_sets_low(void) {

    pcu_api_precharge_start();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_LOW, "pcu_precharge_start mismatch");
}

void test_pcu_precharge_stop_sets_high(void) {

    pcu_api_precharge_stop();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "pcu_precharge_stop mismatch");
}

void test_pcu_ams_activate_sets_low(void) {

    pcu_api_ams_activate();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AMS, PCU_PIN_STATUS_LOW, "pcu_ams_activate mismatch");
}

void test_pcu_ams_deactivate_sets_high(void) {

    pcu_api_ams_deactivate();

    TEST_ASSERT_EQUAL_UINT32(1U, pcu_set_fake.call_count);
    assert_set_call(0U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "pcu_ams_deactivate mismatch");
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

void test_pcu_set_state_from_ecu_handle_null(void) {
    pcu_handler.event.type = FSM_EVENT_TYPE_IGNORED;
    pcu_api_set_state_from_ecu_handle(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        pcu_handler.event.type,
        "NULL ECU payload should not change event");
}

void test_pcu_set_state_from_ecu_handle_on(void) {
    primary_hv_set_status_ecu_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = true;

    pcu_api_set_state_from_ecu_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_ON,
        pcu_handler.event.type,
        "ECU status=true should map to TS_ON");
}

void test_pcu_set_state_from_ecu_handle_off(void) {
    primary_hv_set_status_ecu_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = false;

    pcu_api_set_state_from_ecu_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_OFF,
        pcu_handler.event.type,
        "ECU status=false should map to TS_OFF");
}

void test_pcu_set_state_from_handcart_handle_null(void) {
    pcu_handler.event.type = FSM_EVENT_TYPE_IGNORED;
    pcu_api_set_state_from_handcart_handle(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        pcu_handler.event.type,
        "NULL handcart payload should not change event");
}

void test_pcu_set_state_from_handcart_handle_on(void) {
    primary_hv_set_status_handcart_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = true;

    pcu_api_set_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_ON,
        pcu_handler.event.type,
        "Handcart status=true should map to TS_ON");
}

void test_pcu_set_state_from_handcart_handle_off(void) {
    primary_hv_set_status_handcart_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = false;

    pcu_api_set_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_OFF,
        pcu_handler.event.type,
        "Handcart status=false should map to TS_OFF");
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
    RUN_TEST(test_pcu_set_state_from_ecu_handle_null);
    RUN_TEST(test_pcu_set_state_from_ecu_handle_on);
    RUN_TEST(test_pcu_set_state_from_ecu_handle_off);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_null);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_on);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_off);
    return UNITY_END();
}
