/**
 * @file imd.c
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Insulation Monitoring Device (IMD) handing functions
 */

#include <string.h>

#include "imd.h"

#ifdef CONF_IMD_MODULE_ENABLE

_STATIC struct {
    imd_pwm_start_callback_t start;

    Ir1553204Handler ir1153204;
} himd;

ImdReturnCode imd_init(imd_pwm_start_callback_t start) {
    memset(&himd, 0U, sizeof(himd));
    himd.start = start;
    ImdReturnCode code = (ImdReturnCode)ir1553204_init(&himd.ir1153204);
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
    ticks_t source_frequency,
    ticks_t period_count,
    ticks_t high_count)
{
    if (period_count == 0)
        return IMD_INVALID_DATA;
    hertz_t frequency = source_frequency / (float)period_count;
    precise_percentage_t duty_cycle = (high_count * 100.f) / period_count;
    (void)ir1553204_set_frequency(&himd.ir1153204, frequency);
    (void)ir1553204_set_duty_cycle(&himd.ir1153204, duty_cycle);
    return IMD_OK;
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

#endif // CONF_IMD_STRINGS_ENABLE

#endif // CONF_IMD_MODULE_ENABLE
