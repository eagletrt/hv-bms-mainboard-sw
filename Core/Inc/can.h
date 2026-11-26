/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "can-communication.h"

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

extern CAN_HandleTypeDef hcan2;

/* USER CODE BEGIN Private defines */

#define HCAN_PRIMARY hcan1
#define HCAN_BMS hcan2

/* USER CODE END Private defines */

void MX_CAN1_Init(void);
void MX_CAN2_Init(void);

/* USER CODE BEGIN Prototypes */

/** @brief Configure CAN1 peripheral to work with 250k baudrate */
void MX_CAN1_Init_250K(void);
/* @brief Configure CAN1 peripheral to work with 1M baudrate */
void MX_CAN1_Init_1M(void);

/*!
 * \brief Send a CAN frame on the BMS CAN network.
 *
 * \param[in] frame The frame to send.
 *
 * \retval CAN_COMMUNICATION_RC_OK if the frame was sent successfully.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if the frame pointer was NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if the frame length exceeds CAN_COMMUNICATION_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR if every TX mailbox is busy or the HAL call reported a failure.
 */
enum CanCommunicationReturnCode can_send_bms(const struct CanCommunicationFrame *frame);

/*!
 * \brief Send a CAN frame on the primary CAN network.
 *
 * \param[in] frame The frame to send.
 *
 * \retval CAN_COMMUNICATION_RC_OK if the frame was sent successfully.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if the frame pointer was NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if the frame length exceeds CAN_COMMUNICATION_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR if every TX mailbox is busy or the HAL call reported a failure.
 */
enum CanCommunicationReturnCode can_send_primary(const struct CanCommunicationFrame *frame);

/*!
 * \brief Tell if the primary network has a free TX mailbox, so that a frame can be sent right away
 *
 * \return true if a mailbox is free, false otherwise
 */
bool can_tx_ready_primary(void);

/*!
 * \brief Tell if the BMS network has a free TX mailbox, so that a frame can be sent right away
 *
 * \return true if a mailbox is free, false otherwise
 */
bool can_tx_ready_bms(void);

/*!
 * \brief Read the state of the primary network transceiver and its counters
 *
 * \details Main loop only, the reading also accounts the TX mailboxes and the last bus error
 *
 * \param[out] status The status to fill
 *
 * \retval CAN_COMMUNICATION_RC_OK on success
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p status is \c NULL
 */
enum CanCommunicationReturnCode can_get_status_primary(struct CanCommunicationTransceiverStatus *status);

/*!
 * \brief Read the state of the BMS network transceiver and its counters, see \ref can_get_status_primary
 *
 * \param[out] status The status to fill
 *
 * \retval CAN_COMMUNICATION_RC_OK on success
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p status is \c NULL
 */
enum CanCommunicationReturnCode can_get_status_bms(struct CanCommunicationTransceiverStatus *status);

/*!
 * \brief Configure the can primary to use 250K baudrate if the handcart is connected or 1M otherwise
 *
 * \param[in] is_handcart_connected True if the handcart is connected, false otherwise
 */
void can_configure_and_start_primary(bool is_handcart_connected);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */
