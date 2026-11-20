/*!
 * \file pcu-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU): drives the relays and the AMS output on behalf of the FSM
 */

#include "pcu-api.h"

#include <string.h>

#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-status-api.h"

EAGLETRT_STATIC struct PcuHandler pcu_handler;

/*! \brief Fixed description of every relay, indexed by \ref PcuRelay */
EAGLETRT_STATIC const struct PcuRelayInfo pcu_relays[PCU_RELAY_COUNT] = {
    [PCU_RELAY_AIR_NEGATIVE] = {
                                .pin = PCU_PIN_AIR_NEGATIVE,
                                .commanded = MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED,
                                .done = MAINBOARD_STATUS_CONDITION_AIRN_CLOSED,
                                .timeout = PCU_AIRN_TIMEOUT_MS,
                                },
    [PCU_RELAY_PRECHARGE] = {
                                .pin = PCU_PIN_PRECHARGE,
                                .commanded = MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED,
                                .done = MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE,
                                .timeout = PCU_PRECHARGE_TIMEOUT_MS,
                                },
    [PCU_RELAY_AIR_POSITIVE] = {
                                .pin = PCU_PIN_AIR_POSITIVE,
                                .commanded = MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED,
                                .done = MAINBOARD_STATUS_CONDITION_AIRP_CLOSED,
                                .timeout = PCU_AIRP_TIMEOUT_MS,
                                },
};

enum PcuReturnCode pcu_api_init(const pcu_set_state_callback set, const pcu_get_tick_callback tick, const system_reset_callback reset) {
    if (set == NULL || tick == NULL || reset == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    memset(&pcu_handler, 0U, sizeof(pcu_handler));

    pcu_handler.set = set;
    pcu_handler.tick = tick; // TODO: remove and use watchdogs for everything
    pcu_handler.reset = reset;

    pcu_api_open_all();
    pcu_handler.set(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);
    return PCU_RC_OK;
}

enum PcuReturnCode pcu_api_close(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return PCU_RC_INVALID_RELAY;
    }
    if (pcu_handler.set == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    // The timeout counts from the first close command, repeating it does not extend it
    if (!pcu_handler.closed[relay]) {
        pcu_handler.close_time[relay] = pcu_api_get_tick();
        pcu_handler.closed[relay] = true;
    }
    pcu_handler.set(pcu_relays[relay].pin, PCU_PIN_STATUS_LOW);
    (void)mainboard_status_api_set_condition(pcu_relays[relay].commanded, true);
    return PCU_RC_OK;
}

enum PcuReturnCode pcu_api_open(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return PCU_RC_INVALID_RELAY;
    }
    if (pcu_handler.set == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    pcu_handler.set(pcu_relays[relay].pin, PCU_PIN_STATUS_HIGH);
    pcu_handler.closed[relay] = false;
    (void)mainboard_status_api_set_condition(pcu_relays[relay].commanded, false);
    return PCU_RC_OK;
}

void pcu_api_open_all(void) {
    // Opened in the reverse order of the TS on sequence
    (void)pcu_api_open(PCU_RELAY_AIR_POSITIVE);
    (void)pcu_api_open(PCU_RELAY_PRECHARGE);
    (void)pcu_api_open(PCU_RELAY_AIR_NEGATIVE);
}

bool pcu_api_is_closed(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return false;
    }
    return pcu_handler.closed[relay];
}

bool pcu_api_is_timed_out(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT || !pcu_handler.closed[relay]) {
        return false;
    }
    if (mainboard_status_api_get_condition(pcu_relays[relay].done)) {
        return false;
    }
    // Unsigned subtraction, correct across the counter overflow
    return (pcu_api_get_tick() - pcu_handler.close_time[relay]) > pcu_relays[relay].timeout;
}

void pcu_api_ams_activate(void) {
    if (pcu_handler.set == NULL) {
        return;
    }
    pcu_handler.set(PCU_PIN_AMS, PCU_PIN_STATUS_LOW);
}

milliseconds pcu_api_get_tick(void) {
    if (pcu_handler.tick == NULL) {
        return 0U;
    }
    return pcu_handler.tick();
}

void pcu_api_reset_microcontroller(void) {
    if (pcu_handler.reset == NULL) {
        return;
    }
    pcu_handler.reset();
}
