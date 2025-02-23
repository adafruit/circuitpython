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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__DYNAMIC_MESSAGE_TYPE_SUPPORT_STRUCT_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__DYNAMIC_MESSAGE_TYPE_SUPPORT_STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rcutils/types/rcutils_ret.h>
#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rosidl_runtime_c/type_description/type_description__struct.h>
#include <rosidl_runtime_c/type_description/type_source__struct.h>

#include "rosidl_dynamic_typesupport/api/dynamic_type.h"
#include "rosidl_dynamic_typesupport/api/dynamic_data.h"
#include "rosidl_dynamic_typesupport/api/serialization_support.h"
#include "rosidl_dynamic_typesupport/identifier.h"
#include "rosidl_dynamic_typesupport/types.h"
#include "rosidl_dynamic_typesupport/visibility_control.h"


// RUNTIME INTERFACE REFLECTION TYPE SUPPORT =======================================================
// Every field of this struct is expected to be populated.
//
// NOTE(methylDragon): There is an opportunity to defer the population of the members by waiting
//                     for discovery, but this path is currently not supported.
//
// Ownership:
//   - The struct owns its `description` field. It is responsible for deallocating it.
//   - The struct owns its `serialization_support` field. It is responsible for deallocating it.
//   - The struct owns its `dynamic_message_type` field. It is responsible for deallocating it.
//   - The struct owns its `dynamic_message` field. It is responsible for deallocating it.
//
// Downstream classes are expected to borrow the `serialization_support` field, and potentially the
// `dynamic_message_type` and `dynamic_message` fields. As such, it is important that this struct
// outlives those downstream classes.
typedef struct rosidl_dynamic_message_type_support_impl_s
{
  rcutils_allocator_t allocator;

  rosidl_type_hash_t type_hash;
  rosidl_runtime_c__type_description__TypeDescription type_description;

  // Unused for now, but placed here just in case
  rosidl_runtime_c__type_description__TypeSource__Sequence type_description_sources;

  rosidl_dynamic_typesupport_serialization_support_t serialization_support;

  // The dynamic_message_type allows us to do a one time alloc and reuse it for subscription
  // creation and data creation
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_message_type;

  // The dynamic_message allows us to either reuse it, or clone it, but it's technically redundant
  // because data can be created from dynamic_message_type
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_message;
} rosidl_dynamic_message_type_support_impl_t;

/// Initialize a dynamic type message type support with encapsulated message description
/**
 * Take note of the ownership rules for the returned struct and the `description` argument.
 *
 * The `rosidl_message_type_support_t *` returned from this function has different ownership rules
 * compared to the statically allocated `rosidl_message_type_support_t` structs from code-generated
 * types.
 *
 * The `type_hash`, `type_description`, `type_description_sources`, and `allocator` arguments are
 * copied.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 */
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_message_type_support_handle_init(
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support,
  const rosidl_type_hash_t * type_hash,
  const rosidl_runtime_c__type_description__TypeDescription * type_description,
  const rosidl_runtime_c__type_description__TypeSource__Sequence * type_description_sources,
  rcutils_allocator_t * allocator,
  rosidl_message_type_support_t * ts);  // OUT

/// Finalize a rosidl_message_type_support_t obtained with
/// `rosidl_dynamic_message_type_support_handle_init()`, which has dynamically allocated members
///
/// NOTE: Using this on a statically allocated typesupport will cause undefined behavior!
///       (Static memory will get freed in that case.)
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_message_type_support_handle_fini(rosidl_message_type_support_t * ts);

/// Initialized a `rosidl_dynamic_message_type_support_impl_t` with encapsulated message description
/**
 * The `type_hash`, `type_description`, `type_description_sources`, and `allocator` arguments are
 * copied.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 */
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_message_type_support_handle_impl_init(
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support,
  const rosidl_type_hash_t * type_hash,
  const rosidl_runtime_c__type_description__TypeDescription * type_description,
  const rosidl_runtime_c__type_description__TypeSource__Sequence * type_description_sources,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_message_type_support_impl_t * ts_impl);  // OUT

/// Finalize a `rosidl_dynamic_message_type_support_impl_t`
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_message_type_support_handle_impl_fini(
  rosidl_dynamic_message_type_support_impl_t * ts_impl);

/// Return type_hash member in rosidl_dynamic_message_type_support_impl_t
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
const rosidl_type_hash_t *
rosidl_get_dynamic_message_type_support_type_hash_function(
  const rosidl_message_type_support_t * type_support);

/// Return description member in rosidl_dynamic_message_type_support_impl_t
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
const rosidl_runtime_c__type_description__TypeDescription *
rosidl_get_dynamic_message_type_support_type_description_function(
  const rosidl_message_type_support_t * type_support);

/// Return type_description_sources member in rosidl_dynamic_message_type_support_impl_t
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
const rosidl_runtime_c__type_description__TypeSource__Sequence *
rosidl_get_dynamic_message_type_support_type_description_sources_function(
  const rosidl_message_type_support_t * type_support);


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__DYNAMIC_MESSAGE_TYPE_SUPPORT_STRUCT_H_
