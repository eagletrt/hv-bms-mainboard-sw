/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32f4xx_it.h"

#include "eagletrt-api.h"

#include "mainboard-tasks-api.h"
#include "mainboard-watchdogs-api.h"
#include "pcu-api.h"
#include "post-api.h"
#include "post.h"
#include "serializer-tasks-api.h"
#include "mainboard-logger-api.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void system_reset(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();
    MX_ADC3_Init();
    MX_SPI2_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();
    MX_USART1_UART_Init();
    MX_SPI3_Init();
    MX_TIM6_Init();
    MX_TIM7_Init();
    MX_TIM5_Init();
    /* USER CODE BEGIN 2 */

    // First so that every following initialization can be logged
    const struct MainboardLoggerInitData logger_data = {
        .write = usart_send,
        .read = usart_receive,
        .free_space = usart_get_tx_free_space,
        .get_tick = HAL_GetTick,
        .reset = system_reset,
        .verbosity = LOGGER_ENABLE_ALL,
    };
    if (mainboard_logger_api_init(&logger_data) != LOGGER_RC_OK) {
        Error_Handler();
    }

    MAINBOARD_LOGGER_INFO("Logger initialized, running POST");

    // Configure and start CAN given if the handcart is connected or not
    // The handcart charger uses 250K baud rate, the vehicle 1M
    can_configure_and_start_primary(gpio_read_hc_connected());

    /*
     * Open the relays and pull the AMS OK pin high as soon as possible,
     * the GPIO initialization leaves them low (relays closed, AMS active)
     */
    if (pcu_api_init(gpio_pcu_set_state, system_reset) != PCU_RC_OK) {
        Error_Handler();
    }

    const struct PostInitData init_data = {
        .send_primary = can_send_primary,
        .send_bms = can_send_bms,
        .tx_ready_primary = can_tx_ready_primary,
        .tx_ready_bms = can_tx_ready_bms,
        .status_primary = can_get_status_primary,
        .status_bms = can_get_status_bms,
        .cs_enter = it_cs_enter,
        .cs_exit = it_cs_exit,
        .led_set = gpio_led_set_state,
        .led_toggle = gpio_led_toggle_state,
        .display_set = gpio_display_segment_set_state,
        .display_toggle = gpio_display_segment_toggle_state,
        .get_tick = HAL_GetTick,
        .sensing = {
                    .gpio_read = gpio_sensing_read,
                    .adc_start = adc_sensing_start,
                    .imd_start = tim_start_pwm_imd,
                    .spi_send = spi_send,
                    .spi_send_receive = spi_send_receive,
                    },
    };

    EAGLETRT_API_UNUSED(post_api_run(&init_data));

    MAINBOARD_LOGGER_INFO("POST ran, starting main application, press h for the console commands");

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1) {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        EAGLETRT_API_UNUSED(serializer_tasks_api_routine());
        EAGLETRT_API_UNUSED(mainboard_tasks_api_routine());

        // Outside of the tasks so that the timeouts do not depend on the scheduler, an error latches the watchdogs fault
        EAGLETRT_API_UNUSED(mainboard_watchdogs_api_routine());
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
  */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
  */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void system_reset(void) {
    HAL_NVIC_SystemReset();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
