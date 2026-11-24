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
    BAL_CONTROL_RC_OK,                 /*!< Operation completed successfully */
    BAL_CONTROL_RC_INVALID_PARAMETERS, /*!< Invalid parameters provided */
};

/*!
 * \brief Parameters for the balancing control
 */
struct BalancingControlParameters {
    volt threshold; /*!< The voltage delta below which the pack is considered balanced */
};

static_assert(CELLBOARD_SEGMENT_SERIES_COUNT <= 32, "balancing_cells flag is too small to hold all cellboard states");

/*!
 * \brief Handler for the balancing control
 */
struct BalancingControlHandler {
    struct BalancingControlParameters parameters; /*!< The parameters for the balancing control */
    bool valid_parameters;                        /*!< Flag indicating if the parameters are valid */
    bool discharging;                             /*!< Flag indicating if the cellboards are told to discharge */
    volt target;                                  /*!< The target voltage of the discharge */
    uint32_t balancing_cells[CELLBOARD_COUNT];    /*!< Bit flag containing the balancing status of the single cells as reported by the cellboards */
};

#endif // BAL_CONTROL_H
