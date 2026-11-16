/*!
 * \file request-control-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief External requests control: validates the requests received via CAN
 *        and sets the corresponding mainboard status flags
 */

#include "request-control-api.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"
#include "request-control.h"
#include "flash-control-api.h"
#include "bal-control-api.h"
#include "eagletrt.h"

/*!
 * \brief Set a TS request, this ensures that only one of the flags is enabled at once.
 * 
 * \param req_state The state that you want the flag to be
 */
EAGLETRT_STATIC void prv_set_TS_request(enum RequestConetolTSRequestState req_state) {

    if (req_state >= REQUEST_CONTROL_TS_REQUEST_STATE_COUNT) {
        req_state = (REQUEST_CONTROL_TS_REQUEST_STATE_OFF);
    }

    mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_TS_OFF, (req_state == REQUEST_CONTROL_TS_REQUEST_STATE_OFF));
    mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_TS_ON, (req_state == REQUEST_CONTROL_TS_REQUEST_STATE_ON));
    mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_KEEP_TS_ON, (req_state == REQUEST_CONTROL_TS_REQUEST_STATE_KEEP));
}

enum RequestControlReturnCode request_control_api_handle_request(struct RequestControlRequest request) {

    enum RequestControlRequestType request_type = request.request_type;
    union RequestControlData data = request.request_data;

    if (request_type >= REQUEST_CONTROL_REQUEST_COUNT) {
        return REQUEST_CONTROL_RC_INVALID_REQUEST;
    }
    switch (request_type) {
        case REQUEST_CONTROL_REQUEST_TS:

            prv_set_TS_request(data.ts_request_state);

            return REQUEST_CONTROL_RC_OK;

        case REQUEST_CONTROL_REQUEST_FLASH:

            if (flash_control_api_set_target(data.flash_request_state)) {
                // Set the bit only AFTER the module validated the data
                mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_FLASH, (data.flash_request_state != REQUEST_CONTROL_FLASH_REQUEST_NONE));

                return REQUEST_CONTROL_RC_OK;
            }
            break;

        case REQUEST_CONTROL_REQUEST_BALANCING:

            if (bal_control_api_set_parameters(data.balancing_request_data)) {
                // Set the bit only AFTER the module validated the data
                mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_BALANCING, data.balancing_request_data.balancing_request_state);

                return REQUEST_CONTROL_RC_OK;
            }
            break;
        default:
            return REQUEST_CONTROL_RC_INVALID_REQUEST;
    }
    return REQUEST_CONTROL_RC_INVALID_REQUEST;
}