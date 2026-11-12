/*!
 * \file sensors-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Sensors handling (SD, IMD, AIRs, precharge, ...)
 */

#include "sensors-api.h"

#include <stdint.h>
#include <string.h>

#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-status-api.h"
#include "sensors.h"

EAGLETRT_STATIC struct SensorsHandler sensors_handler;

EAGLETRT_STATIC struct SensorsAnalogTreshold standard_thr = {
    .high_thr = SENSORS_THRESHOLD_HIGH_V,
    .low_thr = SENSORS_THRESHOLD_LOW_V,
    .contained = false
};

EAGLETRT_STATIC struct SensorsAnalogTreshold probing_3v3 = {
    .high_thr = SENSORS_THRESHOLD_LOW_V,
    .low_thr = SENSORS_THRESHOLD_HIGH_V,
    .contained = true
};

// TODO(TEST): see if when connected to handcart the high threshold needs to be lowered to 1V like in the old code.

bool prv_sensors_digital_majority_filter(const volatile bool *buffer) {
    uint8_t count = 0;
    for (size_t i = 0; i < SENSORS_BUFFER_SIZE; i++) {
        count += (uint8_t)buffer[i];
    }

    return count > (SENSORS_BUFFER_SIZE / 2);
}

void sensors_api_init(void) {
    memset(&sensors_handler, 0, sizeof(sensors_handler));
}

enum SensorsReturnCode sensors_api_set_digital_value(enum SensorsDigitalIndex index, bool value) {

    if (index > SENSORS_DIGITAL_INDEX_COUNT) {
        return SENSORS_RC_INVALID_INDEX;
    }

    struct SensorsDigitalValue *sensor = &sensors_handler.digital[index];

    if (sensor->head >= SENSORS_BUFFER_SIZE) {
        return SENSORS_RC_INVALID_INDEX;
    }

    sensor->buffer[sensor->head] = value;

    sensor->head = (sensor->head + 1) % SENSORS_BUFFER_SIZE;

    sensor->ready = (bool)(sensor->ready || sensor->head == 0);

    if (sensor->ready) {
        sensor->filtered = prv_sensors_digital_majority_filter(sensor->buffer);
    }

    return SENSORS_RC_OK;
}

enum SensorsReturnCode sensors_api_set_analog_value(enum SensorsAnalogIndex index, volt value) {

    if (index > SENSORS_ANALOG_INDEX_COUNT) {
        return SENSORS_RC_INVALID_INDEX;
    }

    struct SensorsAnalogValue *sensor = &sensors_handler.analog[index];

    if (sensor->head >= SENSORS_BUFFER_SIZE) {
        return SENSORS_RC_INVALID_INDEX;
    }

    sensor->buffer[sensor->head] = value;

    sensor->head = (sensor->head + 1) % SENSORS_BUFFER_SIZE;

    sensor->ready = (bool)(sensor->ready || sensor->head == 0);

    if (sensor->ready) {
        sensor->filtered = prv_sensors_analog_majority_filter(sensor->buffer);
    }

    return SENSORS_RC_OK;
}