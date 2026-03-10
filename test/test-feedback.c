#include "unity.h"
#include "feedback.h"
#include "mainboard-def.h"
#include <string.h>

extern _FeedbackHandler hfeedback;
extern uint32_t debug_cnt;

/* Internal helpers under test (not static in feedback.c) */
extern FeedbackId _feedback_get_id_from_digital_bit(const FeedbackDigitalBit bit);
extern FeedbackId _feedback_get_id_from_analog_index(const FeedbackAnalogIndex index);
extern FeedbackStatus _feedback_get_analog_status(const FeedbackAnalogIndex index);

/* ---------- Mocks ---------- */

static bit_flag32_t g_fake_digital_value = 0U;
static uint32_t g_start_conversion_called = 0U;

static bit_flag32_t _feedback_read_digital_all_mock(void) {
    return g_fake_digital_value;
}

static void _feedback_start_analog_conversion_fake(void) {
    ++g_start_conversion_called;
}

/* ---------- Tests ---------- */

void test_feedback_init_null_read_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_NULL_POINTER,
        feedback_init(NULL, _feedback_start_analog_conversion_fake),
        "feedback_init() should fail with NULL read callback");
}

void test_feedback_init_null_start_conversion_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_NULL_POINTER,
        feedback_init(_feedback_read_digital_all_mock, NULL),
        "feedback_init() should fail with NULL start_conversion callback");
}

void test_feedback_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_OK,
        feedback_init(_feedback_read_digital_all_mock, _feedback_start_analog_conversion_fake),
        "feedback_init() failed to return FEEDBACK_OK");
}

void test_feedback_update_digital_feedback_all_ok(void) {
    g_fake_digital_value = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_OK, feedback_update_digital_feedback_all(), "Update digital failed");
    TEST_ASSERT_TRUE_MESSAGE(feedback_get_digital(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM), "Digital bit was not updated");
}

void test_feedback_start_analog_conversion_all_ok(void) {
    g_start_conversion_called = 0U;

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_OK, feedback_start_analog_conversion_all(), "Start conversion failed");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, g_start_conversion_called, "Start conversion callback should be called once");
}

void test_feedback_update_analog_feedback_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_OK,
        feedback_update_analog_feedback(FEEDBACK_ANALOG_INDEX_IMD_OK, 2.5f),
        "Valid analog index should return FEEDBACK_OK");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 2.5f, feedback_get_analog(FEEDBACK_ANALOG_INDEX_IMD_OK), "Analog value mismatch");
}

void test_feedback_update_analog_feedback_invalid_index(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_INVALID_INDEX,
        feedback_update_analog_feedback((FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT, 1.0f),
        "Invalid analog index should return FEEDBACK_INVALID_INDEX");
}

void test_feedback_get_digital_invalid_bit(void) {
    TEST_ASSERT_FALSE_MESSAGE(
        feedback_get_digital((FeedbackDigitalBit)FEEDBACK_DIGITAL_BIT_COUNT),
        "Invalid digital bit should return false");
}

void test_feedback_get_analog_invalid_index(void) {
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(
        0.0001f,
        0.0f,
        feedback_get_analog((FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT),
        "Invalid analog index should return 0");
}

void test_feedback_get_status_invalid_id(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_STATUS_ERROR,
        feedback_get_status((FeedbackId)FEEDBACK_ID_COUNT),
        "Invalid feedback id should return FEEDBACK_STATUS_ERROR");
}

void test_feedback_get_id_from_digital_bit_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_ID_AIRN_OPEN_COM,
        _feedback_get_id_from_digital_bit(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM),
        "Digital bit to ID mapping mismatch");
}

void test_feedback_get_id_from_analog_index_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_ID_IMD_OK,
        _feedback_get_id_from_analog_index(FEEDBACK_ANALOG_INDEX_IMD_OK),
        "Analog index to ID mapping mismatch");
}

void test_feedback_get_analog_status_probing_3v3_in_range(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5f;
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_STATUS_HIGH,
        _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3),
        "3V3 probing in-range should be HIGH");
}

void test_feedback_get_analog_status_probing_3v3_out_of_range(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_HIGH_V + 1.0f;
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_STATUS_ERROR,
        _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3),
        "3V3 probing out-of-range should be ERROR");
}

void test_feedback_get_analog_status_generic_low_high_error(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_LOW_V - 0.1f;
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN), "TSAL_GREEN should be LOW");

    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.1f;
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN), "TSAL_GREEN should be HIGH");

    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5f;
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN), "TSAL_GREEN should be ERROR (indecisive state)");
}

void test_feedback_get_analog_status_imd_special_low_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_IMD_OK] = 1.5f;
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_STATUS_LOW,
        _feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_IMD_OK),
        "IMD_OK should use special low threshold (1.6V)");
}

