/*!
 * \file test-feedback.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */
#include "feedback.h"
#include "primary_network.h"
#include "unity.h"
#include "feedback-api.h"
#include "mainboard-def.h"
#include "unity_internals.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fff.h>

DEFINE_FFF_GLOBALS;

extern struct FeedbackHandler feedback_handler;

extern enum FeedbackId prv_feedback_get_id_from_digital_bit(enum FeedbackDigitalBit bit);
extern enum FeedbackId prv_feedback_get_id_from_analog_index(enum FeedbackAnalogIndex index);
extern enum FeedbackStatus prv_feedback_api_get_analog_status(enum FeedbackAnalogIndex index);

FAKE_VALUE_FUNC(uint32_t, feedback_read_digital_all);
FAKE_VOID_FUNC(feedback_start_analog_conversion);

/*!
 * \defgroup            id_from_digital_bit Test for feedback ID from digital bit index function
 * @{
 */

void test_feedback_get_id_from_digital_bit_returns_correct_id(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_digital_bit(FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, result, "Expected AIRN_OPEN_COM id for AIRN_OPEN_COM digital bit");
}

void test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_digital_bit((enum FeedbackDigitalBit)FEEDBACK_DIGITAL_BIT_COUNT);

    TEST_ASSERT_EQUAL(FEEDBACK_ID_INVALID, result);
}

/*! @} */

/*!
 * \defgroup            id_from_analog_index Tests for feedback ID from analog index function
 * @{
 */

void test_feedback_get_id_from_analog_index_returns_correct_id(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_analog_index(FEEDBACK_ANALOG_INDEX_IMD_OK);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ID_IMD_OK, result, "Expected IMD_OK id for IMD_OK analog index");
}

void test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index(void) {
    const enum FeedbackId result = prv_feedback_get_id_from_analog_index((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT);

    TEST_ASSERT_EQUAL(FEEDBACK_ID_INVALID, result);
}

/*! @} */

/*!
 * \defgroup            get_analog_status Test get analog status
 * @{
 */
void test_feedback_api_get_analog_status_probing_3v3_returns_high_when_in_range(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status for PROBING_3V3 value within valid range");
}

void test_feedback_api_get_analog_status_probing_3v3_returns_error_when_above_high_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_HIGH_V + 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value above high threshold");
}

void test_feedback_api_get_analog_status_probing_3v3_returns_error_when_below_low_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_PROBING_3V3] = FEEDBACK_THRESHOLD_LOW_V - 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_PROBING_3V3);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status for PROBING_3V3 value below low threshold");
}

void test_feedback_api_get_analog_status_returns_low_when_below_low_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_LOW_V - 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_LOW, result, "Expected LOW status when analog value is below low threshold");
}

void test_feedback_api_get_analog_status_returns_high_when_above_high_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.1F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_HIGH, result, "Expected HIGH status when analog value is above high threshold");
}

void test_feedback_api_get_analog_status_returns_error_when_between_thresholds(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = (FEEDBACK_THRESHOLD_LOW_V + FEEDBACK_THRESHOLD_HIGH_V) * 0.5F;

    const enum FeedbackStatus result = prv_feedback_api_get_analog_status(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_STATUS_ERROR, result, "Expected ERROR status when analog value is in the forbidden mid-range");
}

/*! @} */

/*!
 * \defgroup            init Test initialization
 * @{
 */

void test_feedback_api_init_returns_null_pointer_when_read_callback_is_null(void) {
    const enum FeedbackReturnCode result = feedback_api_init(NULL, feedback_start_analog_conversion);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when read callback is NULL");
}

void test_feedback_api_init_returns_null_pointer_when_start_conversion_callback_is_null(void) {
    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_NULL_POINTER, result, "Expected NULL_POINTER when start_conversion callback is NULL");
}

void test_feedback_api_init_saves_callbacks_and_returns_ok(void) {
    const enum FeedbackReturnCode result = feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code after successful init");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_read_digital_all, feedback_handler.read_digital, "read_digital callback not stored correctly");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(feedback_start_analog_conversion, feedback_handler.start_conversion, "start_conversion callback not stored correctly");
}

/*! @} */

/*!
 * \defgroup            update_analog Test update analog feedback
 * @{
 */

