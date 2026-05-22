/*!
 * \file pcu-api.c
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU) used to manage the main logic of the pack
 */

#include "pcu-api.h"

#include <string.h>

#include "timebase.h"
#include "fsm.h"
#include "internal-voltage.h"

#ifdef CONF_PCU_MODULE_ENABLE

EAGLETRT_STATIC struct PcuHandler hpcu;

/*! \brief Callback executed when the AIR- watchdog times out */
void prv_pcu_api_airn_timeout(void) {
    // Send AIR- timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_AIRN_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

/*! \brief Callback executed when the precharge watchdog times out */
void prv_pcu_api_precharge_timeout(void) {
    // Send precharge timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_PRECHARGE_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

/*! \brief Callback executed when the AIR+ watchdog times out */
void prv_pcu_api_airp_timeout(void) {
    // Send AIR+ timeout event to the FSM
    hpcu.timeout_event.type = FSM_EVENT_TYPE_AIRP_TIMEOUT;
    fsm_event_trigger(&hpcu.timeout_event);
}

/*! \brief Initialize all the watchdogs */
void prv_pcu_api_init_watchdogs(void) {
    const milliseconds_t res = timebase_get_resolution();
    watchdog_init(
        &hpcu.airn_watchdog,
        TIMEBASE_TIME_TO_TICKS(pcu_api_airn_TIMEOUT_MS, res),
        prv_pcu_api_airn_timeout);
    watchdog_init(
        &hpcu.precharge_watchdog,
        TIMEBASE_TIME_TO_TICKS(PCU_PRECHARGE_TIMEOUT_MS, res),
        prv_pcu_api_precharge_timeout);
    watchdog_init(
        &hpcu.airp_watchdog,
        TIMEBASE_TIME_TO_TICKS(pcu_api_airp_TIMEOUT_MS, res),
        prv_pcu_api_airp_timeout);
}
/*! \brief Uninitialize all the watchdogs to be able to use them again */
void prv_pcu_api_deinit_watchdogs(void) {
    watchdog_deinit(&hpcu.airn_watchdog);
    watchdog_deinit(&hpcu.precharge_watchdog);
    watchdog_deinit(&hpcu.airp_watchdog);
}

enum PcuReturnCode pcu_api_init(const pcu_set_state_callback set, const pcu_toggle_state_callback toggle) {
    if (set == NULL || toggle == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    memset(&hpcu, 0U, sizeof(hpcu));

    hpcu.set = set;
    hpcu.toggle = toggle;
    hpcu.event.type = FSM_EVENT_TYPE_IGNORED;

    // Reset all gpios
    pcu_api_reset_all();
    return PCU_RC_OK;
}

void pcu_api_reset_all(void) {
    /*
     * If a watchdog has timed-out, to use it again it needs to be uninitialized
     * an then initialized again
     */
    prv_pcu_api_deinit_watchdogs();

    // Set all pins to their default state
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
    hpcu.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH);
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
    hpcu.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);

    hpcu.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
    prv_pcu_api_init_watchdogs();
}

void pcu_api_airn_open(void) {
    watchdog_stop(&hpcu.airn_watchdog);
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_api_airn_close(void) {
    hpcu.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_LOW);
    watchdog_start(&hpcu.airn_watchdog);
}
void pcu_api_airn_stop_watchdog(void) {
    watchdog_stop(&hpcu.airn_watchdog);
}

void pcu_api_airp_open(void) {
    watchdog_stop(&hpcu.airp_watchdog);
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_api_airp_close(void) {
    hpcu.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW);
    watchdog_start(&hpcu.airp_watchdog);
}
void pcu_api_airp_stop_watchdog(void) {
    watchdog_stop(&hpcu.airp_watchdog);
}

// TODO: Handler watchdog return codes
void pcu_api_precharge_start(void) {
    watchdog_start(&hpcu.precharge_watchdog);
    hpcu.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_LOW);
}
void pcu_api_precharge_stop(void) {
    hpcu.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH);
    watchdog_stop(&hpcu.precharge_watchdog);
}
void pcu_api_precharge_stop_watchdog(void) {
    watchdog_stop(&hpcu.precharge_watchdog);
}

void pcu_api_ams_activate(void) {
    hpcu.set(PCU_PIN_AMS, PCU_PIN_STATUS_LOW);
}
void pcu_api_ams_deactivate(void) {
    hpcu.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);
}

precise_percentage_t pcu_api_get_precharge_percentage(void) {
    volt_t tractive_system = internal_voltage_get_ts();
    volt_t batt = internal_voltage_get_pack();
    return tractive_system / batt;
}

bool pcu_api_is_precharge_complete(void) {
    return pcu_api_get_precharge_percentage() >= PCU_PRECHARGE_THRESHOLD_PERCENT;
}

// TODO: Add watchdog for the set state canlib message
void pcu_api_set_state_from_ecu_handle(primary_hv_set_status_ecu_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    hpcu.event.type = payload->status ? FSM_EVENT_TYPE_TS_ON : FSM_EVENT_TYPE_TS_OFF;
    fsm_event_trigger(&hpcu.event);
}

void pcu_api_set_state_from_handcart_handle(primary_hv_set_status_handcart_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    hpcu.event.type = payload->status ? FSM_EVENT_TYPE_TS_ON : FSM_EVENT_TYPE_TS_OFF;
    fsm_event_trigger(&hpcu.event);
}

#ifdef CONF_PCU_STRING_ENABLE

EAGLETRT_STATIC char *pcu_module_name = "pcu";

EAGLETRT_STATIC char *pcu_return_code_name[] = {
    [PCU_RC_OK] = "ok",
    [PCU_RC_NULL_POINTER] = "null pointer",
};

EAGLETRT_STATIC char *pcu_return_code_description[] = {
    [PCU_RC_OK] = "executed succesfully",
    [PCU_RC_NULL_POINTER] = "attempt to dereference a NULL pointer",
};

#endif // CONF_PCU_STRING_ENABLE

#endif // CONF_PCU_MODULE_ENABLE
