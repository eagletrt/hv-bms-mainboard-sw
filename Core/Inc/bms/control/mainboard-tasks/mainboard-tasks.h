/*!
 * \file mainboard-tasks.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard tasks
 */

#ifndef MAINBOARD_TASKS_H
#define MAINBOARD_TASKS_H

#include <stdint.h>

/*!
 * \brief IDs of the mainboard tasks
 *
 * \details The IDs are sequential and start from 0, as required by the tasks module,
 *          so they can be used directly as indices of the task list
 */
enum MainboardTasksID : uint8_t {
    // Sensing
    MAINBOARD_TASKS_SENSING_RUN = 0, /*!< Acquire the GPIOs, ADCs and external ADC values */
    MAINBOARD_TASKS_FEEDBACK_RUN,    /*!< Update the mainboard status conditions from the filtered feedbacks */

    // FSM
    MAINBOARD_TASKS_FSM_RUN, /*!< Run the mainboard FSM */

    // CAN
    MAINBOARD_TASKS_CAN_TX_PRIMARY, /*!< Process the TX queue of the primary network */
    MAINBOARD_TASKS_CAN_TX_BMS,     /*!< Process the TX queue of the BMS network */
    MAINBOARD_TASKS_CAN_RX_PRIMARY, /*!< Process the RX queue of the primary network */
    MAINBOARD_TASKS_CAN_RX_BMS,     /*!< Process the RX queue of the BMS network */

    // PHC faults
    MAINBOARD_TASKS_PHC_CELL_UNDER_VOLTAGE,             /*!< Check for cell under voltage */
    MAINBOARD_TASKS_PHC_CELL_OVER_VOLTAGE,              /*!< Check for cell over voltage */
    MAINBOARD_TASKS_PHC_CELL_UNDER_TEMPERATURE,         /*!< Check for cell under temperature */
    MAINBOARD_TASKS_PHC_CELL_OVER_TEMPERATURE,          /*!< Check for cell over temperature */
    MAINBOARD_TASKS_PHC_DISCHARGE_UNDER_TEMPERATURE,    /*!< Check for discharge resistor under temperature */
    MAINBOARD_TASKS_PHC_DISCHARGE_OVER_TEMPERATURE,     /*!< Check for discharge resistor over temperature */
    MAINBOARD_TASKS_PHC_OVER_CURRENT,                   /*!< Check for over current */
    MAINBOARD_TASKS_PHC_OVER_POWER,                     /*!< Check for over power */
    MAINBOARD_TASKS_PHC_COOLING_UNDER_TEMPERATURE,      /*!< Check for cooling under temperature */
    MAINBOARD_TASKS_PHC_COOLING_OVER_TEMPERATURE,       /*!< Check for cooling over temperature */
    MAINBOARD_TASKS_PHC_CELLBOARD_FAULT,                /*!< Check for cellboard fault */
    MAINBOARD_TASKS_PHC_IMD_FAULT,                      /*!< Check for IMD fault */
    MAINBOARD_TASKS_PHC_VOLTAGE_INDICATOR_DISCONNECTED, /*!< Check for voltage indicator disconnected */

    // PHC conditions
    MAINBOARD_TASKS_PHC_PACK_BALANCED,      /*!< Update the pack balanced condition */
    MAINBOARD_TASKS_PHC_PRECHARGE_COMPLETE, /*!< Update the precharge complete condition */

    // User interface
    MAINBOARD_TASKS_LED_HEARTBEAT,  /*!< Toggle the heartbeat LED */
    MAINBOARD_TASKS_DISPLAY_UPDATE, /*!< Show the FSM state on the 7-segment display */

    MAINBOARD_TASKS_COUNT /*!< Number of mainboard tasks */
};

#endif // MAINBOARD_TASKS_H
