/*!
 * \file bal-control-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cellboard balancing control: starts the balancing, keeps track of the
 *        voltage delta and stops safely when done
 */

#ifndef BAL_CONTROL_API_H
#define BAL_CONTROL_API_H

#include "bal-control.h"

/*!
 * \brief Sets the parameters for the balancing control
 *
 * \param parameters The parameters to set
 * 
 * \retval BAL_CONTROL_RC_OK if the parameters were set successfully
 * \retval BAL_CONTROL_RC_INVALID_PARAMETERS if the parameters are invalid
 */
enum BalancingControlReturnCode bal_control_api_set_parameters(struct BalancingControlParameters parameters);

#endif // BAL_CONTROL_API_H