void test_feedback_api_update_analog_feedback_stores_value(void) {
    const enum FeedbackReturnCode result = feedback_api_update_analog_feedback(FEEDBACK_ANALOG_INDEX_IMD_OK, 2.5F);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_OK, result, "Expected OK return code from analog update");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 2.5F, feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK], "Analog value for IMD_OK not stored correctly");
}

void test_feedback_api_update_analog_feedback_returns_invalid_index_when_out_of_range(void) {
    const enum FeedbackReturnCode result = feedback_api_update_analog_feedback((enum FeedbackAnalogIndex)FEEDBACK_ANALOG_INDEX_COUNT, 1.0F);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_RC_INVALID_INDEX, result, "Expected INVALID_INDEX for out-of-range analog index");
}

/*! @} */

/*!
 * \defgroup            update_status Test update status feedback
 * @{
 */

void test_feedback_api_update_status_sets_high_for_set_digital_bits(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE);

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE], "feedback status should be HIGH");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_low_for_cleared_digital_bits(void) {
    feedback_handler.digital = 0U;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_LOW, feedback_handler.status[FEEDBACK_ID_PLAUSIBLE_STATE], "feedback status should be LOW");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_high_for_analog_above_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = FEEDBACK_THRESHOLD_HIGH_V + 0.2F;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, feedback_handler.status[FEEDBACK_ID_TSAL_GREEN], "feedback status should be HIGH");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

void test_feedback_api_update_status_sets_low_for_analog_below_threshold(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = FEEDBACK_THRESHOLD_LOW_V - 0.2F;

    const enum FeedbackReturnCode rc = feedback_api_update_status();

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_LOW, feedback_handler.status[FEEDBACK_ID_TSAL_GREEN], "feedback status should be LOW");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_RC_OK, rc, "return code is not successful");
}

/*! @} */

/*!
 * \defgroup            get_status Test get status feedback
 * @{
 */

void test_feedback_api_get_status_with_invalid_id(void) {
    const enum FeedbackStatus status = feedback_api_get_status(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_ERROR, status, "feedback status should be ERROR");
}

/*! @} */

/*!
 * \defgroup            check_values Test feedback check values
 * @{
 */

void test_feedback_api_check_values_when_all_states_matches(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;

    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(
        EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM),
        FEEDBACK_ID_SD_IMD_FB);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_COUNT;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "at least one value in the mask do not match");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_INVALID, out, "return feedback id is not invalid");
}

void test_feedback_api_check_values_when_any_state_dont_match(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_LOW;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_AIRN_OPEN_COM);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_FALSE_MESSAGE(result, "all values in mask matches");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_AIRN_OPEN_COM, out, "unexpected feedback id");
}

void test_feedback_api_check_values_skips_feedbacks_not_in_mask(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_HIGH;
    constexpr bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    constexpr bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    enum FeedbackId out = FEEDBACK_ID_INVALID;

    const bool result = feedback_api_check_values(mask, value, &out);

    TEST_ASSERT_TRUE_MESSAGE(result, "all values in mask matches");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_ID_INVALID, out, "return feedback id is not invalid");
}

void test_feedback_api_check_values_when_out_pointer_is_null(void) {
    feedback_handler.status[FEEDBACK_ID_SD_IMD_FB] = FEEDBACK_STATUS_LOW;
    const bit_flag32_t mask = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);
    const bit_flag32_t value = EAGLETRT_API_BIT_SET(0, FEEDBACK_ID_SD_IMD_FB);

    const bool result = feedback_api_check_values(mask, value, NULL);

    TEST_ASSERT_FALSE_MESSAGE(result, "all values in mask matches");
}

/*! @} */

/*!
 * \defgroup             is_digital Test feedback is digital
 * @{
 */

void test_feedback_api_is_digital_with_invalid_id(void) {
    const bool is_digital = feedback_api_is_digital(FEEDBACK_ID_INVALID);

    TEST_ASSERT_FALSE(is_digital);
}

/*! @} */

/*!
 * \defgroup             get_digital Test feedback get digital
 * @{
 */

void test_feedback_api_get_digital_bit_from_id_with_invalid_id(void) {
    const enum FeedbackDigitalBit result = feedback_api_get_digital_bit_from_id(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_DIGITAL_BIT_INVALID, result, "the feedback digital bit is not invalid");
}

