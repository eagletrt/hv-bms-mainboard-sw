/*******************************************************************************
 * Critical error handler library generator
 * Generated by error_gen ruby gem, for more information see:
 * https://github.com/eagletrt/micro-utils/tree/master/error-handler-generator
 *
 * Error_gen version 1.6.1
 * Generation date: 2024-06-19 22:25:31 +0200
 * Generated from: errors.json
 * With prefix: none
 * The error handler contains:
 *     - 1 error groups
 *     - 1 total error instances
 ******************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Total number of error instances
#define ERROR_INSTANCE_COUNT 1

/**
 * @brief Set or reset an instance of an error based on a condition
 *
 * @details If the condition is true the error is set, otherwise it is reset
 *
 * @param condition A boolean expression
 * @param group The group to which the error belongs
 * @param instance The instance of the error
 * @param The current time (in ms)
 */
#define ERROR_TOGGLE_IF(condition, group, instance, timestamp) \
    ((condition) ? error_set(group, instance, timestamp) : error_reset(group, instance))

/**
 * @brief Type of the error that categorize a group of instances
 *
 * @details
 *     - ERROR_GROUP_POST the Power On Self Test procedure failed
 */
typedef enum {
    ERROR_GROUP_POST,
    ERROR_GROUP_COUNT
} ErrorGroup;

/** @brief Single error instance type definition */
typedef uint16_t ErrorInstance;

/**
 * @brief Error type definition
 *
 * @param group The group to which the error belongs
 * @param timestamp The time when the error was set (in ms)
 * @param is_running True if the error is set, false otherwise
 * @param is_expired True if the error has expired, false otherwise
 */
typedef struct {
    ErrorGroup group;
    uint32_t timestamp;
    bool is_running;
    bool is_expired;
} Error;

/**
 * @brief Initialize the internal error handler structures
 *
 * @details A critical section is defined as a block of code where, if an interrupt
 * happens, undefined behaviour with the modified data within the block can happen
 *
 * @param cs_enter A pointer to a function that should manage a critical section
 * @param cs_exit A pointer to a function that shuold manage a critical section
 */
void error_init(void (* cs_enter)(void), void (* cs_exit)(void));

/**
 * @brief Get the number of errors that has been set but they still have to expire
 *
 * @param size_t The number of running errors
 */
size_t error_get_running(void);

/**
 * @brief Get the number of expired errors
 *
 * @param size_t The number of expired errors
 */
size_t error_get_expired(void);

/**
 * @brief Get the number of running error of a specific group
 *
 * @param group The error group
 *
 * @return uint16_t The number of running errors
 */
uint16_t error_get_group_running(ErrorGroup group);

/**
 * @brief Get the number of expired error of a specific group
 *
 * @param group The error group
 *
 * @return uint16_t The number of running errors
 */
uint16_t error_get_group_expired(ErrorGroup group);

/**
 * @brief Get a copy of all the errors that are currently running
 *
 * @attention This function can be quite expensive in terms of time
 * and should be used wisely, do not call to often
 * @attention This function calls the critical section handler functions
 *
 * @details The out array should be able to contain all the instances
 *
 * @param out A pointer to an array of errors where the data is copied into
 *
 * @return size_t The number of copied errors
 */
size_t error_dump_running(Error * out);

/**
 * @brief Get a copy of all the errors that are expired
 *
 * @attention This function can be quite expensive in terms of time
 * and should be used wisely, do not call to often
 * @attention This function calls the critical section handler functions
 *
 * @details The out array should be able to contain all the instances
 *
 * @param out A pointer to an array of errors where the data is copied into
 *
 * @return size_t The number of copied errors
 */
size_t error_dump_expired(Error * out);

/**
 * @brief Get all the groups in which at least one error is running
 *
 * @param out A pointer to an array of groups where the data is copied into
 *
 * @return size_t The number of copied groups
 */
size_t error_dump_running_groups(ErrorGroup * out);

/**
 * @brief Get all the groups in which at least one error is expired
 *
 * @param out A pointer to an array of groups where the data is copied into
 *
 * @return size_t The number of copied groups
 */
size_t error_dump_expired_groups(ErrorGroup * out);

/**
 * @brief Set an error which will expire after a certain amount of time (the timeout)
 *
 * @param group The group to which the error belongs
 * @param instance The instance of the error
 * @param The current time (in ms)
 */
void error_set(ErrorGroup group, ErrorInstance instance, uint32_t timestamp);

/**
 * @brief Reset an error to avoid its expiration
 *
 * @param group The group to which the error belongs
 * @param instance The instance of the error
 */
void error_reset(ErrorGroup group, ErrorInstance instance);

/** @brief Set the error as expired */
void error_expire(void);

/**
 * @brief Set the error as expired immediately even if it is not running
 *
 * @param group The group to which the error belongs
 * @param instance The instance of the error
 */
void error_expire_immediate(ErrorGroup group, ErrorInstance instance);

/**
 * @brief Routine that updates the internal error states
 *
 * @attention This function should not be called inside interrupts callback
 * or other threads
 *
 * @details This function should be called periodically
 */
void error_routine(void);

/**
 * @brief Update the timer that should expire the error after a certain amount of time
 *
 * @attention This function have to be defined by the user
 *
 * @details This function is called internally when an error is set, reset or expired
 *
 * @param timestamp The time in which the error was set (in ms)
 * @param timeout The time that should elapse after the timestamp to expire the error (in ms)
 */
void error_update_timer_callback(uint32_t timestamp, uint16_t timeout);

/**
 * @brief Stop the timer that should expire the errors
 *
 * @attention This function have to be defined by the user
 *
 * @details This function is called internally when an error is reset or expired
 */
void error_stop_timer_callback(void);

#endif  // ERROR_H
