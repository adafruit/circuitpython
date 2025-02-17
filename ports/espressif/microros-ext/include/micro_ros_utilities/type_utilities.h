// Copyright (c) 2021 - for information on the respective copyright owner
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

#ifndef MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_
#define MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_

#if __cplusplus
extern "C"
{
#endif  // if __cplusplus

#include <rosidl_typesupport_introspection_c/message_introspection.h>
#include <rosidl_runtime_c/string.h>
#include <rcutils/allocator.h>
#include <micro_ros_utilities/visibility_control.h>

/** \addtogroup typeutils micro-ROS Types Utilities
 *  @{
 */

/**
 * Memory rule
 */
typedef struct micro_ros_utilities_memory_rule_t
{
  /**
   * Pattern matching the fully qualified name of a msg field.
   *
   */
  const char * rule;

  /**
   * Maximum string or sequence capacity for fields matching \ref rule.
   *
   */
  size_t size;
} micro_ros_utilities_memory_rule_t;

/**
 * Memory configuration string
 */
typedef struct micro_ros_utilities_memory_conf_t
{
  /**
   * Maximum string capacity to use for msg fields in case they don't have a
   * custom rule assigned to them.
   *
   */
  size_t max_string_capacity;

  /**
   * Maximum capacity to use for sequence type msg fields (ie: unbounded
   * arrays and lists) which contain ROS 2 msg types, in case they don't have
   * a custom rule assigned to them.
   *
   */
  size_t max_ros2_type_sequence_capacity;

  /**
   * Maximum capacity to use for sequence type msg fields (ie: unbounded
   * arrays and lists) which contain basic types (ie: primitive field types),
   * in case they don't have a custom rule assigned to them.
   *
   */
  size_t max_basic_type_sequence_capacity;

  /**
   * All rules defined in this configuration.
   *
   */
  const micro_ros_utilities_memory_rule_t * rules;

  /**
   * Total number of rules defined in this configuration.
   *
   */
  size_t n_rules;

  /**
   * The allocator to use when applying this configuration.
   *
   */
  const rcutils_allocator_t * allocator;
} micro_ros_utilities_memory_conf_t;

// Default memory configuration
static const micro_ros_utilities_memory_conf_t micro_ros_utilities_memory_conf_default =
{20, 5, 5, NULL, 0, NULL};


/**
 *  Returns a string with the type introspection data
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support ROS 2 typesupport
 * \return `rosidl_runtime_c__String` string containing data
 */
MICRO_ROS_UTILITIES_PUBLIC
rosidl_runtime_c__String
micro_ros_utilities_type_info(
  const rosidl_message_type_support_t * type_support);

/**
 *  Returns the dynamic memory size that will be used for a type
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support ROS 2 typesupport
 * \param[in] conf         Utils configurator
 * \return `size_t` Size in Bytes that will be used
 */
MICRO_ROS_UTILITIES_PUBLIC
size_t
micro_ros_utilities_get_dynamic_size(
  const rosidl_message_type_support_t * type_support,
  const micro_ros_utilities_memory_conf_t conf);

/**
 *  Returns the static memory size that will be used for a type
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support ROS 2 typesupport
 * \param[in] conf         Utils configurator
 * \return `size_t` Size in Bytes that will be used
 */
MICRO_ROS_UTILITIES_PUBLIC
size_t
micro_ros_utilities_get_static_size(
  const rosidl_message_type_support_t * type_support,
  const micro_ros_utilities_memory_conf_t conf);

/**
 *  Allocates the dynamic memory required for a message
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support       ROS 2 typesupport
 * \param[inout] ros_msg         ROS 2 msg with no type
 * \param[in] conf               Utils configurator
 * \return `bool` true if success
 */
MICRO_ROS_UTILITIES_PUBLIC
bool
micro_ros_utilities_create_message_memory(
  const rosidl_message_type_support_t * type_support,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf);

/**
 *  Allocates the memory required for a message in a user-provided buffer
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support       ROS 2 typesupport
 * \param[inout] ros_msg         ROS 2 msg with no type
 * \param[in] conf               Utils configurator
 * \param[in] buffer             User buffer
 * \param[in] buffer_len         User buffer length
 * \return `bool` true if success
 */
MICRO_ROS_UTILITIES_PUBLIC
bool
micro_ros_utilities_create_static_message_memory(
  const rosidl_message_type_support_t * type_support, void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf, uint8_t * buffer, size_t buffer_len);

/**
 *  Deallocates the dynamic memory of a message
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] type_support       ROS 2 typesupport
 * \param[inout] ros_msg         ROS 2 msg with no type
 * \param[in] conf               Utils configurator
 * \return `bool` true if success
 */
MICRO_ROS_UTILITIES_PUBLIC
bool
micro_ros_utilities_destroy_message_memory(
  const rosidl_message_type_support_t * type_support,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf);

/** @}*/

#if __cplusplus
}
#endif  // if __cplusplus

#endif  // MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_
