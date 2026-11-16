/*!
 * \file bal-control-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cellboard balancing control: starts the balancing, keeps track of the
 *        voltage delta and stops safely when done
 */

#include "bal-control-api.h"
#include "eagletrt.h"
#include "voltages-api.h"
#include <string.h>

EAGLETRT_STATIC struct BalancingControlHandler bal_control_handler;

enum BalancingControlReturnCode bal_control_api_set_parameters(const struct BalancingControlParameters parameters) {
    if (parameters.threshold < 0.0F) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    bal_control_handler.parameters = parameters;
    bal_control_handler.valid_parameters = true;
    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_init(void) {
    memset(&bal_control_handler, 0, sizeof(bal_control_handler));
    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_start_balancing(void) {
    if (!bal_control_handler.valid_parameters) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    if (bal_control_handler.balancing_active) {
        return BAL_CONTROL_RC_BALANCING_ALREADY_STARTED;
    }

    bal_control_handler.target = voltages_api_get_pack_info().min;

    // START THE TASK THAT FORWARDS THE BALANCING PARAMETERS TO THE CELLBOARDS

    bal_control_handler.balancing_active = true;
    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_routine(void) {
    if (!bal_control_handler.balancing_active) {
        return BAL_CONTROL_RC_BALANCING_NOT_STARTED;
    }

    // implement way for the mainboard to choose when cellboards are actively balancing so that it can update the target voltage accordingly
    // the routine should be check target voltage -> x cycles of discharge -> wait for stable voltages -> update target voltage -> repeat until delta is below threshold

    volt delta = voltages_api_get_pack_info().max - voltages_api_get_pack_info().min;

    if (delta <= bal_control_handler.parameters.threshold) {
        bal_control_handler.balancing_active = false;
    }

    return BAL_CONTROL_RC_OK;
}

enum BalancingControlReturnCode bal_control_api_stop_balancing(void) {
    if (!bal_control_handler.balancing_active) {
        return BAL_CONTROL_RC_BALANCING_NOT_STARTED;
    }

    // STOP THE TASK THAT FORWARDS THE BALANCING PARAMETERS TO THE CELLBOARDS

    bal_control_handler.balancing_active = false;
    return BAL_CONTROL_RC_OK;
}