/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdbool.h>
#include <stddef.h>

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

// Size of the TX ring buffer, at most (USART_TX_BUFFER_SIZE - 1) bytes can be queued
#define USART_TX_BUFFER_SIZE (1024U)
// Size of the circular RX DMA buffer
#define USART_RX_BUFFER_SIZE (256U)

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief Send data via the UART with DMA (non blocking)
 *
 * \details The data is copied in a TX ring buffer, so the caller can reuse its
 *          buffer as soon as the function returns; the queued data is sent in
 *          the background by the DMA, one transfer after the other
 *
 * \note The signature matches logger_write_callback so it can be given directly
 *       to the logger; it can be called from interrupts too
 *
 * \param data A pointer to the data to send
 * \param size The length of data in bytes
 *
 * \returns The number of queued bytes (equal to size), or -1 if data is NULL or
 *          there is not enough free space in the buffer (nothing is queued)
 */
int usart_send(const char *data, size_t size);

/*!
 * \brief Check if new data was received via the UART
 *
 * \details The flag is set from the interrupt when the DMA receives data (on
 *          idle line, half and full buffer) and it is cleared by usart_receive
 *          once all the received data has been read
 *
 * \returns true if there is received data that has not been read yet
 */
bool usart_is_data_received(void);

/*!
 * \brief Read the data received via the UART
 *
 * \details The reception always runs in the background with a circular DMA,
 *          data that is not read in time is overwritten
 *          (more than USART_RX_BUFFER_SIZE bytes between two calls)
 *
 * \param out[out] A pointer to the array where the received data is copied
 * \param size The maximum number of bytes to copy
 *
 * \returns The number of bytes copied in out (0 if nothing was received)
 */
size_t usart_receive(char *out, size_t size);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
