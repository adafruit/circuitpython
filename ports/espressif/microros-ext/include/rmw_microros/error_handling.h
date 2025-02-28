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

#ifndef RMW_MICROROS__ERROR_HANDLING_H_
#define RMW_MICROROS__ERROR_HANDLING_H_

#ifdef RMW_UROS_ERROR_HANDLING

#include <ucdr/microcdr.h>
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>
#include <rosidl_typesupport_microxrcedds_c/service_type_support.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

typedef enum
{
  RMW_UROS_ERROR_ON_UNKNOWN = 0,
  RMW_UROS_ERROR_ON_NODE,
  RMW_UROS_ERROR_ON_SERVICE,
  RMW_UROS_ERROR_ON_CLIENT,
  RMW_UROS_ERROR_ON_SUBSCRIPTION,
  RMW_UROS_ERROR_ON_PUBLISHER,
  RMW_UROS_ERROR_ON_GRAPH,
  RMW_UROS_ERROR_ON_GUARD_CONDITION,
  RMW_UROS_ERROR_ON_TOPIC
} rmw_uros_error_entity_type_t;

typedef enum
{
  RMW_UROS_ERROR_ENTITY_CREATION = 0,
  RMW_UROS_ERROR_ENTITY_DESTRUCTION,
  RMW_UROS_ERROR_CHECK,
  RMW_UROS_ERROR_NOT_IMPLEMENTED,
  RMW_UROS_ERROR_MIDDLEWARE_ALLOCATION,
} rmw_uros_error_source_t;

typedef struct
{
  const char * node;
  const char * node_namespace;
  const char * topic_name;
  const ucdrBuffer * ucdr;
  const size_t size;
  union {
    const message_type_support_callbacks_t * message_callbacks;
    const service_type_support_callbacks_t * service_callbacks;
  } type_support;
  const char * description;
} rmw_uros_error_context_t;

typedef void (* rmw_uros_error_handling)(
  const rmw_uros_error_entity_type_t entity,
  const rmw_uros_error_source_t source,
  const rmw_uros_error_context_t context,
  const char * file,
  const int line);

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Sets the callback functions for handling error in static memory handling
 *
 * \param[in] error_cb callback to be triggered on static memory failure
 */
void rmw_uros_set_error_handling_callback(
  rmw_uros_error_handling error_cb);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_UROS_ERROR_HANDLING

#endif  // RMW_MICROROS__ERROR_HANDLING_H_
