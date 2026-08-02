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

#include "eagletrt-api.h"
#include "feedback.h"
#include "main.h"

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
void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, SPARE_6_Pin | SPARE_5_Pin | SPARE_12_Pin | SPARE_13_Pin | SEG7_S5_Pin | SEG7_S6_Pin | SEG7_S7_Pin | SEG7_DP_Pin | SEG7_S1_Pin | SEG7_S2_Pin | SEG7_S3_Pin | SEG7_S4_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, SPARE_4_Pin | SPARE_3_Pin | SPARE_14_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOF, SPARE_2_Pin | SPARE_15_Pin | SPARE_1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, SPARE_16_Pin | CS_ADC_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOG, LED_1_Pin | LED_2_Pin | BMS_OK_3V3_Pin | NOT_PRECHARGE_Pin | AIRN_OFF_Pin | AIRP_OFF_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, CS_EEPROM_Pin | NOT_HOLD_Pin | NOT_ADC_INT_Pin | SPARE_11_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, SPARE_7_Pin | SPARE_10_Pin | SPARE_8_Pin | SPARE_9_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : PEPin PEPin PEPin */
    GPIO_InitStruct.Pin = HC_CONNECTED_MCU_Pin | AIRN_CLOSE_COM_MCU_Pin | AIRP_CLOSE_COM_MCU_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin */
    GPIO_InitStruct.Pin = SPARE_6_Pin | SPARE_5_Pin | SPARE_12_Pin | SPARE_13_Pin | SEG7_S5_Pin | SEG7_S6_Pin | SEG7_S7_Pin | SEG7_DP_Pin | SEG7_S1_Pin | SEG7_S2_Pin | SEG7_S3_Pin | SEG7_S4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : PCPin PCPin PCPin */
    GPIO_InitStruct.Pin = SPARE_4_Pin | SPARE_3_Pin | SPARE_14_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PFPin PFPin PFPin */
    GPIO_InitStruct.Pin = SPARE_2_Pin | SPARE_15_Pin | SPARE_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pins : PAPin PAPin */
    GPIO_InitStruct.Pin = SPARE_16_Pin | CS_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PGPin PGPin PGPin PGPin
                           PGPin PGPin */
    GPIO_InitStruct.Pin = LED_1_Pin | LED_2_Pin | BMS_OK_3V3_Pin | NOT_PRECHARGE_Pin | AIRN_OFF_Pin | AIRP_OFF_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /*Configure GPIO pins : PDPin PDPin PDPin PDPin */
    GPIO_InitStruct.Pin = CS_EEPROM_Pin | NOT_HOLD_Pin | NOT_ADC_INT_Pin | SPARE_11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : PDPin PDPin PDPin PDPin
                           PDPin PDPin */
    GPIO_InitStruct.Pin = NOT_EXT_FAULT_LATCHED_MCU_Pin | NOT_IMPLAUSIBLE_STATE_LATCHED_MCU_Pin | NOT_BMS_FAULT_LATCHED_MCU_Pin | NOT_IMD_FAULT_LATCHED_MCU_Pin | IMD_CONNECTED_MCU_Pin | NOT_LATCH_RESET_MCU_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : PGPin PGPin PGPin PGPin
                           PGPin PGPin PGPin */
    GPIO_InitStruct.Pin = INDICATOR_CONNECTED_MCU_Pin | NOT_IMD_FAULT_COCKPIT_LED_MCU_Pin | NOT_BMS_FAULT_COCKPIT_LED_MCU_Pin | PLAUSIBLE_STATE_MCU_Pin | PLAUSIBLE_STATE_PERSISTED_MCU_Pin | TS_LESS_THAN_60V_MCU_Pin | PRECH_OPEN_MEC_MCU_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = PRECH_CLOSE_COM_MCU_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PRECH_CLOSE_COM_MCU_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
    GPIO_InitStruct.Pin = SPARE_7_Pin | SPARE_10_Pin | SPARE_8_Pin | SPARE_9_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/*!
 * \brief Get the LED GPIO port from its identifier
 *
 * \param led The led identifier
 *
 * \returns A pointer to the GPIO port, or NULL if the id is not valid
 */
