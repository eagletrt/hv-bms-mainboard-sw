/**
 * @file pcu.c
 * @date 2024-07-09
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#include "pcu.h"

#include <string.h>

#include "watchdog.h"
#include "timebase.h"
#include "fsm.h"

#ifdef CONF_PCU_MODULE_ENABLE

/**
 * @brief PCU handler structure
 *
 * @param set A pointer to the function callback used to set a PCU pin
 * @param toggle A pointer to the function callback used to toggle a PCU pin
 * @param precharge_watchdog Precharge watchdog
 */
_STATIC struct {
    pcu_set_state_callback_t set;
    pcu_toggle_state_callback_t toggle;

    fsm_event_data_t event;
    fsm_event_data_t timeout_event;

    Watchdog airn_watchdog;
    Watchdog precharge_watchdog;
    Watchdog airp_watchdog;
} hpcu;

void _pcu_airn_timeout(void) {
    // Send AIR- timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_AIRN_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

/**
 * @brief Callback executed when the precharge watchdog times out
 */
void _pcu_precharge_timeout(void) {
    // Send precharge timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_PRECHARGE_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

void _pcu_airp_timeout(void) {
    // Send AIR+ timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_AIRP_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

void _pcu_init_watchdogs(void) {
    milliseconds_t res = timebase_get_resolution();
    watchdog_init(
        &hpcu.airn_watchdog,
        TIMEBASE_TIME_TO_TICKS(PCU_AIRN_TIMEOUT, res),
        _pcu_airn_timeout
    );
    watchdog_init(
        &hpcu.precharge_watchdog,
        TIMEBASE_TIME_TO_TICKS(PCU_PRECHARGE_TIMEOUT, res),
        _pcu_precharge_timeout
    );
    watchdog_init(
        &hpcu.airp_watchdog,
        TIMEBASE_TIME_TO_TICKS(PCU_AIRP_TIMEOUT, res),
        _pcu_airp_timeout
    );
}
void _pcu_deinit_watchdogs(void) {
    watchdog_deinit(&hpcu.airn_watchdog);
    watchdog_deinit(&hpcu.precharge_watchdog);
    watchdog_deinit(&hpcu.airp_watchdog);
}

PcuReturnCode pcu_init(pcu_set_state_callback_t set, pcu_toggle_state_callback_t toggle) {
    if (set == NULL || toggle == NULL)
        return PCU_NULL_POINTER;
    memset(&hpcu, 0U, sizeof(hpcu));

    hpcu.set = set;
    hpcu.toggle = toggle;
    hpcu.event.type = FSM_EVENT_TYPE_IGNORED;

    // Reset all gpios
    pcu_reset_all();
    return PCU_OK;
}

void pcu_reset_all(void) {
    _pcu_deinit_watchdogs();

    // Set all pins to their default state
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
    hpcu.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_LOW);
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
    hpcu.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);

    hpcu.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    _pcu_init_watchdogs();
}

void pcu_airn_open(void) {
    watchdog_stop(&hpcu.airn_watchdog);
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_airn_close(void) {
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_LOW);
    watchdog_start(&hpcu.airn_watchdog);
}
void pcu_airn_stop_watchdog(void) {
    watchdog_stop(&hpcu.airn_watchdog);
}

void pcu_airp_open(void) {
    watchdog_stop(&hpcu.airp_watchdog);
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_airp_close(void) {
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW);
    watchdog_start(&hpcu.airp_watchdog);
}
void pcu_airp_stop_watchdog(void) {
    watchdog_stop(&hpcu.airp_watchdog);
}

// TODO: Handler watchdog return codes
void pcu_precharge_start(void) {
    watchdog_start(&hpcu.precharge_watchdog);
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_precharge_stop(void) {
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW);
    watchdog_stop(&hpcu.precharge_watchdog);
}
void pcu_precharge_stop_watchdog(void) {
    watchdog_stop(&hpcu.precharge_watchdog);
}

void pcu_ams_activate(void) {
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW);
}
void pcu_ams_deactivate(void) {
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
}

void pcu_set_state_from_ecu_handle(primary_hv_set_status_ecu_converted_t * payload) {
    if (payload == NULL)
        return;
    hpcu.event.type = payload->status ?
        FSM_EVENT_TYPE_TS_ON :
        FSM_EVENT_TYPE_TS_OFF;
    fsm_event_trigger(&hpcu.event);
}

void pcu_set_state_from_handcart_handle(primary_hv_set_status_handcart_converted_t * payload) {
    if (payload == NULL)
        return;
    hpcu.event.type = payload->status ?
        FSM_EVENT_TYPE_TS_ON :
        FSM_EVENT_TYPE_TS_OFF;
    fsm_event_trigger(&hpcu.event);
}

#ifdef CONF_PCU_STRING_ENABLE

_STATIC char * pcu_module_name = "pcu";

_STATIC char * pcu_return_code_name[] = {
    [PCU_OK] = "ok",
    [PCU_NULL_POINTER] = "null pointer",
};

_STATIC char * pcu_return_code_description[] = {
    [PCU_OK] = "executed succesfully",
    [PCU_NULL_POINTER] = "attempt to dereference a NULL pointer",
};

#endif // CONF_PCU_STRING_ENABLE

#endif // CONF_PCU_MODULE_ENABLE
