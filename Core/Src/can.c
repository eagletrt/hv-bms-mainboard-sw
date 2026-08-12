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

#include "can-communication.h"
#include "can-communication-api.h"
#include "can-primary.h"
#include "mainboard-def.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "stm32f4xx_hal_can.h"

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void) {

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
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN CAN1_Init 2 */
    /* USER CODE END CAN1_Init 2 */
}
/* CAN2 init function */
void MX_CAN2_Init(void) {

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
    if (HAL_CAN_Init(&hcan2) != HAL_OK) {
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
        .FilterIdLow = 0,                      // Take all ids from 0
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

static uint32_t HAL_RCC_CAN1_CLK_ENABLED = 0;

void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (canHandle->Instance == CAN1) {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
        /* CAN1 clock enable */
        HAL_RCC_CAN1_CLK_ENABLED++;
        if (HAL_RCC_CAN1_CLK_ENABLED == 1) {
            __HAL_RCC_CAN1_CLK_ENABLE();
        }

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
        GPIO_InitStruct.Pin = CAN_RX_Pin | CAN_TX_Pin;
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
    } else if (canHandle->Instance == CAN2) {
        /* USER CODE BEGIN CAN2_MspInit 0 */

        /* USER CODE END CAN2_MspInit 0 */
        /* CAN2 clock enable */
        __HAL_RCC_CAN2_CLK_ENABLE();
        HAL_RCC_CAN1_CLK_ENABLED++;
        if (HAL_RCC_CAN1_CLK_ENABLED == 1) {
            __HAL_RCC_CAN1_CLK_ENABLE();
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
        GPIO_InitStruct.Pin = isoCAN_RX_Pin | isoCAN_TX_Pin;
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

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle) {

    if (canHandle->Instance == CAN1) {
        /* USER CODE BEGIN CAN1_MspDeInit 0 */

        /* USER CODE END CAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_CAN1_CLK_ENABLED--;
        if (HAL_RCC_CAN1_CLK_ENABLED == 0) {
            __HAL_RCC_CAN1_CLK_DISABLE();
        }

        /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
        HAL_GPIO_DeInit(GPIOA, CAN_RX_Pin | CAN_TX_Pin);

        /* CAN1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
        /* USER CODE BEGIN CAN1_MspDeInit 1 */

        /* USER CODE END CAN1_MspDeInit 1 */
    } else if (canHandle->Instance == CAN2) {
        /* USER CODE BEGIN CAN2_MspDeInit 0 */

        /* USER CODE END CAN2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_CAN2_CLK_DISABLE();
        HAL_RCC_CAN1_CLK_ENABLED--;
        if (HAL_RCC_CAN1_CLK_ENABLED == 0) {
            __HAL_RCC_CAN1_CLK_DISABLE();
        }

        /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
        HAL_GPIO_DeInit(GPIOB, isoCAN_RX_Pin | isoCAN_TX_Pin);

        /* CAN2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
        /* USER CODE BEGIN CAN2_MspDeInit 1 */

        /* USER CODE END CAN2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

void prv_can_primary_start() {
    /* HAL considers IdLow and IdHigh not as just the ID of the can message but
      as the combination of:
      STDID + RTR + IDE + 4 most significant bits of EXTID
    */
    CAN_FilterTypeDef filter = {
        .FilterActivation = CAN_FILTER_ENABLE,
        .FilterBank = 0,
        .FilterFIFOAssignment = CAN_FILTER_FIFO0,
        .FilterIdHigh = CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYBALANCINGSET << 5,
        .FilterIdLow = CAN_PRIMARY_MESSAGE_FRAME_ID_ECUFSM << 5,
        .FilterMaskIdHigh = CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET << 5,
        .FilterMaskIdLow = 0x20U << 5,
        .FilterMode = CAN_FILTERMODE_IDLIST,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .SlaveStartFilterBank = 14
    };

    // Enable filters and start CAN
    HAL_CAN_ConfigFilter(&HCAN_PRIMARY, &filter);
    HAL_CAN_ActivateNotification(&HCAN_PRIMARY, CAN_IT_ERROR | CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_Start(&HCAN_PRIMARY);
}

void MX_CAN1_Init_250K(void) {
    HAL_CAN_DeInit(&HCAN_PRIMARY);

    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 10;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        Error_Handler();
    }

    prv_can_primary_start();
}

void MX_CAN1_Init_1M(void) {
    HAL_CAN_DeInit(&HCAN_PRIMARY);

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
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        Error_Handler();
    }

    prv_can_primary_start();
}

/*!
 * \brief Returns the native ST HAL CAN handler based on the network enum.
 * \param[in] network The target network track enum.
 * \return Pointer to the matched global CAN_HandleTypeDef, or \c NULL if invalid.
 */
EAGLETRT_STATIC_INLINE CAN_HandleTypeDef *prv_can_get_handler(enum CanCommunicationNetwork network) {
    switch (network) {
        case CAN_COMMUNICATION_NETWORK_BMS:
            return &HCAN_BMS;
        case CAN_COMMUNICATION_NETWORK_PRIMARY:
            return &HCAN_PRIMARY;
        default:
            return NULL;
    }
}

/*!
 * \brief Internal unified helper to write an abstract frame out to an ST HAL CAN peripheral.
 * \param[in] network The network track enum indicating which hardware peripheral to target.
 * \param[in] frame Pointer to the abstract frame structure containing the payload.
 *
 * \retval CAN_COMMUNICATION_RC_OK if the frame was sent successfully.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if a required pointer configuration is \c NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if the frame length exceeds CAN_COMMUNICATION_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR if the native HAL layer rejects the transmission.
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_can_send_to_hardware(enum CanCommunicationNetwork network, const struct CanCommunicationFrame *frame) {
    CAN_HandleTypeDef *hcan = prv_can_get_handler(network);

    if (hcan == NULL || frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (frame->length > CAN_COMMUNICATION_FRAME_DATA_SIZE) {
        return CAN_COMMUNICATION_RC_INVALID_LENGTH;
    }

    CAN_TxHeaderTypeDef tx_header = {
        .StdId = frame->id,
        .ExtId = 0U,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = frame->length,
        .TransmitGlobalTime = DISABLE
    };
    uint32_t tx_mailbox = 0U;
    if (HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)frame->data, &tx_mailbox) != HAL_OK) {
        return CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
    }
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_send_bms(const struct CanCommunicationFrame *frame) {
    return prv_can_send_to_hardware(CAN_COMMUNICATION_NETWORK_BMS, frame);
}

enum CanCommunicationReturnCode can_send_primary(const struct CanCommunicationFrame *frame) {
    return prv_can_send_to_hardware(CAN_COMMUNICATION_NETWORK_PRIMARY, frame);
}

void can_configure_and_start_primary(bool is_handcart_connected) {
    if (is_handcart_connected) {
        MX_CAN1_Init_250K();
    } else {
        MX_CAN1_Init_1M();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != HCAN_PRIMARY.Instance) {
        return;
    }

    CAN_RxHeaderTypeDef header = { 0 };
    struct CanCommunicationFrame frame = { 0 };
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, frame.data) == HAL_OK) {
        frame.id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
        frame.length = (uint8_t)header.DLC;

        // Based on the handler, retrieve the selected network
        constexpr enum CanCommunicationNetwork network = CAN_COMMUNICATION_NETWORK_PRIMARY;

        /* TODO: Handle return value of RX function */
        EAGLETRT_API_UNUSED(can_communication_api_add_to_rx(network, &frame));
    }
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != HCAN_BMS.Instance) {
        return;
    }

    CAN_RxHeaderTypeDef header = { 0 };
    struct CanCommunicationFrame frame = { 0 };
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &header, frame.data) == HAL_OK) {
        frame.id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
        frame.length = (uint8_t)header.DLC;

        // Based on the handler, retrieve the selected network
        constexpr enum CanCommunicationNetwork network = CAN_COMMUNICATION_NETWORK_BMS;

        /* TODO: Handle return value of RX function */
        EAGLETRT_API_UNUSED(can_communication_api_add_to_rx(network, &frame));
    }
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

/* USER CODE END 1 */