/*! @} */

/*!
 * \defgroup             get_analog Test feedback get analog
 * @{
 */

void test_feedback_api_get_analog_index_from_id_with_invalid_id(void) {
    const enum FeedbackAnalogIndex result = feedback_api_get_analog_index_from_id(FEEDBACK_ID_INVALID);

    TEST_ASSERT_EQUAL_MESSAGE(FEEDBACK_ANALOG_INDEX_INVALID, result, "the feedback analog index is not invalid");
}

/*! @} */

/*!
 * \defgroup             get_status_payload Test feedback get status payload
 * @{
 */

void test_feedback_api_get_status_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;

    const primary_hv_feedback_status_converted_t *payload = feedback_api_get_status_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.status_can_payload, payload, "returned pointer do not match with expected canlib payload address");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.status_can_payload), (uint32_t)byte_size, "Status payload byte size mismatch");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, payload->airn_open_com, "payload content is not matching");
}

void test_feedback_api_get_status_payload_with_null_pointer(void) {
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;

    const primary_hv_feedback_status_converted_t *payload = feedback_api_get_status_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.status_can_payload, payload, "returned pointer do not match with expected canlib payload address");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(FEEDBACK_STATUS_HIGH, payload->airn_open_com, "payload content is not matching");
}

/*! @} */

/*!
 * \defgroup             get_digital_payload Test feedback get digital payload
 * @{
 */

void test_feedback_api_get_digital_payload_returns_correct_pointer_and_size(void) {
    primary_hv_feedback_digital_converted_t expected = { 0 };
    // BUG: Direct struct initialization using bitset result in unexpected behavior. Struct values changes unpredictably
    expected.digital_airn_open_com = 1;
    expected.digital_indicator_connected = 1;
    size_t byte_size = 0U;
    feedback_handler.digital = EAGLETRT_API_BIT_SET(
        EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM),
        FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);

    const primary_hv_feedback_digital_converted_t *payload = feedback_api_get_digital_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.digital_can_payload, payload, "Digital payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.digital_can_payload), (uint32_t)byte_size, "Digital payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Digital payload content does not match expected bit fields");
}

void test_feedback_api_get_digital_payload_with_null_pointer(void) {
    feedback_handler.digital = EAGLETRT_API_BIT_SET(
        EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM),
        FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED);
    primary_hv_feedback_digital_converted_t expected = {
        .digital_airn_open_com = 1,
        .digital_indicator_connected = 1
    };

    const primary_hv_feedback_digital_converted_t *payload = feedback_api_get_digital_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.digital_can_payload, payload, "Digital payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Digital payload content does not match expected bit fields");
}

/*! @} */

/*!
 * \defgroup             get_analog_payload Test feedback get analog payload
 * @{
 */

void test_feedback_api_get_analog_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;

    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.23F;

    const primary_hv_feedback_analog_converted_t *payload = feedback_api_get_analog_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.analog_can_payload, payload, "Analog payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.analog_can_payload), (uint32_t)byte_size, "Analog payload byte size mismatch");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 1.23F, payload->analog_airn_open_mec, "analog_airn_open_mec value mismatch");
}

void test_feedback_api_get_analog_payload_with_null_pointer(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = 1.23F;

    const primary_hv_feedback_analog_converted_t *payload = feedback_api_get_analog_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.analog_can_payload, payload, "Analog payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.0001F, 1.23F, payload->analog_airn_open_mec, "analog_airn_open_mec value mismatch");
}

/*! @} */

/*!
 * \defgroup             get_analog_sd_payload Test feedback get analog SD payload
 * @{
 */

void test_feedback_api_get_analog_sd_payload_returns_correct_pointer_and_size(void) {
    size_t byte_size = 0U;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = 1.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = 2.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_END] = 3.0F;

    const primary_hv_feedback_analog_sd_converted_t expected = {
        .sd_out = FEEDBACK_VOLTAGE_TO_SD_VOLT(1.0F),
        .sd_in = FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0F),
        .sd_end = FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0F),
    };

    const primary_hv_feedback_analog_sd_converted_t *payload = feedback_api_get_analog_sd_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.analog_sd_can_payload, payload, "Analog SD payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.analog_sd_can_payload), (uint32_t)byte_size, "Analog SD payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Analog SD payload content does not match expected voltage-converted values");
}

