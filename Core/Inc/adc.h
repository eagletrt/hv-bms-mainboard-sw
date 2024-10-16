/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc3;

/* USER CODE BEGIN Private defines */

/** @brief Redefinition of the ADC handlers */
#define HADC_1 hadc1
#define HADC_2 hadc3

/** @brief Total number of channels used by the ADCs */
#define ADC_1_CHANNEL_COUNT (ADC_1_CHANNEL_INDEX_COUNT)
#define ADC_2_CHANNEL_COUNT (ADC_2_CHANNEL_INDEX_COUNT)

/** @brief Feedbacks reference voltage in V */
#define ADC_VREF (3.3f)

/** @brief Feedbacks ADC resolution in bits */
#define ADC_RESOLUTION (12U)

/**
 * @brief Indices of the first ADC channels
 *
 * @attention The order of the items have to match the order of the ADC channels in the configuration
 *
 * @details
 *     - ADC_1_CHANNEL_INDEX_SD_OUT Shutdown out feedback
 *     - ADC_1_CHANNEL_INDEX_SD_IN Shutdown in feedback
 *     - ADC_1_CHANNEL_INDEX_SD_END Shutdown end feedback
 *     - ADC_1_CHANNEL_INDEX_V5_MCU Feedback on the 5V line
 *     - ADC_1_CHANNEL_INDEX_INLET_LIQUID_TEMPERATURE Cooling temperature of the inlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_1 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_2 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_3 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_4 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_5 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_6 Cooling temperature of the outlet liquid
 *     - ADC_1_CHANNEL_INDEX_AUX_TEMP Auxiliry temperature
 *     - ADC_1_CHANNEL_INDEX_COOLING_CONSUMPTION Consumption of the cooling system
 *     - ADC_1_CHANNEL_INDEX_ITS_HC_F First channel of an Hall-Effect sensor (unused, for retrocompatibility)
 *     - ADC_1_CHANNEL_INDEX_ITS_LC_F Second channel of an Hall-Effect sensor (unused, for retrocompatibility)
 */
typedef enum {
    ADC_1_CHANNEL_INDEX_SD_OUT = 0U,
    ADC_1_CHANNEL_INDEX_SD_IN,
    ADC_1_CHANNEL_INDEX_SD_END,
    ADC_1_CHANNEL_INDEX_V5_MCU,
    ADC_1_CHANNEL_INDEX_INLET_LIQUID_TEMPERATURE,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_1,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_2,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_5,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_6,
    ADC_1_CHANNEL_INDEX_COOLING_CONSUMPTION,
    ADC_1_CHANNEL_INDEX_ITS_HC_F,
    ADC_1_CHANNEL_INDEX_ITS_LC_F,
    ADC_1_CHANNEL_INDEX_AUX_TEMP,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_3,
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_4,
    ADC_1_CHANNEL_INDEX_COUNT
} Adc1ChannelIndex;

/**
 * @brief Indices of the second ADC channels
 *
 * @attention The order of the items have to match the order of the ADC channels in the configuartion
 * @attention AIR- and AIR+ are swapped in the schematic, the issue is temporarily
 * solved here by swapping the indices
 *
 * @details
 *     - ADC_2_CHANNEL_INDEX_PLAUSIBLE_STATE_RC Plausible state after the RC circuit feedback
 *     - ADC_2_CHANNEL_INDEX_TSAL_GREEN Feedback of the TSAL status
 *     - ADC_2_CHANNEL_INDEX_AMBIENT_TEMPERATURE Ambient temperature
 *     - ADC_2_CHANNEL_INDEX_PROBING_3V3 Feedback on the 3V3 line
 *     - ADC_2_CHANNEL_INDEX_AIRN_OPEN_MEC Feedback of the mechanical status of the AIR-
 *     - ADC_2_CHANNEL_INDEX_AIRP_OPEN_MEC Feedback of the mechanical status of the AIR+
 *     - ADC_2_CHANNEL_INDEX_IMD_OK Feedback of the status of the IMD
 */
typedef enum {
    ADC_2_CHANNEL_INDEX_PLAUSIBLE_STATE_RC = 0U,
    ADC_2_CHANNEL_INDEX_TSAL_GREEN,
    ADC_2_CHANNEL_INDEX_AMBIENT_TEMPERATURE,
    ADC_2_CHANNEL_INDEX_PROBING_3V3,
    ADC_2_CHANNEL_INDEX_AIRP_OPEN_MEC, // This indices are swapped
    ADC_2_CHANNEL_INDEX_AIRN_OPEN_MEC, // Read above
    ADC_2_CHANNEL_INDEX_IMD_OK,
    ADC_2_CHANNEL_INDEX_COUNT
} Adc2ChannelIndex;

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC3_Init(void);

/* USER CODE BEGIN Prototypes */

// TODO: Change DMA settings to run in continous conversion instead of just once
// so that this function is not called only for the feedback module

/** @brief Start the ADC conversions for the feedbacks acquisition */
void adc_start_feedback_conversion(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

