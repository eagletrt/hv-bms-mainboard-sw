/*!
 * \file cellboards-monitor-api.h
 * \date 2026-11-18
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Monitoring of the cellboards state and errors
 */

#ifndef CELLBOARDS_MONITOR_API_H
#define CELLBOARDS_MONITOR_API_H

#include "cellboards-monitor.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the cellboards monitor API
 *
 * \details Every cellboard starts in the CELLBOARDS_MONITOR_STATE_INIT state with no errors
 *
 * \retval CELLBOARDS_MONITOR_RC_OK if the initialization was successful
 */
enum CellboardsMonitorReturnCode cellboards_monitor_api_init(void);

/*!
 * \brief Set the FSM state of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] state The state reported by the cellboard
 *
 * \retval CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval CELLBOARDS_MONITOR_RC_INVALID_STATE if the given state does not exist
 * \retval CELLBOARDS_MONITOR_RC_OK otherwise
 */
enum CellboardsMonitorReturnCode cellboards_monitor_api_set_state(enum CellboardId cellboard, enum CellboardsMonitorState state);

/*!
 * \brief Set the errors of a specific cellboard
 *
 * \details The given flags replace the ones previously reported by the cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] errors The errors reported by the cellboard, bit N for CellboardsMonitorError N
 * \param[in] open_wires The open wire errors reported by the cellboard, bit N for open wire N + 1
 *
 * \retval CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval CELLBOARDS_MONITOR_RC_OK otherwise
 */
enum CellboardsMonitorReturnCode cellboards_monitor_api_set_errors(enum CellboardId cellboard, bit_flag16 errors, bit_flag16 open_wires);

/*!
 * \brief Reset the updated flag for the state of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval CELLBOARDS_MONITOR_RC_OK otherwise
 */
enum CellboardsMonitorReturnCode cellboards_monitor_api_reset_state_updated_flag(enum CellboardId cellboard);

/*!
 * \brief Reset the updated flag for the errors of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval CELLBOARDS_MONITOR_RC_OK otherwise
 */
enum CellboardsMonitorReturnCode cellboards_monitor_api_reset_errors_updated_flag(enum CellboardId cellboard);

/*!
 * \brief Get the FSM state of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The state of the cellboard, CELLBOARDS_MONITOR_STATE_COUNT if the cellboard does not exist
 */
enum CellboardsMonitorState cellboards_monitor_api_get_state(enum CellboardId cellboard);

/*!
 * \brief Check if a specific cellboard reported an error
 *
 * \param[in] cellboard The cellboard id
 * \param[in] error The error to check
 *
 * \retval true if the cellboard reported the error
 * \retval false if it did not, or if the cellboard or the error do not exist
 */
bool cellboards_monitor_api_is_error_set(enum CellboardId cellboard, enum CellboardsMonitorError error);

/*!
 * \brief Get the errors of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The errors of the cellboard, bit N for CellboardsMonitorError N, 0 if the cellboard does not exist
 */
bit_flag16 cellboards_monitor_api_get_errors(enum CellboardId cellboard);

/*!
 * \brief Get the open wire errors of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The open wire errors of the cellboard, bit N for open wire N + 1, 0 if the cellboard does not exist
 */
bit_flag16 cellboards_monitor_api_get_open_wires(enum CellboardId cellboard);

/*!
 * \brief Check if a specific cellboard is faulty
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval true if the cellboard is in its error state or reported any error, open wires included
 * \retval false otherwise, or if the cellboard does not exist
 */
bool cellboards_monitor_api_is_cellboard_faulty(enum CellboardId cellboard);

/*!
 * \brief Check if any cellboard is faulty
 *
 * \retval true if at least one cellboard is faulty, see cellboards_monitor_api_is_cellboard_faulty
 * \retval false otherwise
 */
bool cellboards_monitor_api_is_any_cellboard_faulty(void);

#endif // CELLBOARDS_MONITOR_API_H
