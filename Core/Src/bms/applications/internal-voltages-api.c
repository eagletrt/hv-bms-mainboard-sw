/*!
 * \file internal-voltages-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages (TS, pack) and current measurement and control
 */

#include "internal-voltages-api.h"

#include <string.h>

#include "eagletrt.h"
#include "mainboard-logger-api.h"

/*!
 * \brief The global internal voltages handler
 */
EAGLETRT_STATIC struct InternalVoltagesHandler internal_voltages_handler;

void internal_voltages_api_init(void) {
    memset(&internal_voltages_handler, 0, sizeof(internal_voltages_handler));
}

enum InternalVoltagesReturnCode internal_voltages_api_set_value(enum InternalVoltagesIndex index, volt voltage) {
    if (index >= INTERNAL_VOLTAGES_INDEX_COUNT) {
        return INTERNAL_VOLTAGES_RC_INVALID_INDEX;
    }

    internal_voltages_handler.voltages[index] = voltage;

    if (!internal_voltages_handler.is_received_voltages[index]) {
        internal_voltages_handler.is_received_voltages[index] = true;
        if (internal_voltages_api_is_voltages_ready()) {
            MAINBOARD_LOGGER_INFO("TS and pack voltages ready");
        }
    }

    return INTERNAL_VOLTAGES_RC_OK;
}

void internal_voltages_api_set_current(ampere current) {
    internal_voltages_handler.current = current;

    if (!internal_voltages_handler.is_received_current) {
        MAINBOARD_LOGGER_INFO("Current ready");
    }
    internal_voltages_handler.is_received_current = true;
}

volt internal_voltages_api_get_value(enum InternalVoltagesIndex index) {
    if (index >= INTERNAL_VOLTAGES_INDEX_COUNT) {
        return 0.F;
    }
    return internal_voltages_handler.voltages[index];
}

ampere internal_voltages_api_get_current(void) {
    return internal_voltages_handler.current;
}

kilowatt internal_voltages_api_get_power(void) {
    return internal_voltages_handler.current *
           internal_voltages_handler.voltages[INTERNAL_VOLTAGES_INDEX_TS] *
           INTERNAL_VOLTAGES_W_TO_KW;
}

bool internal_voltages_api_is_voltages_ready(void) {
    for (enum InternalVoltagesIndex index = 0; index < INTERNAL_VOLTAGES_INDEX_COUNT; ++index) {
        if (!internal_voltages_handler.is_received_voltages[index]) {
            return false;
        }
    }
    return true;
}

bool internal_voltages_api_is_current_ready(void) {
    return internal_voltages_handler.is_received_current;
}

struct InternalVoltagesInfo internal_voltages_api_get_info(void) {
    const struct InternalVoltagesInfo info = {
        .ts = internal_voltages_handler.voltages[INTERNAL_VOLTAGES_INDEX_TS],
        .pack = internal_voltages_handler.voltages[INTERNAL_VOLTAGES_INDEX_PACK],
        .current = internal_voltages_handler.current,
        .power = internal_voltages_api_get_power()
    };
    return info;
}
