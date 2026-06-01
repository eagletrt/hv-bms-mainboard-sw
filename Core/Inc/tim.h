/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim6;

extern TIM_HandleTypeDef htim7;

/* USER CODE BEGIN Private defines */

/** @brief Aliases for the timer handler */
#define HTIM_IMD htim4
#define HTIM_TIMEBASE htim6
#define HTIM_ERROR htim7

#define _M_GET_TIM_APB_PLACEMENT(__HANDLE__) (((__HANDLE__)->Instance < (TIM_TypeDef *)APB2PERIPH_BASE) ? 1U : 0U)

#define TIM_GET_FREQ(TIM) (uint32_t)(TIM_GetInternalClkFreq((TIM)) / ((TIM)->Instance->PSC + 1))

#define TIM_MS_TO_TICKS(TIM, MS) (uint32_t)(((float)TIM_GET_FREQ((TIM)) * (MS)) / 1000)

#define TIM_TICKS_TO_MS(TIM, TICKS) (((float)(TICKS) * 1000) / TIM_GET_FREQ((TIM)))

#define TIM_GET_MAX_AUTORELOAD(TIM) (IS_TIM_32B_COUNTER_INSTANCE((TIM)->Instance) ? UINT32_MAX : UINT16_MAX)

/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM4_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */

uint32_t TIM_GetInternalClkFreq(TIM_HandleTypeDef *htim);

/**
 * @brief Starts the timer with PWM for the IMD
 */
void tim_start_pwm_imd(void);

/**
 * @brief Function used to update the error timer
 *
 * @param timestamp The time in which the error was set
 * @param timeout The time after which the error should expire after it was set
 */
void tim_update_error_timer(const uint32_t timestamp, const uint16_t timeout);

/**
 * @brief Function used to stop the error timer
 */
void tim_stop_error_timer(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
