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

#ifndef RMW_MICROROS__PING_H_
#define RMW_MICROROS__PING_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <rmw_microxrcedds_c/config.h>
#include <ucdr/microcdr.h>

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM
#include <uxr/client/profile/transport/custom/custom_transport.h>
#endif  // RMW_MICROROS__PING_H_


#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Check if micro-ROS Agent is up and running.
 *        This function can be called even when the micro-ROS context has not yet been
 *        initialized by the application logics.
 *        This function cannot be called concurrently with `rmw_init()` or `rmw_shutdown()`.
 * \param[in] timeout_ms Timeout in ms (per attempt).
 * \param[in] attempts Number of tries before considering the ping as failed.
 * \return RMW_RET_OK If micro-ROS Agent is available.
 * \return RMW_RET_ERROR If micro-ROS Agent is not available.
 */
rmw_ret_t rmw_uros_ping_agent(
  const int timeout_ms,
  const uint8_t attempts);

/**
 * \brief Check if micro-ROS Agent is up and running using the transport set on the given rmw options.
 *        This function can be called even when the micro-ROS context has not yet been initialized.
 *        The transport will be initialized and closed once during the ping process.
 *        This function cannot be called concurrently with `rmw_init()` or `rmw_shutdown()`.
 * \param[in] timeout_ms Timeout in ms (per attempt).
 * \param[in] attempts Number of tries before considering the ping as failed.
 * \param[in] rmw_options rmw options with populated transport parameters.
 * \return RMW_RET_OK If micro-ROS Agent is available.
 * \return RMW_RET_ERROR If micro-ROS Agent is not available.
 */
rmw_ret_t rmw_uros_ping_agent_options(
  const int timeout_ms,
  const uint8_t attempts,
  rmw_init_options_t * rmw_options);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__PING_H_
