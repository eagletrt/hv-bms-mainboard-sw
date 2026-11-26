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

#include <stddef.h>

#include "can-communication.h"
#include "can-communication-api.h"
#include "can-communication-router-api.h"
#include "main.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_gpio.h"

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
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
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
    hcan2.Init.AutoBusOff = ENABLE;
    hcan2.Init.AutoWakeUp = DISABLE;
    hcan2.Init.AutoRetransmission = ENABLE;
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
    // The mainboard processes every frame of the BMS network: a zero mask makes every ID match
    CAN_FilterTypeDef filter = {
        .FilterActivation = CAN_FILTER_ENABLE,
        .FilterBank = 14,
        .FilterFIFOAssignment = CAN_FILTER_FIFO1,
        .FilterIdHigh = 0,
        .FilterIdLow = 0,
        .FilterMaskIdHigh = 0,
        .FilterMaskIdLow = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .SlaveStartFilterBank = 14
    };

    // Enable filters and start CAN
    HAL_CAN_ConfigFilter(&HCAN_BMS, &filter);
    HAL_CAN_ActivateNotification(&HCAN_BMS, CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_RX_FIFO1_MSG_PENDING);
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
        HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
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
        HAL_NVIC_SetPriority(CAN2_SCE_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
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
        HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
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
        HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
        /* USER CODE BEGIN CAN2_MspDeInit 1 */

        /* USER CODE END CAN2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/*! \brief First filter bank of CAN2, the banks before belong to CAN1 (primary network) */
#define CAN_FILTER_CAN2_START_BANK (14U)

/*! \brief ID and mask pairs of a filter bank in 16-bit mask mode */
#define CAN_FILTER_ENTRIES_PER_BANK (2U)

/*! \brief ID and mask pairs that fit in the filter banks of the primary network */
#define CAN_FILTER_PRIMARY_MAX_ENTRIES (CAN_FILTER_CAN2_START_BANK * CAN_FILTER_ENTRIES_PER_BANK)

static_assert(CAN_FILTER_PRIMARY_MAX_ENTRIES <= UINT8_MAX, "The filter entries must fit the transceiver status");

/*!
 * \brief Layout of a 16-bit filter register: STDID[10:0] RTR IDE EXTID[17:15]
 * \{
 */
#define CAN_FILTER16_STDID_SHIFT (5U)
#define CAN_FILTER16_RTR (1U << 4U)
#define CAN_FILTER16_IDE (1U << 3U)
#define CAN_FILTER16_STDID_MASK (0x7FFU)
/*! \} */

/*! \brief Filter entries in use on the primary network, 0 if it receives every frame */
EAGLETRT_STATIC uint8_t can_filter_primary_entries;

/*!
 * \brief Turn the ID ranges handled on the primary network into filter ID and mask pairs
 *
 * \details Each range is split into blocks of a power of two IDs aligned to their size,
 *          which a mask matches exactly: no other ID is let through. Standard data frames only
 *
 * \param[out] ids The IDs, in the 16-bit register layout
 * \param[out] masks The masks, in the 16-bit register layout
 * \param[in] capacity The size of ids and masks
 *
 * \return The number of pairs, 0 if they do not fit or a range is invalid
 */
EAGLETRT_STATIC size_t prv_can_primary_filter_entries(uint16_t *const ids, uint16_t *const masks, const size_t capacity) {
    size_t range_count = 0U;
    const struct CanCommunicationIdRange *const ranges = can_communication_router_api_get_primary_rx_ranges(&range_count);
    size_t count = 0U;

    for (size_t i = 0U; i < range_count; ++i) {
        const uint32_t last = ranges[i].last;
        if (ranges[i].first > last || last > CAN_FILTER16_STDID_MASK) {
            return 0U;
        }
        uint32_t id = ranges[i].first;
        while (id <= last) {
            uint32_t size = 1U;
            while ((id % (size * 2U)) == 0U && id + (size * 2U) - 1U <= last) {
                size *= 2U;
            }
            if (count == capacity) {
                return 0U;
            }
            ids[count] = (uint16_t)(id << CAN_FILTER16_STDID_SHIFT);
            // RTR and IDE must be 0: standard data frames only
            masks[count] = (uint16_t)(((~(size - 1U) & CAN_FILTER16_STDID_MASK) << CAN_FILTER16_STDID_SHIFT) | CAN_FILTER16_RTR | CAN_FILTER16_IDE);
            ++count;
            id += size;
        }
    }
    return count;
}

void prv_can_primary_start() {
    uint16_t ids[CAN_FILTER_PRIMARY_MAX_ENTRIES] = { 0 };
    uint16_t masks[CAN_FILTER_PRIMARY_MAX_ENTRIES] = { 0 };
    size_t count = prv_can_primary_filter_entries(ids, masks, CAN_FILTER_PRIMARY_MAX_ENTRIES);
    can_filter_primary_entries = (uint8_t)count;
    if (count == 0U) {
        // A zero mask makes every ID match, the router ignores what it does not handle
        count = 1U;
    }

    // Every bank of CAN1 is written, so the banks left over are turned off
    for (uint32_t bank = 0U; bank < CAN_FILTER_CAN2_START_BANK; ++bank) {
        const size_t first = bank * CAN_FILTER_ENTRIES_PER_BANK;
        // A bank holds two pairs, the last one is repeated when the count is odd
        const size_t second = (first + 1U < count) ? first + 1U : first;
        CAN_FilterTypeDef filter = {
            .FilterActivation = (first < count) ? CAN_FILTER_ENABLE : CAN_FILTER_DISABLE,
            .FilterBank = bank,
            .FilterFIFOAssignment = CAN_FILTER_FIFO0,
            .FilterIdLow = (first < count) ? ids[first] : 0U,
            .FilterMaskIdLow = (first < count) ? masks[first] : 0U,
            .FilterIdHigh = (first < count) ? ids[second] : 0U,
            .FilterMaskIdHigh = (first < count) ? masks[second] : 0U,
            .FilterMode = CAN_FILTERMODE_IDMASK,
            .FilterScale = CAN_FILTERSCALE_16BIT,
            .SlaveStartFilterBank = CAN_FILTER_CAN2_START_BANK,
        };
        EAGLETRT_API_UNUSED(HAL_CAN_ConfigFilter(&HCAN_PRIMARY, &filter));
    }

    // Start CAN
    HAL_CAN_ActivateNotification(&HCAN_PRIMARY, CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_RX_FIFO0_MSG_PENDING);
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
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
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
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        Error_Handler();
    }

    prv_can_primary_start();
}

