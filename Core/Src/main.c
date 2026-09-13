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
#include "logger.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fsm.h"
#include "post.h"

#include "stm32f4xx_it.h"

#include "can-communication-router-api.h"
#include <stdint.h>
#include <ctype.h>

#include "feedback-api.h"
#include "arena-allocator-api.h"
#include "logger-api.h"

#include "volt-api.h"
#include "temp-api.h"
#include "pcu-api.h"
#include "bal-api.h"
#include "current-api.h"
#include "imd-api.h"
#include "internal-voltage-api.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOGGER_ENABLED (true)           /*!< Logger status: true to enable active logging, false to mute entirely. */
#define LOGGER_RX_CAPACITY (1U)         /*!< Receive queue depth. Set to 1 because the logger is transmit-only but needs to be > 0 because of arena allocator. */
#define LOGGER_TX_CAPACITY (10U)        /*!< Maximum number of log message packets allowed to sit in the outbound transmission queue. */
#define LOGGER_UART_MAX_MSG_SIZE (128U) /*!< Maximum allocation allowed for an individual log string. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
EAGLETRT_STATIC struct ArenaAllocatorHandler arena_allocator_handler;
EAGLETRT_STATIC struct PalHandler logger_pal_handler;
EAGLETRT_STATIC constexpr milliseconds_t MAIN_LOG_INTERVAL_MS = 1000U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void system_reset(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*!
 * \brief Initializes the low-level memory allocation and logging framework.
 */
EAGLETRT_STATIC void prv_main_init_logging_configuration() {
    arena_allocator_api_init(&arena_allocator_handler);

    EAGLETRT_API_UNUSED(pal_api_init(&logger_pal_handler,
                                     LOGGER_RX_CAPACITY,
                                     LOGGER_TX_CAPACITY,
                                     LOGGER_UART_MAX_MSG_SIZE,
                                     NULL,
                                     usart_logger_transmit,
                                     NULL,
                                     NULL,
                                     &arena_allocator_handler));
}

typedef enum MainLogMode {
    MAIN_LOG_MODE_VOLTAGE,
    MAIN_LOG_MODE_TEMPERATURE,
    MAIN_LOG_MODE_INTERNAL_VOLTAGE,
    MAIN_LOG_MODE_PCU,
    MAIN_LOG_MODE_BALANCING,
    MAIN_LOG_MODE_CURRENT,
    MAIN_LOG_MODE_IMD,
    MAIN_LOG_MODE_FEEDBACK
} MainLogMode;

EAGLETRT_STATIC MainLogMode main_log_mode = MAIN_LOG_MODE_VOLTAGE;

EAGLETRT_STATIC const char *prv_main_fsm_state_name(void) {
    const fsm_state_t state = fsm_get_status();
    if (state < FSM_NUM_STATES) {
        return fsm_state_names[state];
    }
    return "no change";
}

EAGLETRT_STATIC void prv_main_print_current_log(void) {
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
    logger_api_log(LOGGER_LEVEL_EMPTY, "Current report");
    logger_api_log(LOGGER_LEVEL_INFO, "FSM state: %s", prv_main_fsm_state_name());
    logger_api_log(LOGGER_LEVEL_INFO, "Current: %.3f A", current_api_get_current());
    logger_api_log(LOGGER_LEVEL_INFO, "Power: %.3f kW", current_api_get_power());
    logger_api_log(LOGGER_LEVEL_INFO, "Max current: %.3f A", CURRENT_MAX_A);
    logger_api_log(LOGGER_LEVEL_INFO, "Max power: %.3f kW", CURRENT_MAX_POWER_KW);
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
}

EAGLETRT_STATIC void prv_main_print_imd_log(void) {
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
    logger_api_log(LOGGER_LEVEL_EMPTY, "IMD report");
    logger_api_log(LOGGER_LEVEL_INFO, "FSM state: %s", prv_main_fsm_state_name());
    logger_api_log(LOGGER_LEVEL_INFO, "Status: %s", imd_api_get_imd_status_name(imd_api_get_status()));
    logger_api_log(LOGGER_LEVEL_INFO, "Frequency: %.3f Hz", imd_api_get_frequency());
    logger_api_log(LOGGER_LEVEL_INFO, "Duty cycle: %.1f %%", imd_api_get_duty_cycle() * 100.0F);
    logger_api_log(LOGGER_LEVEL_INFO, "Period: %.3f ms", imd_api_get_period());
    logger_api_log(
        LOGGER_LEVEL_INFO,
        "IMD OK: %s",
        feedback_api_get_status(FEEDBACK_ID_IMD_OK) == FEEDBACK_STATUS_HIGH ? "yes" : "no");
    logger_api_log(LOGGER_LEVEL_EMPTY, "========================================");
}

EAGLETRT_STATIC void prv_main_print_selected_log(void) {
    switch (main_log_mode) {
        case MAIN_LOG_MODE_VOLTAGE:
            volt_api_print_log();
            break;
        case MAIN_LOG_MODE_TEMPERATURE:
            temp_api_print_log();
            break;
        case MAIN_LOG_MODE_INTERNAL_VOLTAGE:
            internal_voltage_api_print_log();
            break;
        case MAIN_LOG_MODE_PCU:
            pcu_api_print_log();
            break;
        case MAIN_LOG_MODE_BALANCING:
            bal_api_print_log();
            break;
        case MAIN_LOG_MODE_CURRENT:
            prv_main_print_current_log();
            break;
        case MAIN_LOG_MODE_IMD:
            prv_main_print_imd_log();
            break;
        case MAIN_LOG_MODE_FEEDBACK:
            feedback_api_print_log();
            break;
        default:
            break;
    }
}

EAGLETRT_STATIC void prv_main_toggle_balancing(void) {
    enum BalReturnCode result;

    if (bal_api_is_active()) {
        result = bal_api_stop();
        if (result == BAL_RC_OK) {
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART balancing action: stop");
        } else {
            logger_api_log(LOGGER_LEVEL_INFO, "UART balancing action failed: stop");
        }
    } else {
        result = bal_api_start();
        if (result == BAL_RC_OK) {
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART balancing action: start");
        } else {
            logger_api_log(LOGGER_LEVEL_INFO, "UART balancing action failed: start");
        }
    }
}

EAGLETRT_STATIC void prv_main_handle_uart_log_selection(void) {
    char input = usart_read();
    if (input == '\0' || input == '\r' || input == '\n') {
        return;
    }

    switch (tolower((unsigned char)input)) {
        case 'v':
            main_log_mode = MAIN_LOG_MODE_VOLTAGE;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: voltage");
            break;
        case 't':
            main_log_mode = MAIN_LOG_MODE_TEMPERATURE;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: temperature");
            break;
        case 'i':
            main_log_mode = MAIN_LOG_MODE_INTERNAL_VOLTAGE;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: internal voltage");
            break;
        case 'p':
            main_log_mode = MAIN_LOG_MODE_PCU;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: pcu");
            break;
        case 'b':
            main_log_mode = MAIN_LOG_MODE_BALANCING;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: balancing");
            break;
        case 'c':
            main_log_mode = MAIN_LOG_MODE_CURRENT;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: current");
            break;
        case 'm':
            main_log_mode = MAIN_LOG_MODE_IMD;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: imd");
            break;
        case 'f':
            main_log_mode = MAIN_LOG_MODE_FEEDBACK;
            logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: feedback");
            break;
        case 's':
            prv_main_toggle_balancing();
            break;
        case 'h':
            logger_api_log(LOGGER_LEVEL_INFO, "FSM state: %s", prv_main_fsm_state_name());
            logger_api_log(LOGGER_LEVEL_EMPTY, "Keys: v voltages | t temperatures | i internal voltage | p pcu | b balancing | c current | m imd | f feedback | s balance toggle");
            break;
        case 'q':
            static fsm_event_data_t event = { .type = FSM_EVENT_TYPE_TS_ON };
            fsm_event_trigger(&event);
            break;
        case 'w':
            static fsm_event_data_t event2 = { .type = FSM_EVENT_TYPE_TS_OFF };
            fsm_event_trigger(&event2);
            break;
        default:
            break;
    }
    prv_main_print_selected_log();
}

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

    prv_main_init_logging_configuration();
    EAGLETRT_API_UNUSED(logger_api_init(&logger_pal_handler, LOGGER_LEVEL_ERROR));

    logger_api_log(LOGGER_LEVEL_INFO, "Starting main application");
    logger_api_log(LOGGER_LEVEL_EMPTY, "UART log mode: v voltages | t temperatures | i internal voltage | p pcu | b balancing | c current | m imd | f feedback | s balance toggle");

    // Configure and start CAN given if the handcart is connected or not
    // The handcart charger uses 250K baud rate, the vehicle 1M
    const GPIO_PinState handcart_connected = HAL_GPIO_ReadPin(HC_CONNECTED_MCU_GPIO_Port, HC_CONNECTED_MCU_Pin);
    can_configure_and_start_primary(handcart_connected == GPIO_PIN_SET);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    /*
   * This GPIO has to be always pulled high, if at any moment the pin state is
   * low that means that the board is not working properly
   */
    gpio_pcu_set_state(PCU_PIN_AMS, PCU_PIN_STATUS_HIGH);

    /**
   * Start the timer used to increment the timebase internal counter
   */
    HAL_TIM_Base_Start_IT(&HTIM_TIMEBASE);

    fsm_state_t fsm_state = FSM_STATE_INIT;

    // Prepare data for the POST procedure
    struct PostInitData init_data = {
        .can_networks = {
            [CAN_COMMUNICATION_NETWORK_BMS] = {
                .send = can_send_bms,
                .on_receive = can_communication_router_api_receive_bms,
                .cs_enter = __disable_irq,
                .cs_exit = __enable_irq,
            },
            [CAN_COMMUNICATION_NETWORK_PRIMARY] = {
                .send = can_send_primary,
                .on_receive = can_communication_router_api_receive_primary,
                .cs_enter = __disable_irq,
                .cs_exit = __enable_irq,
            } },
        .system_reset = system_reset,
        .cs_enter = it_cs_enter,
        .cs_exit = it_cs_exit,
        // .error_update_timer = tim_update_error_timer,
        // .error_stop_timer = tim_stop_error_timer,
        .led_set = gpio_led_set_state,
        .led_toggle = gpio_led_toggle_state,
        .imd_start = tim_start_pwm_imd,
        .pcu_set = gpio_pcu_set_state,
        .pcu_toggle = gpio_pcu_toggle_state,
        .feedback_read_all = gpio_feedback_read_all,
        .feedback_start_conversion = adc_start_feedback_conversion,
        .display_set = gpio_display_segment_set_state,
        .display_toggle = gpio_display_segment_toggle_state,
        .spi_send = spi_send,
        .spi_send_receive = spi_send_receive
    };

    fsm_state = fsm_run_state(fsm_state, &init_data);

    logger_api_log(LOGGER_LEVEL_INFO, "POST procedure completed, entering main loop");

    uint32_t last_log_tick = 0U;
    while (1) {
        prv_main_handle_uart_log_selection();

        fsm_state = fsm_run_state(fsm_state, NULL);

        if (HAL_GetTick() - last_log_tick >= MAIN_LOG_INTERVAL_MS) {
            HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
            //prv_main_print_selected_log();
            last_log_tick = HAL_GetTick();
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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
