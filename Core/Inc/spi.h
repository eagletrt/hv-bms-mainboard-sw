/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdint.h>

#include "mainboard-def.h"

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi2;

extern SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN Private defines */

/** @brief Macros used as alias for the SPI peripherals handlers */
#define HSPI_EEPROM hspi2
#define HSPI_ADC hspi3


/* USER CODE END Private defines */

void MX_SPI2_Init(void);
void MX_SPI3_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Send data via an SPI network
 *
 * @param network The SPI network to select
 * @param data A pointer to the data to send
 * @param size The length of data in bytes
 */
void spi_send(
    const SpiNetwork network,
    uint8_t * const data,
    const size_t size
);

/**
 * @brief Send and receive data via an SPI network
 *
 * @param network The SPI network to select
 * @param data A pointer to the data to send
 * @param out[out] A pointer to the array where the received data is stored
 * @param size The length of data in bytes
 * @param out_size The number of bytes that should be received
 */
void spi_send_receive(
    const SpiNetwork network,
    uint8_t * const data,
    uint8_t * const out,
    const size_t size,
    const size_t out_size
);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

