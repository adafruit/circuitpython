// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__MESSAGE_SEQUENCE_H_
#define RMW__MESSAGE_SEQUENCE_H_

#include <stddef.h>

#include "rmw/macros.h"
#include "rmw/visibility_control.h"
#include "rmw/types.h"

#if __cplusplus
extern "C"
{
#endif

/// Structure to hold a sequence of ROS messages.
typedef struct RMW_PUBLIC_TYPE rmw_message_sequence_s
{
  /// Array of pointers to ROS messages.
  void ** data;
  /// The number of valid entries in `data`.
  size_t size;
  /// The total allocated capacity of the data array.
  size_t capacity;
  /// The allocator used to allocate the data array.
  rcutils_allocator_t * allocator;
} rmw_message_sequence_t;

/// Structure to hold a sequence of message infos.
typedef struct RMW_PUBLIC_TYPE rmw_message_info_sequence_s
{
  /// Array of message info.
  rmw_message_info_t * data;
  /// The number of valid entries in data.
  size_t size;
  /// The total allocated capacity of the data array.
  size_t capacity;
  /// The allocator used to allocate the data array.
  rcutils_allocator_t * allocator;
} rmw_message_info_sequence_t;

/// Return an rmw_message_sequence_t struct with members initialized to `NULL`
RMW_PUBLIC
rmw_message_sequence_t
rmw_get_zero_initialized_message_sequence(void);

/// Initialize an rmw_message_sequence_t object.
/**
 * \param[inout] sequence sequence object to be initialized.
 * \param[in] size capacity of the sequence to be allocated.
 * \param[in] allocator the allcator used to allocate memory.
 */
RMW_PUBLIC
rmw_ret_t
rmw_message_sequence_init(
  rmw_message_sequence_t * sequence,
  size_t size,
  rcutils_allocator_t * allocator);

/// Finalize an rmw_message_sequence_t object.
/**
 * The rmw_message_sequence_t struct has members which require memory to be allocated to them
 * before setting values.
 * This function reclaims any allocated resources within the object and zeroes out all other
 * members.
 *
 * Note: This will not call `fini` or deallocate the underlying message structures.
 *
 * \param[inout] sequence sequence object to be finalized.
 */
RMW_PUBLIC
rmw_ret_t
rmw_message_sequence_fini(rmw_message_sequence_t * sequence);

/// Return an rmw_message_info_sequence_t struct with members initialized to `NULL`
RMW_PUBLIC
rmw_message_info_sequence_t
rmw_get_zero_initialized_message_info_sequence(void);

/// Initialize an rmw_message_info_sequence_t object.
/**
 * \param[inout] sequence sequence object to be initialized.
 * \param[in] size capacity of the sequence to be allocated.
 * \param[in] allocator the allcator used to allocate memory.
 */
RMW_PUBLIC
rmw_ret_t
rmw_message_info_sequence_init(
  rmw_message_info_sequence_t * sequence,
  size_t size,
  rcutils_allocator_t * allocator);

/// Finalize an rmw_message_sequence_t object.
/**
 * The rmw_message_sequence_t struct has members which require memory to be allocated to them
 * before setting values.
 * This function reclaims any allocated resources within the object and zeroes out all other
 * members.
 *
 * \param[inout] sequence sequence object to be finalized.
 */
RMW_PUBLIC
rmw_ret_t
rmw_message_info_sequence_fini(rmw_message_info_sequence_t * sequence);

#if __cplusplus
}
#endif

#endif  // RMW__MESSAGE_SEQUENCE_H_
