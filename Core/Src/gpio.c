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

#include "feedback.h"

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
  HAL_GPIO_WritePin(GPIOE, SPARE_2_Pin|SPARE_3_Pin|SPARE_4_Pin|SPARE_5_Pin
                          |SEG7_S1_Pin|SEG7_S2_Pin|SEG7_S3_Pin|SEG7_S4_Pin
                          |SEG7_S5_Pin|SEG7_S6_Pin|SEG7_S7_Pin|SEG7_DP_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = HANDCART_CONNECTED_Pin|AIRP_OPEN_COM_Pin|AIRN_OPEN_COM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = SPARE_2_Pin|SPARE_3_Pin|SPARE_4_Pin|SPARE_5_Pin
                          |SEG7_S1_Pin|SEG7_S2_Pin|SEG7_S3_Pin|SEG7_S4_Pin
                          |SEG7_S5_Pin|SEG7_S6_Pin|SEG7_S7_Pin|SEG7_DP_Pin;
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
  GPIO_InitStruct.Pin = INDICATOR_CONNECTED_Pin|IMD_FAULT_COCKPIT_LED_Pin|BMS_FAULT_COCKPIT_LED_Pin|PLAUSIBLE_STATE_Pin
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

}

/* USER CODE BEGIN 2 */

/**
 * @brief Get the LED GPIO port from its identifier
 *
 * @param led The led identifier
 *
 * @return GPIO_TypeDef* A pointer to the GPIO port, or NULL if the id is not valid
 */
GPIO_TypeDef * _gpio_get_port_from_led_id(LedId led) {
    switch (led) {
        case LED_ID_1:
            return LED_1_GPIO_Port;
        case LED_ID_2:
            return LED_2_GPIO_Port;
        default:
            return NULL;
    }
}

/**
 * @brief Get the LED GPIO pin from its identifier
 *
 * @param led The led identifier
 *
 * @return int16_t The GPIO pin, or -1 if the id is not valid
 */
int16_t _gpio_get_pin_from_led_id(LedId led) {
    switch (led) {
        case LED_ID_1:
            return LED_1_Pin;
        case LED_ID_2:
            return LED_2_Pin;
        default:
            return -1;
    }
}

/**
 * @brief Get the 7-segment display segment GPIO port from its identifier
 *
 * @param segment The segment identifier
 *
 * @return GPIO_TypeDef* A pointer to the GPIO port, or NULL if the id is not valid
 */
GPIO_TypeDef * _gpio_get_port_from_display_segment(DisplaySegment segment) {
    switch (segment) {
        case DISPLAY_SEGMENT_TOP:
            return SEG7_S1_GPIO_Port;
        case DISPLAY_SEGMENT_TOP_RIGHT:
            return SEG7_S2_GPIO_Port;
        case DISPLAY_SEGMENT_TOP_LEFT:
            return SEG7_S6_GPIO_Port;
        case DISPLAY_SEGMENT_MIDDLE:
            return SEG7_S7_GPIO_Port;
        case DISPLAY_SEGMENT_BOTTOM:
            return SEG7_S4_GPIO_Port;
        case DISPLAY_SEGMENT_BOTTOM_RIGHT:
            return SEG7_S3_GPIO_Port;
        case DISPLAY_SEGMENT_BOTTOM_LEFT:
            return SEG7_S5_GPIO_Port;
        case DISPLAY_SEGMENT_DECIMAL_POINT:
            return SEG7_DP_GPIO_Port; 

        default:
            return NULL;
    }
}

/**
 * @brief Get the 7-segment display segment GPIO pin from its identifier
 *
 * @param segment The segment identifier
 *
 * @return int16_t The GPIO pin, or -1 if the id is not valid
 */
int16_t _gpio_get_pin_from_display_segment(DisplaySegment segment) {
    switch (segment) {
        case DISPLAY_SEGMENT_TOP:
            return SEG7_S1_Pin;
        case DISPLAY_SEGMENT_TOP_RIGHT:
            return SEG7_S2_Pin;
        case DISPLAY_SEGMENT_TOP_LEFT:
            return SEG7_S6_Pin;
        case DISPLAY_SEGMENT_MIDDLE:
            return SEG7_S7_Pin;
        case DISPLAY_SEGMENT_BOTTOM:
            return SEG7_S4_Pin;
        case DISPLAY_SEGMENT_BOTTOM_RIGHT:
            return SEG7_S3_Pin;
        case DISPLAY_SEGMENT_BOTTOM_LEFT:
            return SEG7_S5_Pin;
        case DISPLAY_SEGMENT_DECIMAL_POINT:
            return SEG7_DP_Pin; 

        default:
            return -1;
    }
}

/**
 * @brief Get the PCU pin GPIO port from its name
 *
 * @param pin The PCU pin name
 *
 * @return GPIO_TypeDef* A pointer to the GPIO port, or NULL if the PCU pin is not valid
 */
