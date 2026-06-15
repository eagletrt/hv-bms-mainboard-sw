/*!
 * \file ir1553204.c
 * \date 2024-07-03
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Driver for the IR155-3204 Insulation Monitoring Device (IMD)
 */

#include "ir1553204-api.h"

#include <stddef.h>
#include <string.h>
#include <math.h>

enum Ir1553204ReturnCode ir1553204_api_init(struct Ir1553204Handler *const handler) {
    if (handler == NULL) {
        return IR1553204_RC_NULL_POINTER;
    }
    memset(handler, 0U, sizeof(*handler));
    return IR1553204_RC_OK;
}

precise_percentage_t ir1553204_api_get_duty_cycle(struct Ir1553204Handler *const handler) {
    if (handler == NULL) {
        return 0.F;
    }
    return handler->duty_cycle;
}

enum Ir1553204ReturnCode ir1553204_api_set_duty_cycle(struct Ir1553204Handler *const handler, const precise_percentage_t duty_cycle) {
    if (handler == NULL) {
        return IR1553204_RC_NULL_POINTER;
    }
    handler->duty_cycle = duty_cycle;
    return IR1553204_RC_OK;
}

hertz_t ir1553204_api_get_frequency(struct Ir1553204Handler *const handler) {
    if (handler == NULL) {
        return 0.F;
    }
    return handler->frequency;
}

enum Ir1553204ReturnCode ir1553204_api_set_frequency(struct Ir1553204Handler *const handler, const hertz_t frequency) {
    if (handler == NULL) {
        return IR1553204_RC_NULL_POINTER;
    }
    handler->frequency = frequency;
    return IR1553204_RC_OK;
}

milliseconds_t ir1553204_api_get_period(struct Ir1553204Handler *const handler) {

    constexpr milliseconds_t milliseconds_per_second = 1000U;

    if (handler == NULL || handler->frequency == 0.F) {
        return 0U;
    }
    // To get the period in ms the period (1/f) is multiplied by 1000
    return (milliseconds_t)(milliseconds_per_second / handler->frequency);
}

enum Ir1553204Status ir1553204_api_get_status(struct Ir1553204Handler *const handler) {

    constexpr hertz_t frequency_per_status = 10.F;

    if (handler == NULL) {
        return IR1553204_STATUS_UNKNOWN;
    }
    // Round frequency to avoid precision errors
    uint32_t status = (uint32_t)roundf(handler->frequency / frequency_per_status);
    if (status >= IR1553204_STATUS_COUNT) {
        return IR1553204_STATUS_UNKNOWN;
    }
    return (enum Ir1553204Status)status;
}
