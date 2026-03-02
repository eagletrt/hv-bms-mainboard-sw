#include "unity.h"
#include "internal-voltage.h"
#include "mainboard-def.h"
#include "volt.h"

#include <string.h>

extern _InternalVoltageHandler hvolt_int;

static raw_volt_t g_raw_channels[MAX22530_CHANNEL_COUNT + 1U];

static void _mock_spi_send(
    const SpiNetwork network,
    uint8_t * const data,
    const size_t size)
{
    (void)network;
    (void)data;
    (void)size;
}

static void _mock_spi_send_receive(
    const SpiNetwork network,
    uint8_t * const data,
    uint8_t * const out,
    const size_t size,
    const size_t out_size)
{
    (void)network;
    (void)data;
    (void)size;
    (void)out_size;

    for (size_t i = 0U; i < MAX22530_CHANNEL_COUNT + 1U; ++i) {
        out[i * 2U] = (g_raw_channels[i] >> 8U) & 0x0FU;
        out[i * 2U + 1U] = g_raw_channels[i] & 0xFFU;
    }
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

void test_internal_voltage_read_all_updates_ts_and_pack() {
    g_raw_channels[MAX22530_CHANNEL_1] = 2000U;
    g_raw_channels[MAX22530_CHANNEL_2] = 3000U;

    (void)internal_voltage_read_all();

    const volt_t expected_ts = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(
        MAX22530_RAW_VALUE_TO_VOLT(g_raw_channels[MAX22530_CHANNEL_1]));
    const volt_t expected_pack = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(
        MAX22530_RAW_VALUE_TO_VOLT(g_raw_channels[MAX22530_CHANNEL_2]));

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
        0.001f,
        expected_ts,
        internal_voltage_get_ts(),
        "TS voltage was not converted correctly from ADC value");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
        0.001f,
        expected_pack,
        internal_voltage_get_pack(),
        "Pack voltage was not converted correctly from ADC value");
}

void test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size() {
    size_t byte_size = 0U;
    primary_hv_ts_voltage_converted_t * payload = internal_voltage_get_ts_voltage_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(
        &hvolt_int.ts_voltage_can_payload,
        payload,
        "Returned payload pointer does not match internal handler");
    TEST_ASSERT_EQUAL_MESSAGE(
        sizeof(hvolt_int.ts_voltage_can_payload),
        byte_size,
        "Returned payload size mismatch");
}

void test_internal_voltage_get_ts_voltage_canlib_payload_content() {
    size_t byte_size = 0U;
    g_raw_channels[MAX22530_CHANNEL_1] = 1200U;
    g_raw_channels[MAX22530_CHANNEL_2] = 1600U;

    (void)internal_voltage_read_all();
    primary_hv_ts_voltage_converted_t * payload = internal_voltage_get_ts_voltage_canlib_payload(&byte_size);

    const volt_t expected_ts = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(
        MAX22530_RAW_VALUE_TO_VOLT(g_raw_channels[MAX22530_CHANNEL_1]));
    const volt_t expected_pack = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(
        MAX22530_RAW_VALUE_TO_VOLT(g_raw_channels[MAX22530_CHANNEL_2]));

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected_ts, payload->ts, "Payload ts voltage mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, expected_pack, payload->pack, "Payload pack voltage mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001f, volt_get_sum(), payload->cells_sum, "Payload cells_sum mismatch");
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
    RUN_TEST(test_internal_voltage_read_all_updates_ts_and_pack);
    RUN_TEST(test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size);
    RUN_TEST(test_internal_voltage_get_ts_voltage_canlib_payload_content);
    return UNITY_END();
}

#endif // INTERNAL_VOLTAGE_TESTS
