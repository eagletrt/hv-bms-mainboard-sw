/*!
 * \file sensing-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Low level sensing (ADC, GPIO, interrupts) that provides the physical values
 */

#include "sensing-api.h"

#include <string.h>

#include "eagletrt-api.h"
#include "eagletrt.h"
#include "feedback-api.h"
#include "internal-temperatures-api.h"
#include "internal-voltages-api.h"
#include "ir1553204-api.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"
#include "mainboard-watchdogs-api.h"
#include "max22530-api.h"
#include "sensing.h"
#include "watchdogs.h"

EAGLETRT_STATIC struct SensingHandler sensing_handler;

// TODO: ADC error handling

/*! \brief Digital feedback fed by every GPIO */
EAGLETRT_STATIC const enum FeedbackDigitalIndex sensing_gpio_feedback[SENSING_GPIO_COUNT] = {
    [SENSING_GPIO_PRECH_OPEN_MEC] = FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC,
    [SENSING_GPIO_TS_LESS_THAN_60V] = FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V,
    [SENSING_GPIO_PLAUSIBLE_STATE_PERSISTED] = FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED,
    [SENSING_GPIO_PLAUSIBLE_STATE] = FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE,
    [SENSING_GPIO_NOT_BMS_FAULT_COCKPIT_LED] = FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED,
    [SENSING_GPIO_NOT_IMD_FAULT_COCKPIT_LED] = FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED,
    [SENSING_GPIO_INDICATOR_CONNECTED] = FEEDBACK_DIGITAL_INDEX_INDICATOR_CONNECTED,
    [SENSING_GPIO_NOT_LATCH_RESET] = FEEDBACK_DIGITAL_INDEX_LATCH_RESET,
    [SENSING_GPIO_NOT_IMPLAUSIBLE_STATE_LATCHED] = FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED,
    [SENSING_GPIO_NOT_BMS_FAULT_LATCHED] = FEEDBACK_DIGITAL_INDEX_AMS_FAULT_LATCHED,
    [SENSING_GPIO_NOT_IMD_FAULT_LATCHED] = FEEDBACK_DIGITAL_INDEX_IMD_FAULT_LATCHED,
    [SENSING_GPIO_NOT_EXT_FAULT_LATCHED] = FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED,
    [SENSING_GPIO_HC_CONNECTED] = FEEDBACK_DIGITAL_INDEX_HANDCART_CONNECTED
};

/*! \brief Destination of every sample of the first ADC (missing entries are unused) */
EAGLETRT_STATIC const struct SensingAdcRoute sensing_adc_1_routes[SENSING_ADC_1_CHANNEL_COUNT] = {
    [SENSING_ADC_1_CHANNEL_SD_IN] = { SENSING_TARGET_FEEDBACK,     FEEDBACK_ANALOG_INDEX_SD_IN                  },
    [SENSING_ADC_1_CHANNEL_SD_END] = { SENSING_TARGET_FEEDBACK,     FEEDBACK_ANALOG_INDEX_SD_END                 },
    [SENSING_ADC_1_CHANNEL_SD_OUT] = { SENSING_TARGET_FEEDBACK,     FEEDBACK_ANALOG_INDEX_SD_OUT                 },
    [SENSING_ADC_1_CHANNEL_TSAL_GREEN] = { SENSING_TARGET_FEEDBACK,     FEEDBACK_ANALOG_INDEX_TSAL_GREEN             },
    [SENSING_ADC_1_CHANNEL_IMD_OK] = { SENSING_TARGET_FEEDBACK,     FEEDBACK_ANALOG_INDEX_IMD_OK                 },
    [SENSING_ADC_1_CHANNEL_INLET_LIQUID_TEMP] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_INLET    },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_1] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_1 },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_2] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_2 },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_3] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_3 },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_4] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_4 },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_5] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_5 },
    [SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_6] = { SENSING_TARGET_COOLING_TEMP, INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_6 }
};

