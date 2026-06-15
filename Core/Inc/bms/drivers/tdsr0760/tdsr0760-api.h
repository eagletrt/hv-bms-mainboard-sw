/*!
 * \file tdsr0760-api.h
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief API functions for the TDSR0760 7-segment 1 digit display
 */

#ifndef TDSR0760_API_H
#define TDSR0760_API_H

#include "tdsr0760.h"

/*!
 * \brief Initialize the 7-segment display handler structure
 *
 * \param handler A pointer to the display handler structure
 * 
 * \retval TDSR0760_RC_NULL_POINTER if any of the parameters is NULL
 * \retval TDSR0760_RC_OK otherwise
 */
enum Tdsr0760ReturnCode tdsr0760_api_init(struct Tdsr0760Handler *handler);

/*!
 * \brief Get the status of a single segment
 *
 * \param handler A pointer to the 7-segment display handler structure
 * \param segment The segment to select
 *
 * \returns enum Tdsr0760SegmentStatus The current status of the segment, or
 * TDSR0760_SEGMENT_STATUS_UNKOWN on error
 */
enum Tdsr0760SegmentStatus tdsr0760_api_get_segment(struct Tdsr0760Handler *handler, Tdsr0760Segment segment);

/*!
 * \brief Set the status of a single segment
 *
 * \param handler A pointer to the 7-segment display handler structure
 * \param segment The segment to select
 * \param state The new status to set
 *
 * \retval TDSR0760_RC_NULL_POINTER if any of the parameters is NULL
 * \retval TDSR0760_RC_INVALID_SEGMENT if the selected segment does not exists
 * \retval TDSR0760_RC_OK otherwise
 */
enum Tdsr0760ReturnCode tdsr0760_api_set_segment(struct Tdsr0760Handler *handler, Tdsr0760Segment segment, enum Tdsr0760SegmentStatus state);

/*!
 * \brief Toggle the status of a single segment
 *
 * \details If the status is unknown it is set to TDSR0760_SEGMENT_STATUS_ON
 *
 * \param handler A pointer to the 7-segment display handler structure
 * \param segment The segment to select
 *
 * \retval TDSR0760_RC_NULL_POINTER if any of the parameters is NULL
 * \retval TDSR0760_RC_INVALID_SEGMENT if the selected segment does not exists
 * \retval TDSR0760_RC_OK otherwise
 */
enum Tdsr0760ReturnCode tdsr0760_api_toggle_segment(struct Tdsr0760Handler *handler, Tdsr0760Segment segment);

#endif // TDSR0760_API_H
