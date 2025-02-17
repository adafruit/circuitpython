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

#ifndef MICRO_ROS_UTILITIES__STRING_UTILITIES_H_
#define MICRO_ROS_UTILITIES__STRING_UTILITIES_H_

#if __cplusplus
extern "C"
{
#endif  // if __cplusplus

#include <rosidl_runtime_c/string.h>
#include <micro_ros_utilities/visibility_control.h>

/** \addtogroup stringutils micro-ROS String Utilities
 *  @{
 */

/**
 *  Create a rosidl_runtime_c__String from a char pointer
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] data char pointer
 * \return `rosidl_runtime_c__String` string containing data
 */
MICRO_ROS_UTILITIES_PUBLIC
rosidl_runtime_c__String
micro_ros_string_utilities_init(const char * data);

/**
 *  Create a rosidl_runtime_c__String from a size
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] size size of the required string
 * \return `rosidl_runtime_c__String` string of size size
 */
MICRO_ROS_UTILITIES_PUBLIC
rosidl_runtime_c__String
micro_ros_string_utilities_init_with_size(const size_t size);

/**
 *  Create a rosidl_runtime_c__String from a char pointer
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str  rosidl_runtime_c__String to set
 * \param[in] data char pointer
 * \return `rosidl_runtime_c__String` string containing data
 */
MICRO_ROS_UTILITIES_PUBLIC
rosidl_runtime_c__String
micro_ros_string_utilities_set(const rosidl_runtime_c__String str, const char * data);

/**
 *  Returns the char pointer to the rosidl_runtime_c__String data
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str a rosidl_runtime_c__String
 * \return `const char` char pointer
 */
MICRO_ROS_UTILITIES_PUBLIC
const char *
micro_ros_string_utilities_get_c_str(const rosidl_runtime_c__String str);

/**
 *  Appends a char pointer to the end of a rosidl_runtime_c__String
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str   a rosidl_runtime_c__String
 * \param[in] data  characters to append
 * \return `rosidl_runtime_c__String` new string
 */
rosidl_runtime_c__String micro_ros_string_utilities_append(
  const rosidl_runtime_c__String str,
  const char * data);

/**
 *  Removes characters from the end of a string
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str   a rosidl_runtime_c__String
 * \param[in] n     number of characters to remove
 * \return `rosidl_runtime_c__String` new string
 */
MICRO_ROS_UTILITIES_PUBLIC
rosidl_runtime_c__String
micro_ros_string_utilities_remove_tail_chars(
  const rosidl_runtime_c__String str,
  const size_t n);

/**
 *  Destroys a  rosidl_runtime_c__String
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str   a rosidl_runtime_c__String
 */
MICRO_ROS_UTILITIES_PUBLIC
void
micro_ros_string_utilities_destroy(rosidl_runtime_c__String * const str);

/** @}*/

#if __cplusplus
}
#endif  // if __cplusplus

#endif  // MICRO_ROS_UTILITIES__STRING_UTILITIES_H_