/*! \brief Hardware TX mailboxes of the bxCAN peripheral */
#define CAN_TX_MAILBOX_COUNT (3U)

/*! \brief Frames that fit in a hardware RX FIFO of the bxCAN peripheral */
#define CAN_RX_FIFO_SIZE (3U)

/*!
 * \brief Lifetime counters of a transceiver, see \ref CanCommunicationTransceiverStatus
 *
 * \details The RX counters are written by the interrupts, the others by the main loop only
 */
struct CanHardwareCounters {
    uint32_t tx_requested;
    uint32_t tx_completed;
    uint32_t tx_arbitration_lost;
    uint32_t tx_errors;
    uint32_t tx_mailboxes_busy;
    uint32_t bus_errors;
    enum CanCommunicationBusError last_error;
    volatile uint32_t rx_received;
    volatile uint32_t rx_fifo_overruns;
    volatile uint32_t rx_queue_full;
    volatile uint32_t bus_off_count;
};

EAGLETRT_STATIC struct CanHardwareCounters can_counters[CAN_NETWORK_COUNT];

/*!
 * \brief Increment a counter, saturating at its maximum
 *
 * \param[in,out] counter The counter
 */
EAGLETRT_STATIC_INLINE void prv_can_count(volatile uint32_t *const counter) {
    if (*counter < UINT32_MAX) {
        *counter = *counter + 1U;
    }
}

/*!
 * \brief Returns the native ST HAL CAN handler based on the network enum.
 * \param[in] network The target network track enum.
 * \return Pointer to the matched global CAN_HandleTypeDef, or \c NULL if invalid.
 */
EAGLETRT_STATIC_INLINE CAN_HandleTypeDef *prv_can_get_handler(enum CanNetwork network) {
    switch (network) {
        case CAN_NETWORK_BMS:
            return &HCAN_BMS;
        case CAN_NETWORK_PRIMARY:
            return &HCAN_PRIMARY;
        default:
            return NULL;
    }
}

