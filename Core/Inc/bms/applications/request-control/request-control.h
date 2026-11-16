/*!
 * \file request-control.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the external requests control
 */

#ifndef REQUEST_CONTROL_H
#define REQUEST_CONTROL_H

#include "watchdogs-api.h"

/*!
 * \brief Type of external requests
 */
enum RequestControlRequestType {
    REQUEST_CONTROL_REQUEST_TS,
    REQUEST_CONTROL_REQUEST_BALANCING_ON, /*!< Turn the balancing on */
    REQUEST_CONTROL_REQUEST_FLASH         /*!< Recieved flash request */
};

/*!
 * \brief State of the TS request
 */
enum RequestContolTSRequestState {
    REQUEST_CONTROL_TS_REQUEST_STATE_OFF, /*!< The TS request is off */
    REQUEST_CONTROL_TS_REQUEST_STATE_ON,  /*!< The TS request is on */
    REQUEST_CONTROL_TS_REQUEST_STATE_KEEP /*!< The TS request is keep state */
};

/*!
 * \brief State of the flash request
 */
enum RequestControlFlashRequestState {
    REQUEST_CONTROL_FLASH_REQUEST_MAINBOARD, /*!< The flash request pointed to the mainboard */
    REQUEST_CONTROL_FLASH_REQUEST_CELLBOARD, /*!< The flash request is pointed to a cellboard */
    REQUEST_CONTROL_FLASH_REQUEST_GENERAL    /*!< The flash request is for a board that is not specifically the mainboard or a cellboard */
};

/*!
 * \brief Data for the request control
 */
union RequestControlData {
    enum RequestContolTSRequestState ts_request_state;        /*!< The state of the TS request */
    enum RequestControlFlashRequestState flash_request_state; /*!< The state of the flash request */
    bool balancing_request_state;                             /*!< The state of the balancing request */
};

/*!
 * \brief Return code for the request control
 */
enum RequestControlReturnCode {
    REQUEST_CONTROL_RC_OK = 0,              /*!< The operation was successful */
    REQUEST_CONTROL_RC_INVALID_REQUEST = 1, /*!< The request type is invalid */
    REQUEST_CONTROL_RC_ERROR = 2            /*!< An error occurred during the operation */
};

/*!
 * \brief Structure for the request control handler
 */
struct RequestControlHandler {
    struct WatchdogHandler *watchdog_handler; /*!< The watchdog pool to use for the request control */

    struct Watchdog TS_request_watchdog;        /*!< The watchdog for the TS request */
    struct Watchdog balancing_request_watchdog; /*!< The watchdog for the balancing request */
    struct Watchdog flash_request_watchdog;     /*!< The watchdog for the flash request */
};

#endif // REQUEST_CONTROL_H
