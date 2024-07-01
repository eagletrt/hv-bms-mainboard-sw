/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, HANDCART_CONNECTED_Pin|SPARE_2_Pin|SPARE_3_Pin|SPARE_4_Pin
                          |SPARE_5_Pin|SEG7_S1_Pin|SEG7_S2_Pin|SEG7_S3_Pin
                          |SEG7_S4_Pin|SEG7_S5_Pin|SEG7_S6_Pin|SEG7_S7_Pin
                          |SEG7_DP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SPARE_6_Pin|SPARE_7_Pin|SPARE_8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, SPARE_9_Pin|SPARE_10_Pin|SPARE_11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SPI_EEPROM_CS_Pin|HOLD_EEPROM_Pin|LED_2_Pin|LED_1_Pin
                          |SPI_ADC_INT_Pin|SPARE_13_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, BMS_OK_Pin|PRECHARGE_Pin|AIRN_OFF_Pin|AIRP_OFF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_ADC_CS_GPIO_Port, SPI_ADC_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPARE_12_Pin|SPARE_14_Pin|SPARE_15_Pin|SPARE_16_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin */
  GPIO_InitStruct.Pin = HANDCART_CONNECTED_Pin|SPARE_2_Pin|SPARE_3_Pin|SPARE_4_Pin
                          |SPARE_5_Pin|SEG7_S1_Pin|SEG7_S2_Pin|SEG7_S3_Pin
                          |SEG7_S4_Pin|SEG7_S5_Pin|SEG7_S6_Pin|SEG7_S7_Pin
                          |SEG7_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = SPARE_6_Pin|SPARE_7_Pin|SPARE_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = SPARE_9_Pin|SPARE_10_Pin|SPARE_11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin PDPin */
  GPIO_InitStruct.Pin = SPI_EEPROM_CS_Pin|HOLD_EEPROM_Pin|LED_2_Pin|LED_1_Pin
                          |SPI_ADC_INT_Pin|SPARE_13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = BMS_OK_Pin|PRECHARGE_Pin|AIRN_OFF_Pin|AIRP_OFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI_ADC_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_ADC_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin */
  GPIO_InitStruct.Pin = EXT_FAULT_LATCHED_Pin|IMD_FAULT_LATCHED_Pin|BMS_FAULT_LATCHED_Pin|IMPLAUSIBLE_STATE_LATCHED_Pin
                          |LATCH_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin
                           PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = INDICATOR_CONNECTED_Pin|IMD_FAULT_COCKPID_LED_Pin|BMS_FAULT_COCKPID_LED_Pin|PLAUSIBLE_STATE_Pin
                          |PLAUSIBLE_STATE_PERSISTED_Pin|TS_LESS_THAN_60V_Pin|PRECHARGE_OPEN_MEC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = PRECHARGE_OPEN_COM_Pin|SD_BMS_FB_Pin|SD_IMD_FB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SPARE_12_Pin|SPARE_14_Pin|SPARE_15_Pin|SPARE_16_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = AIRP_OPEN_COM_Pin|AIRN_OPEN_COM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

GPIO_TypeDef * _gpio_get_port_from_led_id(LedId led) {
    switch (led) {
        case LED_1:
            return LED_1_GPIO_Port;
        case LED_2:
            return LED_2_GPIO_Port;
        default:
            return NULL;
    }
}

int16_t _gpio_get_pin_from_led_id(LedId led) {
    switch (led) {
        case LED_1:
            return LED_1_Pin;
        case LED_2:
            return LED_2_Pin;
        default:
            return -1;
    }
}

void gpio_led_set_state(LedId led, LedStatus state) {
    if (led >= LED_COUNT)
        return;

    GPIO_TypeDef * port = _gpio_get_port_from_led_id(led);
    int16_t pin = _gpio_get_pin_from_led_id(led);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_led_toggle_state(LedId led) {
    if (led >= LED_COUNT)
        return;

    GPIO_TypeDef * port = _gpio_get_port_from_led_id(led);
    int16_t pin = _gpio_get_pin_from_led_id(led);
    HAL_GPIO_TogglePin(port, pin);
}

/* USER CODE END 2 */
