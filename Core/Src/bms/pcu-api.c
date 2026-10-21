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

#include "error-api.h"
#include "timebase.h"
#include "fsm.h"
#include "internal-voltage-api.h"
#include "eagletrt.h"
#include "logger-api.h"
#include "mainboard-watchdogs-api.h"

#ifdef CONF_PCU_MODULE_ENABLE

EAGLETRT_STATIC struct PcuHandler pcu_handler;

/*! \brief Callback executed when the AIR- watchdog times out */
void prv_pcu_api_airn_timeout(void) {
    // Send AIR- timeout event to the FSM
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_AIRN_TIMEOUT;
    fsm_event_trigger(&pcu_handler.timeout_event);
}

/*! \brief Callback executed when the precharge watchdog times out */
void prv_pcu_api_precharge_timeout(void) {
    // Send precharge timeout event to the FSM
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_PRECHARGE_TIMEOUT;
    fsm_event_trigger(&pcu_handler.timeout_event);
}

/*! \brief Callback executed when the AIR+ watchdog times out */
void prv_pcu_api_airp_timeout(void) {
    // Send AIR+ timeout event to the FSM
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_AIRP_TIMEOUT;
    fsm_event_trigger(&pcu_handler.timeout_event);
}

/*! \brief Callback executed when the TSON watchdog times out */
void prv_pcu_api_ecu_timeout(void) {
    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_ECU_TIMEOUT;
    fsm_event_trigger(&pcu_handler.timeout_event);
}

/*!
 * \brief Initialize all the watchdogs
 *
 * \return True if all the watchdogs were initialized, false otherwise
 */
EAGLETRT_STATIC bool prv_pcu_api_init_watchdogs(void) {
    return mainboard_watchdogs_api_init_watchdog(
               &pcu_handler.airn_watchdog,
               PCU_AIRN_TIMEOUT_MS,
               prv_pcu_api_airn_timeout) == WATCHDOG_RC_OK &&
           mainboard_watchdogs_api_init_watchdog(
               &pcu_handler.precharge_watchdog,
               PCU_PRECHARGE_TIMEOUT_MS,
               prv_pcu_api_precharge_timeout) == WATCHDOG_RC_OK &&
           mainboard_watchdogs_api_init_watchdog(
               &pcu_handler.airp_watchdog,
               PCU_AIRP_TIMEOUT_MS,
               prv_pcu_api_airp_timeout) == WATCHDOG_RC_OK &&
           mainboard_watchdogs_api_init_watchdog(
               &pcu_handler.ecu_watchdog,
               PCU_TSON_TIMEOUT_MS,
               prv_pcu_api_ecu_timeout) == WATCHDOG_RC_OK;
}

/*!
 * \brief Stop the pin watchdogs and clear their timed-out state, they stay initialized
 *
 * \details The ECU watchdog is not touched because it monitors the ECU and not a pin
 */
EAGLETRT_STATIC void prv_pcu_api_reset_pin_watchdogs(void) {
    (void)mainboard_watchdogs_api_reset(&pcu_handler.airn_watchdog);
    (void)mainboard_watchdogs_api_reset(&pcu_handler.precharge_watchdog);
    (void)mainboard_watchdogs_api_reset(&pcu_handler.airp_watchdog);
}

/*!
 * \brief Check the result of a start or stop of a pin watchdog
 *
 * \details Starting a running watchdog, or stopping a stopped or timed out one, is harmless.
 *          Anything else means the scheduler failed so it is at least reported
 *
 * \param name The name of the watchdog
 * \param code The return code of the operation
 */
EAGLETRT_STATIC void prv_pcu_api_check_watchdog(const char *const name, const enum WatchdogReturnCode code) {
    switch (code) {
        case WATCHDOG_RC_OK:
        case WATCHDOG_RC_BUSY:
        case WATCHDOG_RC_NOT_RUNNING:
        case WATCHDOG_RC_TIMED_OUT:
            break;
        default:
            logger_api_log(LOGGER_LEVEL_ERROR, "PCU %s watchdog error: %d", name, (int)code);
            break;
    }
}

enum PcuReturnCode pcu_api_init(const pcu_set_state_callback set, const pcu_toggle_state_callback toggle) {
    if (set == NULL || toggle == NULL) {
        return PCU_RC_NULL_POINTER;
    }

    // Unregister the watchdogs before wiping them, otherwise the scheduler keeps dangling entries
    prv_pcu_api_reset_pin_watchdogs();
    (void)mainboard_watchdogs_api_reset(&pcu_handler.ecu_watchdog);
    memset(&pcu_handler, 0U, sizeof(pcu_handler));

    if (!prv_pcu_api_init_watchdogs()) {
        return PCU_RC_WATCHDOG_ERROR;
    }

    pcu_handler.set = set;
    pcu_handler.toggle = toggle;
    pcu_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    // Reset all gpios
    pcu_api_reset_all();

    // TODO: Stop the watchdog if handcart is connected or use different message?
    //watchdog_start(&pcu_handler.ecu_watchdog);
    return PCU_RC_OK;
}

void pcu_api_reset_all(void) {
    // A timed-out watchdog can be started again only after it is reset
    prv_pcu_api_reset_pin_watchdogs();

    // Set all pins to their default state
    pcu_handler.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
    pcu_handler.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH);
    pcu_handler.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
    pcu_handler.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);

    pcu_handler.timeout_event.type = FSM_EVENT_TYPE_IGNORED;
}

