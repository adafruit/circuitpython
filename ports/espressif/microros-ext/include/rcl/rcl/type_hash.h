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

#ifndef RCL__TYPE_HASH_H_
#define RCL__TYPE_HASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rcutils/sha256.h"
#include "rosidl_runtime_c/type_hash.h"
#include "type_description_interfaces/msg/type_description.h"

/// Given a TypeDescription, output a string of the hashable JSON representation of that data.
/**
 * The output here is generally hashed via rcl_calculate_type_hash() below.
 * Compare this reference implementation with the .json output files from
 * `rosidl_generator_type_description.generate_type_hash`.
 * Both must produce the same output for the same types, providing a stable reference for
 * external implementations of the ROS 2 Type Version Hash.
 *
 * The JSON representation contains all types and fields of the original message, but excludes:
 * - Default values
 * - Comments
 * - The input plaintext files that generated the TypeDescription
 *
 * \param[in] type_description Prefilled TypeDescription message to be translated
 * \param[out] output_repr An initialized empty char array that will be filled with
 *   the JSON representation of type_description. Note that output_repr will have a
 *   terminating null character, which should be omitted from hashing. To do so, use
 *   (output_repr.buffer_length - 1) or strlen(output_repr.buffer) for the size of data to hash.
 * \return #RCL_RET_OK on success, or
 * \return #RCL_RET_ERROR if any problems occur in translation.
 */
RCL_PUBLIC
rcl_ret_t
rcl_type_description_to_hashable_json(
  const type_description_interfaces__msg__TypeDescription * type_description,
  rcutils_char_array_t * output_repr);

/// Calculate the Type Version Hash for a given TypeDescription.
/**
 * This function produces a stable hash value for a ROS communication interface type.
 * For design motivations leading to this implementation, see REP-2011.
 *
 * This convenience wrapper calls rcl_type_description_to_hashable_json,
 * then runs sha256 hash on the result.
 *
 * \param[in] msg Prefilled TypeDescription message describing the type to be hashed
 * \param[out] message_digest Preallocated buffer, to be filled with calculated checksum
 * \return #RCL_RET_OK on success, or
 * \return #RCL_RET_ERROR if any problems occur while hashing.
 */
RCL_PUBLIC
rcl_ret_t
rcl_calculate_type_hash(
  const type_description_interfaces__msg__TypeDescription * type_description,
  rosidl_type_hash_t * out_type_hash);

#ifdef __cplusplus
}
#endif

#endif  // RCL__TYPE_HASH_H_
