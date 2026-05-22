/*!
 * \file test-feedback.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */
#include "unity.h"
#include "feedback-api.h"
#include "mainboard-def.h"
#include <string.h>

#include <fff.h>

DEFINE_FFF_GLOBALS;

extern struct FeedbackHandler hfeedback;

extern enum FeedbackId prv_feedback_get_id_from_digital_bit(enum FeedbackDigitalBit bit);
extern enum FeedbackId prv_feedback_get_id_from_analog_index(enum FeedbackAnalogIndex index);
extern enum FeedbackStatus prv_feedback_get_analog_status(enum FeedbackAnalogIndex index);

FAKE_VALUE_FUNC(uint32_t, feedback_read_digital_all);
FAKE_VOID_FUNC(feedback_start_analog_conversion);

// Feedback init
void test_feedback_init_returns_null_pointer_when_read_callback_is_null(void) {
    enum FeedbackReturnCode result = feedback_init(NULL, feedback_start_analog_conversion);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when read callback is NULL");
}

void test_feedback_init_returns_null_pointer_when_start_conversion_callback_is_null(void) {
    enum FeedbackReturnCode result = feedback_init(feedback_read_digital_all, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when start_conversion callback is NULL");
}

void test_feedback_init_saves_callbacks_and_returns_ok(void) {
    enum FeedbackReturnCode result = feedback_init(feedback_read_digital_all, feedback_start_analog_conversion);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code after successful init");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_read_digital_all, hfeedback.read_digital, "read_digital callback not stored correctly");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_start_analog_conversion, hfeedback.start_conversion, "start_conversion callback not stored correctly");
}

// Feedback update digital tests
void test_feedback_update_digital_stores_read_result(void) {
    feedback_read_digital_all_fake.return_val = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    enum FeedbackReturnCode result = feedback_update_digital_feedback_all();

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code from digital update");
    TEST_ASSERT_TRUE_MESSAGE(hfeedback.digital & (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM), "AIRN_OPEN_COM bit not set in digital field");
    TEST_ASSERT_EQUAL_MESSAGE(1U, feedback_read_digital_all_fake.call_count, "Expected exactly one call to feedback_read_digital_all");
}

// Feedback update analog tests
void test_feedback_update_analog_feedback_stores_value(void) {
    enum FeedbackReturnCode result = feedback_update_analog_feedback(FEEDBACK_ANALOG_INDEX_IMD_OK, 2.5f);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code from analog update");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 2.5f, hfeedback.analog[FEEDBACK_ANALOG_INDEX_IMD_OK], "Analog value for IMD_OK not stored correctly");
}

void test_feedback_update_analog_feedback_returns_invalid_index_when_out_of_range(void) {
    enum FeedbackReturnCode result = feedback_update_analog_feedback((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT, 1.0f);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_INVALID_INDEX, result, "Expected INVALID_INDEX for out-of-range analog index");
}

// Private getters tests
void test_feedback_get_id_from_digital_bit_returns_correct_id(void) {
    enum FeedbackId result = prv_feedback_get_id_from_digital_bit(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, result, "Expected AIRN_OPEN_COM id for AIRN_OPEN_COM digital bit");
}

void test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit(void) {
    enum FeedbackId result = prv_feedback_get_id_from_digital_bit((enum FeedbackDigitalBit)FEEDBACK_DIGITAL_BIT_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 for unknown digital bit");
}

void test_feedback_get_id_from_analog_index_returns_correct_id(void) {
    enum FeedbackId result = prv_feedback_get_id_from_analog_index(FEEDBACK_ANALOG_INDEX_IMD_OK);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_IMD_OK, result, "Expected IMD_OK id for IMD_OK analog index");
}

void test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index(void) {
    enum FeedbackId result = prv_feedback_get_id_from_analog_index((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(-1, result, "Expected -1 for unknown analog index");
}

// Analog status tests
void test_feedback_get_analog_status_probing_3v3_returns_high_when_in_range(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status for PROBING_3V3 value within valid range");
}

void test_feedback_get_analog_status_probing_3v3_returns_error_when_above_high_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_HIGH_V + 0.1f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value above high threshold");
}

void test_feedback_get_analog_status_probing_3v3_returns_error_when_below_low_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_LOW_V - 0.1f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value below low threshold");
}

void test_feedback_get_analog_status_returns_low_when_below_low_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_LOW_V - 0.1f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, result, "Expected LOW status when analog value is below low threshold");
}

