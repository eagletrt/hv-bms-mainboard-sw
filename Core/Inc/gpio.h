/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "led.h"
#include "display.h"
#include "pcu.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Set the state of a LED
 *
 * @param led The LED to select
 * @param state The new state of the LED to set
 */
void gpio_led_set_state(LedId led, LedStatus state);

/**
 * @brief Toggle the state of a LED
 *
 * @param led The LED to select
 */
void gpio_led_toggle_state(LedId led);

/**
 * @brief Set the state of a segment of the 7-segment display
 *
 * @param segment The segment to select
 * @param state The new state of the segment to set
 */
void gpio_display_segment_set_state(DisplaySegment segment, DisplaySegmentStatus state);

/**
 * @brief Toggle the state of a segment of the 7-segment display
 *
 * @param segment The segment to select
 */
void gpio_display_segment_toggle_state(DisplaySegment segment);

/**
 * @brief Set the state of a pin controlled by the PCU
 *
 * @param pin The pin to select
 * @parma state The new state of the pin to set
 */
void gpio_pcu_set_state(PcuPin pin, PcuPinStatus state);

/**
 * @brief Toggle the state of a pin controlled by the PCU
 *
 * @param pin The pin to select
 */
void gpio_pcu_toggle_state(PcuPin pin);

/**
 * @brief Read all the digital feedbacks in one go
 *
 * @return bit_flag32_t The state of the feedbacks as a bit flag
 */
bit_flag32_t gpio_feedback_read_all(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

