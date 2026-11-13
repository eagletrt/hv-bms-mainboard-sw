/*!
 * \file mainboard-status-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Mainboard status, the single source of truth for the requests, faults
 *        and conditions of the mainboard
 */

#ifndef MAINBOARD_STATUS_API_H
#define MAINBOARD_STATUS_API_H

#include "mainboard-status.h"

/*!
 * \brief Clear every fault, request and condition
 *
 * \warning Must be called only once at startup, faults must never be cleared at runtime
 */
void mainboard_status_api_init(void);

/*!
 * \defgroup Faults
 * \{
 */

/*!
 * \brief Set a fault
 *
 * \details There is no way to clear a fault once it is set
 *
 * \param fault The fault to set
 *
 * \retval MAINBOARD_STATUS_RC_INVALID_FAULT if the fault does not exist
 * \retval MAINBOARD_STATUS_RC_OK otherwise
 */
enum MainboardStatusReturnCode mainboard_status_api_set_fault(enum MainboardStatusFault fault);

/*!
 * \brief Check if a fault is set
 *
 * \param fault The fault to check
 *
 * \returns true if the fault is set, false if it is not or it does not exist
 */
bool mainboard_status_api_is_fault_set(enum MainboardStatusFault fault);

/*!
 * \brief Check if any fault is set
 *
 * \returns true if at least one fault is set, false otherwise
 */
bool mainboard_status_api_has_faults(void);

/*!
 * \brief Get all the faults
 *
 * \returns A bit flag where each bit is a fault, see \ref MAINBOARD_STATUS_BIT
 */
bit_flag32 mainboard_status_api_get_faults(void);

/*!
 * \brief Get the name of a fault, used for logging
 *
 * \param fault The fault
 *
 * \returns The name of the fault, or "invalid fault" if it does not exist
 */
const char *mainboard_status_api_get_fault_name(enum MainboardStatusFault fault);

/*! \} */

/*!
 * \defgroup Requests
 * \{
 */

/*!
 * \brief Set or clear a request
 *
 * \param request The request to update
 * \param active true to set the request, false to clear it
 *
 * \retval MAINBOARD_STATUS_RC_INVALID_REQUEST if the request does not exist
 * \retval MAINBOARD_STATUS_RC_OK otherwise
 */
enum MainboardStatusReturnCode mainboard_status_api_set_request(enum MainboardStatusRequest request, bool active);

/*!
 * \brief Check if a request is set
 *
 * \param request The request to check
 *
 * \returns true if the request is set, false if it is not or it does not exist
 */
bool mainboard_status_api_is_request_set(enum MainboardStatusRequest request);

/*!
 * \brief Get all the requests
 *
 * \returns A bit flag where each bit is a request, see \ref MAINBOARD_STATUS_BIT
 */
bit_flag32 mainboard_status_api_get_requests(void);

/*!
 * \brief Get the name of a request, used for logging
 *
 * \param request The request
 *
 * \returns The name of the request, or "invalid request" if it does not exist
 */
const char *mainboard_status_api_get_request_name(enum MainboardStatusRequest request);

/*! \} */

/*!
 * \defgroup Conditions
 * \{
 */

/*!
 * \brief Update a condition
 *
 * \param condition The condition to update
 * \param value true if the condition holds, false otherwise
 *
 * \retval MAINBOARD_STATUS_RC_INVALID_CONDITION if the condition does not exist
 * \retval MAINBOARD_STATUS_RC_OK otherwise
 */
enum MainboardStatusReturnCode mainboard_status_api_set_condition(enum MainboardStatusCondition condition, bool value);

/*!
 * \brief Get the value of a condition
 *
 * \param condition The condition to read
 *
 * \returns true if the condition holds, false if it does not or it does not exist
 */
bool mainboard_status_api_get_condition(enum MainboardStatusCondition condition);

/*!
 * \brief Get all the conditions
 *
 * \returns A bit flag where each bit is a condition, see \ref MAINBOARD_STATUS_BIT
 */
bit_flag32 mainboard_status_api_get_conditions(void);

/*!
 * \brief Check that a group of conditions has the expected values
 *
 * \details Example: AIR- closed and SD end closed, precharge relay open
 * \code
 * const bit_flag32 mask = MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |
 *                         MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED) |
 *                         MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED);
 * const bit_flag32 expected = MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_AIRN_CLOSED) |
 *                             MAINBOARD_STATUS_BIT(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED);
 * \endcode
 *
 * \param mask The conditions to check
 * \param expected The expected value of each condition in the mask (bits outside the mask are ignored)
 * \param mismatches OPTIONAL: set to a bit flag with a bit for every condition
 *                   in the mask that doesn't match, 0 if all of them match
 *
 * \returns true if every condition in the mask has the expected value, false otherwise
 */
bool mainboard_status_api_check_conditions(bit_flag32 mask, bit_flag32 expected, bit_flag32 *mismatches);

/*!
 * \brief Get the name of a condition, used for logging
 *
 * \param condition The condition
 *
 * \returns The name of the condition, or "invalid condition" if it does not exist
 */
const char *mainboard_status_api_get_condition_name(enum MainboardStatusCondition condition);

/*! \} */

#endif // MAINBOARD_STATUS_API_H