void test_feedback_update_status_digital_and_analog(void) {
    g_fake_digital_value = 0U;
    g_fake_digital_value |= (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    g_fake_digital_value |= (1UL << FEEDBACK_DIGITAL_BIT_SD_BMS_FB);
    (void)feedback_update_digital_feedback_all();

    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.2f; /* HIGH */
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = FEEDBACK_THRESHOLD_LOW_V - 0.2f;      /* LOW */

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_OK, feedback_update_status(), "feedback_update_status failed");

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_get_status(FEEDBACK_ID_AIRN_OPEN_COM), "AIRN_OPEN_COM should be HIGH");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_get_status(FEEDBACK_ID_SD_BMS_FB), "SD_BMS_FB should be HIGH");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, feedback_get_status(FEEDBACK_ID_AIRP_OPEN_COM), "AIRP_OPEN_COM should be LOW");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_get_status(FEEDBACK_ID_TSAL_GREEN), "TSAL_GREEN should be HIGH");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, feedback_get_status(FEEDBACK_ID_SD_OUT), "SD_OUT should be LOW");
}

void test_feedback_update_status_air_noise_branch_sets_low_and_increments_debug(void) {
    debug_cnt = 0U;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.8f; /* special thr_low=1.6, if thr_high < 1.8 => ERROR path */

    (void)feedback_update_status();

    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_STATUS_LOW,
        feedback_get_status(FEEDBACK_ID_AIRN_OPEN_MEC),
        "AIRN_OPEN_MEC ERROR branch should fallback to LOW");
}

void test_feedback_check_values_ok(void) {
    FeedbackId out = FEEDBACK_ID_AIRN_OPEN_COM;

    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    hfeedback.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;

    const bit_flag32_t mask =
        (1UL << FEEDBACK_ID_AIRN_OPEN_COM) |
        (1UL << FEEDBACK_ID_SD_IMD_FB);

    const bit_flag32_t value =
        (0UL << FEEDBACK_ID_AIRN_OPEN_COM) |
        (1UL << FEEDBACK_ID_SD_IMD_FB);

    TEST_ASSERT_TRUE_MESSAGE(feedback_check_values(mask, value, &out), "Expected feedback_check_values to pass");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_UNKNOWN, out, "out should be FEEDBACK_ID_UNKNOWN on success");
}

void test_feedback_check_values_mismatch_sets_out(void) {
    FeedbackId out = FEEDBACK_ID_UNKNOWN;

    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;

    const bit_flag32_t mask = (1UL << FEEDBACK_ID_AIRN_OPEN_COM);
    const bit_flag32_t value = (1UL << FEEDBACK_ID_AIRN_OPEN_COM); /* expected HIGH */

    TEST_ASSERT_FALSE_MESSAGE(feedback_check_values(mask, value, &out), "Expected mismatch");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, out, "Mismatch id not reported");
}

void test_feedback_is_digital_true_false(void) {
    TEST_ASSERT_TRUE_MESSAGE(feedback_is_digital(FEEDBACK_ID_AIRN_OPEN_COM), "AIRN_OPEN_COM should be digital");
    TEST_ASSERT_FALSE_MESSAGE(feedback_is_digital(FEEDBACK_ID_IMD_OK), "IMD_OK should not be digital");
}

void test_feedback_get_digital_bit_from_id(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM,
        feedback_get_digital_bit_from_id(FEEDBACK_ID_AIRN_OPEN_COM),
        "Expected digital bit for AIRN_OPEN_COM");
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_DIGITAL_BIT_UNKNOWN,
        feedback_get_digital_bit_from_id(FEEDBACK_ID_IMD_OK),
        "Expected unknown digital bit for IMD_OK");
}

void test_feedback_get_analog_index_from_id(void) {
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_ANALOG_INDEX_IMD_OK,
        feedback_get_analog_index_from_id(FEEDBACK_ID_IMD_OK),
        "Expected analog index for IMD_OK");
    TEST_ASSERT_EQUAL_MESSAGE(
        FEEDBACK_ANALOG_INDEX_UNKNOWN,
        feedback_get_analog_index_from_id(FEEDBACK_ID_AIRN_OPEN_COM),
        "Expected unknown analog index for AIRN_OPEN_COM");
}

void test_feedback_get_status_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;

    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    hfeedback.status[FEEDBACK_ID_SD_BMS_FB] = FEEDBACK_STATUS_LOW;

    primary_hv_feedback_status_converted_t *payload = feedback_get_status_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.status_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.status_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_EQUAL_MESSAGE((int)FEEDBACK_STATUS_HIGH, (int)payload->airn_open_com, "AIRN_OPEN_COM status mismatch");
    TEST_ASSERT_EQUAL_MESSAGE((int)FEEDBACK_STATUS_LOW, (int)payload->sd_bms_fb, "SD_BMS_FB status mismatch");
}

void test_feedback_get_digital_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;
    hfeedback.digital = 0U;
    hfeedback.digital |= (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);
    hfeedback.digital |= (1UL << FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);

    primary_hv_feedback_digital_converted_t *payload = feedback_get_digital_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.digital_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.digital_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_TRUE_MESSAGE(payload->digital_airn_open_com, "AIRN_OPEN_COM digital bit should be set");
    TEST_ASSERT_TRUE_MESSAGE(payload->digital_indicator_connected, "INDICATOR_CONNECTED digital bit should be set");
    TEST_ASSERT_FALSE_MESSAGE(payload->digital_airp_open_com, "AIRP_OPEN_COM digital bit should not be set");
}

