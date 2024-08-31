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
#define HADC_FEEDBACK_1 hadc1
#define HADC_FEEDBACK_2 hadc3

/** @brief Total number of channels used by the ADC */
#define ADC_FEEDBACK_1_COUNT (ADC_FEEDBACK_1_CHANNEL_INDEX_COUNT)
#define ADC_FEEDBACK_2_COUNT (ADC_FEEDBACK_2_CHANNEL_INDEX_COUNT)

/** @brief Feedbacks reference voltage in V */
#define ADC_VREF (3.3f)

/** @brief Feedbacks ADC resolution in bits */
#define ADC_RESOLUTION (12U)

/**
 * @brief Indices of the first ADC used for the feedbacks
 *
 * @attention The order of the items have to match the order of the ADC channels
 *
 * @details
 *     - 
 */
typedef enum {
    ADC_FEEDBACK_1_CHANNEL_INDEX_SD_OUT = 0U,
    ADC_FEEDBACK_1_CHANNEL_INDEX_SD_IN,
    ADC_FEEDBACK_1_CHANNEL_INDEX_SD_END,
    ADC_FEEDBACK_1_CHANNEL_INDEX_V5_MCU,
    ADC_FEEDBACK_1_CHANNEL_INDEX_INLET_LIQUID_TEMPERATURE,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_1,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_2,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_5,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_6,
    ADC_FEEDBACK_1_CHANNEL_INDEX_COOLING_CONSUMPTION,
    ADC_FEEDBACK_1_CHANNEL_INDEX_ITS_HC_F,
    ADC_FEEDBACK_1_CHANNEL_INDEX_ITS_LC_F,
    ADC_FEEDBACK_1_CHANNEL_INDEX_AUX_TEMP,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_3,
    ADC_FEEDBACK_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMPERATURE_4,
    ADC_FEEDBACK_1_CHANNEL_INDEX_COUNT
} AdcFeedback1ChannelIndex;

/**
 * @brief Indices of the second ADC used for the feedbacks
 *
 * @attention The order of the items have to match the order of the ADC channels
 * @attention AIR- and AIR+ are swapped in the schematic, the issue is temporarily
 * solved here by swapping the indices
 *
 * @details
 *     - 
 */
typedef enum {
    ADC_FEEDBACK_2_CHANNEL_INDEX_PLAUSIBLE_STATE_RC = 0U,
    ADC_FEEDBACK_2_CHANNEL_INDEX_TSAL_GREEN,
    ADC_FEEDBACK_2_CHANNEL_INDEX_AMBIENT_TEMPERATURE,
    ADC_FEEDBACK_2_CHANNEL_INDEX_PROBING_3V3,
    ADC_FEEDBACK_2_CHANNEL_INDEX_AIRP_OPEN_MEC, // This indices are swapped
    ADC_FEEDBACK_2_CHANNEL_INDEX_AIRN_OPEN_MEC, // Read above
    ADC_FEEDBACK_2_CHANNEL_INDEX_IMD_OK,
    ADC_FEEDBACK_2_CHANNEL_INDEX_COUNT
} AdcFeedback2ChannelIndex;

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC3_Init(void);

/* USER CODE BEGIN Prototypes */

/** @brief Start the ADC conversions for the feedbacks acquisition */
void adc_start_feedback_conversion(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

