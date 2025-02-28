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

#ifndef RMW_MICROROS__DISCOVERY_H_
#define RMW_MICROROS__DISCOVERY_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <rmw_microxrcedds_c/config.h>
#include <ucdr/microcdr.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Fills rmw implementation-specific options with the autodicovered address of an micro-ROS Agent.
 *
 * \param[in,out] rmw_options Updated options with rmw specifics.
 * \return RMW_RET_OK If arguments were valid and set in rmw_init_options.
 * \return RMW_RET_TIMEOUT If micro-ROS agent autodiscovery is timeout.
 * \return RMW_RET_INVALID_ARGUMENT If rmw_init_options is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_discover_agent(
  rmw_init_options_t * rmw_options);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__DISCOVERY_H_
