/*!
 * \file bal-control-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Cellboard balancing control: holds the balancing parameters, the discharge
 *        target and the balancing status reported by the cellboards
 *
 * \details The module only holds the state, the FSM decides when to read and when to discharge:
 *          - BALANCING_READ stops the discharge, waits for the voltages to settle and updates the target
 *          - BALANCING_DISCHARGE starts the discharge for a fixed time
 *
 *          The balancing set message is sent periodically by the serializer tasks with the
 *          values of \ref bal_control_api_is_discharging, \ref bal_control_api_get_target_voltage
 *          and \ref bal_control_api_get_threshold, so starting or stopping the discharge only
 *          changes the next message sent
 */

#ifndef BAL_CONTROL_API_H
#define BAL_CONTROL_API_H

#include "bal-control.h"

/*!
 * \brief Initializes the balancing control
 *
 * \details The parameters are invalid and the discharge is stopped
 *
 * \retval BAL_CONTROL_RC_OK if the initialization was successful
 */
enum BalancingControlReturnCode bal_control_api_init(void);

/*!
 * \brief Sets the parameters for the balancing control
 *
 * \details Valid parameters are stored even while the balancing is not requested
 *
 * \param parameters The parameters to set
 *
 * \retval BAL_CONTROL_RC_OK if the parameters were set successfully
 * \retval BAL_CONTROL_RC_INVALID_PARAMETERS if the parameters are invalid
 */
enum BalancingControlReturnCode bal_control_api_set_parameters(struct BalancingControlParameters parameters);

/*!
 * \brief Checks if the balancing control has valid parameters
 *
 * \retval true if the parameters are valid
 * \retval false if the parameters are invalid
 */
bool bal_control_api_has_valid_parameters(void);

/*!
 * \brief Sets the discharge target to the current minimum cell voltage of the pack
 *
 * \attention Must be called with the discharge stopped and the voltages settled,
 *            otherwise the minimum is lower than the real one
 */
void bal_control_api_update_target(void);

/*!
 * \brief Tells the cellboards to discharge the cells above the target
 *
 * \details Does nothing if the parameters are invalid, calling it while already
 *          discharging has no effect
 */
void bal_control_api_start_discharge(void);

/*!
 * \brief Tells the cellboards to stop discharging
 *
 * \details Always succeeds, calling it while not discharging has no effect
 */
void bal_control_api_stop_discharge(void);

/*!
 * \brief Checks if the cellboards are told to discharge
 *
 * \retval true if the discharge was started and not stopped yet
 * \retval false otherwise
 */
bool bal_control_api_is_discharging(void);

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
 * \returns The target voltage set by the last \ref bal_control_api_update_target
 */
volt bal_control_api_get_target_voltage(void);

/*!
 * \brief Gets the voltage delta threshold for balancing
 *
 * \returns The threshold below which the pack is considered balanced
 */
volt bal_control_api_get_threshold(void);

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
