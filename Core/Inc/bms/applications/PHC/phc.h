/*!
 * \file phc.h
 * \date 2026-11-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the Pack Health Controller (PHC)
 */

#ifndef PHC_H
#define PHC_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-status.h"
#include "watchdogs.h"

enum PHCReturnCode : uint8_t {
    PHC_RC_OK,           /*!< The function completed successfully */
    PHC_RC_NULL_POINTER, /*!< A NULL pointer was passed to the function */
    PHC_RC_ERROR,        /*!< An error occurred */
    PHC_RC_NOT_READY,    /*!< The check did not run because its data has not been received yet */
};

enum PHCControl : uint8_t {
    // FAULTS
    PHC_CONTROL_CELL_UNDER_VOLTAGE,             /*!< Check for cell under voltage */
    PHC_CONTROL_CELL_OVER_VOLTAGE,              /*!< Check for cell over voltage */
    PHC_CONTROL_CELL_UNDER_TEMPERATURE,         /*!< Check for cell under temperature */
    PHC_CONTROL_CELL_OVER_TEMPERATURE,          /*!< Check for cell over temperature */
    PHC_CONTROL_DISCHARGE_UNDER_TEMPERATURE,    /*!< Check for discharge resistor under temperature */
    PHC_CONTROL_DISCHARGE_OVER_TEMPERATURE,     /*!< Check for discharge resistor over temperature */
    PHC_CONTROL_OVER_CURRENT,                   /*!< Check for over current */
    PHC_CONTROL_OVER_POWER,                     /*!< Check for over power */
    PHC_CONTROL_COOLING_UNDER_TEMPERATURE,      /*!< Check for cooling under temperature */
    PHC_CONTROL_COOLING_OVER_TEMPERATURE,       /*!< Check for cooling over temperature */
    PHC_CONTROL_CELLBOARD_FAULT,                /*!< Check for cellboard fault */
    PHC_CONTROL_IMD_FAULT,                      /*!< Check for IMD fault */
    PHC_CONTROL_VOLTAGE_INDICATOR_DISCONNECTED, /*!< Check for voltage indicator disconnected */

    // CONDITIONS
    PHC_CONTROL_PACK_BALANCED,      /*!< Update the pack balanced condition */
    PHC_CONTROL_PRECHARGE_COMPLETE, /*!< Update the precharge complete condition */

    PHC_CONTROL_COUNT /*!< Number of controls */
};

/*!
 * \brief What a PHC check writes to the mainboard status
 */
enum PHCCheckKind : uint8_t {
    PHC_CHECK_KIND_FAULT,     /*!< A PHC_CHECK_RESULT_FAILED result sets the fault, which is never cleared */
    PHC_CHECK_KIND_CONDITION, /*!< The result is written to the condition every time it runs */
};

/*!
 * \brief Result of a PHC check function
 */
enum PhcCheckResult : uint8_t {
    PHC_CHECK_RESULT_OK,        /*!< Fault check: the values are within their limits. Condition check: the condition holds */
    PHC_CHECK_RESULT_FAILED,    /*!< Fault check: a value is out of its limits. Condition check: the condition does not hold */
    PHC_CHECK_RESULT_NOT_READY, /*!< The data needed by the check has not been received yet, nothing is written and the check does not count as run */
};

/*!
 * \brief Function pointer type for PHC check functions
 *
 * \returns The result of the check, see \ref PhcCheckResult
 */
typedef enum PhcCheckResult (*phc_check_function)(void);

/*!
 * \brief Structure representing a PHC check
 */
struct PHCCheck {
    enum PHCControl control; /*!< The PHC control to check */
    enum PHCCheckKind kind;  /*!< Whether the check sets a fault or updates a condition */
    union {
        enum MainboardStatusFault fault;         /*!< The fault set when the check fails (kind FAULT) */
        enum MainboardStatusCondition condition; /*!< The condition updated with the result (kind CONDITION) */
    };
    phc_check_function check_function; /*!< The function to perform the check */
};

/*!
 * \brief Structure representing the PHC handler
 */
struct PHCHandler {
    struct Watchdog check_watchdogs[PHC_CONTROL_COUNT]; /*!< One per check, started at its first run and petted at every run, sets the PHC fault if a check stops running */
    struct Watchdog startup_watchdog;                   /*!< Started at init, sets the PHC fault if a check has never run when it times out */
    bool initialized;                                   /*!< True once every watchdog is initialized and the startup watchdog is started */
};

#endif // PHC_H
