/**
 * @file error.h
 * @date 2024-07-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Simple wrapper for the error handler generated code
 */

#ifndef ERROR_H
#define ERROR_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "error-handler.h"

/**
 * @brief Type definition for the callback used to upate the error timer
 *
 * @param timestamp The time in which the error was set
 * @param timeout The time after which the error should expire after it was set
 */
typedef void (* error_update_timer_callback_t)(uint32_t timestamp, uint16_t timeout);

/** @brief Function used to stop the error timer */
typedef void (* error_stop_timer_callback_t)(void);

/**
 * @brief Return code for the error module functions
 *
 * @details
 *     - ERROR_OK the function executed succesfully
 *     - ERROR_NULL_POINTER a NULL pointer was given to a function
 */
typedef enum {
    ERROR_OK,
    ERROR_NULL_POINTER
} ErrorReturnCode;

/**
 * @brief Type definition for the internal error handler structure
 *
 * @attention This struct should not be used outside of this module
 *
 * @param update_timer A pointer to the callback used to update the timer
 * @param stop_timer A pointer to the callback used to stop the timer
 */
typedef struct {
    error_update_timer_callback_t update_timer;
    error_stop_timer_callback_t stop_timer;
} _ErrorHandler;

#ifdef CONF_ERROR_MODULE_ENABLE

/**
 * @brief Initialization of the internal error handler structure
 *
 * @param cs_enter A pointer to the function used to enter a critical section
 * @param cs_exit A pointer to the function used to exit a critical section
 * @param update_timer A pointer to the function used to update the error timer
 * @param stop_timer A pointer to the function used to stop the error timer
 */
ErrorReturnCode error_init(
    interrupt_critical_section_enter_t cs_enter,
    interrupt_critical_section_exit_t cs_exit,
    error_update_timer_callback_t update_timer,
    error_stop_timer_callback_t stop_timer
);

#else  // CONF_ERROR_MODULE_ENABLE

#define error_init(cs_enter, cs_exit, update_timer, stop_timer) (ERROR_OK)

#endif // CONF_ERROR_MODULE_ENABLE

#endif  // ERROR_H
