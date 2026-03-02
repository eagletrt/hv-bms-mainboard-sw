#include "unity.h"
#include "pcu.h"
#include "fsm.h"
#include "timebase.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern _PcuHandler hpcu;

extern void _pcu_airn_timeout(void);
extern void _pcu_precharge_timeout(void);
extern void _pcu_airp_timeout(void);

/* ---------- spies ---------- */

#define PCU_SET_CALLS_MAX 32U

static uint32_t g_set_calls_count = 0U;
static PcuPin g_set_calls_pin[PCU_SET_CALLS_MAX];
static PcuPinStatus g_set_calls_status[PCU_SET_CALLS_MAX];

static uint32_t g_toggle_calls_count = 0U;
static PcuPin g_toggle_last_pin = (PcuPin)0U;

static void _pcu_set_spy(const PcuPin pin, const PcuPinStatus status) {
    if (g_set_calls_count < PCU_SET_CALLS_MAX) {
        g_set_calls_pin[g_set_calls_count] = pin;
        g_set_calls_status[g_set_calls_count] = status;
        g_set_calls_count++;
    }
}

static void _pcu_toggle_spy(const PcuPin pin) {
    g_toggle_last_pin = pin;
    g_toggle_calls_count++;
}

static void _pcu_reset_spies(void) {
    g_set_calls_count = 0U;
    g_toggle_calls_count = 0U;
    g_toggle_last_pin = (PcuPin)0U;
    memset(g_set_calls_pin, 0, sizeof(g_set_calls_pin));
    memset(g_set_calls_status, 0, sizeof(g_set_calls_status));
}

static void _assert_set_call(const uint32_t idx, const PcuPin pin, const PcuPinStatus status, const char *msg) {
    TEST_ASSERT_TRUE_MESSAGE(idx < g_set_calls_count, msg);
    TEST_ASSERT_EQUAL_MESSAGE(pin, g_set_calls_pin[idx], msg);
    TEST_ASSERT_EQUAL_MESSAGE(status, g_set_calls_status[idx], msg);
}

/* ---------- tests ---------- */

void test_pcu_init_null_set_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        PCU_NULL_POINTER,
        pcu_init(NULL, _pcu_toggle_spy),
        "pcu_init should return PCU_NULL_POINTER when set callback is NULL");
}

void test_pcu_init_null_toggle_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        PCU_NULL_POINTER,
        pcu_init(_pcu_set_spy, NULL),
        "pcu_init should return PCU_NULL_POINTER when toggle callback is NULL");
}

void test_pcu_init_ok(void) {
    _pcu_reset_spies();
    TEST_ASSERT_EQUAL_MESSAGE(
        PCU_OK,
        pcu_init(_pcu_set_spy, _pcu_toggle_spy),
        "pcu_init failed to return PCU_OK");
}

void test_pcu_init_event_type_ignored(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        hpcu.event.type,
        "Initial PCU event type should be IGNORED");
}

void test_pcu_init_sets_default_pins_high(void) {
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(4U, g_set_calls_count, "pcu_init should reset 4 pins");
    _assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "AIR- default state mismatch");
    _assert_set_call(1U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "Precharge default state mismatch");
    _assert_set_call(2U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "AIR+ default state mismatch");
    _assert_set_call(3U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "AMS default state mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        hpcu.timeout_event.type,
        "Timeout event type should be IGNORED after reset");
}

void test_pcu_reset_all_sets_default_pins_high(void) {
    _pcu_reset_spies();
    pcu_reset_all();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(4U, g_set_calls_count, "pcu_reset_all should write 4 pins");
    _assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "AIR- reset mismatch");
    _assert_set_call(1U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "Precharge reset mismatch");
    _assert_set_call(2U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "AIR+ reset mismatch");
    _assert_set_call(3U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "AMS reset mismatch");
}

void test_pcu_airn_open_sets_high(void) {
    _pcu_reset_spies();
    pcu_airn_open();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH, "pcu_airn_open mismatch");
}

void test_pcu_airn_close_sets_low(void) {
    _pcu_reset_spies();
    pcu_airn_close();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_LOW, "pcu_airn_close mismatch");
}

void test_pcu_airp_open_sets_high(void) {
    _pcu_reset_spies();
    pcu_airp_open();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH, "pcu_airp_open mismatch");
}

void test_pcu_airp_close_sets_low(void) {
    _pcu_reset_spies();
    pcu_airp_close();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW, "pcu_airp_close mismatch");
}

void test_pcu_precharge_start_sets_low(void) {
    _pcu_reset_spies();
    pcu_precharge_start();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_LOW, "pcu_precharge_start mismatch");
}

void test_pcu_precharge_stop_sets_high(void) {
    _pcu_reset_spies();
    pcu_precharge_stop();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH, "pcu_precharge_stop mismatch");
}

