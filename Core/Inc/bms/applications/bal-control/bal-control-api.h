/*!
 * \file bal-control-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
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

/*!
 * \brief Initializes the balancing control
 *
 * \retval BAL_CONTROL_RC_OK if the initialization was successful
 */
enum BalancingControlReturnCode bal_control_api_init(void);

/*!
 * \brief Starts the balancing procedure
 *
 * \retval BAL_CONTROL_RC_OK if the balancing was started successfully
 * \retval BAL_CONTROL_RC_INVALID_PARAMETERS if the parameters are invalid
 * \retval BAL_CONTROL_RC_BALANCING_ALREADY_STARTED if the balancing is already started
 */
enum BalancingControlReturnCode bal_control_api_start_balancing(void);

/*!
 * \brief Routine to be called periodically to check the balancing status
 *
 * \retval BAL_CONTROL_RC_OK if the routine executed successfully
 * \retval BAL_CONTROL_RC_BALANCING_NOT_STARTED if the balancing has not been started
 */
enum BalancingControlReturnCode bal_control_api_routine(void);

/*!
 * \brief Stops the balancing procedure
 *
 * \retval BAL_CONTROL_RC_OK if the balancing was stopped successfully
 * \retval BAL_CONTROL_RC_BALANCING_NOT_STARTED if the balancing has not been started
 */
enum BalancingControlReturnCode bal_control_api_stop_balancing(void);

/*!
 * \brief Checks if the balancing control has valid parameters
 *
 * \retval true if the parameters are valid
 * \retval false if the parameters are invalid
 */
bool bal_control_api_has_valid_parameters(void);

/*!
 * \brief Checks if the pack is balanced according to the set threshold
 *
 * \retval true if the pack is balanced
 * \retval false if the pack is not balanced or if the parameters are invalid
 */
bool bal_control_api_is_pack_balanced(void);

/*!
 * \brief Gets the target voltage for balancing
 *
 * \returns The target voltage for balancing
 */
volt bal_control_api_get_target_voltage(void);

/*!
 * \brief Sets the balancing status for a specific cellboard
 *
 * \param set The desired balancing status (true for balancing, false for not balancing)
 * \param cellboard The cellboard to set the status for
 * \param cell The specific cell within the cellboard to set the status for
 *
 * \retval BAL_CONTROL_RC_OK if the status was set successfully
 * \retval BAL_CONTROL_RC_INVALID_PARAMETERS if the parameters are invalid
 */
enum BalancingControlReturnCode bal_control_api_set_balancing_status(bool set, enum CellboardId cellboard, uint8_t cell);

/*!
 * \brief Sets the balancing status of all the cells of a cellboard at once
 *
 * \param cellboard The cellboard to set the status for
 * \param flag Bit flag of the cells being discharged, bit N for cell N + 1
 *
 * \retval BAL_CONTROL_RC_OK if the status was set successfully
 * \retval BAL_CONTROL_RC_INVALID_PARAMETERS if the cellboard is invalid
 */
enum BalancingControlReturnCode bal_control_api_set_balancing_status_flag(enum CellboardId cellboard, uint32_t flag);

/*!
 * \brief Gets the balancing status for a specific cellboard
 *
 * \param cellboard The cellboard to get the status for
 * \returns The balancing status for the specified cellboard
 */
uint32_t bal_control_api_get_balancing_status(enum CellboardId cellboard);

#endif // BAL_CONTROL_API_H
