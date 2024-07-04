/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */

#include <string.h>

#include "can-comm.h"

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_12TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  /* HAL considers IdLow and IdHigh not as just the ID of the can message but
      as the combination of: 
      STDID + RTR + IDE + 4 most significant bits of EXTID
  */
  CAN_FilterTypeDef filter = {
      .FilterActivation = CAN_FILTER_ENABLE,
      .FilterBank = 0,
      .FilterFIFOAssignment = CAN_FILTER_FIFO0,
      .FilterIdHigh = ((1U << 11) - 1) << 5, // Take all ids to 2^11 - 1
      .FilterIdLow = 0, // Take all ids from 0
      .FilterMaskIdHigh = 0,
      .FilterMaskIdLow = 0,
      .FilterMode = CAN_FILTERMODE_IDMASK,
      .FilterScale = CAN_FILTERSCALE_16BIT,
      .SlaveStartFilterBank = 14
  };
  
  // Enable filters and start CAN
  HAL_CAN_ConfigFilter(&HCAN_PRIMARY, &filter);
  HAL_CAN_ActivateNotification(&HCAN_PRIMARY, CAN_IT_ERROR | CAN_IT_RX_FIFO0_MSG_PENDING);
  HAL_CAN_Start(&HCAN_PRIMARY);
  /* USER CODE END CAN1_Init 2 */

}
/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 3;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_12TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  /* HAL considers IdLow and IdHigh not as just the ID of the can message but
      as the combination of: 
      STDID + RTR + IDE + 4 most significant bits of EXTID
  */
  CAN_FilterTypeDef filter = {
      .FilterActivation = CAN_FILTER_ENABLE,
      .FilterBank = 14,
      .FilterFIFOAssignment = CAN_FILTER_FIFO1,
      .FilterIdHigh = ((1U << 11) - 1) << 5, // Take all ids to 2^11 - 1
      .FilterIdLow = 0, // Take all ids from 0
      .FilterMaskIdHigh = 0,
      .FilterMaskIdLow = 0,
      .FilterMode = CAN_FILTERMODE_IDMASK,
      .FilterScale = CAN_FILTERSCALE_16BIT,
      .SlaveStartFilterBank = 14
  };
  
  // Enable filters and start CAN
  HAL_CAN_ConfigFilter(&HCAN_BMS, &filter);
  HAL_CAN_ActivateNotification(&HCAN_BMS, CAN_IT_ERROR | CAN_IT_RX_FIFO1_MSG_PENDING);
  HAL_CAN_Start(&HCAN_BMS);
  /* USER CODE END CAN2_Init 2 */

}

static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = CAN_PRIMARY_RX_Pin|CAN_PRIMARY_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = CAN_BMS_RX_Pin|CAN_BMS_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, CAN_PRIMARY_RX_Pin|CAN_PRIMARY_TX_Pin);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, CAN_BMS_RX_Pin|CAN_BMS_TX_Pin);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief Get the pointer to the CAN handler from the corresponding canlib network
 *
 * @param network The canlib network
 *
 * @return CAN_HandleTypeDef * A pointer to the CAN handler or NULL if the network
 * does not corresponds to any valid handler
 */
CAN_HandleTypeDef * _can_get_peripheral_from_network(CanNetwork network) {
    switch (network) {
        case CAN_NETWORK_BMS:
            return &HCAN_BMS;
        case CAN_NETWORK_PRIMARY:
            return &HCAN_PRIMARY;
        default:
            return NULL;
    }
}

/**
 * @brief Get CAN TxFrameType value from the CanFrameType enum
 *
 * @param type The frame type enum value
 * 
 * @return int32_t The frame type or negative error code
 */
int32_t _can_get_rtr_from_frame_type(CanFrameType type) {
    switch (type) {
        case CAN_FRAME_TYPE_DATA:
            return CAN_RTR_DATA;
        case CAN_FRAME_TYPE_REMOTE:
            return CAN_RTR_REMOTE;
        default:
            return -1;
    }
}

/**
 * @brief Get the canFrameType enum value from the CAN TxFrameType
 *
 * @param rtr The CAN frame type value
 * 
 * @return CanFrameType The frame type enum value or negative error code
 */
CanFrameType _can_get_frame_type_from_rtr(uint32_t rtr) {
    switch (rtr) {
        case CAN_RTR_DATA:
            return CAN_FRAME_TYPE_DATA;
        case CAN_RTR_REMOTE:
            return CAN_FRAME_TYPE_REMOTE;
        default:
            return -1;
    }
}

// TODO: Return and check errors
CanCommReturnCode can_send(
    CanNetwork network,
    can_id_t id,
    CanFrameType frame_type,
    const uint8_t * data,
    size_t size)
{
    if (network >= CAN_NETWORK_COUNT)
        return CAN_COMM_INVALID_NETWORK;
    if (id > CAN_COMM_ID_MASK)
        return CAN_COMM_INVALID_INDEX;
    if (size > CAN_COMM_MAX_PAYLOAD_BYTE_SIZE)
        return CAN_COMM_INVALID_PAYLOAD_SIZE;

    // Get and check the CAN handler
    CAN_HandleTypeDef * hcan = _can_get_peripheral_from_network(network);
    if (hcan == NULL)
        return CAN_COMM_INVALID_NETWORK;

    // Get and check the frame type
    int32_t type = _can_get_rtr_from_frame_type(frame_type);
    if (type < 0)
        return CAN_COMM_INVALID_FRAME_TYPE;
 
    // Setup transmission header
    const CAN_TxHeaderTypeDef header = {
        .StdId = id,
        .IDE = CAN_ID_STD,
        .RTR = type,
        .DLC = size,
        .ExtId = 0U,
        .TransmitGlobalTime = DISABLE
    };

    // Send message
    uint32_t mailbox = 0U;
    if (HAL_CAN_AddTxMessage(hcan, &header, data, &mailbox) != HAL_OK)
        return CAN_COMM_TRANSMISSION_ERROR;
    return CAN_COMM_OK;
}


// TODO: Define CAN RX callbacks
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan) {
    if (hcan->Instance != HCAN_PRIMARY.Instance)
        return;

    CAN_RxHeaderTypeDef header;
    uint8_t data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, data) != HAL_OK)
        Error_Handler();

    // Ignore extended IDs
    if (header.IDE == CAN_ID_EXT)
        return;

    CanFrameType frame_type = _can_get_frame_type_from_rtr(header.RTR);
    if (frame_type < 0)
        return;

    can_comm_rx_add(
        CAN_NETWORK_PRIMARY,
        bms_index_from_id(header.StdId),
        frame_type,
        data,
        header.DLC
    );

}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != HCAN_BMS.Instance)
        return;

    CAN_RxHeaderTypeDef header;
    uint8_t data[CAN_COMM_MAX_PAYLOAD_BYTE_SIZE];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, data) != HAL_OK)
        Error_Handler();

    // Ignore extended IDs
    if (header.IDE == CAN_ID_EXT)
        return;

    CanFrameType frame_type = _can_get_frame_type_from_rtr(header.RTR);
    if (frame_type < 0)
        return;

    can_comm_rx_add(
        CAN_NETWORK_BMS,
        bms_index_from_id(header.StdId),
        frame_type,
        data,
        header.DLC
    );
}

/* USER CODE END 1 */
