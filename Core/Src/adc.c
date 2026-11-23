/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

#include <stdint.h>

#include "eagletrt-api.h"
#include "sensing-api.h"

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

/* ADC1 init function */
void MX_ADC1_Init(void) {

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = { 0 };

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 15;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 6;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = 7;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = 8;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = 9;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 10;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = 11;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_12;
    sConfig.Rank = 12;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_13;
    sConfig.Rank = 13;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = 14;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = 15;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}
/* ADC3 init function */
void MX_ADC3_Init(void) {

    /* USER CODE BEGIN ADC3_Init 0 */

    /* USER CODE END ADC3_Init 0 */

    ADC_ChannelConfTypeDef sConfig = { 0 };

    /* USER CODE BEGIN ADC3_Init 1 */

    /* USER CODE END ADC3_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.ScanConvMode = ENABLE;
    hadc3.Init.ContinuousConvMode = DISABLE;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.NbrOfConversion = 8;
    hadc3.Init.DMAContinuousRequests = DISABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if (HAL_ADC_Init(&hadc3) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = 5;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = 6;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = 7;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = 8;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC3_Init 2 */

    /* USER CODE END ADC3_Init 2 */
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */
        /* ADC1 clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    PC3     ------> ADC1_IN13
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PC4     ------> ADC1_IN14
    PC5     ------> ADC1_IN15
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    */
        GPIO_InitStruct.Pin = TSAL_GREEN_MCU_Pin | IMD_OK_MCU_Pin | ITS_HC_F_Pin | AUX_TEMP_MCU_Pin | OUTLET_LIQUID_TEMP_3_MCU_Pin | INLET_LIQUID_TEMP_MCU_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = SD_IN_MCU_Pin | SD_END_MCU_Pin | SD_OUT_Pin | OUTLET_LIQUID_TEMP_4_MCU_Pin | OUTLET_LIQUID_TEMP_5_MCU_Pin | OUTLET_LIQUID_TEMP_6_MCU_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = OUTLET_LIQUID_TEMP_1_MCU_Pin | OUTLET_LIQUID_TEMP_2_MCU_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc1.Instance = DMA2_Stream0;
        hdma_adc1.Init.Channel = DMA_CHANNEL_0;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_NORMAL;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    } else if (adcHandle->Instance == ADC3) {
        /* USER CODE BEGIN ADC3_MspInit 0 */

        /* USER CODE END ADC3_MspInit 0 */
        /* ADC3 clock enable */
        __HAL_RCC_ADC3_CLK_ENABLE();

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**ADC3 GPIO Configuration
    PF3     ------> ADC3_IN9
    PF4     ------> ADC3_IN14
    PF5     ------> ADC3_IN15
    PF6     ------> ADC3_IN4
    PF7     ------> ADC3_IN5
    PF8     ------> ADC3_IN6
    PF9     ------> ADC3_IN7
    PF10     ------> ADC3_IN8
    */
        GPIO_InitStruct.Pin = SD_R_BMS_OUT_MCU_Pin | SD_R_IMD_OUT_MCU_Pin | AIRP_OPEN_MEC_MCU_Pin | AIRN_OPEN_MEC_MCU_Pin | V5_MCU_Pin | PLAUSIBLE_STATE_RC_MCU_Pin | AMB_TEMP_Pin | PROBING_3V3_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* ADC3 DMA Init */
        /* ADC3 Init */
        hdma_adc3.Instance = DMA2_Stream1;
        hdma_adc3.Init.Channel = DMA_CHANNEL_2;
        hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc3.Init.Mode = DMA_NORMAL;
        hdma_adc3.Init.Priority = DMA_PRIORITY_LOW;
        hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc3) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc3);

        /* USER CODE BEGIN ADC3_MspInit 1 */

        /* USER CODE END ADC3_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle) {

    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspDeInit 0 */

        /* USER CODE END ADC1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    PC3     ------> ADC1_IN13
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PC4     ------> ADC1_IN14
    PC5     ------> ADC1_IN15
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    */
        HAL_GPIO_DeInit(GPIOC, TSAL_GREEN_MCU_Pin | IMD_OK_MCU_Pin | ITS_HC_F_Pin | AUX_TEMP_MCU_Pin | OUTLET_LIQUID_TEMP_3_MCU_Pin | INLET_LIQUID_TEMP_MCU_Pin);

        HAL_GPIO_DeInit(GPIOA, SD_IN_MCU_Pin | SD_END_MCU_Pin | SD_OUT_Pin | OUTLET_LIQUID_TEMP_4_MCU_Pin | OUTLET_LIQUID_TEMP_5_MCU_Pin | OUTLET_LIQUID_TEMP_6_MCU_Pin);

        HAL_GPIO_DeInit(GPIOB, OUTLET_LIQUID_TEMP_1_MCU_Pin | OUTLET_LIQUID_TEMP_2_MCU_Pin);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC1_MspDeInit 1 */

        /* USER CODE END ADC1_MspDeInit 1 */
    } else if (adcHandle->Instance == ADC3) {
        /* USER CODE BEGIN ADC3_MspDeInit 0 */

        /* USER CODE END ADC3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC3_CLK_DISABLE();

        /**ADC3 GPIO Configuration
    PF3     ------> ADC3_IN9
    PF4     ------> ADC3_IN14
    PF5     ------> ADC3_IN15
    PF6     ------> ADC3_IN4
    PF7     ------> ADC3_IN5
    PF8     ------> ADC3_IN6
    PF9     ------> ADC3_IN7
    PF10     ------> ADC3_IN8
    */
        HAL_GPIO_DeInit(GPIOF, SD_R_BMS_OUT_MCU_Pin | SD_R_IMD_OUT_MCU_Pin | AIRP_OPEN_MEC_MCU_Pin | AIRN_OPEN_MEC_MCU_Pin | V5_MCU_Pin | PLAUSIBLE_STATE_RC_MCU_Pin | AMB_TEMP_Pin | PROBING_3V3_Pin);

        /* ADC3 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC3_MspDeInit 1 */

        /* USER CODE END ADC3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/*!
 * \brief Get the HAL handler of an MCU ADC
 *
 * \param adc The ADC
 *
 * \returns A pointer to the HAL handler, or NULL if the ADC is not valid
 */
static ADC_HandleTypeDef *prv_adc_get_handle(const enum SensingAdc adc) {
    switch (adc) {
        case SENSING_ADC_1:
            return &HADC_1;
        case SENSING_ADC_2:
            return &HADC_2;
        default:
            return NULL;
    }
}

bool adc_sensing_start(const enum SensingAdc adc, volatile raw_volt *const buffer, const size_t count) {
    ADC_HandleTypeDef *const hadc = prv_adc_get_handle(adc);
    if (hadc == NULL || buffer == NULL || count == 0U) {
        return false;
    }

    /*
     * The DMA writes 16 bit samples (half word alignment), the HAL just takes a
     * generic pointer; the volatile qualifier is dropped because the buffer is
     * only read by the sensing module after the conversion complete notification
     */
    return HAL_ADC_Start_DMA(hadc, (uint32_t *)(uintptr_t)buffer, (uint32_t)count) == HAL_OK;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == HADC_1.Instance) {
        EAGLETRT_API_UNUSED(sensing_api_adc_conversion_complete(SENSING_ADC_1));
    } else if (hadc->Instance == HADC_2.Instance) {
        EAGLETRT_API_UNUSED(sensing_api_adc_conversion_complete(SENSING_ADC_2));
    }
}

/* USER CODE END 1 */
