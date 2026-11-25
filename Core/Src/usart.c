/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

#include <string.h>

#include "eagletrt-api.h"

static bool prv_usart_rx_start(void);

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void) {

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    // The reception is always running, the received data is read with usart_receive
    if (!prv_usart_rx_start()) {
        Error_Handler();
    }

    /* USER CODE END USART1_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
        GPIO_InitStruct.Pin = TX_SWD_Pin | RX_SWD_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA2_Stream2;
        hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart1_rx);

        /* USART1_TX Init */
        hdma_usart1_tx.Instance = DMA2_Stream7;
        hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart1_tx);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle) {

    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
        HAL_GPIO_DeInit(GPIOA, TX_SWD_Pin | RX_SWD_Pin);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

// The size of a single DMA transfer is a 16 bit value
static_assert(USART_TX_BUFFER_SIZE <= UINT16_MAX, "USART TX buffer too big for a DMA transfer");
static_assert(USART_RX_BUFFER_SIZE <= UINT16_MAX, "USART RX buffer too big for a DMA transfer");

/*
 * TX ring buffer, the data is copied here by usart_send and read by the DMA
 * One slot is always left empty to tell a full buffer from an empty one
 */
static char prv_usart_tx_buffer[USART_TX_BUFFER_SIZE];
static volatile size_t prv_usart_tx_head;    // Next free slot
static volatile size_t prv_usart_tx_tail;    // First byte not sent yet
static volatile size_t prv_usart_tx_pending; // Bytes of the ongoing DMA transfer, 0 if the TX is idle

/*
 * RX buffer, written by the DMA in circular mode
 * Unread data is overwritten if more than USART_RX_BUFFER_SIZE bytes are
 * received between two calls of usart_receive
 */
static char prv_usart_rx_buffer[USART_RX_BUFFER_SIZE];
static volatile size_t prv_usart_rx_head; // DMA write position
static volatile size_t prv_usart_rx_tail; // First byte not read yet
static volatile bool prv_usart_rx_flag;   // Set when new data is received, cleared when everything is read

/*!
 * \brief Disable the interrupts saving the previous state (can be nested)
 *
 * \returns The previous value of PRIMASK, to give to prv_usart_cs_exit
 */
static uint32_t prv_usart_cs_enter(void) {
    const uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

/*!
 * \brief Restore the interrupts state saved by prv_usart_cs_enter
 *
 * \param primask The value returned by prv_usart_cs_enter
 */
static void prv_usart_cs_exit(const uint32_t primask) {
    __set_PRIMASK(primask);
}

/*!
 * \brief Start a DMA transfer of the queued data if the TX is idle
 *
 * \details Only the contiguous part of the ring buffer is sent, what is left
 *          after the wrap is sent from the transfer complete callback
 *
 * \attention Call with the interrupts disabled or from the UART interrupt
 */
static void prv_usart_tx_start(void) {
    const size_t head = prv_usart_tx_head;
    const size_t tail = prv_usart_tx_tail;
    if (prv_usart_tx_pending != 0U || head == tail) {
        return;
    }

    const size_t size = (head > tail) ? (head - tail) : (USART_TX_BUFFER_SIZE - tail);
    if (HAL_UART_Transmit_DMA(&huart1, (const uint8_t *)&prv_usart_tx_buffer[tail], (uint16_t)size) == HAL_OK) {
        prv_usart_tx_pending = size;
    }
    // On failure the data stays queued and the transfer is retried at the next usart_send
}

/*!
 * \brief Mark the ongoing DMA transfer as done and start the next one
 *
 * \attention Call from the UART interrupt only
 */
static void prv_usart_tx_next(void) {
    prv_usart_tx_tail = (prv_usart_tx_tail + prv_usart_tx_pending) % USART_TX_BUFFER_SIZE;
    prv_usart_tx_pending = 0U;
    prv_usart_tx_start();
}

/*!
 * \brief (Re)start the circular DMA reception, unread data is discarded
 *
 * \returns true if the reception was started, false otherwise
 */
static bool prv_usart_rx_start(void) {
    prv_usart_rx_head = 0U;
    prv_usart_rx_tail = 0U;
    prv_usart_rx_flag = false;

    // The RX event callback is called on half transfer, transfer complete and idle line
    return HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)prv_usart_rx_buffer, USART_RX_BUFFER_SIZE) == HAL_OK;
}

int usart_send(const char *const data, const size_t size) {
    if (data == NULL || size >= USART_TX_BUFFER_SIZE) {
        return -1;
    }
    if (size == 0U) {
        return 0;
    }

    // Can be called from both the main loop and the interrupts
    const uint32_t primask = prv_usart_cs_enter();

    const size_t head = prv_usart_tx_head;
    const size_t tail = prv_usart_tx_tail;
    const size_t free_space = (tail + USART_TX_BUFFER_SIZE - head - 1U) % USART_TX_BUFFER_SIZE;
    if (size > free_space) {
        prv_usart_cs_exit(primask);
        return -1;
    }

    // Copy the data in two parts if it wraps around the end of the buffer
    const size_t first = (size < USART_TX_BUFFER_SIZE - head) ? size : (USART_TX_BUFFER_SIZE - head);
    memcpy(&prv_usart_tx_buffer[head], data, first);
    memcpy(prv_usart_tx_buffer, &data[first], size - first);
    prv_usart_tx_head = (head + size) % USART_TX_BUFFER_SIZE;

    prv_usart_tx_start();

    prv_usart_cs_exit(primask);
    return (int)size;
}

bool usart_is_data_received(void) {
    return prv_usart_rx_flag;
}

size_t usart_receive(char *const out, const size_t size) {
    if (out == NULL || size == 0U) {
        return 0U;
    }

    // The indices are also changed by the RX event and error callbacks
    const uint32_t primask = prv_usart_cs_enter();

    const size_t head = prv_usart_rx_head;
    const size_t tail = prv_usart_rx_tail;
    const size_t available = (head + USART_RX_BUFFER_SIZE - tail) % USART_RX_BUFFER_SIZE;
    const size_t count = (size < available) ? size : available;

    // Copy the data in two parts if it wraps around the end of the buffer
    const size_t first = (count < USART_RX_BUFFER_SIZE - tail) ? count : (USART_RX_BUFFER_SIZE - tail);
    memcpy(out, &prv_usart_rx_buffer[tail], first);
    memcpy(&out[first], prv_usart_rx_buffer, count - first);
    prv_usart_rx_tail = (tail + count) % USART_RX_BUFFER_SIZE;

    if (prv_usart_rx_tail == head) {
        prv_usart_rx_flag = false;
    }

    prv_usart_cs_exit(primask);
    return count;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart1.Instance) {
        prv_usart_tx_next();
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance != huart1.Instance) {
        return;
    }

    // Size is the DMA write position, equal to the buffer size right before the wrap
    prv_usart_rx_head = Size % USART_RX_BUFFER_SIZE;
    if (prv_usart_rx_head != prv_usart_rx_tail) {
        prv_usart_rx_flag = true;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance != huart1.Instance) {
        return;
    }

    // DMA TX error: the transfer was stopped, drop the chunk that was being sent and go on
    if (huart->gState == HAL_UART_STATE_READY && prv_usart_tx_pending != 0U) {
        prv_usart_tx_next();
    }

    // Any error during a DMA reception (overrun, noise, framing, ...) aborts it, restart it
    if (huart->RxState == HAL_UART_STATE_READY) {
        EAGLETRT_API_UNUSED(prv_usart_rx_start());
    }
}

/* USER CODE END 1 */
