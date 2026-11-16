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
enum RequestContolTSRequestState {
    REQUEST_CONTROL_TS_REQUEST_STATE_OFF,  /*!< The TS request is off */
    REQUEST_CONTROL_TS_REQUEST_STATE_ON,   /*!< The TS request is on */
    REQUEST_CONTROL_TS_REQUEST_STATE_KEEP, /*!< The TS request is keep state */

    REQUEST_CONTROL_TS_REQUEST_STATE_COUNT
};

//TODO: MOVE THIS TO FLASH CONTROL API ONCE THE MODULE IS READY
/*!
 * \brief State of the flash request
 */
enum RequestControlFlashRequestState {
    REQUEST_CONTROL_FLASH_REQUEST_MAINBOARD, /*!< The flash request pointed to the mainboard */
    REQUEST_CONTROL_FLASH_REQUEST_CELLBOARD, /*!< The flash request is pointed to a cellboard */
    REQUEST_CONTROL_FLASH_REQUEST_GENERAL,   /*!< The flash request is for a board that is not specifically the mainboard or a cellboard */
    REQUEST_CONTROL_FLASH_REQUEST_NONE,      /*!< There is no more flash request */

    REQUEST_CONTROL_FLASH_REQUEST_COUNT
};

//TODO: MOVE THIS TO BAL CONTROL API ONCE THE MODULE IS READY
/*!
 * \brief Data for the balancing request
 */
struct RequestControlBalancingData {
    bool balancing_request_state; /*!< The state of the balancing request */
    float threshold;              /*!< The treshold for the balancing procedure */
};

/*!
 * \brief Data for the request control
 */
union RequestControlData {
    enum RequestContolTSRequestState ts_request_state;         /*!< The state of the TS request */
    enum RequestControlFlashRequestState flash_request_state;  /*!< The state of the flash request */
    struct RequestControlBalancingData balancing_request_data; /*!< The data for the balancing request*/
};

/*!
 * \brief Return code for the request control
 */
enum RequestControlReturnCode {
    REQUEST_CONTROL_RC_OK,             /*!< The operation was successful */
    REQUEST_CONTROL_RC_INVALID_REQUEST /*!< The request type is invalid */
};

#endif // REQUEST_CONTROL_H
