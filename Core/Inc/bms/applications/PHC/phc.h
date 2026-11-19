/*!
 * \file phc.h
 * \date 2026-11-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the Pack Health Controller (PHC)
 */

#ifndef PHC_H
#define PHC_H

#include <stdint.h>
#include "mainboard-status.h"

enum PHCReturnCode : uint8_t {
    PHC_RC_OK,           /*!< The function completed successfully */
    PHC_RC_NULL_POINTER, /*!< A NULL pointer was passed to the function */
    PHC_RC_ERROR,        /*!< An error occurred */
};

enum PHCControl : uint8_t {
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
    PHC_CONTROL_PACK_BALANCED,                  /*!< Update the pack balanced condition */
    PHC_CONTROL_COUNT                           /*!< Number of controls */
};

/*!
 * \brief What a PHC check writes to the mainboard status
 */
enum PHCCheckKind : uint8_t {
    PHC_CHECK_KIND_FAULT,     /*!< A false result sets the fault, which is never cleared */
    PHC_CHECK_KIND_CONDITION, /*!< The result is written to the condition every time */
};

/*!
 * \brief Function pointer type for PHC check functions
 *
 * \returns For a fault check: true if everything is OK, false to set the fault.
 *          For a condition check: true if the condition holds, false otherwise.
 */
typedef bool (*PHCCheckFunction)(void);

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
    PHCCheckFunction check_function; /*!< The function to perform the check */
};

/*!
 * \brief Structure representing the PHC handler
 */
struct PHCHandler {
    uint32_t last_check_time[PHC_CONTROL_COUNT]; /*!< The time of the last check of the condition */
    bool last_check_updated[PHC_CONTROL_COUNT];  /*!< Whether the last check time has been updated once*/
};

#endif // PHC_H