void test_feedback_get_analog_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.23f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_V5_MCU] = 2.50f;

    primary_hv_feedback_analog_converted_t *payload = feedback_get_analog_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.analog_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.analog_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 1.23f, payload->analog_airn_open_mec, "AIRN_OPEN_MEC analog value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, FEEDBACK_VOLTAGE_TO_5V_VOLT(2.50f), payload->analog_v5_mcu, "V5_MCU analog value mismatch");
}

void test_feedback_get_analog_sd_payload_pointer_size_and_content(void) {
    size_t byte_size = 0U;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = 1.0f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = 2.0f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_END] = 3.0f;

    primary_hv_feedback_analog_sd_converted_t *payload = feedback_get_analog_sd_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.analog_sd_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.analog_sd_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, FEEDBACK_VOLTAGE_TO_SD_VOLT(1.0f), payload->sd_out, "SD_OUT analog value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0f), payload->sd_in, "SD_IN analog value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0f), payload->sd_end, "SD_END analog value mismatch");
}

void test_feedback_get_enzomma_payload_digital(void) {
    size_t byte_size = 0U;
    const FeedbackId id = FEEDBACK_ID_AIRN_OPEN_COM;
    hfeedback.status[id] = FEEDBACK_STATUS_HIGH;
    hfeedback.digital = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    primary_hv_feedback_enzomma_converted_t *payload = feedback_get_enzomma_payload(id, &byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.enzomma_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.enzomma_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_TRUE_MESSAGE(payload->is_digital, "Expected digital feedback");
    TEST_ASSERT_TRUE_MESSAGE(payload->digital, "Expected digital value");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.0f, payload->analog, "Analog value mismatch for digital feedback");
}

void test_feedback_get_enzomma_payload_analog(void) {
    size_t byte_size = 0U;
    const FeedbackId id = FEEDBACK_ID_IMD_OK;
    hfeedback.status[id] = FEEDBACK_STATUS_LOW;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_IMD_OK] = 2.22f;

    primary_hv_feedback_enzomma_converted_t *payload = feedback_get_enzomma_payload(id, &byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.enzomma_can_payload, payload, "Payload pointer mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)sizeof(hfeedback.enzomma_can_payload), (uint32_t)byte_size, "Payload size mismatch");
    TEST_ASSERT_FALSE_MESSAGE(payload->is_digital, "Expected analog feedback");
    TEST_ASSERT_FALSE_MESSAGE(payload->digital, "Expected analog value");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 2.22f, payload->analog, "Analog value mismatch for analog feedback");
}

#ifdef FEEDBACK_TESTS

void setUp(void) {
    g_fake_digital_value = 0U;
    g_start_conversion_called = 0U;
    debug_cnt = 0U;
    (void)feedback_init(_feedback_read_digital_all_mock, _feedback_start_analog_conversion_fake);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_feedback_init_null_read_callback);
    RUN_TEST(test_feedback_init_null_start_conversion_callback);
    RUN_TEST(test_feedback_init_ok);
    RUN_TEST(test_feedback_update_digital_feedback_all_ok);
    RUN_TEST(test_feedback_start_analog_conversion_all_ok);
    RUN_TEST(test_feedback_update_analog_feedback_ok);
    RUN_TEST(test_feedback_update_analog_feedback_invalid_index);
    RUN_TEST(test_feedback_get_digital_invalid_bit);
    RUN_TEST(test_feedback_get_analog_invalid_index);
    RUN_TEST(test_feedback_get_status_invalid_id);
    RUN_TEST(test_feedback_get_id_from_digital_bit_ok);
    RUN_TEST(test_feedback_get_id_from_analog_index_ok);
    RUN_TEST(test_feedback_get_analog_status_probing_3v3_in_range);
    RUN_TEST(test_feedback_get_analog_status_probing_3v3_out_of_range);
    RUN_TEST(test_feedback_get_analog_status_generic_low_high_error);
    RUN_TEST(test_feedback_get_analog_status_imd_special_low_threshold);
    RUN_TEST(test_feedback_update_status_digital_and_analog);
    RUN_TEST(test_feedback_update_status_air_noise_branch_sets_low_and_increments_debug);
    RUN_TEST(test_feedback_check_values_ok);
    RUN_TEST(test_feedback_check_values_mismatch_sets_out);
    RUN_TEST(test_feedback_is_digital_true_false);
    RUN_TEST(test_feedback_get_digital_bit_from_id);
    RUN_TEST(test_feedback_get_analog_index_from_id);
    RUN_TEST(test_feedback_get_status_payload_pointer_size_and_content);
    RUN_TEST(test_feedback_get_digital_payload_pointer_size_and_content);
    RUN_TEST(test_feedback_get_analog_payload_pointer_size_and_content);
    RUN_TEST(test_feedback_get_analog_sd_payload_pointer_size_and_content);
    RUN_TEST(test_feedback_get_enzomma_payload_digital);
    RUN_TEST(test_feedback_get_enzomma_payload_analog);
    return UNITY_END();
}

#endif // FEEDBACK_TESTS