void pcu_api_airn_open(void) {
    prv_pcu_api_check_watchdog("airn", mainboard_watchdogs_api_stop(&pcu_handler.airn_watchdog));
    pcu_handler.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_api_airn_close(void) {
    pcu_handler.set(PCU_PIN_AIR_NEGATIVE, PCU_PIN_STATUS_LOW);
    prv_pcu_api_check_watchdog("airn", mainboard_watchdogs_api_start(&pcu_handler.airn_watchdog));
}
void pcu_api_airn_stop_watchdog(void) {
    prv_pcu_api_check_watchdog("airn", mainboard_watchdogs_api_stop(&pcu_handler.airn_watchdog));
}

void pcu_api_airp_open(void) {
    prv_pcu_api_check_watchdog("airp", mainboard_watchdogs_api_stop(&pcu_handler.airp_watchdog));
    pcu_handler.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_HIGH);
}
void pcu_api_airp_close(void) {
    pcu_handler.set(PCU_PIN_AIR_POSITIVE, PCU_PIN_STATUS_LOW);
    prv_pcu_api_check_watchdog("airp", mainboard_watchdogs_api_start(&pcu_handler.airp_watchdog));
}
void pcu_api_airp_stop_watchdog(void) {
    prv_pcu_api_check_watchdog("airp", mainboard_watchdogs_api_stop(&pcu_handler.airp_watchdog));
}

void pcu_api_precharge_start(void) {
    prv_pcu_api_check_watchdog("precharge", mainboard_watchdogs_api_start(&pcu_handler.precharge_watchdog));
    pcu_handler.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_LOW);
}
void pcu_api_precharge_stop(void) {
    pcu_handler.set(PCU_PIN_PRECHARGE, PCU_PIN_STATUS_HIGH);
    prv_pcu_api_check_watchdog("precharge", mainboard_watchdogs_api_stop(&pcu_handler.precharge_watchdog));
}
void pcu_api_precharge_stop_watchdog(void) {
    prv_pcu_api_check_watchdog("precharge", mainboard_watchdogs_api_stop(&pcu_handler.precharge_watchdog));
}

void pcu_api_ams_activate(void) {
    pcu_handler.set(PCU_PIN_AMS, PCU_PIN_STATUS_LOW);
}
void pcu_api_ams_deactivate(void) {
    pcu_handler.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);
}

precise_percentage_t pcu_api_get_precharge_percentage(void) {
    volt_t tractive_system = internal_voltage_api_get_ts();
    volt_t batt = internal_voltage_api_get_pack();

    if (batt == 0) {
        return 0;
    }

    return tractive_system / batt;
}

bool pcu_api_is_precharge_complete(void) {
    return pcu_api_get_precharge_percentage() >= PCU_PRECHARGE_THRESHOLD_PERCENT;
}

void pcu_api_bms_set_handle(bool tson) {
    pcu_handler.event.type = tson ? FSM_EVENT_TYPE_TS_ON : FSM_EVENT_TYPE_TS_OFF;
    fsm_event_trigger(&pcu_handler.event);
}

void pcu_api_ecu_fsm_handle(void) {
    (void)mainboard_watchdogs_api_feed(&pcu_handler.ecu_watchdog);
}

void pcu_api_print_log(void) {
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
    logger_api_log(LOGGER_LEVEL_EMPTY, "PCU report");
    logger_api_log(LOGGER_LEVEL_INFO, "FSM state: %s", fsm_state_names[fsm_get_status() < FSM_NUM_STATES ? fsm_get_status() : FSM_STATE_IDLE]);
    logger_api_log(LOGGER_LEVEL_INFO, "Precharge percentage: %.1f %%", pcu_api_get_precharge_percentage() * 100.0f);
    logger_api_log(LOGGER_LEVEL_INFO, "Precharge complete: %s", pcu_api_is_precharge_complete() ? "yes" : "no");
    logger_api_log(
        LOGGER_LEVEL_INFO,
        "AIR- watchdog: %s%s",
        mainboard_watchdogs_api_is_running(&pcu_handler.airn_watchdog) ? "running" : "stopped",
        mainboard_watchdogs_api_is_timed_out(&pcu_handler.airn_watchdog) ? ", timed out" : "");
    logger_api_log(
        LOGGER_LEVEL_INFO,
        "Precharge watchdog: %s%s",
        mainboard_watchdogs_api_is_running(&pcu_handler.precharge_watchdog) ? "running" : "stopped",
        mainboard_watchdogs_api_is_timed_out(&pcu_handler.precharge_watchdog) ? ", timed out" : "");
    logger_api_log(
        LOGGER_LEVEL_INFO,
        "AIR+ watchdog: %s%s",
        mainboard_watchdogs_api_is_running(&pcu_handler.airp_watchdog) ? "running" : "stopped",
        mainboard_watchdogs_api_is_timed_out(&pcu_handler.airp_watchdog) ? ", timed out" : "");
    logger_api_log(
        LOGGER_LEVEL_INFO,
        "ECU watchdog: %s%s",
        mainboard_watchdogs_api_is_running(&pcu_handler.ecu_watchdog) ? "running" : "stopped",
        mainboard_watchdogs_api_is_timed_out(&pcu_handler.ecu_watchdog) ? ", timed out" : "");
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
}

#ifdef CONF_PCU_STRING_ENABLE

EAGLETRT_STATIC char *pcu_module_name = "pcu";

EAGLETRT_STATIC char *pcu_return_code_name[] = {
    [PCU_RC_OK] = "ok",
    [PCU_RC_NULL_POINTER] = "null pointer"
};

EAGLETRT_STATIC char *pcu_return_code_description[] = {
    [PCU_RC_OK] = "executed succesfully",
    [PCU_RC_NULL_POINTER] = "attempt to dereference a NULL pointer"
};

#endif // CONF_PCU_STRING_ENABLE

#endif // CONF_PCU_MODULE_ENABLE
