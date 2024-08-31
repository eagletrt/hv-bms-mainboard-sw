/**
 * @file ir1553204.c
 * @date 2024-07-03
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Driver for the IR155-3204 Insulation Monitoring Device (IMD)
 */

#include "ir1553204.h"

#include <stddef.h>
#include <string.h>
#include <math.h>

Ir1553204ReturnCode ir1553204_init(Ir1553204Handler * const handler) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    memset(handler, 0U, sizeof(*handler));
    return IR1553204_OK;
}

precise_percentage_t ir1553204_get_duty_cycle(Ir1553204Handler * const handler) {
    if (handler == NULL)
        return 0.f;
    return handler->duty_cycle;
}

Ir1553204ReturnCode ir1553204_set_duty_cycle(Ir1553204Handler * const handler, const precise_percentage_t duty_cycle) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    handler->duty_cycle = duty_cycle;
    return IR1553204_OK;
}

hertz_t ir1553204_get_frequency(Ir1553204Handler * const handler) {
    if (handler == NULL)
        return 0.f;
    return handler->frequency;
}

Ir1553204ReturnCode ir1553204_set_frequency(Ir1553204Handler * const handler, const hertz_t frequency) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    handler->frequency = frequency;
    return IR1553204_OK;
}

milliseconds_t ir1553204_get_period(Ir1553204Handler * const handler) {
    if (handler == NULL || handler->frequency == 0.f)
        return 0U;
    // To get the period in ms the period (1/f) is multiplied by 1000
    return 1000U / handler->frequency;
}

Ir1553204Status ir1553204_get_status(Ir1553204Handler * const handler) {
    if (handler == NULL)
        return IR1553204_STATUS_UNKNOWN;
    // Round frequency to avoid precision errors
    uint32_t freq = roundf(handler->frequency / 10.f) * 10.f;
    switch (freq) {
        case 0U: 
            return IR1553204_STATUS_SHORT_CIRCUIT;
        case 10U: 
            return IR1553204_STATUS_NORMAL;
        case 20U:
            return IR1553204_STATUS_UNDER_VOLTAGE;
        case 30U:
            return IR1553204_STATUS_START_MEASURE;
        case 40U:
            return IR1553204_STATUS_DEVICE_ERROR;
        case 50U:
            return IR1553204_STATUS_EARTH_FAULT;
        default:
            return IR1553204_STATUS_UNKNOWN;
    }
}