/*!
 * \brief Account the outcome of the TX mailboxes that completed a request and release them
 *
 * \details With the automatic retransmission a request completes once the frame is sent,
 *          or when it is aborted: TXOK, ALST and TERR then tell the last attempt.
 *          The outcome is lost if the mailbox is reused before this is called,
 *          so it runs before every transmission and every status read
 *
 * \param[in] hcan The peripheral
 * \param[in,out] counters The counters of its network
 */
EAGLETRT_STATIC void prv_can_account_tx_mailboxes(CAN_HandleTypeDef *const hcan, struct CanHardwareCounters *const counters) {
    static const uint32_t request_completed[CAN_TX_MAILBOX_COUNT] = { CAN_TSR_RQCP0, CAN_TSR_RQCP1, CAN_TSR_RQCP2 };
    static const uint32_t transmit_ok[CAN_TX_MAILBOX_COUNT] = { CAN_TSR_TXOK0, CAN_TSR_TXOK1, CAN_TSR_TXOK2 };
    static const uint32_t arbitration_lost[CAN_TX_MAILBOX_COUNT] = { CAN_TSR_ALST0, CAN_TSR_ALST1, CAN_TSR_ALST2 };
    static const uint32_t transmit_error[CAN_TX_MAILBOX_COUNT] = { CAN_TSR_TERR0, CAN_TSR_TERR1, CAN_TSR_TERR2 };

    const uint32_t tsr = hcan->Instance->TSR;
    for (size_t mailbox = 0U; mailbox < CAN_TX_MAILBOX_COUNT; ++mailbox) {
        if ((tsr & request_completed[mailbox]) == 0U) {
            continue;
        }
        if ((tsr & transmit_ok[mailbox]) != 0U) {
            prv_can_count(&counters->tx_completed);
        } else if ((tsr & arbitration_lost[mailbox]) != 0U) {
            prv_can_count(&counters->tx_arbitration_lost);
        } else if ((tsr & transmit_error[mailbox]) != 0U) {
            prv_can_count(&counters->tx_errors);
        }
        // Writing 1 to RQCP clears it with TXOK, ALST and TERR, the zeros have no effect
        hcan->Instance->TSR = request_completed[mailbox];
    }
}

/*!
 * \brief Account the last error code of the peripheral and reset it
 *
 * \details The code is reset to 7, a value the hardware never sets, so a new error is
 *          seen as a code from 1 to 6. A frame sent or received without errors
 *          clears the code, so errors between two reads can be missed: the count is
 *          a lower bound. The error interrupt is not used, a broken bus would flood it
 *
 * \param[in] hcan The peripheral
 * \param[in,out] counters The counters of its network
 */
EAGLETRT_STATIC void prv_can_account_bus_error(CAN_HandleTypeDef *const hcan, struct CanHardwareCounters *const counters) {
    // The LEC field of ESR, 0 is no error and 7 is set only by software
    static const enum CanCommunicationBusError errors[] = {
        [1U] = CAN_COMMUNICATION_BUS_ERROR_STUFF,
        [2U] = CAN_COMMUNICATION_BUS_ERROR_FORM,
        [3U] = CAN_COMMUNICATION_BUS_ERROR_ACK,
        [4U] = CAN_COMMUNICATION_BUS_ERROR_BIT_RECESSIVE,
        [5U] = CAN_COMMUNICATION_BUS_ERROR_BIT_DOMINANT,
        [6U] = CAN_COMMUNICATION_BUS_ERROR_CRC,
    };
    constexpr uint32_t first_error = 1U;
    constexpr uint32_t last_error = 6U;

    const uint32_t code = (hcan->Instance->ESR & CAN_ESR_LEC) >> CAN_ESR_LEC_Pos;
    if (code >= first_error && code <= last_error) {
        prv_can_count(&counters->bus_errors);
        counters->last_error = errors[code];
        // Only LEC is writable in ESR
        hcan->Instance->ESR = CAN_ESR_LEC;
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
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_can_send_to_hardware(enum CanNetwork network, const struct CanCommunicationFrame *frame) {
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
    struct CanHardwareCounters *const counters = &can_counters[network];

    // Before a mailbox is reused, or the outcome of its last request is lost
    prv_can_account_tx_mailboxes(hcan, counters);

    // No busy wait: with the automatic retransmission the mailboxes stay busy for as long as
    // nobody acknowledges the frames, waiting would stall the main loop at every frame
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0U) {
        prv_can_count(&counters->tx_mailboxes_busy);
        return CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
    }

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)frame->data, &tx_mailbox) != HAL_OK) {
        return CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
    }
    prv_can_count(&counters->tx_requested);
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_send_bms(const struct CanCommunicationFrame *frame) {
    return prv_can_send_to_hardware(CAN_NETWORK_BMS, frame);
}

