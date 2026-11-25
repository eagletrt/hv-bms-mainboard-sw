/*!
 * \file post-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Power-On Self Test and initialization functions needed to make sure
 *        that the system is working as expected
 */

#ifndef POST_API_H
#define POST_API_H

#include "post.h"

/*!
 * \brief Run the Power-On Self Test, initializing every module of the mainboard
 *
 * \details The mainboard status is cleared first, then every other module is initialized
 *          even if a previous one failed, so that the CAN communication and the FSM can
 *          still report the failure. Every failure sets MAINBOARD_STATUS_FAULT_POST_FAILED,
 *          which makes the FSM go from INIT to FATAL on its first run
 *
 * \attention Must be called once at startup, after \ref pcu_api_init and before the
 *            mainboard tasks routine is run for the first time
 *
 * \param data The hardware callbacks
 *
 * \retval POST_RC_NULL_POINTER if data is NULL, nothing is initialized
 * \retval POST_RC_ERROR if at least one module failed to initialize
 * \retval POST_RC_OK otherwise
 */
enum PostReturnCode post_api_run(const struct PostInitData *data);

#endif // POST_API_H