void test_feedback_get_analog_status_returns_high_when_above_high_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.1f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status when analog value is above high threshold");
}

void test_feedback_get_analog_status_returns_error_when_between_thresholds(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5f;

    enum FeedbackStatus result = prv_feedback_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status when analog value is in the forbidden mid-range");
}

// Feedback update status tests
void test_feedback_update_status_sets_high_for_set_digital_bits(void) {
    hfeedback.digital = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM) |
                        (1UL << FEEDBACK_DIGITAL_BIT_SD_BMS_FB);

    (void)feedback_update_status();

    enum FeedbackStatus expected[FEEDBACK_ID_COUNT];
    memset(expected, FEEDBACK_STATUS_LOW, sizeof(expected));
    expected[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    expected[FEEDBACK_ID_SD_BMS_FB] = FEEDBACK_STATUS_HIGH;

    TEST_ASSERT_EQUAL_MESSAGE(expected[FEEDBACK_ID_AIRN_OPEN_COM], hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM], "AIRN_OPEN_COM status should be HIGH when its digital bit is set");
    TEST_ASSERT_EQUAL_MESSAGE(expected[FEEDBACK_ID_SD_BMS_FB], hfeedback.status[FEEDBACK_ID_SD_BMS_FB], "SD_BMS_FB status should be HIGH when its digital bit is set");
}

void test_feedback_update_status_sets_low_for_cleared_digital_bits(void) {
    hfeedback.digital = 0U;
    (void)feedback_update_status();

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM], "AIRN_OPEN_COM status should be LOW when its digital bit is cleared");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, hfeedback.status[FEEDBACK_ID_AIRP_OPEN_COM], "AIRP_OPEN_COM status should be LOW when its digital bit is cleared");
}

void test_feedback_update_status_sets_high_for_analog_above_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.2f;

    (void)feedback_update_status();

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, hfeedback.status[FEEDBACK_ID_TSAL_GREEN], "TSAL_GREEN status should be HIGH when analog value exceeds high threshold");
}

void test_feedback_update_status_sets_low_for_analog_below_threshold(void) {
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = FEEDBACK_THRESHOLD_LOW_V - 0.2f;

    (void)feedback_update_status();

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, hfeedback.status[FEEDBACK_ID_SD_OUT], "SD_OUT status should be LOW when analog value is below low threshold");
}

// Feedback check values tests
void test_feedback_check_values_returns_true_and_sets_unknown_when_all_match(void) {
    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    hfeedback.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;
    const bit_flag32_t mask = (1UL << FEEDBACK_ID_AIRN_OPEN_COM) | (1UL << FEEDBACK_ID_SD_IMD_FB);
    const bit_flag32_t value = (0UL << FEEDBACK_ID_AIRN_OPEN_COM) | (1UL << FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_AIRN_OPEN_COM;

    bool result = feedback_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "Expected true when all masked feedback values match");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_UNKNOWN, out, "Expected out to be set to UNKNOWN when all values match");
}

void test_feedback_check_values_returns_false_and_sets_failing_id_on_mismatch(void) {
    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    const bit_flag32_t mask = (1UL << FEEDBACK_ID_AIRN_OPEN_COM);
    const bit_flag32_t value = (1UL << FEEDBACK_ID_AIRN_OPEN_COM); /* expects HIGH, is LOW */
    enum FeedbackId out = FEEDBACK_ID_UNKNOWN;

    bool result = feedback_check_values(mask, value, &out);

    TEST_ASSERT_FALSE_MESSAGE(result, "Expected false when a masked feedback value does not match");
    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, out, "Expected out to be set to the failing feedback id");
}

void test_feedback_check_values_skips_feedbacks_not_in_mask(void) {
    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH; /* would fail if checked */
    hfeedback.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;
    const bit_flag32_t mask = (1UL << FEEDBACK_ID_SD_IMD_FB); /* AIRN_OPEN_COM not in mask */
    const bit_flag32_t value = (1UL << FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_UNKNOWN;

    bool result = feedback_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "Expected true when only masked feedbacks are checked and they match");
}