GPIO_TypeDef *prv_gpio_get_port_from_led_id(const enum LedId led) {
    switch (led) {
        case LED_ID_1:
            return LED_1_GPIO_Port;
        case LED_ID_2:
            return LED_2_GPIO_Port;
        default:
            return NULL;
    }
}

/*!
 * \brief Get the LED GPIO pin from its identifier
 *
 * \param led The led identifier
 *
 * \returns The GPIO pin, or -1 if the id is not valid
 */
int16_t prv_gpio_get_pin_from_led_id(const enum LedId led) {
    switch (led) {
        case LED_ID_1:
            return LED_1_Pin;
        case LED_ID_2:
            return LED_2_Pin;
        default:
            return -1;
    }
}

/*!
 * \brief Get the 7-segment display segment GPIO port from its identifier
 *
 * \param segment The segment identifier
 *
 * \return A pointer to the GPIO port, or NULL if the id is not valid
 */
GPIO_TypeDef *prv_gpio_get_port_from_display_segment(const enum DisplaySegment segment) {
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

/*!
 * \brief Get the 7-segment display segment GPIO pin from its identifier
 *
 * \param segment The segment identifier
 *
 * \returns The GPIO pin, or -1 if the id is not valid
 */
int16_t prv_gpio_get_pin_from_display_segment(const enum DisplaySegment segment) {
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

/*!
 * \brief Get the PCU pin GPIO port from its name
 *
 * \param pin The PCU pin name
 *
 * \returns A pointer to the GPIO port, or NULL if the PCU pin is not valid
 */
GPIO_TypeDef *prv_gpio_get_port_from_pcu_pin(const enum PcuPin pin) {
    switch (pin) {
        case PCU_PIN_AIR_NEGATIVE:
            return AIRN_OFF_GPIO_Port;
        case PCU_PIN_AIR_POSITIVE:
            return AIRP_OFF_GPIO_Port;
        case PCU_PIN_PRECHARGE:
            return NOT_PRECHARGE_GPIO_Port;
        case PCU_PIN_AMS:
            return BMS_OK_3V3_GPIO_Port;
        default:
            return NULL;
    }
}

/*!
 * \brief Get the PCU GPIO pin from its name
 *
 * \param pin The PCU pin name
 *
 * \return int16_t The GPIO pin, or -1 if the PCU pin is not valid
 */
int16_t prv_gpio_get_pin_from_pcu_pin(const enum PcuPin pin) {
    switch (pin) {
        case PCU_PIN_AIR_NEGATIVE:
            return AIRN_OFF_Pin;
        case PCU_PIN_AIR_POSITIVE:
            return AIRP_OFF_Pin;
        case PCU_PIN_PRECHARGE:
            return NOT_PRECHARGE_Pin;
        case PCU_PIN_AMS:
            return BMS_OK_3V3_Pin;
        default:
            return -1;
    }
}

void gpio_led_set_state(const enum LedId led, const enum LedStatus state) {
    if (led >= LED_ID_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_led_id(led);
    const int16_t pin = prv_gpio_get_pin_from_led_id(led);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_led_toggle_state(const enum LedId led) {
    if (led >= LED_ID_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_led_id(led);
    const int16_t pin = prv_gpio_get_pin_from_led_id(led);
    HAL_GPIO_TogglePin(port, pin);
}

void gpio_display_segment_set_state(const enum DisplaySegment segment, const enum DisplaySegmentStatus state) {
    if (segment >= DISPLAY_SEGMENT_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_display_segment(segment);
    const int16_t pin = prv_gpio_get_pin_from_display_segment(segment);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_display_segment_toggle_state(const enum DisplaySegment segment) {
    if (segment >= DISPLAY_SEGMENT_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_display_segment(segment);
    const int16_t pin = prv_gpio_get_pin_from_display_segment(segment);
    HAL_GPIO_TogglePin(port, pin);
}

void gpio_pcu_set_state(const enum PcuPin pcu_pin, const enum PcuPinStatus state) {
    if (pcu_pin >= PCU_PIN_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_pcu_pin(pcu_pin);
    const uint16_t pin = prv_gpio_get_pin_from_pcu_pin(pcu_pin);
    HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state);
}

void gpio_pcu_toggle_state(const enum PcuPin pcu_pin) {
    if (pcu_pin >= PCU_PIN_COUNT) {
        return;
    }
    GPIO_TypeDef *const port = prv_gpio_get_port_from_pcu_pin(pcu_pin);
    const uint16_t pin = prv_gpio_get_pin_from_pcu_pin(pcu_pin);
    HAL_GPIO_TogglePin(port, pin);
}

bit_flag32_t gpio_feedback_read_all(void) {
    register bit_flag32_t feedbacks = 0U;
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM, HAL_GPIO_ReadPin(AIRP_CLOSE_COM_MCU_GPIO_Port, AIRP_CLOSE_COM_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM, HAL_GPIO_ReadPin(AIRN_CLOSE_COM_MCU_GPIO_Port, AIRN_CLOSE_COM_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM, HAL_GPIO_ReadPin(PRECH_CLOSE_COM_MCU_GPIO_Port, PRECH_CLOSE_COM_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC, HAL_GPIO_ReadPin(PRECH_OPEN_MEC_MCU_GPIO_Port, PRECH_OPEN_MEC_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V, HAL_GPIO_ReadPin(TS_LESS_THAN_60V_MCU_GPIO_Port, TS_LESS_THAN_60V_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED, HAL_GPIO_ReadPin(PLAUSIBLE_STATE_PERSISTED_MCU_GPIO_Port, PLAUSIBLE_STATE_PERSISTED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE, HAL_GPIO_ReadPin(PLAUSIBLE_STATE_MCU_GPIO_Port, PLAUSIBLE_STATE_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED, HAL_GPIO_ReadPin(NOT_BMS_FAULT_COCKPIT_LED_MCU_GPIO_Port, NOT_BMS_FAULT_COCKPIT_LED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED, HAL_GPIO_ReadPin(NOT_IMD_FAULT_COCKPIT_LED_MCU_GPIO_Port, NOT_IMD_FAULT_COCKPIT_LED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED, HAL_GPIO_ReadPin(INDICATOR_CONNECTED_MCU_GPIO_Port, INDICATOR_CONNECTED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_LATCH_RESET, HAL_GPIO_ReadPin(NOT_LATCH_RESET_MCU_GPIO_Port, NOT_LATCH_RESET_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED, HAL_GPIO_ReadPin(NOT_IMPLAUSIBLE_STATE_LATCHED_MCU_GPIO_Port, NOT_IMPLAUSIBLE_STATE_LATCHED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED, HAL_GPIO_ReadPin(NOT_BMS_FAULT_LATCHED_MCU_GPIO_Port, NOT_BMS_FAULT_LATCHED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED, HAL_GPIO_ReadPin(NOT_IMD_FAULT_LATCHED_MCU_GPIO_Port, NOT_IMD_FAULT_LATCHED_MCU_Pin) == GPIO_PIN_SET);
    feedbacks = EAGLETRT_API_BIT_TOGGLE_IF(feedbacks, FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED, HAL_GPIO_ReadPin(NOT_EXT_FAULT_LATCHED_MCU_GPIO_Port, NOT_EXT_FAULT_LATCHED_MCU_Pin) == GPIO_PIN_SET);
    return feedbacks;
}

/* USER CODE END 2 */
