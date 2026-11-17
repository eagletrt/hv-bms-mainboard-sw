/*!
 * \file bal-control.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Data types and structures for the cellboard balancing control
 */

#ifndef BAL_CONTROL_H
#define BAL_CONTROL_H

#include <stdbool.h>
#include "mainboard-def.h"

/*! 
 * \brief The return codes for the balancing control 
 */
enum BalancingControlReturnCode {
    BAL_CONTROL_RC_OK,                       /*!< Operation completed successfully */
    BAL_CONTROL_RC_INVALID_PARAMETERS,       /*!< Invalid parameters provided */
    BAL_CONTROL_RC_BALANCING_NOT_STARTED,    /*!< Balancing has not been started */
    BAL_CONTROL_RC_BALANCING_ALREADY_STARTED /*!< Balancing has already been started */
};

/*!
 * \brief Parameters for the balancing control
 *
 * \details This structure holds the parameters for the balancing control.
 */
struct BalancingControlParameters {
    bool balancing_request_state; /*!< The state of the balancing request */
    volt threshold;               /*!< The threshold for the balancing procedure */
};

/*!
 * \brief Handler for the balancing control
 */
struct BalancingControlHandler {
    struct BalancingControlParameters parameters; /*!< The parameters for the balancing control */
    bool balancing_active;                        /*!< Flag indicating if balancing is currently active */
    volt target;                                  /*!< The target voltage for balancing */
    bool valid_parameters;                        /*!< Flag indicating if the parameters are valid */
};

#endif // BAL_CONTROL_H