void test_pcu_ams_activate_sets_low(void) {
    _pcu_reset_spies();
    pcu_ams_activate();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AMS, PCU_PIN_STATUS_LOW, "pcu_ams_activate mismatch");
}

void test_pcu_ams_deactivate_sets_high(void) {
    _pcu_reset_spies();
    pcu_ams_deactivate();

    TEST_ASSERT_EQUAL_UINT32(1U, g_set_calls_count);
    _assert_set_call(0U, PCU_PIN_AMS, PCU_PIN_STATUS_HIGH, "pcu_ams_deactivate mismatch");
}

void test_pcu_airn_timeout_sets_event_type(void) {
    hpcu.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    _pcu_airn_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_AIRN_TIMEOUT,
        hpcu.timeout_event.type,
        "AIR- timeout should set AIRN_TIMEOUT event");
}

void test_pcu_precharge_timeout_sets_event_type(void) {
    hpcu.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    _pcu_precharge_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_PRECHARGE_TIMEOUT,
        hpcu.timeout_event.type,
        "Precharge timeout should set PRECHARGE_TIMEOUT event");
}

void test_pcu_airp_timeout_sets_event_type(void) {
    hpcu.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    _pcu_airp_timeout();
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_AIRP_TIMEOUT,
        hpcu.timeout_event.type,
        "AIR+ timeout should set AIRP_TIMEOUT event");
}

void test_pcu_set_state_from_ecu_handle_null(void) {
    hpcu.event.type = FSM_EVENT_TYPE_IGNORED;
    pcu_set_state_from_ecu_handle(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        hpcu.event.type,
        "NULL ECU payload should not change event");
}

void test_pcu_set_state_from_ecu_handle_on(void) {
    primary_hv_set_status_ecu_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = true;

    pcu_set_state_from_ecu_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_ON,
        hpcu.event.type,
        "ECU status=true should map to TS_ON");
}

void test_pcu_set_state_from_ecu_handle_off(void) {
    primary_hv_set_status_ecu_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = false;

    pcu_set_state_from_ecu_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_OFF,
        hpcu.event.type,
        "ECU status=false should map to TS_OFF");
}

void test_pcu_set_state_from_handcart_handle_null(void) {
    hpcu.event.type = FSM_EVENT_TYPE_IGNORED;
    pcu_set_state_from_handcart_handle(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_IGNORED,
        hpcu.event.type,
        "NULL handcart payload should not change event");
}

void test_pcu_set_state_from_handcart_handle_on(void) {
    primary_hv_set_status_handcart_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = true;

    pcu_set_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_ON,
        hpcu.event.type,
        "Handcart status=true should map to TS_ON");
}

void test_pcu_set_state_from_handcart_handle_off(void) {
    primary_hv_set_status_handcart_converted_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.status = false;

    pcu_set_state_from_handcart_handle(&payload);

    TEST_ASSERT_EQUAL_MESSAGE(
        FSM_EVENT_TYPE_TS_OFF,
        hpcu.event.type,
        "Handcart status=false should map to TS_OFF");
}

#ifdef PCU_TESTS

void setUp(void) {
    timebase_init(500U);
    _pcu_reset_spies();
    (void)pcu_init(_pcu_set_spy, _pcu_toggle_spy);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pcu_init_null_set_callback);
    RUN_TEST(test_pcu_init_null_toggle_callback);
    RUN_TEST(test_pcu_init_ok);
    RUN_TEST(test_pcu_init_event_type_ignored);
    RUN_TEST(test_pcu_init_sets_default_pins_high);
    RUN_TEST(test_pcu_reset_all_sets_default_pins_high);
    RUN_TEST(test_pcu_airn_open_sets_high);
    RUN_TEST(test_pcu_airn_close_sets_low);
    RUN_TEST(test_pcu_airp_open_sets_high);
    RUN_TEST(test_pcu_airp_close_sets_low);
    RUN_TEST(test_pcu_precharge_start_sets_low);
    RUN_TEST(test_pcu_precharge_stop_sets_high);
    RUN_TEST(test_pcu_ams_activate_sets_low);
    RUN_TEST(test_pcu_ams_deactivate_sets_high);
    RUN_TEST(test_pcu_airn_timeout_sets_event_type);
    RUN_TEST(test_pcu_precharge_timeout_sets_event_type);
    RUN_TEST(test_pcu_airp_timeout_sets_event_type);
    RUN_TEST(test_pcu_set_state_from_ecu_handle_null);
    RUN_TEST(test_pcu_set_state_from_ecu_handle_on);
    RUN_TEST(test_pcu_set_state_from_ecu_handle_off);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_null);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_on);
    RUN_TEST(test_pcu_set_state_from_handcart_handle_off);
    return UNITY_END();
}

#endif // PCU_TESTS