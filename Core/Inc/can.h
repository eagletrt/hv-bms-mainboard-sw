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

#include "mainboard-def.h"
#include "can-comm.h"

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

/**
 * @brief Send a message via the CAN bus
 *
 * @param network The canlib network to select
 * @param id The identifier of the CAN message
 * @param frame_type The frame type of the message (see CanFrameType)
 * @param data A pointer to the data to send
 * @param size The size of the payload in bytes
 *
 * @return CanCommReturnCode
 *     - CAN_COMM_INVALID_NETWORK if the network is not associated with any existing CAN bus
 *     - CAN_COMM_INVALID_INDEX if the id is not a valid identifier
 *     - CAN_COMM_INVALID_PAYLOAD_SIZE if the payload size exceed the maximum allowd message length
 *     - CAN_COMM_INVALID_FRAME_TYPE the given frame type does not correspond to any existing CAN frame type
 *     - CAN_COMM_TRANSMISSION_ERROR there was an error during the transmission of the message   
 *     - CAN_COMM_OK otherwise
 */
CanCommReturnCode can_send(
    const CanNetwork network,
    const can_id_t id,
    const CanFrameType frame_type,
    const uint8_t * const data,
    const size_t size
);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

