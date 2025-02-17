// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file
 */

#ifndef RMW_MICROROS__TIMING_H_
#define RMW_MICROROS__TIMING_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <uxr/client/util/time.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Sets the DDS-XRCE session spin time in reliable publication
 *
 * \param[in] publisher publisher where the spin time is configured
 * \param[in] session_timeout time in milliseconds
 * \return RMW_RET_OK when success.
 * \return RMW_RET_INVALID_ARGUMENT If publisher is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_set_publisher_session_timeout(
  rmw_publisher_t * publisher,
  int session_timeout);

/**
 * \brief Sets the DDS-XRCE session spin time in reliable service server
 *
 * \param[in] service service where the spin time is configured
 * \param[in] session_timeout time in milliseconds
 * \return RMW_RET_OK when success.
 * \return RMW_RET_INVALID_ARGUMENT If service is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_set_service_session_timeout(
  rmw_service_t * service,
  int session_timeout);

/**
 * \brief Sets the DDS-XRCE session spin time in reliable service client
 *
 * \param[in] client client where the spin time is configured
 * \param[in] session_timeout time in milliseconds
 * \return RMW_RET_OK when success.
 * \return RMW_RET_INVALID_ARGUMENT If client is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_set_client_session_timeout(
  rmw_client_t * client,
  int session_timeout);

/**
 * \brief Sets the DDS-XRCE session spin time for creating entities
 *
 * \param[in] context RWM context where the spin time is configured
 * \param[in] session_timeout time in milliseconds
 * \return RMW_RET_OK when success.
 * \return RMW_RET_INVALID_ARGUMENT If client is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_set_context_entity_creation_session_timeout(
  rmw_context_t * context,
  int session_timeout);

/**
 * \brief Sets the DDS-XRCE session spin time for destroying entities
 *
 * \param[in] context RWM context where the spin time is configured
 * \param[in] session_timeout time in milliseconds
 * \return RMW_RET_OK when success.
 * \return RMW_RET_INVALID_ARGUMENT If client is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_set_context_entity_destroy_session_timeout(
  rmw_context_t * context,
  int session_timeout);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__TIMING_H_