/*! \brief Destination of every sample of the second ADC (missing entries are unused) */
EAGLETRT_STATIC const struct SensingAdcRoute sensing_adc_2_routes[SENSING_ADC_2_CHANNEL_COUNT] = {
    [SENSING_ADC_2_CHANNEL_AIRN_OPEN_MEC] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC      },
    [SENSING_ADC_2_CHANNEL_V5_MCU] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_V5_MCU             },
    [SENSING_ADC_2_CHANNEL_PLAUSIBLE_STATE_RC] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC },
    [SENSING_ADC_2_CHANNEL_PROBING_3V3] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_PROBING_3V3        },
    [SENSING_ADC_2_CHANNEL_SD_R_BMS_OUT] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_SD_AMS_FB          },
    [SENSING_ADC_2_CHANNEL_SD_R_IMD_OUT] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_SD_IMD_FB          },
    [SENSING_ADC_2_CHANNEL_AIRP_OPEN_MEC] = { SENSING_TARGET_FEEDBACK, FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC      }
};

/*! \brief Routing table and number of samples of every MCU ADC */
EAGLETRT_STATIC const struct {
    const struct SensingAdcRoute *routes;
    size_t count;
} sensing_adcs[SENSING_ADC_COUNT] = {
    [SENSING_ADC_1] = { sensing_adc_1_routes, SENSING_ADC_1_CHANNEL_COUNT },
    [SENSING_ADC_2] = { sensing_adc_2_routes, SENSING_ADC_2_CHANNEL_COUNT }
};

/*!
 * \brief Convert a raw sample of an MCU ADC to the voltage at the pin
 *
 * \param raw The raw sample
 * \return The voltage in V
 */
EAGLETRT_STATIC volt prv_sensing_adc_raw_to_volt(raw_volt raw) {
    return MAINBOARD_ADC_RAW_VALUE_TO_VOLT(raw, SENSING_ADC_VREF, SENSING_ADC_RESOLUTION); //NOLINT
}

/*!
 * \brief Convert the voltage of a cooling NTC to its temperature
 *
 * \param value The voltage in V, clamped to the validity range of the polynomial
 * \return The temperature in °C
 */
EAGLETRT_STATIC celsius prv_sensing_cooling_ntc_to_celsius(volt value) {
    const volt clamped = EAGLETRT_API_CLAMP(value, SENSING_COOLING_NTC_MIN_V, SENSING_COOLING_NTC_MAX_V);

    // Horner's method
    celsius temp = SENSING_COOLING_NTC_COEFF_6;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_5;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_4;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_3;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_2;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_1;
    temp = (temp * clamped) + SENSING_COOLING_NTC_COEFF_0;
    return temp;
}

/*!
 * \brief Convert the external ADC voltage of the TS or pack divider to the high voltage
 *
 * \param value The voltage read by the external ADC in V
 * \param divider_gain The nominal gain of the divider
 * \return The high voltage in V (never negative)
 */
EAGLETRT_STATIC volt prv_sensing_hv_divider_to_volt(volt value, float divider_gain) {
    const volt high_voltage = ((value / divider_gain) * SENSING_HV_CALIBRATION_MULTIPLIER) - SENSING_HV_CALIBRATION_OFFSET_V;
    return EAGLETRT_API_MAX(0.F, high_voltage);
}

/*!
 * \brief Convert the external ADC voltage of the linear regulator temperature sensor
 *
 * \todo PLACEHOLDER: replace with the real sensor conversion
 *
 * \param value The voltage read by the external ADC in V
 * \return The temperature in °C
 */
EAGLETRT_STATIC celsius prv_sensing_linear_regulator_temp_to_celsius(volt value) {
    return (value * SENSING_LINEAR_REGULATOR_TEMP_GAIN_C_PER_V) + SENSING_LINEAR_REGULATOR_TEMP_OFFSET_C;
}

/*!
 * \brief Convert the external ADC voltage of the precharge resistors temperature sensor
 *
 * \todo PLACEHOLDER: replace with the real sensor conversion
 *
 * \param value The voltage read by the external ADC in V
 * \return The temperature in °C
 */
EAGLETRT_STATIC celsius prv_sensing_precharge_temp_to_celsius(volt value) {
    return (value * SENSING_PRECHARGE_TEMP_GAIN_C_PER_V) + SENSING_PRECHARGE_TEMP_OFFSET_C;
}

/*!
 * \brief Read every GPIO and push its level to the feedback module
 */
