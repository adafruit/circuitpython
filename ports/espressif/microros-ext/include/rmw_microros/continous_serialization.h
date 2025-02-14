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

#ifndef RMW_MICROROS__CONTINOUS_SERIALIZATION_H_
#define RMW_MICROROS__CONTINOUS_SERIALIZATION_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <rmw_microxrcedds_c/config.h>
#include <ucdr/microcdr.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

typedef void (* rmw_uros_continous_serialization_size)(
  uint32_t * topic_length);
typedef void (* rmw_uros_continous_serialization)(
  ucdrBuffer * ucdr);

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Sets the callback functions for continous serialization for a publisher
 *
 * \param[in] publisher publisher where continous serialization is being configured
 * \param[in] size_cb callback that should modify the total serialization size
 * \param[in] serialization_cb callback that should serialize the user part of the message
 */
void rmw_uros_set_continous_serialization_callbacks(
  rmw_publisher_t * publisher,
  rmw_uros_continous_serialization_size size_cb,
  rmw_uros_continous_serialization serialization_cb);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__CONTINOUS_SERIALIZATION_H_
