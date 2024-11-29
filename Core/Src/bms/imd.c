/**
 * @file imd.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Insulation Monitoring Device (IMD) handing functions
 */

#include <string.h>

#include "imd.h"
#include "feedback.h"

#ifdef CONF_IMD_MODULE_ENABLE

_STATIC _ImdHandler himd;

ImdReturnCode imd_init(const imd_pwm_start_callback_t start) {
    memset(&himd, 0U, sizeof(himd));
    himd.start = start;
    const ImdReturnCode code = (const ImdReturnCode)ir1553204_init(&himd.ir1153204);
    start();
    return code;
}

precise_percentage_t imd_get_duty_cycle(void) {
    return ir1553204_get_duty_cycle(&himd.ir1153204);
}

hertz_t imd_get_frequency() {
    return ir1553204_get_frequency(&himd.ir1153204);
}

milliseconds_t imd_get_period() {
    return ir1553204_get_period(&himd.ir1153204);
}

ImdStatus imd_get_status() {
    return (ImdStatus)ir1553204_get_status(&himd.ir1153204);
}

ImdReturnCode imd_update(
    const ticks_t source_frequency,
    const ticks_t period_count,
    const ticks_t high_count)
{
    if (period_count == 0)
        return IMD_INVALID_DATA;
    hertz_t frequency = source_frequency / (float)period_count;
    precise_percentage_t duty_cycle = high_count / (float)period_count;
    (void)ir1553204_set_frequency(&himd.ir1153204, frequency);
    (void)ir1553204_set_duty_cycle(&himd.ir1153204, duty_cycle);
    return IMD_OK;
}

primary_hv_imd_status_converted_t * imd_get_status_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(himd.status_can_payload);
    // The value of the IMD status of the canlib differs by one from the IMD status
    // of the BMS
    himd.status_can_payload.status = (primary_hv_imd_status_status)(imd_get_status() + 1U);
    himd.status_can_payload.frequency = imd_get_frequency();
    himd.status_can_payload.duty_cycle = imd_get_duty_cycle();
    himd.status_can_payload.feedback_not_imd_fault_cockpit_led = (primary_hv_imd_status_feedback_not_imd_fault_cockpit_led)feedback_get_status(FEEDBACK_ID_IMD_FAULT_COCKPIT_LED);
    himd.status_can_payload.feedback_not_imd_fault_latched = (primary_hv_imd_status_feedback_not_imd_fault_latched)feedback_get_status(FEEDBACK_ID_IMD_FAULT_LATCHED);
    return &himd.status_can_payload;
}

#ifdef CONF_IMD_STRINGS_ENABLE

_STATIC char * imd_module_name = "imd";

_STATIC char * imd_return_code_name[] = {
    [IMD_OK] = "ok",
    [IMD_NULL_POINTER] = "null pointer",
    [IMD_INVALID_DATA] = "invalid data"
};

_STATIC char * imd_return_code_description[] = {
    [IMD_OK] = "executed succesfully",
    [IMD_NULL_POINTER] = "attempt to dereference a null pointer",
    [IMD_INVALID_DATA] = "given data is not valid"
};

// IMD status unknown is negative so it can't be used in an array
_STATIC char * imd_status_unknown_name = "unknown";
_STATIC char * imd_status_name[] = {
    [IMD_STATUS_SHORT_CIRCUIT] = "short circuit",
    [IMD_STATUS_NORMAL] = "normal",
    [IMD_STATUS_UNDER_VOLTAGE] = "undervoltage",
    [IMD_STATUS_START_MEASURE] = "start measure",
    [IMD_STATUS_DEVICE_ERROR] = "device error", 
    [IMD_STATUS_EARTH_FAULT] = "earth fault"
};

const char * const imd_get_imd_status_name(const ImdStatus status) {
    if (status > IMD_STATUS_COUNT)
        return "unknown";
    if (status == IMD_STATUS_UNKNOWN)
        return imd_status_unknown_name;
    return imd_status_name[status];
}

#endif // CONF_IMD_STRINGS_ENABLE

#endif // CONF_IMD_MODULE_ENABLE
