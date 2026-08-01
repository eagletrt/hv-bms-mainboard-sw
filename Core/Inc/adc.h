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

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc3;

/* USER CODE BEGIN Private defines */

/*! \brief Redefinition of the ADC handlers */
#define HADC_1 hadc1
#define HADC_2 hadc3

/*! \brief Total number of channels used by the ADCs */
#define ADC_1_CHANNEL_COUNT (ADC_1_CHANNEL_INDEX_COUNT)
#define ADC_2_CHANNEL_COUNT (ADC_2_CHANNEL_INDEX_COUNT)

/*! \brief Feedbacks reference voltage in V */
#define ADC_VREF (3.3f)

/*! \brief Feedbacks ADC resolution in bits */
#define ADC_RESOLUTION (12U)

/*!
 * \brief Indices of the first ADC channels
 *
 * \attention The order of the items have to match the order of the ADC channels in the configuration
 */
enum Adc1ChannelIndex : uint8_t {
    ADC_1_CHANNEL_INDEX_ITS_LC_F,                 /*!< IN0: Second channel of an auxiliry current sensor (UNUSED) */
    ADC_1_CHANNEL_INDEX_SD_IN_MCU,                /*!< IN1: ShutDown voltage before the HV BAT Interlock entering the BMS */
    ADC_1_CHANNEL_INDEX_SD_END_MCU,               /*!< IN2: ShutDown voltage before the AIRs, end of the line */
    ADC_1_CHANNEL_INDEX_SD_OUT,                   /*!< IN3: ShutDown after the IMD exiting the BMS */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_4_MCU, /*!< IN5: Fourth cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_5_MCU, /*!< IN6: Fifth cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_6_MCU, /*!< IN7: Sixth cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_1_MCU, /*!< IN8: First cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_2_MCU, /*!< IN9: Second cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_TSAL_GREEN_MCU,           /*!< IN10: TSAL current status */
    ADC_1_CHANNEL_INDEX_IMD_OK_MCU,               /*!< IN11: IMD status */
    ADC_1_CHANNEL_INDEX_ITS_HC_F,                 /*!< IN12: First channel of an auxiliry current sensor (UNUSED)*/
    ADC_1_CHANNEL_INDEX_AUX_TEMP_MCU,             /*!< IN13: Auxiliary cooling temperature */
    ADC_1_CHANNEL_INDEX_OUTLET_LIQUID_TEMP_3_MCU, /*!< IN14: Third cooling temperature of the outlet liquid */
    ADC_1_CHANNEL_INDEX_INLET_LIQUID_TEMP_MCU,    /*!< IN15: Cooling temperature of the inlet liquid */
    ADC_1_CHANNEL_INDEX_COUNT,                    /*!< Total number of ADC 1 channels */
    ADC_1_CHANNEL_INDEX_INVALID                   /*!< Invalid ADC 1 channel */
};

/*!
 * \brief Indices of the second ADC channels
 *
 * \attention The order of the items have to match the order of the ADC channels in the configuartion
 */
enum Adc2ChannelIndex : uint8_t {
    ADC_2_CHANNEL_INDEX_AIRN_OPEN_MEC_MCU,      /*!< IN4: Mechanical status of the AIR- */
    ADC_2_CHANNEL_INDEX_V5_MCU,                 /*!< IN5: 5V supply */
    ADC_2_CHANNEL_INDEX_PLAUSIBLE_STATE_RC_MCU, /*!< IN6: Plausible state voltage after the RC circuit */
    ADC_2_CHANNEL_INDEX_AMB_TEMP,               /*!< IN7: Ambient temperature */
    ADC_2_CHANNEL_INDEX_PROBING_3V3,            /*!< IN8: 3V3 supply voltage */
    ADC_2_CHANNEL_INDEX_SD_R_BMS_OUT_MCU,       /*!< IN9: Voltage after the AMS ShutDown node */
    ADC_2_CHANNEL_INDEX_SD_R_IMD_OUT_MCU,       /*!< IN14: Voltage after the IMD ShutDown node */
    ADC_2_CHANNEL_INDEX_AIRP_OPEN_MEC_MCU,      /*!< IN15: Mechanical status of the AIR+ */
    ADC_2_CHANNEL_INDEX_COUNT,                  /*!< Total number of ADC 2 channels */
    ADC_2_CHANNEL_INDEX_INVALID                 /*!< Invalid ADC 2 channel */
};

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC3_Init(void);

/* USER CODE BEGIN Prototypes */

// TODO: Change DMA settings to run in continous conversion instead of just once
// so that this function is not called only for the feedback module

/*! \brief Start the ADC conversions for the feedbacks acquisition */
void adc_start_feedback_conversion(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