EAGLETRT_STATIC void prv_sensing_read_gpios(void) {
    for (size_t i = 0U; i < SENSING_GPIO_COUNT; ++i) {
        const bool level = sensing_handler.gpio_read((enum SensingGpio)i);
        (void)feedback_api_set_digital_value(sensing_gpio_feedback[i], level);
    }
}

/*!
 * \brief Convert the samples of a completed conversion and push them to the application modules
 *
 * \param adc The ADC whose samples are processed
 */
EAGLETRT_STATIC void prv_sensing_process_adc_samples(enum SensingAdc adc) {
    const struct SensingAdcRoute *routes = sensing_adcs[adc].routes;

    for (size_t i = 0U; i < sensing_adcs[adc].count; ++i) {
        const volt value = prv_sensing_adc_raw_to_volt(sensing_handler.adc_samples[adc][i]);

        switch (routes[i].target) {
            case SENSING_TARGET_FEEDBACK:
                (void)feedback_api_set_analog_value((enum FeedbackAnalogIndex)routes[i].index, value);
                break;
            case SENSING_TARGET_COOLING_TEMP:
                (void)internal_temperatures_api_set_value(
                    (enum InternalTemperaturesIndex)routes[i].index,
                    prv_sensing_cooling_ntc_to_celsius(value));
                break;
            case SENSING_TARGET_NONE:
            default:
                break;
        }
    }
}

/*!
 * \brief Process the samples of the MCU ADCs that completed a conversion and start a new one
 *
 * \retval SENSING_RC_HARDWARE_ERROR if a conversion could not be started
 * \retval SENSING_RC_OK otherwise
 */
EAGLETRT_STATIC enum SensingReturnCode prv_sensing_handle_adcs(void) {
    enum SensingReturnCode result = SENSING_RC_OK;

    for (size_t i = 0U; i < SENSING_ADC_COUNT; ++i) {
        const enum SensingAdc adc = (enum SensingAdc)i;

        if (sensing_handler.adc_state[adc] == SENSING_ADC_STATE_CONVERTED) {
            prv_sensing_process_adc_samples(adc);
            sensing_handler.adc_state[adc] = SENSING_ADC_STATE_IDLE;

            // A timed out watchdog stays timed out, the fault is never cleared
            (void)mainboard_watchdogs_api_pet(&sensing_handler.internal_adc_watchdogs[adc]);
        }

        if (sensing_handler.adc_state[adc] == SENSING_ADC_STATE_IDLE) {
            // Set before starting, the complete interrupt can fire before the callback returns
            sensing_handler.adc_state[adc] = SENSING_ADC_STATE_CONVERTING;

            if (!sensing_handler.adc_start(adc, sensing_handler.adc_samples[adc], sensing_adcs[adc].count)) {
                sensing_handler.adc_state[adc] = SENSING_ADC_STATE_IDLE;
                result = SENSING_RC_HARDWARE_ERROR;
            }
        }
    }
    return result;
}

/*!
 * \brief Read the external ADC and push the TS and pack voltages to the internal voltages module
 *
 * \retval SENSING_RC_HARDWARE_ERROR if the external ADC could not be read
 * \retval SENSING_RC_OK otherwise
 */
EAGLETRT_STATIC enum SensingReturnCode prv_sensing_read_external_adc(void) {
    volt values[SENSING_EXT_ADC_CHANNEL_COUNT] = { 0 };

    if (max22530_api_read_channels_all(&sensing_handler.max22530, true, values, NULL) != MAX22530_RC_OK) {
        return SENSING_RC_HARDWARE_ERROR;
    }

    (void)internal_voltages_api_set_value(
        INTERNAL_VOLTAGES_INDEX_TS,
        prv_sensing_hv_divider_to_volt(values[SENSING_EXT_ADC_CHANNEL_TS_VOLTAGE], SENSING_TS_DIVIDER_GAIN));

    (void)internal_voltages_api_set_value(
        INTERNAL_VOLTAGES_INDEX_PACK,
        prv_sensing_hv_divider_to_volt(values[SENSING_EXT_ADC_CHANNEL_PACK_VOLTAGE], SENSING_PACK_DIVIDER_GAIN));

    (void)internal_temperatures_api_set_value(
        INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR,
        prv_sensing_linear_regulator_temp_to_celsius(values[SENSING_EXT_ADC_CHANNEL_LINEAR_REGULATOR_TEMPERATURE]));

    (void)internal_temperatures_api_set_value(
        INTERNAL_TEMPERATURES_INDEX_PRECHARGE,
        prv_sensing_precharge_temp_to_celsius(values[SENSING_EXT_ADC_CHANNEL_PRECHARGE_TEMPERATURE]));

    return SENSING_RC_OK;
}