GPIO_TypeDef * _gpio_get_port_from_pcu_pin(PcuPin pin) {
    switch (pin) {
        case PCU_PIN_AIR_NEGATIVE: 
            return AIRN_OFF_GPIO_Port;
        case PCU_PIN_AIR_POSITIVE: 
            return AIRP_OFF_GPIO_Port;
        case PCU_PIN_PRECHARGE: 
            return PRECHARGE_GPIO_Port;
        case PCU_PIN_AMS:
            return BMS_OK_GPIO_Port;
        default:
            return NULL;
    }
}

/**
 * @brief Get the PCU GPIO pin from its name
 *
 * @param pin The PCU pin name
 *
 * @return int16_t The GPIO pin, or -1 if the PCU pin is not valid
 */
int16_t _gpio_get_pin_from_pcu_pin(PcuPin pin) {
    switch (pin) {
        case PCU_PIN_AIR_NEGATIVE: 
            return AIRN_OFF_Pin;
        case PCU_PIN_AIR_POSITIVE: 
            return AIRP_OFF_Pin;
        case PCU_PIN_PRECHARGE: 
            return PRECHARGE_Pin;
        case PCU_PIN_AMS:
            return BMS_OK_Pin;
        default:
            return -1;
    }
}

void gpio_led_set_state(LedId led, LedStatus state) {
    if (led >= LED_ID_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_led_id(led);
    int16_t pin = _gpio_get_pin_from_led_id(led);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_led_toggle_state(LedId led) {
    if (led >= LED_ID_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_led_id(led);
    int16_t pin = _gpio_get_pin_from_led_id(led);
    HAL_GPIO_TogglePin(port, pin);
}

void gpio_display_segment_set_state(DisplaySegment segment, DisplaySegmentStatus state) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_display_segment(segment);
    int16_t pin = _gpio_get_pin_from_display_segment(segment);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_display_segment_toggle_state(DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_display_segment(segment);
    int16_t pin = _gpio_get_pin_from_display_segment(segment);
    HAL_GPIO_TogglePin(port, pin);
}

void gpio_pcu_set_state(PcuPin pcu_pin, PcuPinStatus state) {
    if (pcu_pin >= PCU_PIN_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_pcu_pin(pcu_pin);
    uint16_t pin = _gpio_get_pin_from_pcu_pin(pcu_pin);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_pcu_toggle_state(PcuPin pcu_pin) {
    if (pcu_pin >= PCU_PIN_COUNT)
        return;
    GPIO_TypeDef * port = _gpio_get_port_from_pcu_pin(pcu_pin);
    uint16_t pin = _gpio_get_pin_from_pcu_pin(pcu_pin);
    HAL_GPIO_TogglePin(port, pin);
}

bit_flag32_t gpio_feedback_read_all(void) {
    register bit_flag32_t feedbacks = 0U;

    // Read all pins
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(AIRN_OPEN_COM_GPIO_Port, AIRN_OPEN_COM_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(AIRP_OPEN_COM_GPIO_Port, AIRP_OPEN_COM_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(SD_IMD_FB_GPIO_Port, SD_IMD_FB_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_SD_IMD_FB);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(SD_BMS_FB_GPIO_Port, SD_BMS_FB_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_SD_BMS_FB);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(PRECHARGE_OPEN_COM_GPIO_Port, PRECHARGE_OPEN_COM_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(PRECHARGE_OPEN_MEC_GPIO_Port, PRECHARGE_OPEN_MEC_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(TS_LESS_THAN_60V_GPIO_Port, TS_LESS_THAN_60V_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(PLAUSIBLE_STATE_GPIO_Port, PLAUSIBLE_STATE_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(BMS_FAULT_COCKPIT_LED_GPIO_Port, BMS_FAULT_COCKPIT_LED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(IMD_FAULT_COCKPIT_LED_GPIO_Port, IMD_FAULT_COCKPIT_LED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(INDICATOR_CONNECTED_GPIO_Port, INDICATOR_CONNECTED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(LATCH_RESET_GPIO_Port, LATCH_RESET_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_LATCH_RESET);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(IMPLAUSIBLE_STATE_LATCHED_GPIO_Port, IMPLAUSIBLE_STATE_LATCHED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(BMS_FAULT_LATCHED_GPIO_Port, BMS_FAULT_LATCHED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(IMD_FAULT_LATCHED_GPIO_Port, IMD_FAULT_LATCHED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED);
    feedbacks = MAINBOARD_BIT_TOGGLE_IF(feedbacks, HAL_GPIO_ReadPin(EXT_FAULT_LATCHED_GPIO_Port, EXT_FAULT_LATCHED_Pin) == GPIO_PIN_SET, FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED);
    return feedbacks;
}

/* USER CODE END 2 */
