/*!
 * \file request-control-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief External requests control: validates the requests received via CAN
 *        and sets the corresponding mainboard status flags
 */

#ifndef REQUEST_CONTROL_API_H
#define REQUEST_CONTROL_API_H

#include "request-control.h"

/*!
 * \brief Handle an external request
 * \param request_type The type of the request
 * \param data The data associated with the request
 *
 * \retval REQUEST_CONTROL_RC_OK if the request was handled successfully
 * \retval REQUEST_CONTROL_RC_INVALID_REQUEST if the request type is invalid
 * \retval REQUEST_CONTROL_RC_ERROR if an error occurred while handling the request
 */
enum RequestControlReturnCode request_control_api_handle_request(enum RequestControlRequestType request_type, union RequestControlData data);

#endif // REQUEST_CONTROL_API_H
