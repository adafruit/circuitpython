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

#ifndef ROSIDL_RUNTIME_C__TYPE_HASH_H_
#define ROSIDL_RUNTIME_C__TYPE_HASH_H_

#include <stdint.h>

#include "rcutils/allocator.h"
#include "rcutils/sha256.h"

#include "rosidl_runtime_c/visibility_control.h"

#define ROSIDL_TYPE_HASH_VERSION_UNSET 0
#define ROSIDL_TYPE_HASH_SIZE RCUTILS_SHA256_BLOCK_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

/// A ROS 2 interface type hash per REP-2011 RIHS standard.
typedef struct rosidl_type_hash_s
{
  uint8_t version;
  uint8_t value[ROSIDL_TYPE_HASH_SIZE];
} rosidl_type_hash_t;

/// Get a new zero-initialized type hash structure.
/**
 * Note that the version equals ROSIDL_TYPE_HASH_VERSION_UNSET.
 */
ROSIDL_GENERATOR_C_PUBLIC
rosidl_type_hash_t
rosidl_get_zero_initialized_type_hash(void);

/// Convert type hash to a standardized string representation.
/**
 * Follows format RIHS{version}_{value}.
 *
 * \param[in] type_hash Type hash to convert to string
 * \param[in] allocator Allocator to use for allocating string space
 * \param[out] output_string Handle to a pointer that will be set
 *   to the newly allocated null-terminated string representation.
 * \return RCUTILS_RET_INVALID_ARGUMENT if any pointer arguments are null or allocator invalid
 * \return RCUTILS_RET_BAD_ALLOC if space could not be allocated for resulting string
 * \return RCUTILS_RET_OK otherwise
 */
ROSIDL_GENERATOR_C_PUBLIC
rcutils_ret_t
rosidl_stringify_type_hash(
  const rosidl_type_hash_t * type_hash,
  rcutils_allocator_t allocator,
  char ** output_string);

/// Parse a stringified type hash to a struct.
/**
 * \param[in] type_hash_string Null-terminated string with the hash representation
 * \param[out] hash_out Preallocated structure to be filled with parsed hash information.
 *   hash_out->version will be 0 if no version could be parsed,
 *   but if a version could be determined this field will be set even if an error is returned
 * \return RCTUILS_RET_INVALID_ARGUMENT on any null pointer argumunts, or malformed hash string.
 * \return RCUTILS_RET_OK otherwise
 */
ROSIDL_GENERATOR_C_PUBLIC
rcutils_ret_t
rosidl_parse_type_hash_string(
  const char * type_hash_string,
  rosidl_type_hash_t * hash_out);

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_RUNTIME_C__TYPE_HASH_H_
