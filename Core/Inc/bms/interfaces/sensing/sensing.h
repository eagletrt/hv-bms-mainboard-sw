/*!
 * \file sensing.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the low level sensing
 *
 * \details The enums below describe the hardware as it is wired and configured
 *          (pins, ADC ranks, external ADC channels); the conversion to the
 *          physical values and the routing to the application modules is done
 *          inside the module
 */

#ifndef SENSING_H
#define SENSING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ir1553204.h"
#include "mainboard-def.h"
#include "max22530.h"
#include "watchdogs.h"

/*!* ######################### CONVERSIONS ############################### *!*/

/*! \brief Voltage reference of the MCU ADCs in V */
#define SENSING_ADC_VREF (3.3F)

/*! \brief Resolution of the MCU ADCs in bits */
#define SENSING_ADC_RESOLUTION (12U)

/*!
 * \brief Gain (Vout / Vin) of a resistive voltage divider
 *
 * \param r_top    Resistor between the input and the output node
 * \param r_bottom Resistor between the output node and ground
 */
#define SENSING_DIVIDER_GAIN(r_top, r_bottom) ((r_bottom) / ((r_top) + (r_bottom)))

/*!
 * \brief HV dividers in front of the external ADC
 *        (schematic: Analog sensing -> HV Voltage sensing)
 */
#define SENSING_TS_DIVIDER_R_TOP_OHM (10.0e6F)    /*!< R58, 10 MOhm 1% */
#define SENSING_TS_DIVIDER_R_BOTTOM_OHM (30.0e3F) /*!< R62, 30 kOhm 1% */
#define SENSING_TS_DIVIDER_GAIN \
    SENSING_DIVIDER_GAIN(SENSING_TS_DIVIDER_R_TOP_OHM, SENSING_TS_DIVIDER_R_BOTTOM_OHM)

#define SENSING_PACK_DIVIDER_R_TOP_OHM (10.0e6F)    /*!< R63, 10 MOhm 0.5% */
#define SENSING_PACK_DIVIDER_R_BOTTOM_OHM (29.4e3F) /*!< R64, 29.4 kOhm 0.1% */
#define SENSING_PACK_DIVIDER_GAIN \
    SENSING_DIVIDER_GAIN(SENSING_PACK_DIVIDER_R_TOP_OHM, SENSING_PACK_DIVIDER_R_BOTTOM_OHM)

/*!
 * \brief Empirical correction applied on top of the nominal divider gain
 *
 * \details volt = max(0, adc / GAIN * MULTIPLIER - OFFSET)
 *          Fitted against <reference meter> on <board / date> TODO
 */
#define SENSING_HV_CALIBRATION_MULTIPLIER (1.0273F)
#define SENSING_HV_CALIBRATION_OFFSET_V (2.19F)
/*!
 * \brief Range of validity in V of the cooling NTC polynomial
 *
 * \details The input voltage is clamped to this range before the conversion
 */
#define SENSING_COOLING_NTC_MIN_V (0.F)
#define SENSING_COOLING_NTC_MAX_V (3.F)

/*!
 * \brief Coefficients of the cooling NTC polynomial (volt to celsius)
 *
 * \details celsius = C0 + C1*v + C2*v^2 + ... + C6*v^6
 */
#define SENSING_COOLING_NTC_COEFF_0 (162.9947F)
#define SENSING_COOLING_NTC_COEFF_1 (-289.0887F)
#define SENSING_COOLING_NTC_COEFF_2 (353.5436F)
#define SENSING_COOLING_NTC_COEFF_3 (-276.9361F)
#define SENSING_COOLING_NTC_COEFF_4 (125.8228F)
#define SENSING_COOLING_NTC_COEFF_5 (-30.5363F)
#define SENSING_COOLING_NTC_COEFF_6 (3.0394F)

/*!
 * \brief Linear conversion of the internal temperature sensors read by the external ADC
 *
 * \details celsius = v * GAIN + OFFSET
 *
 * \todo PLACEHOLDER: the sensors and their conditioning circuit are not
 *       characterized yet, replace with the real conversion
 */
