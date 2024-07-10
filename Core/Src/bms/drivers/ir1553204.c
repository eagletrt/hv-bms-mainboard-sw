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

Ir1553204ReturnCode ir1553204_init(Ir1553204Handler * handler) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    memset(handler, 0U, sizeof(*handler));
    return IR1553204_OK;
}

precise_percentage_t ir1553204_get_duty_cycle(Ir1553204Handler * handler) {
    if (handler == NULL)
        return 0.f;
    return handler->duty_cycle;
}

Ir1553204ReturnCode ir1553204_set_duty_cycle(Ir1553204Handler * handler, precise_percentage_t duty_cycle) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    handler->duty_cycle = roundf(duty_cycle);
    return IR1553204_OK;
}

hertz_t ir1553204_get_frequency(Ir1553204Handler * handler) {
    if (handler == NULL)
        return 0.f;
    return handler->frequency;
}

Ir1553204ReturnCode ir1553204_set_frequency(Ir1553204Handler * handler, hertz_t frequency) {
    if (handler == NULL)
        return IR1553204_NULL_POINTER;
    handler->frequency = roundf(frequency);
    return IR1553204_OK;
}

milliseconds_t ir1553204_get_period(Ir1553204Handler * handler) {
    if (handler == NULL || handler->frequency == 0.f)
        return 0U;
    // To get the period in ms the period (1/f) is multiplied by 1000
    return 1000U / handler->frequency;
}

Ir1553204Status ir1553204_get_status(Ir1553204Handler * handler) {
    if (handler == NULL)
        return IR1553204_STATUS_UNKNOWN;
    uint32_t freq = handler->frequency;
    if (freq == 0)
        return IR1553204_STATUS_SHORT_CIRCUIT;
    if (freq == 10)
        return IR1553204_STATUS_NORMAL;
    if (freq == 20)
        return IR1553204_STATUS_UNDER_VOLTAGE;
    if (freq == 30)
        return IR1553204_STATUS_START_MEASURE;
    if (freq == 40)
        return IR1553204_STATUS_DEVICE_ERROR;
    if (freq == 50)
        return IR1553204_STATUS_EARTH_FAULT;
    return IR1553204_STATUS_UNKNOWN;
}
