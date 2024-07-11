/**
 * @file post.c
 * @date 2024-04-16
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Power-On Self Test function to check that every internal system and
 * peripheral is working correctly
 */

#include "post.h"

#include "identity.h"
#include "programmer.h"
#include "timebase.h"
#include "volt.h"
#include "current.h"

#ifdef CONF_POST_MODULE_ENABLE

/**
 * @brief Initialize all the cellboard modules
 * 
 * @attention The order in which the init functions are called matters
 *
 * @param data A pointer to the initialization data
 *
 * @return PostReturnCode
 *     - POST_OK
 */
PostReturnCode _post_modules_init(PostInitData * data) {
    identity_init();

    /**
     * Some of the function return values can be ignored because they are either
     * always OK or some assertion can be made (like for the NULL checks)
     */
    (void)timebase_init(1U);
    (void)pcu_init(data->pcu_set, data->pcu_toggle);
    (void)volt_init();
    (void)current_init();
    (void)can_comm_init(data->can_send);
    (void)programmer_init(data->system_reset);
    (void)led_init(data->led_set, data->led_toggle);
    (void)imd_init(data->imd_start);
    (void)feedback_init(data->feedback_read_all, data->feedback_start_conversion);

    return POST_OK;
}

PostReturnCode post_run(PostInitData data) {
    if (data.system_reset == NULL ||
        data.can_send == NULL ||
        data.led_set == NULL ||
        data.led_toggle == NULL ||
        data.imd_start == NULL ||
        data.pcu_set == NULL ||
        data.pcu_toggle == NULL ||
        data.feedback_read_all == NULL ||
        data.feedback_start_conversion == NULL)
        return POST_NULL_POINTER;

    PostReturnCode post_code = _post_modules_init(&data);

    // TODO: Test that every peripheral is working

    return post_code;
}

#ifdef CONF_POST_STRINGS_ENABLE

_STATIC char * post_module_name = "post";

_STATIC char * post_return_code_name[] = {
    [POST_OK] = "ok",
    [POST_INVALID_CELLBOARD_ID] = "invalid cellboard id",
    [POST_NULL_POINTER] = "null pointer",
    [POST_WATCHDOG_INVALID_MESSAGE] = "invalid watchdog message"
};

_STATIC char * post_return_code_description[] = {
    [POST_OK] = "executed successfully",
    [POST_INVALID_CELLBOARD_ID] = "the given id does not correspond to any valid cellboard identifier",
    [POST_NULL_POINTER] = "attempt to dereference a null pointer",
    [POST_WATCHDOG_INVALID_MESSAGE] = "the watchdogs are using a non valid can message"
};

#endif // CONF_POST_STRINGS_ENABLE

#endif // CONF_POST_MODULE_ENABLE