#define SENSING_LINEAR_REGULATOR_TEMP_GAIN_C_PER_V (1.F)
#define SENSING_LINEAR_REGULATOR_TEMP_OFFSET_C (0.F)
#define SENSING_PRECHARGE_TEMP_GAIN_C_PER_V (1.F)
#define SENSING_PRECHARGE_TEMP_OFFSET_C (0.F)

/*!* ######################### ENUMS ##################################### *!*/

/*!
 * \brief Return code for the sensing module functions
 */
enum SensingReturnCode {
    SENSING_RC_OK,              /*!< The function executed succesfully */
    SENSING_RC_NULL_POINTER,    /*!< A NULL pointer was given to a function */
    SENSING_RC_INVALID_INDEX,   /*!< The given index is out of range */
    SENSING_RC_INVALID_DATA,    /*!< The given data is not valid */
    SENSING_RC_NOT_INITIALIZED, /*!< The module was not initialized */
    SENSING_RC_HARDWARE_ERROR,  /*!< A hardware callback or a driver reported a failure */
    SENSING_RC_ERROR,           /*!< An identified error occurred */
};

/*!
 * \brief Digital inputs read from the GPIOs
 *
 * \details The names follow the pins, with their original polarity (the NOT_*
 *          pins are active low); the polarity is handled by the feedback module
 */
enum SensingGpio {
    SENSING_GPIO_PRECH_OPEN_MEC,                /*!< PRECH_OPEN_MEC_MCU */
    SENSING_GPIO_TS_LESS_THAN_60V,              /*!< TS_LESS_THAN_60V_MCU */
    SENSING_GPIO_PLAUSIBLE_STATE_PERSISTED,     /*!< PLAUSIBLE_STATE_PERSISTED_MCU */
    SENSING_GPIO_PLAUSIBLE_STATE,               /*!< PLAUSIBLE_STATE_MCU */
    SENSING_GPIO_NOT_BMS_FAULT_COCKPIT_LED,     /*!< NOT_BMS_FAULT_COCKPIT_LED_MCU */
    SENSING_GPIO_NOT_IMD_FAULT_COCKPIT_LED,     /*!< NOT_IMD_FAULT_COCKPIT_LED_MCU */
    SENSING_GPIO_INDICATOR_CONNECTED,           /*!< INDICATOR_CONNECTED_MCU */
    SENSING_GPIO_NOT_LATCH_RESET,               /*!< NOT_LATCH_RESET_MCU */
    SENSING_GPIO_NOT_IMPLAUSIBLE_STATE_LATCHED, /*!< NOT_IMPLAUSIBLE_STATE_LATCHED_MCU */
    SENSING_GPIO_NOT_BMS_FAULT_LATCHED,         /*!< NOT_BMS_FAULT_LATCHED_MCU */
    SENSING_GPIO_NOT_IMD_FAULT_LATCHED,         /*!< NOT_IMD_FAULT_LATCHED_MCU */
    SENSING_GPIO_NOT_EXT_FAULT_LATCHED,         /*!< NOT_EXT_FAULT_LATCHED_MCU */
    SENSING_GPIO_HC_CONNECTED,                  /*!< HC_CONNECTED_MCU */
    SENSING_GPIO_COUNT                          /*!< Total number of digital inputs */
};

/*!
 * \brief MCU ADCs used for the acquisition
 */
enum SensingAdc {
    SENSING_ADC_1,    /*!< ADC1 (hadc1) */
    SENSING_ADC_2,    /*!< ADC3 (hadc3) */
    SENSING_ADC_COUNT /*!< Total number of MCU ADCs */
};

/*!
 * \brief Samples of the first ADC, in DMA (rank) order
 *
 * \attention The order must match the ranks in the ADC1 configuration (.ioc)
 */