void test_feedback_check_values_accepts_null_out_pointer(void) {
    hfeedback.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_LOW;
    const bit_flag32_t mask = (1UL << FEEDBACK_ID_SD_IMD_FB);
    const bit_flag32_t value = (1UL << FEEDBACK_ID_SD_IMD_FB); /* expects HIGH, is LOW */

    bool result = feedback_check_values(mask, value, NULL);

    TEST_ASSERT_FALSE_MESSAGE(result, "Expected false on mismatch even when out pointer is NULL");
}

// Test getter invalid
void test_feedback_get_digital_bit_from_id_returns_unknown_for_invalid_id(void) {
    enum FeedbackDigitalBit result = feedback_get_digital_bit_from_id((enum FeedbackId)FEEDBACK_ID_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_DIGITAL_BIT_UNKNOWN, result, "Expected UNKNOWN digital bit for out-of-range feedback id");
}

void test_feedback_get_analog_index_from_id_returns_unknown_for_invalid_id(void) {
    enum FeedbackAnalogIndex result = feedback_get_analog_index_from_id((enum FeedbackId)FEEDBACK_ID_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ANALOG_INDEX_UNKNOWN, result, "Expected UNKNOWN analog index for out-of-range feedback id");
}

// Payload tests
void test_feedback_get_status_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;
    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    hfeedback.status[FEEDBACK_ID_SD_BMS_FB] = FEEDBACK_STATUS_LOW;

    primary_hv_feedback_status_converted_t *payload = feedback_get_status_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.status_can_payload, payload, "Status payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(hfeedback.status_can_payload), (uint32_t)byte_size, "Status payload byte size mismatch");
    TEST_ASSERT_EQUAL_MESSAGE((int)FEEDBACK_STATUS_HIGH, (int)payload->airn_open_com, "airn_open_com field should reflect HIGH status");
    TEST_ASSERT_EQUAL_MESSAGE((int)FEEDBACK_STATUS_LOW, (int)payload->sd_bms_fb, "sd_bms_fb field should reflect LOW status");
}

void test_feedback_get_digital_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;
    hfeedback.digital = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM) |
                        (1UL << FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);
    primary_hv_feedback_digital_converted_t expected = { 0 };
    expected.digital_airn_open_com = 1U;
    expected.digital_indicator_connected = 1U;

    primary_hv_feedback_digital_converted_t *payload = feedback_get_digital_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.digital_can_payload, payload, "Digital payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(hfeedback.digital_can_payload), (uint32_t)byte_size, "Digital payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Digital payload content does not match expected bit fields");
}

void test_feedback_get_analog_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    hfeedback.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.23f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_V5_MCU] = 2.50f;

    primary_hv_feedback_analog_converted_t *payload = feedback_get_analog_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.analog_can_payload, payload, "Analog payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(hfeedback.analog_can_payload), (uint32_t)byte_size, "Analog payload byte size mismatch");

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 1.23f, payload->analog_airn_open_mec, "analog_airn_open_mec value mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, FEEDBACK_VOLTAGE_TO_5V_VOLT(2.50f), payload->analog_v5_mcu, "analog_v5_mcu value mismatch after voltage conversion");
}

// SD feedback
void test_feedback_get_analog_sd_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = 1.0f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = 2.0f;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_SD_END] = 3.0f;

    primary_hv_feedback_analog_sd_converted_t expected = {
        .sd_out = FEEDBACK_VOLTAGE_TO_SD_VOLT(1.0f),
        .sd_in = FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0f),
        .sd_end = FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0f),
    };

    primary_hv_feedback_analog_sd_converted_t *payload = feedback_get_analog_sd_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.analog_sd_can_payload, payload, "Analog SD payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(hfeedback.analog_sd_can_payload), (uint32_t)byte_size, "Analog SD payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Analog SD payload content does not match expected voltage-converted values");
}

void test_feedback_get_enzomma_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    primary_hv_feedback_enzomma_converted_t *payload =
        feedback_get_enzomma_payload(FEEDBACK_ID_AIRN_OPEN_COM, &byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&hfeedback.enzomma_can_payload, payload, "Enzomma payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(hfeedback.enzomma_can_payload), (uint32_t)byte_size, "Enzomma payload byte size mismatch");
}