void test_feedback_api_get_analog_sd_payload_with_null_pointer(void) {
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_OUT] = 1.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_IN] = 2.0F;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_SD_END] = 3.0F;

    const primary_hv_feedback_analog_sd_converted_t expected = {
        .sd_out = FEEDBACK_VOLTAGE_TO_SD_VOLT(1.0F),
        .sd_in = FEEDBACK_VOLTAGE_TO_SD_VOLT(2.0F),
        .sd_end = FEEDBACK_VOLTAGE_TO_SD_VOLT(3.0F),
    };

    const primary_hv_feedback_analog_sd_converted_t *payload = feedback_api_get_analog_sd_payload(NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.analog_sd_can_payload, payload, "Analog SD payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "Analog SD payload content does not match expected voltage-converted values");
}

/*! @} */

/*!
 * \defgroup             get_enzomma_payload Test feedback get enzomma payload
 * @{
 */

void test_feedback_api_get_enzomma_payload_when_feedback_is_digital(void) {
    primary_hv_feedback_enzomma_converted_t expected = {
        .feedback = primary_hv_feedback_enzomma_feedback_airn_open_com,
        .status = primary_hv_feedback_enzomma_status_high
    };
    // BUG: Direct struct initialization using bitset result in unexpected behavior. Struct values changes unpredictably
    expected.is_digital = 1;
    expected.digital = 1;

    size_t byte_size = 0U;
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    const primary_hv_feedback_enzomma_converted_t *payload =
        feedback_api_get_enzomma_payload(FEEDBACK_ID_AIRN_OPEN_COM, &byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.enzomma_can_payload, payload, "Enzomma payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.enzomma_can_payload), (uint32_t)byte_size, "Enzomma payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "payload content do not match");
}

void test_feedback_api_get_enzomma_payload_when_feedback_is_analog(void) {
    primary_hv_feedback_enzomma_converted_t expected = {
        .feedback = primary_hv_feedback_enzomma_feedback_imd_ok,
        .status = primary_hv_feedback_enzomma_status_low,
        .analog = 2.22F
    };
    // BUG: Direct struct initialization using bitset result in unexpected behavior. Struct values changes unpredictably
    expected.is_digital = 0;
    expected.digital = 0;

    size_t byte_size = 0U;
    feedback_handler.status[FEEDBACK_ID_IMD_OK] = expected.status;
    feedback_handler.analog[FEEDBACK_ANALOG_INDEX_IMD_OK] = expected.analog;

    const primary_hv_feedback_enzomma_converted_t *payload =
        feedback_api_get_enzomma_payload(FEEDBACK_ID_IMD_OK, &byte_size);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.enzomma_can_payload, payload, "Enzomma payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(feedback_handler.enzomma_can_payload), (uint32_t)byte_size, "Enzomma payload byte size mismatch");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "payload content do not match");
}

void test_feedback_api_get_enzomma_payload_with_null_pointer(void) {
    primary_hv_feedback_enzomma_converted_t expected = {
        .feedback = primary_hv_feedback_enzomma_feedback_airn_open_com,
        .status = primary_hv_feedback_enzomma_status_high
    };
    // BUG: Direct struct initialization using bitset result in unexpected behavior. Struct values changes unpredictably
    expected.is_digital = 1;
    expected.digital = 1;
    feedback_handler.status[FEEDBACK_ID_AIRN_OPEN_COM] = FEEDBACK_STATUS_HIGH;
    feedback_handler.digital = EAGLETRT_API_BIT_SET(0, FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM);

    const primary_hv_feedback_enzomma_converted_t *payload =
        feedback_api_get_enzomma_payload(FEEDBACK_ID_AIRN_OPEN_COM, NULL);

    TEST_ASSERT_EQUAL_PTR_MESSAGE(&feedback_handler.enzomma_can_payload, payload, "Enzomma payload pointer should point to internal CAN payload struct");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected, payload, sizeof(expected), "payload content do not match");
}

/*! @} */

