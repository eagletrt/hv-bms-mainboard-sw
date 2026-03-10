#include "unity.h"
#include "internal-voltage.h"
#include "mainboard-def.h"
#include "volt.h"

#include <string.h>

extern _InternalVoltageHandler hvolt_int;

static raw_volt_t g_raw_channels[MAX22530_CHANNEL_COUNT + 1U];

static void _mock_spi_send(
    const SpiNetwork network,
    uint8_t *const data,
    const size_t size) {
    (void)network;
    (void)data;
    (void)size;
}

static void _mock_spi_send_receive(
    const SpiNetwork network,
    uint8_t *const data,
    uint8_t *const out,
    const size_t size,
    const size_t out_size) {
    (void)network;
    (void)data;
    (void)size;
    (void)out_size;
}

void test_internal_voltage_init_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(
        INTERNAL_VOLTAGE_OK,
        internal_voltage_init(_mock_spi_send, _mock_spi_send_receive),
        "internal_voltage_init() failed to return INTERNAL_VOLTAGE_OK");
}

void test_internal_voltage_init_null_pointer() {
    TEST_ASSERT_EQUAL_MESSAGE(
        INTERNAL_VOLTAGE_NULL_POINTER,
        internal_voltage_init(NULL, _mock_spi_send_receive),
        "internal_voltage_init() should return INTERNAL_VOLTAGE_NULL_POINTER if send callback is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(
        INTERNAL_VOLTAGE_NULL_POINTER,
        internal_voltage_init(_mock_spi_send, NULL),
        "internal_voltage_init() should return INTERNAL_VOLTAGE_NULL_POINTER if send_receive callback is NULL");
}

void test_internal_voltage_read_all_ok() {
    TEST_ASSERT_EQUAL_MESSAGE(
        INTERNAL_VOLTAGE_OK,
        internal_voltage_read_all(),
        "internal_voltage_read_all() failed to return INTERNAL_VOLTAGE_OK");
}

void test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size() {
    size_t byte_size = 0U;
    primary_hv_ts_voltage_converted_t *payload = internal_voltage_get_ts_voltage_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(
        &hvolt_int.ts_voltage_can_payload,
        payload,
        "Returned payload pointer does not match internal handler");
    TEST_ASSERT_EQUAL_MESSAGE(
        sizeof(hvolt_int.ts_voltage_can_payload),
        byte_size,
        "Returned payload size mismatch");
}

#ifdef INTERNAL_VOLTAGE_TESTS

void setUp() {
    memset(g_raw_channels, 0U, sizeof(g_raw_channels));
    volt_init();
    internal_voltage_init(_mock_spi_send, _mock_spi_send_receive);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_internal_voltage_init_ok);
    RUN_TEST(test_internal_voltage_init_null_pointer);
    RUN_TEST(test_internal_voltage_read_all_ok);
    RUN_TEST(test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size);
    return UNITY_END();
}

#endif // INTERNAL_VOLTAGE_TESTS
