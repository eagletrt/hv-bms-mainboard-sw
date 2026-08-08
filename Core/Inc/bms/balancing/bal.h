/*!
 * \file bal.h
 * \date 2024-08-02
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Balancing handling functions
 */

#ifndef BAL_H
#define BAL_H

#include <stdbool.h>

#include "can-bms.h"
#include "mainboard-def.h"

#include "watchdog.h"
#include "fsm.h"

/*! \brief Balancing threshold range in V */
#define BAL_THRESHOLD_MIN_V (0.005f)
#define BAL_THRESHOLD_MAX_V (0.200f)

/*! \brief Balancing target range in V */
#define BAL_TARGET_MIN_V (2.8f)
#define BAL_TARGET_MAX_V (4.2f)

/*! \brief Balancing timeouts in ms */
#define BAL_TIMEOUT_MS (3000U)

/*!
 * \brief Return code for the balancing module functions
 */
enum BalReturnCode {
    BAL_RC_OK,            /*!< Function executed successfully */
    BAL_RC_NULL_POINTER,  /*!< A NULL pointer was given to a function */
    BAL_RC_WATCHDOG_ERROR /*!< There was an error during a balancing watchdog operation */
};

/*!
 * \brief Definition of the balancing parameters
 */
struct BalParams {
    volt_t target;    /*!< The minimum allowed voltage that should be reached while discharging in V */
    volt_t threshold; /*!< The minimum allowed difference between the minimum and maximum cell voltages in V */
};

/*!
 * \brief Type definition for the balancing handler structure
 *
 * \attention This structure should not be used outside of this module
 */
struct BalHandler {
    fsm_event_data_t event; /*!< The FSM event data */
    // primary_hv_balancing_status_converted_t status_can_payload;            /*!< The balancing status message canlib payload */
    struct Watchdog watchdog; /*!< The watchdog that stops the balancing procedure when timed out */

    bool active;             /*!< True if the balancing is active, false otherwise */
    struct BalParams params; /*!< The balancing parameters */

    union CanBmsMessages libcan_message_balancing_set;
};

#endif // BAL_H
