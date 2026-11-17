/* USER CODE BEGIN Header */
/*!
  ******************************************************************************
  * \file    adc.h
  * \brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * \attention
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdbool.h>
#include <stddef.h>

#include "mainboard-def.h"
#include "sensing.h"

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc3;

/* USER CODE BEGIN Private defines */

/*! \brief Redefinition of the ADC handlers */
#define HADC_1 hadc1
#define HADC_2 hadc3

/*
 * The sample layout of every ADC (rank order) is defined by SensingAdc1Channel
 * and SensingAdc2Channel in sensing.h and must match the ranks configured in
 * MX_ADC1_Init and MX_ADC3_Init (.ioc)
 */

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC3_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief Start a single scan conversion of an MCU ADC with DMA
 *
 * \details Sensing callback (sensing_adc_start_callback), the end of the scan
 *          is notified to the sensing module from HAL_ADC_ConvCpltCallback
 *
 * \attention The DMA streams are circular: the buffer and the count given at
 *            the first call are kept by the DMA, so they must be the same at
 *            every call (the sensing module always passes its own buffers)
 *
 * \param adc The ADC to start
 * \param buffer The buffer where the DMA stores the samples
 * \param count The number of samples of the scan
 *
 * \returns true if the conversion was started, false otherwise
 */
bool adc_sensing_start(enum SensingAdc adc, volatile raw_volt *buffer, size_t count);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
