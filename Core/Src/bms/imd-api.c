/*!
 * \file imd.c
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Insulation Monitoring Device (IMD) handing functions
 */

#include <string.h>

#include "imd-api.h"
#include "feedback.h"
#include "eagletrt-api.h"

#ifdef CONF_IMD_MODULE_ENABLE

EAGLETRT_STATIC struct ImdHandler imd_handler;

enum ImdReturnCode imd_init(const imd_pwm_start_callback start) {
    if (start == NULL) {
        return IMD_RC_NULL_POINTER;
    }
    memset(&imd_handler, 0U, sizeof(imd_handler));
    imd_handler.start = start;
    const enum ImdReturnCode code = (const enum ImdReturnCode)ir1553204_init(&imd_handler.ir1153204);
    start();
    return code;
}

precise_percentage_t imd_get_duty_cycle(void) {
    return ir1553204_get_duty_cycle(&imd_handler.ir1153204);
}

hertz_t imd_get_frequency() {
    return ir1553204_get_frequency(&imd_handler.ir1153204);
}

milliseconds_t imd_get_period() {
    return ir1553204_get_period(&imd_handler.ir1153204);
}

enum ImdStatus imd_get_status() {
    return (enum ImdStatus)ir1553204_get_status(&imd_handler.ir1153204);
}

enum ImdReturnCode imd_update(const ticks_t source_frequency, const ticks_t period_count, const ticks_t high_count) {
    if (period_count == 0) {
        return IMD_RC_INVALID_DATA;
    }
    /*
     * Signal:
     *              ┌──────┐                   ┌──────┐
     *              │      │                   │      │
     * ─────────────┘      └───────────────────┘      └─────
     *              ↑      ↑                   ↑
     *              │      │                   │
     *              │      └─ CCR2 = CNT       └─ CCR1 = CNT (period)
     *              |         (high_count)          IRQ → callback fires
     *              │                               
     *              │
     *              └─ CNT = 0
     */

    hertz_t frequency = (float)source_frequency / (float)period_count;
    precise_percentage_t duty_cycle = (float)high_count / (float)period_count;
    (void)ir1553204_set_frequency(&imd_handler.ir1153204, frequency);
    (void)ir1553204_set_duty_cycle(&imd_handler.ir1153204, duty_cycle);
    return IMD_RC_OK;
}

primary_hv_imd_status_converted_t *imd_get_status_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(imd_handler.status_can_payload);
    }
    // The value of the IMD status of the canlib differs by one from the IMD status
    // of the BMS
    imd_handler.status_can_payload.status = (primary_hv_imd_status_status)(imd_get_status() + 1U);
    imd_handler.status_can_payload.frequency = imd_get_frequency();
    imd_handler.status_can_payload.duty_cycle = imd_get_duty_cycle();
    imd_handler.status_can_payload.feedback_not_imd_fault_cockpit_led = (primary_hv_imd_status_feedback_not_imd_fault_cockpit_led)feedback_get_status(FEEDBACK_ID_IMD_FAULT_COCKPIT_LED);
    imd_handler.status_can_payload.feedback_not_imd_fault_latched = (primary_hv_imd_status_feedback_not_imd_fault_latched)feedback_get_status(FEEDBACK_ID_IMD_FAULT_LATCHED);
    return &imd_handler.status_can_payload;
}

#ifdef CONF_IMD_STRINGS_ENABLE

EAGLETRT_STATIC char *imd_module_name = "imd";

EAGLETRT_STATIC char *imd_return_code_name[] = {
    [IMD_RC_OK] = "ok",
    [IMD_RC_NULL_POINTER] = "null pointer",
    [IMD_RC_INVALID_DATA] = "invalid data"
};

EAGLETRT_STATIC char *imd_return_code_description[] = {
    [IMD_RC_OK] = "executed succesfully",
    [IMD_RC_NULL_POINTER] = "attempt to dereference a null pointer",
    [IMD_RC_INVALID_DATA] = "given data is not valid"
};

// IMD status unknown is negative so it can't be used in an array
EAGLETRT_STATIC char *imd_status_unknown_name = "unknown";
EAGLETRT_STATIC char *imd_status_name[] = {
    [IMD_STATUS_SHORT_CIRCUIT] = "short circuit",
    [IMD_STATUS_NORMAL] = "normal",
    [IMD_STATUS_UNDER_VOLTAGE] = "undervoltage",
    [IMD_STATUS_START_MEASURE] = "start measure",
    [IMD_STATUS_DEVICE_ERROR] = "device error",
    [IMD_STATUS_EARTH_FAULT] = "earth fault"
};

const char *const imd_get_imd_status_name(const enum ImdStatus status) {
    if (status > IMD_STATUS_COUNT)
        return "unknown";
    if (status == IMD_STATUS_UNKNOWN)
        return imd_status_unknown_name;
    return imd_status_name[status];
}

#endif // CONF_IMD_STRINGS_ENABLE

#endif // CONF_IMD_MODULE_ENABLE
