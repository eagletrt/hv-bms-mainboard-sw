/**
 * @file error.c
 * @date 2024-07-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Simple wrapper for the error handler generated code
 */

#include "error.h"

#include <string.h>

#ifdef CONF_ERROR_MODULE_ENABLE

_STATIC _ErrorHandler herror;

ErrorReturnCode error_init(
    interrupt_critical_section_enter_t cs_enter,
    interrupt_critical_section_exit_t cs_exit,
    error_update_timer_callback_t update_timer,
    error_stop_timer_callback_t stop_timer)
{
    if (cs_enter == NULL ||
        cs_exit == NULL ||
        update_timer == NULL ||
        stop_timer == NULL)
        return ERROR_NULL_POINTER;
    memset(&herror, 0U, sizeof(herror));

    error_handler_error_init(cs_enter, cs_exit);
    herror.update_timer = update_timer;
    herror.stop_timer = stop_timer;
    return ERROR_OK;
}

/* This function is defined inside the error-handler files and has to be implemented here */
void error_handler_error_update_timer_callback(uint32_t timestamp, uint16_t timeout) {
    herror.update_timer(timestamp, timeout);
}

/* This function is defined inside the error-handler files and has to be implemented here */
void error_handler_error_stop_timer_callback(void) {
    herror.stop_timer();
}

#ifdef CONF_ERROR_STRINGS_ENABLE

_STATIC char * error_module_name = "error";

_STATIC char * error_return_code_name[] = {
    [ERROR_OK] = "ok",
    [ERROR_NULL_POINTER] = "null pointer"
}

_STATIC char * error_return_code_description[] = {
    [ERROR_OK] = "executed succesfully",
    [ERROR_NULL_POINTER] = "attempt to dereference a null pointer"
}

#endif // CONF_ERROR_STRINGS_ENABLE

#endif // CONF_ERROR_MODULE_ENABLE