void setUp(void) {
    RESET_FAKE(feedback_read_digital_all);
    RESET_FAKE(feedback_start_analog_conversion);
    EAGLETRT_API_UNUSED(feedback_api_init(feedback_read_digital_all, feedback_start_analog_conversion));
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    /*!
     * \addgroup            id_from_digital_bit Run feedback ID from digital bit index tests
     * @{
     */
    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_digital_bit_returns_invalid_for_unknown_bit);
    /*! @} */

    /*!
     * \addgroup            id_from_analog_index Run feedback ID from analog index tests
     * @{
     */
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_correct_id);
    RUN_TEST(test_feedback_get_id_from_analog_index_returns_invalid_for_unknown_index);
    /*! @} */

    /*!
     * \addgroup            get_analog_status Run get analog status tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_high_when_in_range);
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_error_when_above_high_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_probing_3v3_returns_error_when_below_low_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_low_when_below_low_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_high_when_above_high_threshold);
    RUN_TEST(test_feedback_api_get_analog_status_returns_error_when_between_thresholds);
    /*! @} */

    /*!
     * \addgroup            init Run initialization tests
     * @{
     */
    RUN_TEST(test_feedback_api_init_returns_null_pointer_when_read_callback_is_null);
    RUN_TEST(test_feedback_api_init_returns_null_pointer_when_start_conversion_callback_is_null);
    RUN_TEST(test_feedback_api_init_saves_callbacks_and_returns_ok);
    /*! @} */

    /*!
     * \addgroup            update_analog Run update analog feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_update_analog_feedback_stores_value);
    RUN_TEST(test_feedback_api_update_analog_feedback_returns_invalid_index_when_out_of_range);
    /*! @} */

    /*!
     * \addgroup            update_status Run update status feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_update_status_sets_high_for_set_digital_bits);
    RUN_TEST(test_feedback_api_update_status_sets_low_for_cleared_digital_bits);
    RUN_TEST(test_feedback_api_update_status_sets_high_for_analog_above_threshold);
    RUN_TEST(test_feedback_api_update_status_sets_low_for_analog_below_threshold);
    /*! @} */

    /*!
     * \addgroup            get_status Run get status feedback tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_status_with_invalid_id);
    /*! @} */

    /*!
     * \addgroup            check_values Run feedback check values tests
     * @{
     */
    RUN_TEST(test_feedback_api_check_values_when_all_states_matches);
    RUN_TEST(test_feedback_api_check_values_when_any_state_dont_match);
    RUN_TEST(test_feedback_api_check_values_skips_feedbacks_not_in_mask);
    RUN_TEST(test_feedback_api_check_values_when_out_pointer_is_null);
    /*! @} */

    /*!
     * \addgroup             is_digital Run feedback is digital tests
     * @{
     */
    RUN_TEST(test_feedback_api_is_digital_with_invalid_id);
    /*! @} */

    /*!
     * \addgroup             get_digital Run feedback get digital tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_digital_bit_from_id_with_invalid_id);
    /*! @} */

    /*!
     * \addgroup             get_analog Run feedback get analog tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_index_from_id_with_invalid_id);
    /*! @} */

    /*!
     * \addgroup             get_status_payload Run feedback get status payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_status_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_status_payload_with_null_pointer);
    /*! @} */

    /*!
     * \addgroup             get_digital_payload Run feedback get digital payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_digital_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_digital_payload_with_null_pointer);
    /*! @} */

    /*!
     * \addgroup             get_analog_payload Run feedback get analog payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_analog_payload_with_null_pointer);
    /*! @} */

    /*!
     * \addgroup             get_analog_sd_payload Run feedback get analog SD payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_analog_sd_payload_returns_correct_pointer_and_size);
    RUN_TEST(test_feedback_api_get_analog_sd_payload_with_null_pointer);
    /*! @} */

    /*!
     * \addgroup             get_enzomma_payload Run feedback get enzomma payload tests
     * @{
     */
    RUN_TEST(test_feedback_api_get_enzomma_payload_when_feedback_is_digital);
    RUN_TEST(test_feedback_api_get_enzomma_payload_when_feedback_is_analog);
    RUN_TEST(test_feedback_api_get_enzomma_payload_with_null_pointer);
    /*! @} */

    return UNITY_END();
}
