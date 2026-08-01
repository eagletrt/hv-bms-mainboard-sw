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
#define HC_CONNECTED_MCU_Pin GPIO_PIN_2
#define HC_CONNECTED_MCU_GPIO_Port GPIOE
#define SPARE_6_Pin GPIO_PIN_3
#define SPARE_6_GPIO_Port GPIOE
#define SPARE_5_Pin GPIO_PIN_4
#define SPARE_5_GPIO_Port GPIOE
#define SPARE_12_Pin GPIO_PIN_5
#define SPARE_12_GPIO_Port GPIOE
#define SPARE_13_Pin GPIO_PIN_6
#define SPARE_13_GPIO_Port GPIOE
#define SPARE_4_Pin GPIO_PIN_13
#define SPARE_4_GPIO_Port GPIOC
#define SPARE_3_Pin GPIO_PIN_14
#define SPARE_3_GPIO_Port GPIOC
#define SPARE_14_Pin GPIO_PIN_15
#define SPARE_14_GPIO_Port GPIOC
#define SPARE_2_Pin GPIO_PIN_0
#define SPARE_2_GPIO_Port GPIOF
#define SPARE_15_Pin GPIO_PIN_1
#define SPARE_15_GPIO_Port GPIOF
#define SPARE_1_Pin GPIO_PIN_2
#define SPARE_1_GPIO_Port GPIOF
#define SD_R_BMS_OUT_MCU_Pin GPIO_PIN_3
#define SD_R_BMS_OUT_MCU_GPIO_Port GPIOF
#define SD_R_IMD_OUT_MCU_Pin GPIO_PIN_4
#define SD_R_IMD_OUT_MCU_GPIO_Port GPIOF
#define AIRP_OPEN_MEC_MCU_Pin GPIO_PIN_5
#define AIRP_OPEN_MEC_MCU_GPIO_Port GPIOF
#define AIRN_OPEN_MEC_MCU_Pin GPIO_PIN_6
#define AIRN_OPEN_MEC_MCU_GPIO_Port GPIOF
#define V5_MCU_Pin GPIO_PIN_7
#define V5_MCU_GPIO_Port GPIOF
#define PLAUSIBLE_STATE_RC_MCU_Pin GPIO_PIN_8
#define PLAUSIBLE_STATE_RC_MCU_GPIO_Port GPIOF
#define AMB_TEMP_Pin GPIO_PIN_9
#define AMB_TEMP_GPIO_Port GPIOF
#define PROBING_3V3_Pin GPIO_PIN_10
#define PROBING_3V3_GPIO_Port GPIOF
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define TSAL_GREEN_MCU_Pin GPIO_PIN_0
#define TSAL_GREEN_MCU_GPIO_Port GPIOC
#define IMD_OK_MCU_Pin GPIO_PIN_1
#define IMD_OK_MCU_GPIO_Port GPIOC
#define ITS_HC_F_Pin GPIO_PIN_2
#define ITS_HC_F_GPIO_Port GPIOC
#define AUX_TEMP_MCU_Pin GPIO_PIN_3
#define AUX_TEMP_MCU_GPIO_Port GPIOC
#define ITS_LC_F_Pin GPIO_PIN_0
#define ITS_LC_F_GPIO_Port GPIOA
#define SD_IN_MCU_Pin GPIO_PIN_1
#define SD_IN_MCU_GPIO_Port GPIOA
#define SD_END_MCU_Pin GPIO_PIN_2
#define SD_END_MCU_GPIO_Port GPIOA
#define SD_OUT_Pin GPIO_PIN_3
#define SD_OUT_GPIO_Port GPIOA
#define SPARE_16_Pin GPIO_PIN_4
#define SPARE_16_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_4_MCU_Pin GPIO_PIN_5
#define OUTLET_LIQUID_TEMP_4_MCU_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_5_MCU_Pin GPIO_PIN_6
#define OUTLET_LIQUID_TEMP_5_MCU_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_6_MCU_Pin GPIO_PIN_7
#define OUTLET_LIQUID_TEMP_6_MCU_GPIO_Port GPIOA
#define OUTLET_LIQUID_TEMP_3_MCU_Pin GPIO_PIN_4
#define OUTLET_LIQUID_TEMP_3_MCU_GPIO_Port GPIOC
#define INLET_LIQUID_TEMP_MCU_Pin GPIO_PIN_5
#define INLET_LIQUID_TEMP_MCU_GPIO_Port GPIOC
#define OUTLET_LIQUID_TEMP_1_MCU_Pin GPIO_PIN_0
#define OUTLET_LIQUID_TEMP_1_MCU_GPIO_Port GPIOB
#define OUTLET_LIQUID_TEMP_2_MCU_Pin GPIO_PIN_1
#define OUTLET_LIQUID_TEMP_2_MCU_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_0
#define LED_1_GPIO_Port GPIOG
#define LED_2_Pin GPIO_PIN_1
#define LED_2_GPIO_Port GPIOG
#define SEG7_S5_Pin GPIO_PIN_8
#define SEG7_S5_GPIO_Port GPIOE
#define SEG7_S6_Pin GPIO_PIN_9
#define SEG7_S6_GPIO_Port GPIOE
#define SEG7_S7_Pin GPIO_PIN_10
#define SEG7_S7_GPIO_Port GPIOE
#define SEG7_DP_Pin GPIO_PIN_11
#define SEG7_DP_GPIO_Port GPIOE
#define SEG7_S1_Pin GPIO_PIN_12
#define SEG7_S1_GPIO_Port GPIOE
#define SEG7_S2_Pin GPIO_PIN_13
#define SEG7_S2_GPIO_Port GPIOE
#define SEG7_S3_Pin GPIO_PIN_14
#define SEG7_S3_GPIO_Port GPIOE
#define SEG7_S4_Pin GPIO_PIN_15
#define SEG7_S4_GPIO_Port GPIOE
#define SPI2_SCK_MCU_Pin GPIO_PIN_10
#define SPI2_SCK_MCU_GPIO_Port GPIOB
#define isoCAN_RX_Pin GPIO_PIN_12
#define isoCAN_RX_GPIO_Port GPIOB
#define isoCAN_TX_Pin GPIO_PIN_13
#define isoCAN_TX_GPIO_Port GPIOB
#define SPI2_MISO_MCU_Pin GPIO_PIN_14
#define SPI2_MISO_MCU_GPIO_Port GPIOB
#define SPI2_MOSI_MCU_Pin GPIO_PIN_15
#define SPI2_MOSI_MCU_GPIO_Port GPIOB
#define CS_EEPROM_Pin GPIO_PIN_8
#define CS_EEPROM_GPIO_Port GPIOD
#define NOT_HOLD_Pin GPIO_PIN_9
#define NOT_HOLD_GPIO_Port GPIOD
#define IMD_PWM_Pin GPIO_PIN_12
#define IMD_PWM_GPIO_Port GPIOD
#define BMS_OK_3V3_Pin GPIO_PIN_5
#define BMS_OK_3V3_GPIO_Port GPIOG
#define NOT_PRECHARGE_Pin GPIO_PIN_6
#define NOT_PRECHARGE_GPIO_Port GPIOG
#define AIRN_OFF_Pin GPIO_PIN_7
#define AIRN_OFF_GPIO_Port GPIOG
#define AIRP_OFF_Pin GPIO_PIN_8
#define AIRP_OFF_GPIO_Port GPIOG
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOA
#define TX_SWD_Pin GPIO_PIN_9
#define TX_SWD_GPIO_Port GPIOA
#define RX_SWD_Pin GPIO_PIN_10
#define RX_SWD_GPIO_Port GPIOA
#define CAN_RX_Pin GPIO_PIN_11
#define CAN_RX_GPIO_Port GPIOA
#define CAN_TX_Pin GPIO_PIN_12
#define CAN_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define CS_ADC_Pin GPIO_PIN_15
#define CS_ADC_GPIO_Port GPIOA
#define SPI3_SCK_Pin GPIO_PIN_10
#define SPI3_SCK_GPIO_Port GPIOC
#define SPI3_MISO_Pin GPIO_PIN_11
#define SPI3_MISO_GPIO_Port GPIOC
#define SPI3_MOSI_Pin GPIO_PIN_12
#define SPI3_MOSI_GPIO_Port GPIOC
#define NOT_ADC_INT_Pin GPIO_PIN_0
#define NOT_ADC_INT_GPIO_Port GPIOD
#define NOT_EXT_FAULT_LATCHED_MCU_Pin GPIO_PIN_1
#define NOT_EXT_FAULT_LATCHED_MCU_GPIO_Port GPIOD
#define NOT_IMPLAUSIBLE_STATE_LATCHED_MCU_Pin GPIO_PIN_2
#define NOT_IMPLAUSIBLE_STATE_LATCHED_MCU_GPIO_Port GPIOD
#define NOT_BMS_FAULT_LATCHED_MCU_Pin GPIO_PIN_3
#define NOT_BMS_FAULT_LATCHED_MCU_GPIO_Port GPIOD
#define NOT_IMD_FAULT_LATCHED_MCU_Pin GPIO_PIN_4
#define NOT_IMD_FAULT_LATCHED_MCU_GPIO_Port GPIOD
#define IMD_CONNECTED_MCU_Pin GPIO_PIN_5
#define IMD_CONNECTED_MCU_GPIO_Port GPIOD
#define SPARE_11_Pin GPIO_PIN_6
#define SPARE_11_GPIO_Port GPIOD
#define NOT_LATCH_RESET_MCU_Pin GPIO_PIN_7
#define NOT_LATCH_RESET_MCU_GPIO_Port GPIOD
#define INDICATOR_CONNECTED_MCU_Pin GPIO_PIN_9
#define INDICATOR_CONNECTED_MCU_GPIO_Port GPIOG
#define NOT_IMD_FAULT_COCKPIT_LED_MCU_Pin GPIO_PIN_10
#define NOT_IMD_FAULT_COCKPIT_LED_MCU_GPIO_Port GPIOG
#define NOT_BMS_FAULT_COCKPIT_LED_MCU_Pin GPIO_PIN_11
#define NOT_BMS_FAULT_COCKPIT_LED_MCU_GPIO_Port GPIOG
#define PLAUSIBLE_STATE_MCU_Pin GPIO_PIN_12
#define PLAUSIBLE_STATE_MCU_GPIO_Port GPIOG
#define PLAUSIBLE_STATE_PERSISTED_MCU_Pin GPIO_PIN_13
#define PLAUSIBLE_STATE_PERSISTED_MCU_GPIO_Port GPIOG
#define TS_LESS_THAN_60V_MCU_Pin GPIO_PIN_14
#define TS_LESS_THAN_60V_MCU_GPIO_Port GPIOG
#define PRECH_OPEN_MEC_MCU_Pin GPIO_PIN_15
#define PRECH_OPEN_MEC_MCU_GPIO_Port GPIOG
#define PRECH_CLOSE_COM_MCU_Pin GPIO_PIN_3
#define PRECH_CLOSE_COM_MCU_GPIO_Port GPIOB
#define SPARE_7_Pin GPIO_PIN_4
#define SPARE_7_GPIO_Port GPIOB
#define SPARE_10_Pin GPIO_PIN_5
#define SPARE_10_GPIO_Port GPIOB
#define SPARE_8_Pin GPIO_PIN_6
#define SPARE_8_GPIO_Port GPIOB
#define SPARE_9_Pin GPIO_PIN_7
#define SPARE_9_GPIO_Port GPIOB
#define AIRN_CLOSE_COM_MCU_Pin GPIO_PIN_0
#define AIRN_CLOSE_COM_MCU_GPIO_Port GPIOE
#define AIRP_CLOSE_COM_MCU_Pin GPIO_PIN_1
#define AIRP_CLOSE_COM_MCU_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
