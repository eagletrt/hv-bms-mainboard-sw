#include "unity.h"
#include "fsm.h"
#include "post.h"
#include "led.h"
#include "pcu.h"
#include "can-comm.h"
#include "programmer.h"
#include "imd.h"
#include "feedback.h"

#include <stdio.h>
#include <string.h>

#define SET_BIT(bit) _called |= (1U << bit)

extern _FsmHandler hfsm;

uint16_t _called;
PostInitData _data;

void _pcu_set(PcuPin pcu_pin, PcuPinStatus state) {SET_BIT(0);}
void _pcu_toggle(PcuPin pcu_pin) {SET_BIT(1);}

CanCommReturnCode _can_send(
    CanNetwork network, 
    can_id_t id, 
    CanFrameType frame_type, 
    const uint8_t *data, 
    size_t size
) {
    SET_BIT(2);
    return CAN_COMM_OK;
}

void _system_reset(void) {SET_BIT(3);}

void _led_set(LedId led, LedStatus state) {SET_BIT(4);}
void _led_toggle(LedId led) {SET_BIT(5);}

void _imd_start(void) {SET_BIT(6);}

bit_flag32_t _feedback_read_all(void) {
    SET_BIT(7);
    return 0U; 
}
void _feedback_start_conversion(void) {
    SET_BIT(8);
}

void _update_timer(uint32_t timestamp, uint16_t timeout) {
    SET_BIT(9);
}
void _stop_timer(void) {
    SET_BIT(10);
}

void _interrupt_critical_section_enter(void) {
    SET_BIT(11);
}
void _interrupt_critical_section_exit(void) {
    SET_BIT(12);
}

void setUp() {
    _called = 0;
    _data = (PostInitData){
        .system_reset = _system_reset,
        .can_send = _can_send,
        .led_set = _led_set,
        .led_toggle = _led_toggle,
        .imd_start = _imd_start,
        .feedback_read_all = _feedback_read_all,
        .feedback_start_conversion = _feedback_start_conversion,
        .pcu_set = _pcu_set,
        .pcu_toggle = _pcu_toggle,
        .cs_enter = _interrupt_critical_section_enter,
        .cs_exit = _interrupt_critical_section_exit,
        .error_update_timer = _update_timer,
        .error_stop_timer = _stop_timer
    };

    fsm_do_init(&_data);
}

void tearDown() {}

void test_fsm_do_init_fsm_state() {
    fsm_state_t state = fsm_do_init(NULL);
    TEST_ASSERT_EQUAL(FSM_STATE_INIT, hfsm.fsm_state);
}

void test_fsm_do_init_state_fatal() {
    TEST_ASSERT_EQUAL(FSM_STATE_FATAL, fsm_do_init(NULL));
}

void test_fsm_do_init_state_idle() { 
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_init(&_data));
}

void test_fsm_do_init_initialized() {
    TEST_ASSERT_BITS(0x7ff, 0x41, _called);
}

void test_fsm_do_idle_no_change() {
    TEST_ASSERT_EQUAL(FSM_NO_CHANGE, fsm_do_idle(&_data));
}

void test_fsm_do_init_state_flash() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_FLASH_REQUEST,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_FLASH, fsm_do_idle(&_data));
}

void test_fsm_do_init_state_airn_check() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_TS_ON,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_AIRN_CHECK, fsm_do_idle(&_data));
}

void test_fsm_do_precharge_check_no_change() {
    TEST_ASSERT_EQUAL(FSM_NO_CHANGE, fsm_do_precharge_check(&_data));
}

void test_fsm_do_precharge_check_timeout_idle() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_PRECHARGE_TIMEOUT,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_precharge_check(&_data));
}

void test_fsm_do_precharge_check_ts_off_idle() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_TS_OFF,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_precharge_check(&_data));
}

void test_fsm_do_airp_check_no_change() {
    TEST_ASSERT_EQUAL(FSM_NO_CHANGE, fsm_do_airp_check(&_data));
}

void test_fsm_do_airp_check_timeout_idle() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_AIRP_TIMEOUT,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_airp_check(&_data));
}

void test_fsm_do_airp_check_ts_off_idle() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_TS_OFF,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_airp_check(&_data));
}

void test_fsm_do_ts_on_no_change() {
    TEST_ASSERT_EQUAL(FSM_NO_CHANGE, fsm_do_ts_on(&_data));
}

void test_fsm_do_ts_on_ts_off_idle() {
    fsm_event_data_t event = {
        .type = FSM_EVENT_TYPE_TS_OFF,
    };
    fsm_event_trigger(&event);
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, fsm_do_ts_on(&_data));
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_fsm_do_init_fsm_state);
    RUN_TEST(test_fsm_do_init_state_fatal);
    RUN_TEST(test_fsm_do_init_state_idle);
    RUN_TEST(test_fsm_do_init_initialized);

    RUN_TEST(test_fsm_do_idle_no_change);
    RUN_TEST(test_fsm_do_init_state_flash);
    RUN_TEST(test_fsm_do_init_state_airn_check);

    RUN_TEST(test_fsm_do_precharge_check_no_change);
    RUN_TEST(test_fsm_do_precharge_check_timeout_idle);
    RUN_TEST(test_fsm_do_precharge_check_ts_off_idle);

    RUN_TEST(test_fsm_do_airp_check_no_change);
    RUN_TEST(test_fsm_do_airp_check_timeout_idle);
    RUN_TEST(test_fsm_do_airp_check_ts_off_idle);
    
    RUN_TEST(test_fsm_do_ts_on_no_change);
    RUN_TEST(test_fsm_do_ts_on_ts_off_idle);
    
    return UNITY_END();
}
