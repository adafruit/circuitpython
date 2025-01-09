// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__DYNAMIC_MESSAGE_TYPE_SUPPORT_H_
#define RMW__DYNAMIC_MESSAGE_TYPE_SUPPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rcutils/allocator.h>

#include <rosidl_dynamic_typesupport/api/serialization_support.h>
#include <rosidl_dynamic_typesupport/dynamic_message_type_support_struct.h>
#include <rosidl_dynamic_typesupport/identifier.h>
#include <rosidl_dynamic_typesupport/types.h>

#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rosidl_runtime_c/type_description/type_description__struct.h>
#include <rosidl_runtime_c/type_description/type_source__struct.h>

#include "rmw/features.h"
#include "rmw/serialized_message.h"
#include "rmw/visibility_control.h"

/// Interfaces for runtime interface reflection

RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_take_dynamic_message(
  const rmw_subscription_t * subscription,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message,
  bool * taken,
  rmw_subscription_allocation_t * allocation);

RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_take_dynamic_message_with_info(
  const rmw_subscription_t * subscription,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message,
  bool * taken,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation);

RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_serialization_support_init(
  const char * serialization_lib_name,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support);  // OUT

// TODO(methylDragon): Nice to have only
// RMW_PUBLIC
// RMW_WARN_UNUSED
// rmw_ret_t
// rmw_get_dynamic_type_from_middleware(
//   const rmw_node_t * node,
//   const char * topic_name,
//   const rosidl_message_type_support_t type_support,
//   rosidl_dynamic_typesupport_dynamic_type_t * dynamic_message_type);
//
// RMW_PUBLIC
// RMW_WARN_UNUSED
// rmw_ret_t
// rmw_get_dynamic_data_from_middleware(
//   const rmw_node_t * node,
//   const char * topic_name,
//   const rosidl_message_type_support_t type_support,
//   rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message);

#ifdef __cplusplus
}
#endif

#endif  // RMW__DYNAMIC_MESSAGE_TYPE_SUPPORT_H_