enum SensingAdc1Channel : uint8_t {
    SENSING_ADC_1_CHANNEL_SPARE,                /*!< Rank 1: IN15 in the current configuration (was IN0, ITS_LC_F), unused */
    SENSING_ADC_1_CHANNEL_SD_IN,                /*!< IN1: ShutDown voltage before the HV BAT Interlock entering the BMS */
    SENSING_ADC_1_CHANNEL_SD_END,               /*!< IN2: ShutDown voltage before the AIRs, end of the line */
    SENSING_ADC_1_CHANNEL_SD_OUT,               /*!< IN3: ShutDown after the IMD exiting the BMS */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_4, /*!< IN5: Fourth cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_5, /*!< IN6: Fifth cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_6, /*!< IN7: Sixth cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_1, /*!< IN8: First cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_2, /*!< IN9: Second cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_TSAL_GREEN,           /*!< IN10: TSAL current status */
    SENSING_ADC_1_CHANNEL_IMD_OK,               /*!< IN11: IMD status */
    SENSING_ADC_1_CHANNEL_ITS_HC_F,             /*!< IN12: First channel of an auxiliary current sensor (unused) */
    SENSING_ADC_1_CHANNEL_AUX_TEMP,             /*!< IN13: Auxiliary cooling temperature (unused) */
    SENSING_ADC_1_CHANNEL_OUTLET_LIQUID_TEMP_3, /*!< IN14: Third cooling temperature of the outlet liquid */
    SENSING_ADC_1_CHANNEL_INLET_LIQUID_TEMP,    /*!< IN15: Cooling temperature of the inlet liquid */
    SENSING_ADC_1_CHANNEL_COUNT                 /*!< Total number of ADC 1 samples */
};

/*!
 * \brief Samples of the second ADC, in DMA (rank) order
 *
 * \attention The order must match the ranks in the ADC3 configuration (.ioc)
 */
enum SensingAdc2Channel : uint8_t {
    SENSING_ADC_2_CHANNEL_AIRN_OPEN_MEC,      /*!< IN4: Mechanical status of the AIR- */
    SENSING_ADC_2_CHANNEL_V5_MCU,             /*!< IN5: 5V supply */
    SENSING_ADC_2_CHANNEL_PLAUSIBLE_STATE_RC, /*!< IN6: Plausible state voltage after the RC circuit */
    SENSING_ADC_2_CHANNEL_AMB_TEMP,           /*!< IN7: Ambient temperature (unused) */
    SENSING_ADC_2_CHANNEL_PROBING_3V3,        /*!< IN8: 3V3 supply voltage */
    SENSING_ADC_2_CHANNEL_SD_R_BMS_OUT,       /*!< IN9: Voltage after the AMS ShutDown node */
    SENSING_ADC_2_CHANNEL_SD_R_IMD_OUT,       /*!< IN14: Voltage after the IMD ShutDown node */
    SENSING_ADC_2_CHANNEL_AIRP_OPEN_MEC,      /*!< IN15: Mechanical status of the AIR+ */
    SENSING_ADC_2_CHANNEL_COUNT               /*!< Total number of ADC 2 samples */
};

/*! \brief Size of the sample buffer of every MCU ADC */
#define SENSING_ADC_MAX_CHANNEL_COUNT                                          \
    ((size_t)SENSING_ADC_1_CHANNEL_COUNT > (size_t)SENSING_ADC_2_CHANNEL_COUNT \
         ? (size_t)SENSING_ADC_1_CHANNEL_COUNT                                 \
         : (size_t)SENSING_ADC_2_CHANNEL_COUNT)

/*!
 * \brief Channels of the external ADC (MAX22530)
 */
enum SensingExtAdcChannel {
    SENSING_EXT_ADC_CHANNEL_TS_VOLTAGE = MAX22530_CHANNEL_1,                   /*!< Voltage of the Tractive System */
    SENSING_EXT_ADC_CHANNEL_PACK_VOLTAGE = MAX22530_CHANNEL_2,                 /*!< Voltage of the battery pack */
    SENSING_EXT_ADC_CHANNEL_LINEAR_REGULATOR_TEMPERATURE = MAX22530_CHANNEL_3, /*!< Temperature of the linear regulator mosfet */
    SENSING_EXT_ADC_CHANNEL_PRECHARGE_TEMPERATURE = MAX22530_CHANNEL_4,        /*!< Temperature of the precharge resistors heatsink */
    SENSING_EXT_ADC_CHANNEL_COUNT = MAX22530_CHANNEL_COUNT                     /*!< Total number of external ADC channels */
};

/*!
 * \brief Application module that receives the value of an MCU ADC sample
 */
enum SensingTarget : uint8_t {
    SENSING_TARGET_NONE = 0U,    /*!< The sample is not used */
    SENSING_TARGET_FEEDBACK,     /*!< Analog feedback (pin voltage) */
    SENSING_TARGET_COOLING_TEMP, /*!< Cooling temperature (NTC) */
};