/*!
 * \brief Called when no conversion of an MCU ADC was processed within SENSING_MAX_ADC_CONVERSION_TIME_MS
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_internal_adc_stale_callback(void *context) {
    (void)context;
    (void)mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_STALE_ADC);
}

/*!
 * \brief Called when the external ADC was not read successfully within SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_external_adc_stale_callback(void *context) {
    (void)context;
    (void)mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_STALE_EXTERNAL_ADC);
}

/*!
 * \brief Called when no valid IMD capture arrived within SENSING_MAX_IMD_UPDATE_MS
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_imd_stale_callback(void *context) {
    (void)context;
    (void)mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_STALE_IMD);
}

/*!
 * \brief Initialize the watchdogs of the module, they are not started
 *
 * \retval SENSING_RC_ERROR if a watchdog could not be initialized
 * \retval SENSING_RC_OK otherwise
 */
EAGLETRT_STATIC enum SensingReturnCode prv_sensing_init_watchdogs(void) {
    for (size_t i = 0U; i < SENSING_ADC_COUNT; ++i) {
        if (mainboard_watchdogs_api_init_watchdog(&sensing_handler.internal_adc_watchdogs[i], SENSING_MAX_ADC_CONVERSION_TIME_MS, prv_internal_adc_stale_callback, NULL) != WATCHDOG_RC_OK) {
            return SENSING_RC_ERROR;
        }
    }
    if (mainboard_watchdogs_api_init_watchdog(&sensing_handler.external_adc_watchdog, SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS, prv_external_adc_stale_callback, NULL) != WATCHDOG_RC_OK) {
        return SENSING_RC_ERROR;
    }
    if (mainboard_watchdogs_api_init_watchdog(&sensing_handler.imd_capture_watchdog, SENSING_MAX_IMD_UPDATE_MS, prv_imd_stale_callback, NULL) != WATCHDOG_RC_OK) {
        return SENSING_RC_ERROR;
    }
    return SENSING_RC_OK;
}

/*!
 * \brief Start the watchdogs of the MCU ADCs and of the external ADC
 *
 * \details Started at the first routine instead of at init, so the first
 *          timeout is measured from when the sensing actually runs. A source
 *          that never delivers still times out since the watchdogs are never stopped.
 *          The IMD watchdog is not started here, see \ref prv_sensing_handle_imd_capture.
 *          A failed start latches the watchdogs fault in the wrapper
 */
EAGLETRT_STATIC void prv_sensing_start_watchdogs(void) {
    for (size_t i = 0U; i < SENSING_ADC_COUNT; ++i) {
        (void)mainboard_watchdogs_api_start(&sensing_handler.internal_adc_watchdogs[i]);
    }
    (void)mainboard_watchdogs_api_start(&sensing_handler.external_adc_watchdog);
}

/*!
 * \brief Start or pet the IMD watchdog if a valid capture arrived since the last routine
 *
 * \details The first capture starts the watchdog, so the startup of the IMD
 *          (before its first PWM period) is not timed; until then the IMD
 *          status reads as no signal. A timed out watchdog stays timed out since
 *          the fault is never cleared.
 *          The capture interrupt only sets a flag since the watchdog pool cannot
 *          be touched from an interrupt. A capture landing between the read and
 *          the clear of the flag is lost, which is harmless since the timeout spans
 *          several IMD periods
 */
EAGLETRT_STATIC void prv_sensing_handle_imd_capture(void) {
    if (sensing_handler.imd_captured) {
        sensing_handler.imd_captured = false;

        if (mainboard_watchdogs_api_pet(&sensing_handler.imd_capture_watchdog) == WATCHDOG_RC_NOT_RUNNING) {
            (void)mainboard_watchdogs_api_start(&sensing_handler.imd_capture_watchdog);
        }
    }
}

