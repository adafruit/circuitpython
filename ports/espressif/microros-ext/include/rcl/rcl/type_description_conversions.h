// Copyright 2023 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__TYPE_DESCRIPTION_CONVERSIONS_H_
#define RCL__TYPE_DESCRIPTION_CONVERSIONS_H_

#include "rcl/macros.h"
#include "rcl/visibility_control.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "type_description_interfaces/msg/type_description.h"
#include "type_description_interfaces/msg/type_source.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Convert type description runtime struct to msg struct.
/**
 * This function converts a rosidl_runtime_c__type_description__TypeDescription
 * to the corresponding type_description_interfaces/msg/TypeDescription struct.
 * The retrieved pointer shall be destroyed with
 * `type_description_interfaces__msg__TypeDescription__destroy()` after use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] runtime_description the pointer to the runtime type description struct
 * \return a valid type_description_interfaces/msg/TypeDescription pointer, or
 * \return NULL if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
type_description_interfaces__msg__TypeDescription *
rcl_convert_type_description_runtime_to_msg(
  const rosidl_runtime_c__type_description__TypeDescription * runtime_description);

/// Convert type description msg struct to a rosidl runtime struct.
/**
 * This function converts a type_description_interfaces/msg/TypeDescription
 * to the corresponding rosidl_runtime_c__type_description__TypeDescription
 * struct. The retrieved pointer shall be destroyed with
 * `rosidl_runtime_c__type_description__TypeDescription__destroy()` after use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] description_msg the pointer to the msg type description struct
 * \return a valid pointer to the runtime struct, or
 * \return NULL if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rosidl_runtime_c__type_description__TypeDescription *
rcl_convert_type_description_msg_to_runtime(
  const type_description_interfaces__msg__TypeDescription * description_msg);

/// Convert type sources sequence runtime struct to msg struct.
/**
 * This function converts a rosidl_runtime_c__type_description__TypeSource__Sequence
 * struct to the corresponding type_description_interfaces__msg__TypeSource__Sequence
 * struct. The retrieved pointer shall be destroyed with
 * `type_description_interfaces__msg__TypeSource__Sequence__destroy()` after use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] runtime_type_sources the pointer to the type sources sequence struct
 * \return a valid pointer to the msg struct, or
 * \return NULL if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
type_description_interfaces__msg__TypeSource__Sequence *
rcl_convert_type_source_sequence_runtime_to_msg(
  const rosidl_runtime_c__type_description__TypeSource__Sequence * runtime_type_sources);

/// Convert type sources sequece msg struct to a rosidl runtime struct.
/**
 * This function converts a rosidl_runtime_c__type_description__TypeSource__Sequence
 * struct to the corresponding type_description_interfaces__msg__TypeSource__Sequence
 * struct. The retrieved pointer shall be destroyed with
 * `type_description_interfaces__msg__TypeSource__Sequence__destroy()` after use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_sources_msg the pointer to the type sources sequence struct
 * \return a valid pointer to the msg struct, or
 * \return NULL if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_convert_type_source_sequence_msg_to_runtime(
  const type_description_interfaces__msg__TypeSource__Sequence * type_sources_msg);

#ifdef __cplusplus
}
#endif

#endif  // RCL__TYPE_DESCRIPTION_CONVERSIONS_H_
