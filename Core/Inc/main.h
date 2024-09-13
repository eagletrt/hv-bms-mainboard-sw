/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HANDCART_CONNECTED_Pin GPIO_PIN_2
#define HANDCART_CONNECTED_GPIO_Port GPIOE
#define SPARE_2_Pin GPIO_PIN_3
#define SPARE_2_GPIO_Port GPIOE
#define SPARE_3_Pin GPIO_PIN_4
#define SPARE_3_GPIO_Port GPIOE
#define SPARE_4_Pin GPIO_PIN_5
#define SPARE_4_GPIO_Port GPIOE
#define SPARE_5_Pin GPIO_PIN_6
#define SPARE_5_GPIO_Port GPIOE
#define SPARE_6_Pin GPIO_PIN_13
#define SPARE_6_GPIO_Port GPIOC
#define SPARE_7_Pin GPIO_PIN_14
#define SPARE_7_GPIO_Port GPIOC
#define SPARE_8_Pin GPIO_PIN_15
#define SPARE_8_GPIO_Port GPIOC
#define SPARE_9_Pin GPIO_PIN_0
#define SPARE_9_GPIO_Port GPIOF
#define SPARE_10_Pin GPIO_PIN_1
#define SPARE_10_GPIO_Port GPIOF
#define SPARE_11_Pin GPIO_PIN_2
#define SPARE_11_GPIO_Port GPIOF
#define AIRN_OPEN_MEC_Pin GPIO_PIN_3
#define AIRN_OPEN_MEC_GPIO_Port GPIOF
#define AIRP_OPEN_MEC_Pin GPIO_PIN_4
#define AIRP_OPEN_MEC_GPIO_Port GPIOF
#define IMD_OK_Pin GPIO_PIN_5
#define IMD_OK_GPIO_Port GPIOF
#define PLAUSIBLE_STATE_RC_Pin GPIO_PIN_6
#define PLAUSIBLE_STATE_RC_GPIO_Port GPIOF
#define TSAL_GREEN_Pin GPIO_PIN_7
#define TSAL_GREEN_GPIO_Port GPIOF
#define AMB_TEMP_Pin GPIO_PIN_9
#define AMB_TEMP_GPIO_Port GPIOF
#define PROBING_3V3_Pin GPIO_PIN_10
#define PROBING_3V3_GPIO_Port GPIOF
#define COOLING_CONSUMPTION_Pin GPIO_PIN_0
#define COOLING_CONSUMPTION_GPIO_Port GPIOC
#define ITS_HC_F_Pin GPIO_PIN_1
#define ITS_HC_F_GPIO_Port GPIOC
#define ITS_LC_F_Pin GPIO_PIN_2
#define ITS_LC_F_GPIO_Port GPIOC
#define AUX_TEMP_Pin GPIO_PIN_3
#define AUX_TEMP_GPIO_Port GPIOC
#define SD_OUT_Pin GPIO_PIN_0
#define SD_OUT_GPIO_Port GPIOA
#define SD_IN_Pin GPIO_PIN_1
#define SD_IN_GPIO_Port GPIOA
#define SD_END_Pin GPIO_PIN_2
#define SD_END_GPIO_Port GPIOA
#define V5_MCU_Pin GPIO_PIN_3
#define V5_MCU_GPIO_Port GPIOA
#define DAC_PUMP_Pin GPIO_PIN_4
#define DAC_PUMP_GPIO_Port GPIOA
#define INLET_LIQUID_TEMP_Pin GPIO_PIN_5
#define INLET_LIQUID_TEMP_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_1_Pin GPIO_PIN_6
#define OUTLET_LIQUID_TEMP_1_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_2_Pin GPIO_PIN_7
#define OUTLET_LIQUID_TEMP_2_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_3_Pin GPIO_PIN_4
#define OUTLET_LIQUID_TEMP_3_GPIO_Port GPIOC
#define OUTLET_LIQUID_TEMP_4_Pin GPIO_PIN_5
#define OUTLET_LIQUID_TEMP_4_GPIO_Port GPIOC
#define OUTLET_LIQUID_TEMP_5_Pin GPIO_PIN_0
#define OUTLET_LIQUID_TEMP_5_GPIO_Port GPIOB
#define OUTLET_LIQUID_TEMP_6_Pin GPIO_PIN_1
#define OUTLET_LIQUID_TEMP_6_GPIO_Port GPIOB
#define RADIATOR_PWM_Pin GPIO_PIN_2
#define RADIATOR_PWM_GPIO_Port GPIOB
#define SEG7_S1_Pin GPIO_PIN_7
#define SEG7_S1_GPIO_Port GPIOE
#define SEG7_S2_Pin GPIO_PIN_8
#define SEG7_S2_GPIO_Port GPIOE
#define SEG7_S3_Pin GPIO_PIN_9
#define SEG7_S3_GPIO_Port GPIOE
#define SEG7_S4_Pin GPIO_PIN_10
#define SEG7_S4_GPIO_Port GPIOE
#define SEG7_S5_Pin GPIO_PIN_12
#define SEG7_S5_GPIO_Port GPIOE
#define SEG7_S6_Pin GPIO_PIN_13
#define SEG7_S6_GPIO_Port GPIOE
#define SEG7_S7_Pin GPIO_PIN_14
#define SEG7_S7_GPIO_Port GPIOE
#define SEG7_DP_Pin GPIO_PIN_15
#define SEG7_DP_GPIO_Port GPIOE
#define SPI_EEPROM_SCK_Pin GPIO_PIN_10
#define SPI_EEPROM_SCK_GPIO_Port GPIOB
#define CAN_BMS_RX_Pin GPIO_PIN_12
#define CAN_BMS_RX_GPIO_Port GPIOB
#define CAN_BMS_TX_Pin GPIO_PIN_13
#define CAN_BMS_TX_GPIO_Port GPIOB
#define SPI_EEPROM_MISO_Pin GPIO_PIN_14
#define SPI_EEPROM_MISO_GPIO_Port GPIOB
#define SPI_EEPROM_MOSI_Pin GPIO_PIN_15
#define SPI_EEPROM_MOSI_GPIO_Port GPIOB
#define SPI_EEPROM_CS_Pin GPIO_PIN_8
#define SPI_EEPROM_CS_GPIO_Port GPIOD
#define HOLD_EEPROM_Pin GPIO_PIN_9
#define HOLD_EEPROM_GPIO_Port GPIOD
#define IMD_PWM_Pin GPIO_PIN_12
#define IMD_PWM_GPIO_Port GPIOD
#define LED_2_Pin GPIO_PIN_14
#define LED_2_GPIO_Port GPIOD
#define LED_1_Pin GPIO_PIN_15
#define LED_1_GPIO_Port GPIOD
#define BMS_OK_Pin GPIO_PIN_5
#define BMS_OK_GPIO_Port GPIOG
#define PRECHARGE_Pin GPIO_PIN_6
#define PRECHARGE_GPIO_Port GPIOG
#define AIRN_OFF_Pin GPIO_PIN_7
#define AIRN_OFF_GPIO_Port GPIOG
#define AIRP_OFF_Pin GPIO_PIN_8
#define AIRP_OFF_GPIO_Port GPIOG
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOA
#define CAN_PRIMARY_RX_Pin GPIO_PIN_11
#define CAN_PRIMARY_RX_GPIO_Port GPIOA
#define CAN_PRIMARY_TX_Pin GPIO_PIN_12
#define CAN_PRIMARY_TX_GPIO_Port GPIOA
#define SPI_ADC_CS_Pin GPIO_PIN_15
#define SPI_ADC_CS_GPIO_Port GPIOA
#define SPI_ADC_SCK_Pin GPIO_PIN_10
#define SPI_ADC_SCK_GPIO_Port GPIOC
#define SPI_ADC_MISO_Pin GPIO_PIN_11
#define SPI_ADC_MISO_GPIO_Port GPIOC
#define SPI_ADC_MOSI_Pin GPIO_PIN_12
#define SPI_ADC_MOSI_GPIO_Port GPIOC
#define SPI_ADC_INT_Pin GPIO_PIN_0
#define SPI_ADC_INT_GPIO_Port GPIOD
#define EXT_FAULT_LATCHED_Pin GPIO_PIN_1
#define EXT_FAULT_LATCHED_GPIO_Port GPIOD
#define IMD_FAULT_LATCHED_Pin GPIO_PIN_2
#define IMD_FAULT_LATCHED_GPIO_Port GPIOD
#define BMS_FAULT_LATCHED_Pin GPIO_PIN_3
#define BMS_FAULT_LATCHED_GPIO_Port GPIOD
#define PLAUSIBLE_STATE_LATCHED_Pin GPIO_PIN_5
#define PLAUSIBLE_STATE_LATCHED_GPIO_Port GPIOD
#define SPARE_13_Pin GPIO_PIN_6
#define SPARE_13_GPIO_Port GPIOD
#define LATCH_RESET_Pin GPIO_PIN_7
#define LATCH_RESET_GPIO_Port GPIOD
#define INDICATOR_CONNECTED_Pin GPIO_PIN_9
#define INDICATOR_CONNECTED_GPIO_Port GPIOG
#define IMD_FAULT_COCKPIT_LED_Pin GPIO_PIN_10
#define IMD_FAULT_COCKPIT_LED_GPIO_Port GPIOG
#define BMS_FAULT_COCKPIT_LED_Pin GPIO_PIN_11
#define BMS_FAULT_COCKPIT_LED_GPIO_Port GPIOG
#define PLAUSIBLE_STATE_Pin GPIO_PIN_12
#define PLAUSIBLE_STATE_GPIO_Port GPIOG
#define PLAUSIBLE_STATE_PERSISTED_Pin GPIO_PIN_13
#define PLAUSIBLE_STATE_PERSISTED_GPIO_Port GPIOG
#define TS_LESS_THAN_60V_Pin GPIO_PIN_14
#define TS_LESS_THAN_60V_GPIO_Port GPIOG
#define PRECHARGE_OPEN_MEC_Pin GPIO_PIN_15
#define PRECHARGE_OPEN_MEC_GPIO_Port GPIOG
#define PRECHARGE_OPEN_COM_Pin GPIO_PIN_3
#define PRECHARGE_OPEN_COM_GPIO_Port GPIOB
#define SPARE_12_Pin GPIO_PIN_4
#define SPARE_12_GPIO_Port GPIOB
#define SPARE_14_Pin GPIO_PIN_5
#define SPARE_14_GPIO_Port GPIOB
#define SPARE_15_Pin GPIO_PIN_6
#define SPARE_15_GPIO_Port GPIOB
#define SPARE_16_Pin GPIO_PIN_7
#define SPARE_16_GPIO_Port GPIOB
#define SD_BMS_FB_Pin GPIO_PIN_8
#define SD_BMS_FB_GPIO_Port GPIOB
#define SD_IMD_FB_Pin GPIO_PIN_9
#define SD_IMD_FB_GPIO_Port GPIOB
#define AIRP_OPEN_COM_Pin GPIO_PIN_0
#define AIRP_OPEN_COM_GPIO_Port GPIOE
#define AIRN_OPEN_COM_Pin GPIO_PIN_1
#define AIRN_OPEN_COM_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
