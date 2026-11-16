/*!
 * \file flash-control-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Flash procedure control: resets the microcontroller when the mainboard is
 *        the target or enables the CAN bridge when a cellboard is the target
 */

#include "flash-control-api.h"
#include "eagletrt.h"
#include <string.h>

EAGLETRT_STATIC struct FlashControlHandler flash_control_handler;

enum FlashControlReturnCode flash_control_api_init(void) {
    memset(&flash_control_handler, 0, sizeof(flash_control_handler));
    flash_control_handler.target = REQUEST_CONTROL_FLASH_REQUEST_NONE;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_set_target(const enum RequestControlFlashRequestState target) {
    if (target >= REQUEST_CONTROL_FLASH_REQUEST_COUNT) {
        return FLASH_CONTROL_RC_INVALID_PARAMETERS;
    }

    if (target == REQUEST_CONTROL_FLASH_REQUEST_NONE) {
        flash_control_handler.target = target;
        flash_control_handler.valid_parameters = false;
        return FLASH_CONTROL_RC_OK;
    }

    flash_control_handler.target = target;
    flash_control_handler.valid_parameters = true;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_start_flash(void) {
    if (!flash_control_handler.valid_parameters) {
        return FLASH_CONTROL_RC_INVALID_PARAMETERS;
    }
    if (flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_ALREADY_STARTED;
    }

    if (flash_control_handler.target == REQUEST_CONTROL_FLASH_REQUEST_MAINBOARD) {
        pcu_api_reset_microcontroller();
    }

    if (flash_control_handler.target == REQUEST_CONTROL_FLASH_REQUEST_CELLBOARD) {
        can_bridge_api_enable_bridge();
    }

    serializer_api_suppress_comms(true);

    flash_control_handler.flash_active = true;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_routine(void) {
    if (!flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_NOT_STARTED;
    }

    // Todaloo do nothing ->-

    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_stop_flash(void) {
    if (!flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_NOT_STARTED;
    }

    serializer_api_suppress_comms(false);

    can_bridge_api_disable_bridge();

    flash_control_handler.flash_active = false;
    return FLASH_CONTROL_RC_OK;
}

bool flash_control_api_has_valid_parameters(void) {
    return flash_control_handler.valid_parameters;
}
