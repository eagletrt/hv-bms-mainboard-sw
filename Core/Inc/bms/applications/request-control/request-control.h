/*!
 * \file request-control.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the external requests control
 */

#ifndef REQUEST_CONTROL_H
#define REQUEST_CONTROL_H

#include <stdbool.h>
#include "bal-control.h"
#include "flash-control.h"

/*!
 * \brief Type of external requests
 */
enum RequestControlRequestType {
    REQUEST_CONTROL_REQUEST_TS,
    REQUEST_CONTROL_REQUEST_BALANCING, /*!< Turn the balancing on */
    REQUEST_CONTROL_REQUEST_FLASH,     /*!< Recieved flash request */

    REQUEST_CONTROL_REQUEST_COUNT
};

/*!
 * \brief State of the TS request
 */
enum RequestControlTSRequestState {
    REQUEST_CONTROL_TS_REQUEST_STATE_OFF,  /*!< The TS request is off */
    REQUEST_CONTROL_TS_REQUEST_STATE_ON,   /*!< The TS request is on */
    REQUEST_CONTROL_TS_REQUEST_STATE_KEEP, /*!< The TS request is keep state */

    REQUEST_CONTROL_TS_REQUEST_STATE_COUNT
};

/*!
 * \brief Data for the request control
 */
union RequestControlData {
    enum RequestControlTSRequestState ts_request_state;       /*!< The state of the TS request */
    enum FlashControlRequestState flash_request_state;        /*!< The state of the flash request */
    struct BalancingControlParameters balancing_request_data; /*!< The data for the balancing request*/
};

struct RequestControlRequest {
    enum RequestControlRequestType request_type; /*!< The type of the request */
    union RequestControlData request_data;       /*!< The data for the request */
};

/*!
 * \brief Return code for the request control
 */
enum RequestControlReturnCode {
    REQUEST_CONTROL_RC_OK,             /*!< The operation was successful */
    REQUEST_CONTROL_RC_INVALID_REQUEST /*!< The request type is invalid */
};

#endif // REQUEST_CONTROL_H
