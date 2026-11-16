/*!
 * \file flash-control.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the flash procedure control
 */

#ifndef FLASH_CONTROL_H
#define FLASH_CONTROL_H

#include <stdbool.h>

/*!
 * \brief State of the flash request
 */
enum FlashControlRequestState {
    FLASH_CONTROL_REQUEST_MAINBOARD, /*!< The flash request pointed to the mainboard */
    FLASH_CONTROL_REQUEST_CELLBOARD, /*!< The flash request is pointed to a cellboard */
    FLASH_CONTROL_REQUEST_GENERAL,   /*!< The flash request is for a board that is not specifically the mainboard or a cellboard */
    FLASH_CONTROL_REQUEST_NONE,      /*!< There is no more flash request */

    FLASH_CONTROL_REQUEST_COUNT
};

/*!
 * \brief The return codes for the flash control
 */
enum FlashControlReturnCode {
    FLASH_CONTROL_RC_OK,                   /*!< Operation completed successfully */
    FLASH_CONTROL_RC_INVALID_PARAMETERS,   /*!< Invalid parameters provided */
    FLASH_CONTROL_RC_FLASH_NOT_STARTED,    /*!< The flash procedure has not been started */
    FLASH_CONTROL_RC_FLASH_ALREADY_STARTED /*!< The flash procedure has already been started */
};

/*!
 * \brief Handler for the flash control
 */
struct FlashControlHandler {
    enum FlashControlRequestState target; /*!< The target of the flash procedure */
    bool flash_active;                    /*!< Flag indicating if the flash procedure is currently active */
    bool valid_parameters;                /*!< Flag indicating if the parameters are valid */
};

#endif // FLASH_CONTROL_H