enum CanCommunicationReturnCode can_send_primary(const struct CanCommunicationFrame *frame) {
    return prv_can_send_to_hardware(CAN_NETWORK_PRIMARY, frame);
}

bool can_tx_ready_primary(void) {
    return HAL_CAN_GetTxMailboxesFreeLevel(&HCAN_PRIMARY) > 0U;
}

bool can_tx_ready_bms(void) {
    return HAL_CAN_GetTxMailboxesFreeLevel(&HCAN_BMS) > 0U;
}

/*!
 * \brief Get the hardware RX FIFO that receives the frames of a network
 *
 * \param[in] network The network
 *
 * \return CAN_RX_FIFO0 or CAN_RX_FIFO1
 */
EAGLETRT_STATIC_INLINE uint32_t prv_can_get_rx_fifo(enum CanNetwork network) {
    return (network == CAN_NETWORK_BMS) ? CAN_RX_FIFO1 : CAN_RX_FIFO0;
}

/*!
 * \brief Fill the transceiver status of a network from the peripheral registers
 *
 * \param[in] network The network
 * \param[out] status The status to fill
 *
 * \retval CAN_COMMUNICATION_RC_OK on success
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p status is \c NULL or the network is invalid
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_can_get_status(enum CanNetwork network, struct CanCommunicationTransceiverStatus *const status) {
    CAN_HandleTypeDef *hcan = prv_can_get_handler(network);
    if (hcan == NULL || status == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    struct CanHardwareCounters *const counters = &can_counters[network];

    prv_can_account_tx_mailboxes(hcan, counters);
    prv_can_account_bus_error(hcan, counters);

    // Every field of BTR holds its value minus one, a bit is the sync segment plus the two time segments
    const uint32_t btr = hcan->Instance->BTR;
    const uint32_t prescaler = ((btr & CAN_BTR_BRP) >> CAN_BTR_BRP_Pos) + 1U;
    const uint32_t quanta = 1U + (((btr & CAN_BTR_TS1) >> CAN_BTR_TS1_Pos) + 1U) + (((btr & CAN_BTR_TS2) >> CAN_BTR_TS2_Pos) + 1U);
    // Both CAN peripherals are clocked by APB1
    status->bitrate = HAL_RCC_GetPCLK1Freq() / (prescaler * quanta);

    const uint32_t esr = hcan->Instance->ESR;
    if (HAL_CAN_GetState(hcan) != HAL_CAN_STATE_LISTENING) {
        status->state = CAN_COMMUNICATION_BUS_STATE_STOPPED;
    } else if ((esr & CAN_ESR_BOFF) != 0U) {
        status->state = CAN_COMMUNICATION_BUS_STATE_BUS_OFF;
    } else if ((esr & CAN_ESR_EPVF) != 0U) {
        status->state = CAN_COMMUNICATION_BUS_STATE_ERROR_PASSIVE;
    } else if ((esr & CAN_ESR_EWGF) != 0U) {
        status->state = CAN_COMMUNICATION_BUS_STATE_ERROR_WARNING;
    } else {
        status->state = CAN_COMMUNICATION_BUS_STATE_ERROR_ACTIVE;
    }
    status->tx_error_counter = (uint8_t)((esr & CAN_ESR_TEC) >> CAN_ESR_TEC_Pos);
    status->rx_error_counter = (uint8_t)((esr & CAN_ESR_REC) >> CAN_ESR_REC_Pos);
    status->last_error = counters->last_error;

    status->tx_mailboxes_free = (uint8_t)HAL_CAN_GetTxMailboxesFreeLevel(hcan);
    status->tx_mailboxes_count = CAN_TX_MAILBOX_COUNT;
    status->rx_fifo_pending = (uint8_t)HAL_CAN_GetRxFifoFillLevel(hcan, prv_can_get_rx_fifo(network));
    status->rx_fifo_size = CAN_RX_FIFO_SIZE;

    status->tx_requested = counters->tx_requested;
    status->tx_completed = counters->tx_completed;
    status->tx_arbitration_lost = counters->tx_arbitration_lost;
    status->tx_errors = counters->tx_errors;
    status->tx_mailboxes_busy = counters->tx_mailboxes_busy;
    status->rx_received = counters->rx_received;
    status->rx_fifo_overruns = counters->rx_fifo_overruns;
    status->rx_queue_full = counters->rx_queue_full;
    status->bus_errors = counters->bus_errors;
    status->bus_off_count = counters->bus_off_count;
    status->rx_filter_entries = (network == CAN_NETWORK_PRIMARY) ? can_filter_primary_entries : 0U;
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_get_status_primary(struct CanCommunicationTransceiverStatus *status) {
    return prv_can_get_status(CAN_NETWORK_PRIMARY, status);
}

enum CanCommunicationReturnCode can_get_status_bms(struct CanCommunicationTransceiverStatus *status) {
    return prv_can_get_status(CAN_NETWORK_BMS, status);
}

/*!
 * \brief Move a frame from a hardware RX FIFO to the RX queue of its network, interrupt context
 *
 * \param[in] hcan The peripheral
 * \param[in] network The network of the peripheral
 */