/*!
 * \brief State of a conversion of an MCU ADC
 */
enum SensingAdcState : uint8_t {
    SENSING_ADC_STATE_IDLE = 0U,  /*!< No conversion running and no samples to process */
    SENSING_ADC_STATE_CONVERTING, /*!< A conversion was started, waiting for the complete notification */
    SENSING_ADC_STATE_CONVERTED,  /*!< The samples are ready to be processed */
};

/*!* ######################### CALLBACKS ################################# *!*/

/*!
 * \brief Callback that reads the logical level of a digital input
 *
 * \param gpio The digital input to read
 *
 * \returns true if the pin is high, false otherwise
 */
typedef bool (*sensing_gpio_read_callback)(enum SensingGpio gpio);

/*!
 * \brief Callback that starts a single scan conversion of an MCU ADC with DMA
 *
 * \details When the scan is complete \ref sensing_api_adc_conversion_complete
 *          must be called (e.g. from HAL_ADC_ConvCpltCallback)
 *
 * \param adc The ADC to start
 * \param buffer The buffer where the DMA has to store the samples (16 bit each)
 * \param count The number of samples to convert
 *
 * \returns true if the conversion was started, false otherwise
 */
typedef bool (*sensing_adc_start_callback)(enum SensingAdc adc, volatile raw_volt *buffer, size_t count);

/*!
 * \brief Callback that starts the PWM input capture of the IMD
 *
 * \details Every captured period must be notified with \ref sensing_api_imd_capture
 *          (e.g. from HAL_TIM_IC_CaptureCallback)
 */
typedef void (*sensing_imd_start_callback)(void);

/*!* ######################### STRUCTURES ################################ *!*/

/*!
 * \brief Hardware callbacks needed by the sensing module
 */
struct SensingInitData {
    sensing_gpio_read_callback gpio_read;       /*!< Read a digital input */
    sensing_adc_start_callback adc_start;       /*!< Start a conversion of an MCU ADC */
    sensing_imd_start_callback imd_start;       /*!< Start the IMD PWM input capture */
    spi_send_callback spi_send;                 /*!< SPI send, used by the external ADC */
    spi_send_receive_callback spi_send_receive; /*!< SPI send and receive, used by the external ADC */
};

/*!
 * \brief Destination of an MCU ADC sample
 *
 * \details The index is a FeedbackAnalogIndex or an InternalTemperaturesIndex
 *          depending on the target
 */
struct SensingAdcRoute {
    enum SensingTarget target; /*!< The module that receives the value */
    uint8_t index;             /*!< The index of the value inside the target module */
};

/*!
 * \brief Internal handler of the sensing module
 *
 * \warning This structure should never be used outside of this module
 */
struct SensingHandler {
    sensing_gpio_read_callback gpio_read; /*!< Read a digital input */
    sensing_adc_start_callback adc_start; /*!< Start a conversion of an MCU ADC */

    struct Max22530Handler max22530;   /*!< External ADC (TS and pack voltages, internal temperatures) */
    struct Ir1553204Handler ir1553204; /*!< IMD PWM decoding */

    volatile raw_volt adc_samples[SENSING_ADC_COUNT][SENSING_ADC_MAX_CHANNEL_COUNT]; /*!< DMA buffers of the MCU ADCs */
    volatile enum SensingAdcState adc_state[SENSING_ADC_COUNT];                      /*!< Conversion state of the MCU ADCs */

    struct Watchdog internal_adc_watchdogs[SENSING_ADC_COUNT]; /*!< One per MCU ADC, petted when its samples are processed */
    struct Watchdog external_adc_watchdog;                     /*!< Petted at every successful read of the external ADC */
    struct Watchdog imd_capture_watchdog;                      /*!< Started by the first valid IMD capture, then petted by the routine at every capture */
    volatile bool imd_captured;                                /*!< Set by the IMD capture interrupt, consumed by the routine */
    bool watchdogs_started;                                    /*!< True once the routine has started the watchdogs */

    bool is_initialized; /*!< True once the module has been initialized */
};

#endif // SENSING_H