void test_feedback_get_enzomma_payload_digital_feedback_sets_is_digital_and_digital_value(void) {
    hfeedback.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    hfeedback.digital = (1UL << FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    primary_hv_feedback_enzomma_converted_t *payload =
        feedback_get_enzomma_payload(FEEDBACK_ID_AIRN_OPEN_COM, NULL);

    TEST_ASSERT_TRUE_MESSAGE(payload->is_digital, "is_digital should be true for a digital feedback");
    TEST_ASSERT_TRUE_MESSAGE(payload->digital, "digital should be true when the digital bit is set");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 0.0f, payload->analog, "analog field should be 0 for a digital feedback");
}

void test_feedback_get_enzomma_payload_analog_feedback_sets_analog_value(void) {
    hfeedback.status[FEEDBACK_ID_IMD_OK] = FEEDBACK_STATUS_LOW;
    hfeedback.analog[FEEDBACK_ANALOG_INDEX_IMD_OK] = 2.22f;

    primary_hv_feedback_enzomma_converted_t *payload =
        feedback_get_enzomma_payload(FEEDBACK_ID_IMD_OK, NULL);

    TEST_ASSERT_FALSE_MESSAGE(payload->is_digital, "is_digital should be false for an analog feedback");
    TEST_ASSERT_FALSE_MESSAGE(payload->digital, "digital should be false for an analog feedback");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001f, 2.22f, payload->analog, "analog field should reflect the stored analog value");
}

void setUp(void) {
    RESET_FAKE(feedback_read_digital_all);
    RESET_FAKE(feedback_start_analog_conversion);
    (void)feedback_init(feedback_read_digital_all, feedback_start_analog_conversion);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_feedback_init_returns_null_pointer_when_read_callback_is_null);
    RUN_TEST(test_feedback_init_returns_null_pointer_when_start_conversion_callback_is_null);
    RUN_TEST(test_feedback_init_saves_callbacks_and_returns_ok);

    RUN_TEST(test_feedback_update_digital_stores_read_result);

    RUN_TEST(test_feedback_update_analog_feedback_stores_value);
    RUN_TEST(test_feedback_update_analog_feedback_returns_invalid_index_when_out_of_range);

    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit);
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index);

    RUN_TEST(test_feedback_get_analog_status_probing_3v3_returns_high_when_in_range);
    RUN_TEST(test_feedback_get_analog_status_probing_3v3_returns_error_when_above_high_threshold);
    RUN_TEST(test_feedback_get_analog_status_probing_3v3_returns_error_when_below_low_threshold);
    RUN_TEST(test_feedback_get_analog_status_returns_low_when_below_low_threshold);
    RUN_TEST(test_feedback_get_analog_status_returns_high_when_above_high_threshold);
    RUN_TEST(test_feedback_get_analog_status_returns_error_when_between_thresholds);

    RUN_TEST(test_feedback_update_status_sets_high_for_set_digital_bits);
    RUN_TEST(test_feedback_update_status_sets_low_for_cleared_digital_bits);
    RUN_TEST(test_feedback_update_status_sets_high_for_analog_above_threshold);
    RUN_TEST(test_feedback_update_status_sets_low_for_analog_below_threshold);

    RUN_TEST(test_feedback_check_values_returns_true_and_sets_unknown_when_all_match);
    RUN_TEST(test_feedback_check_values_returns_false_and_sets_failing_id_on_mismatch);
    RUN_TEST(test_feedback_check_values_skips_feedbacks_not_in_mask);
    RUN_TEST(test_feedback_check_values_accepts_null_out_pointer);

    RUN_TEST(test_feedback_get_digital_bit_from_id_returns_unknown_for_invalid_id);
    RUN_TEST(test_feedback_get_analog_index_from_id_returns_unknown_for_invalid_id);

    RUN_TEST(test_feedback_get_status_payload_returns_correct_pointer_and_size);

    RUN_TEST(test_feedback_get_digital_payload_returns_correct_pointer_and_size);

    RUN_TEST(test_feedback_get_analog_payload_returns_correct_pointer_and_size);

    RUN_TEST(test_feedback_get_analog_sd_payload_returns_correct_pointer_and_size);

    RUN_TEST(test_feedback_get_enzomma_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_get_enzomma_payload_digital_feedback_sets_is_digital_and_digital_value);
    RUN_TEST(test_feedback_get_enzomma_payload_analog_feedback_sets_analog_value);

    return UNITY_END();
}