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

EAGLETRT_STATIC struct BalancingControlHandler bal_control_handler;

enum BalancingControlReturnCode bal_control_api_set_parameters(const struct BalancingControlParameters parameters) {
    if (parameters.threshold < 0.0F) {
        return BAL_CONTROL_RC_INVALID_PARAMETERS;
    }
    bal_control_handler.parameters = parameters;
    return BAL_CONTROL_RC_OK;
}
