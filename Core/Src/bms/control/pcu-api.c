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

#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-logger-api.h"

EAGLETRT_STATIC struct PcuHandler pcu_handler;

/*! \brief Fixed description of every relay, indexed by \ref PcuRelay */
EAGLETRT_STATIC const struct PcuRelayInfo pcu_relays[PCU_RELAY_COUNT] = {
    [PCU_RELAY_AIR_NEGATIVE] = { .pin = PCU_PIN_AIR_NEGATIVE },
    [PCU_RELAY_PRECHARGE] = { .pin = PCU_PIN_PRECHARGE },
    [PCU_RELAY_AIR_POSITIVE] = { .pin = PCU_PIN_AIR_POSITIVE },
};

/*! \brief Name of every relay, used by the logs */
EAGLETRT_STATIC const char *const pcu_relay_names[PCU_RELAY_COUNT] = {
    [PCU_RELAY_AIR_NEGATIVE] = "AIR-",
    [PCU_RELAY_PRECHARGE] = "precharge",
    [PCU_RELAY_AIR_POSITIVE] = "AIR+",
};

enum PcuReturnCode pcu_api_init(const pcu_set_state_callback set, const system_reset_callback reset) {
    if (set == NULL || reset == NULL) {
        MAINBOARD_LOGGER_ERROR("PCU: NULL callback");
        return PCU_RC_NULL_POINTER;
    }
    memset(&pcu_handler, 0U, sizeof(pcu_handler));

    pcu_handler.set_pin = set;
    pcu_handler.reset_mcu = reset;

    pcu_api_open_all();
    pcu_handler.set_pin(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);
    MAINBOARD_LOGGER_INFO("PCU: relays open, AMS released");
    return PCU_RC_OK;
}

enum PcuReturnCode pcu_api_close(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return PCU_RC_INVALID_RELAY;
    }
    if (pcu_handler.set_pin == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    // The FSM commands the relay at every run, only the change is logged
    if (!pcu_handler.closed[relay]) {
        MAINBOARD_LOGGER_INFO("PCU: closing %s", pcu_relay_names[relay]);
    }
    pcu_handler.set_pin(pcu_relays[relay].pin, PCU_PIN_STATUS_LOW);
    pcu_handler.closed[relay] = true;
    return PCU_RC_OK;
}

enum PcuReturnCode pcu_api_open(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return PCU_RC_INVALID_RELAY;
    }
    if (pcu_handler.set_pin == NULL) {
        return PCU_RC_NULL_POINTER;
    }
    // The FSM commands the relay at every run, only the change is logged
    if (pcu_handler.closed[relay]) {
        MAINBOARD_LOGGER_INFO("PCU: opening %s", pcu_relay_names[relay]);
    }
    pcu_handler.set_pin(pcu_relays[relay].pin, PCU_PIN_STATUS_HIGH);
    pcu_handler.closed[relay] = false;
    return PCU_RC_OK;
}

void pcu_api_open_all(void) {
    EAGLETRT_API_UNUSED(pcu_api_open(PCU_RELAY_AIR_POSITIVE));
    EAGLETRT_API_UNUSED(pcu_api_open(PCU_RELAY_PRECHARGE));
    EAGLETRT_API_UNUSED(pcu_api_open(PCU_RELAY_AIR_NEGATIVE));
}

bool pcu_api_is_closed(const enum PcuRelay relay) {
    if (relay >= PCU_RELAY_COUNT) {
        return false;
    }
    return pcu_handler.closed[relay];
}

void pcu_api_ams_activate(void) {
    if (pcu_handler.set_pin == NULL) {
        return;
    }
    // FATAL activates the AMS at every run, only the first activation is logged
    if (!pcu_handler.ams_active) {
        MAINBOARD_LOGGER_ERROR("PCU: AMS activated");
        pcu_handler.ams_active = true;
    }
    pcu_handler.set_pin(PCU_PIN_AMS, PCU_PIN_STATUS_LOW);
}

void pcu_api_reset_microcontroller(void) {
    if (pcu_handler.reset_mcu == NULL) {
        return;
    }
    // Probably lost: the UART is not flushed before the reset
    MAINBOARD_LOGGER_WARNING("PCU: resetting the MCU");
    pcu_handler.reset_mcu();
}