EAGLETRT_STATIC void prv_can_receive(CAN_HandleTypeDef *const hcan, enum CanNetwork network) {
    struct CanHardwareCounters *const counters = &can_counters[network];
    const uint32_t fifo = prv_can_get_rx_fifo(network);
    volatile uint32_t *const rfr = (fifo == CAN_RX_FIFO0) ? &hcan->Instance->RF0R : &hcan->Instance->RF1R;
    const uint32_t overrun = (fifo == CAN_RX_FIFO0) ? CAN_RF0R_FOVR0 : CAN_RF1R_FOVR1;

    // Before HAL_CAN_GetRxMessage, which clears the flag while releasing the FIFO output
    if ((*rfr & overrun) != 0U) {
        prv_can_count(&counters->rx_fifo_overruns);
        // Writing 1 clears the flag, the zeros have no effect
        *rfr = overrun;
    }

    CAN_RxHeaderTypeDef header = { 0 };
    struct CanCommunicationFrame frame = { 0 };
    if (HAL_CAN_GetRxMessage(hcan, fifo, &header, frame.data) == HAL_OK) {
        frame.id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
        frame.length = (uint8_t)header.DLC;
        prv_can_count(&counters->rx_received);

        // Interrupt context: counted here, logged by the CAN monitor
        if (can_communication_api_add_to_rx(network, &frame) == CAN_COMMUNICATION_RC_QUEUE_FULL) {
            prv_can_count(&counters->rx_queue_full);
        }
    }
}

void can_configure_and_start_primary(bool is_handcart_connected) {
    if (is_handcart_connected) {
        MX_CAN1_Init_250K();
    } else {
        MX_CAN1_Init_1M();
    }
}

/*!
 * \brief Count the bus-off events, interrupt context
 *
 * \details Only the bus-off interrupt is enabled among the error ones: the peripheral
 *          recovers by itself after 128 sequences of 11 recessive bits, so the bus-off
 *          can end before the CAN monitor polls the state and would be missed
 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    const enum CanNetwork network = (hcan->Instance == HCAN_PRIMARY.Instance) ? CAN_NETWORK_PRIMARY : CAN_NETWORK_BMS;
    if ((HAL_CAN_GetError(hcan) & HAL_CAN_ERROR_BOF) != 0U) {
        prv_can_count(&can_counters[network].bus_off_count);
    }
    // The HAL accumulates the error codes, reset them to see the next bus-off
    EAGLETRT_API_UNUSED(HAL_CAN_ResetError(hcan));
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != HCAN_PRIMARY.Instance) {
        return;
    }
    prv_can_receive(hcan, CAN_NETWORK_PRIMARY);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != HCAN_BMS.Instance) {
        return;
    }
    prv_can_receive(hcan, CAN_NETWORK_BMS);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

/* USER CODE END 1 */
