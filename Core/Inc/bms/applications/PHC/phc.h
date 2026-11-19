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
    PHC_CONTROL_COUNT                           /*!< Number of controls */
};

typedef bool (*PHCCheckFunction)(void);

struct PHCCheck {
    enum PHCControl control;
    enum MainboardStatusFault fault;
    PHCCheckFunction check_function;
};

struct PHCHandler {
    uint32_t last_check_time[PHC_CONTROL_COUNT];
    bool last_check_updated[PHC_CONTROL_COUNT];
};

#endif // PHC_H