enum SensingReturnCode sensing_api_init(const struct SensingInitData *const data) {
    // Zeroing the handler would corrupt the watchdog pool, which still holds the running watchdogs
    if (sensing_handler.is_initialized) {
        return SENSING_RC_ERROR;
    }

    if (data == NULL ||
        data->gpio_read == NULL ||
        data->adc_start == NULL ||
        data->imd_start == NULL ||
        data->spi_send == NULL ||
        data->spi_send_receive == NULL) {
        return SENSING_RC_NULL_POINTER;
    }

    memset(&sensing_handler, 0, sizeof(sensing_handler));

    sensing_handler.gpio_read = data->gpio_read;
    sensing_handler.adc_start = data->adc_start;

    if (prv_sensing_init_watchdogs() != SENSING_RC_OK) {
        return SENSING_RC_ERROR;
    }

    if (max22530_api_init(&sensing_handler.max22530, data->spi_send, data->spi_send_receive) != MAX22530_RC_OK) {
        return SENSING_RC_HARDWARE_ERROR;
    }
    if (ir1553204_api_init(&sensing_handler.ir1553204) != IR1553204_RC_OK) {
        return SENSING_RC_HARDWARE_ERROR;
    }

    data->imd_start();

    sensing_handler.is_initialized = true;
    return SENSING_RC_OK;
}

enum SensingReturnCode sensing_api_routine(void) {
    if (!sensing_handler.is_initialized) {
        return SENSING_RC_NOT_INITIALIZED;
    }

    if (!sensing_handler.watchdogs_started) {
        prv_sensing_start_watchdogs();
        sensing_handler.watchdogs_started = true;
    }

    prv_sensing_read_gpios();

    const enum SensingReturnCode adc_rc = prv_sensing_handle_adcs();
    const enum SensingReturnCode ext_adc_rc = prv_sensing_read_external_adc();

    if (ext_adc_rc == SENSING_RC_OK) {
        (void)mainboard_watchdogs_api_pet(&sensing_handler.external_adc_watchdog);
    }

    prv_sensing_handle_imd_capture();

    return (adc_rc != SENSING_RC_OK) ? adc_rc : ext_adc_rc;
}

enum SensingReturnCode sensing_api_adc_conversion_complete(const enum SensingAdc adc) {
    if (adc >= SENSING_ADC_COUNT) {
        return SENSING_RC_INVALID_INDEX;
    }

    // Ignore spurious notifications, the samples are valid only after a requested conversion
    if (sensing_handler.adc_state[adc] == SENSING_ADC_STATE_CONVERTING) {
        sensing_handler.adc_state[adc] = SENSING_ADC_STATE_CONVERTED;
    }
    return SENSING_RC_OK;
}

enum SensingReturnCode sensing_api_imd_capture(const ticks source_frequency, const ticks period_count, const ticks high_count) {
    /*
     * Signal:
     *              ┌──────┐                   ┌──────┐
     *              │      │                   │      │
     * ─────────────┘      └───────────────────┘      └─────
     *              ↑      ↑                   ↑
     *              │      │                   │
     *              │      └─ CCR2 = CNT       └─ CCR1 = CNT (period)
     *              |         (high_count)          IRQ → callback fires
     *              └─ CNT = 0
     */
    if (period_count == 0U || high_count > period_count) {
        return SENSING_RC_INVALID_DATA;
    }

    // Interrupt context: the watchdog is petted by the routine
    sensing_handler.imd_captured = true;

    const hertz frequency = (hertz)source_frequency / (hertz)period_count;
    const precise_percentage duty_cycle = (precise_percentage)high_count / (precise_percentage)period_count;

    (void)ir1553204_api_set_frequency(&sensing_handler.ir1553204, frequency);
    (void)ir1553204_api_set_duty_cycle(&sensing_handler.ir1553204, duty_cycle);
    return SENSING_RC_OK;
}

enum Ir1553204Status sensing_api_get_imd_status(void) {
    return ir1553204_api_get_status(&sensing_handler.ir1553204);
}

hertz sensing_api_get_imd_frequency(void) {
    return ir1553204_api_get_frequency(&sensing_handler.ir1553204);
}

precise_percentage sensing_api_get_imd_duty_cycle(void) {
    return ir1553204_api_get_duty_cycle(&sensing_handler.ir1553204);
}
