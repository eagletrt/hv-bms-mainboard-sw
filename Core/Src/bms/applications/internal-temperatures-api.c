/*!
 * \file internal-temperatures-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal temperatures measurement and control
 */

#include "internal-temperatures-api.h"

#include <string.h>

#include "eagletrt.h"

/*!
 * \brief The global internal temperatures handler
 */
EAGLETRT_STATIC struct InternalTemperaturesHandler internal_temperatures_handler;

void internal_temperatures_api_init(void) {
    memset(&internal_temperatures_handler, 0, sizeof(internal_temperatures_handler));
}

enum InternalTemperaturesReturnCode internal_temperatures_api_set_value(enum InternalTemperaturesIndex index, celsius temperature) {
    if (index >= INTERNAL_TEMPERATURES_INDEX_COUNT) {
        return INTERNAL_TEMPERATURES_RC_INVALID_INDEX;
    }

    internal_temperatures_handler.temperatures[index] = temperature;
    internal_temperatures_handler.is_updated_temperatures = true;
    internal_temperatures_handler.is_received_temperatures[index] = true;

    return INTERNAL_TEMPERATURES_RC_OK;
}

void internal_temperatures_api_reset_temperatures_updated_flag(void) {
    internal_temperatures_handler.is_updated_temperatures = false;
}

bool internal_temperatures_api_is_ready(void) {
    for (enum InternalTemperaturesIndex index = 0; index < INTERNAL_TEMPERATURES_INDEX_COUNT; ++index) {
        if (!internal_temperatures_handler.is_received_temperatures[index]) {
            return false;
        }
    }
    return true;
}

celsius internal_temperatures_api_get_value(enum InternalTemperaturesIndex index) {
    if (index >= INTERNAL_TEMPERATURES_INDEX_COUNT) {
        return 0.F;
    }
    return internal_temperatures_handler.temperatures[index];
}
