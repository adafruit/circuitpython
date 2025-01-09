// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__NAMES_AND_TYPES_H_
#define RMW__NAMES_AND_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include "rcutils/allocator.h"
#include "rcutils/types.h"
#include "rmw/macros.h"
#include "rmw/types.h"
#include "rmw/visibility_control.h"

/// Associative array of topic or service names and types.
typedef struct RMW_PUBLIC_TYPE rmw_names_and_types_s
{
  /// Array of names
  rcutils_string_array_t names;

  /// Dynamic array of arrays of type names, with the same length as `names`
  rcutils_string_array_t * types;
} rmw_names_and_types_t;

/// Return a zero initialized array of names and types.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_names_and_types_t
rmw_get_zero_initialized_names_and_types(void);

/// Check that the given `names_and_types` array is zero initialized.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \par Thread-safety
 *   Access to the array of names and types is read-only, but it is not synchronized.
 *   Concurrent `names_and_types` reads are safe, but concurrent reads and writes are not.
 *
 * \param[in] names_and_types Array to be checked.
 * \return RMW_RET_OK if array is zero initialized, RMW_RET_INVALID_ARGUMENT otherwise.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_names_and_types_check_zero(rmw_names_and_types_t * names_and_types);

/// Initialize an array of names and types.
/**
 * This function initializes the string array for the names and allocates space
 * for all the string arrays for the types according to the given size, but
 * it does not initialize the string array for each setup of types.
 * However, the string arrays for each set of types is zero initialized.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \par Thread-safety
 *   Initialization is a reentrant procedure, but:
 *   - Access to arrays of names and types is not synchronized.
 *     It is not safe to read or write `names_and_types` during initialization.
 *   - The default allocators are thread-safe objects, but any custom `allocator` may not be.
 *     Check your allocator documentation for further reference.
 *
 * \param[inout] names_and_types Array to be initialized on success,
 *   but left unchanged on failure.
 * \param[in] size Size of the array.
 * \param[in] allocator Allocator to be used to populate `names_and_types`.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `names_and_types` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `names_and_types` is not
 *   a zero initialized array, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `allocator` is invalid,
 *   by rcutils_allocator_is_valid() definition, or
 * \returns `RMW_BAD_ALLOC` if memory allocation fails, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_names_and_types_init(
  rmw_names_and_types_t * names_and_types,
  size_t size,
  rcutils_allocator_t * allocator);

/// Finalize an array of names and types.
/**
 * This function deallocates the string array of names and the array of string arrays of types,
 * and zero initializes the given array.
 * If a logical error, such as `RMW_RET_INVALID_ARGUMENT`, ensues, this function will return
 * early, leaving the given array unchanged.
 * Otherwise, it will proceed despite errors.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \par Thread-safety
 *   Finalization is a reentrant procedure, but access to arrays of names and types
 *   is not synchronized.
 *   It is not safe to read or write `names_and_types` during initialization.
 *
 * \param[inout] names_and_types Array to be finalized.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `names_and_types` is NULL, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_names_and_types_fini(rmw_names_and_types_t * names_and_types);

#ifdef __cplusplus
}
#endif

#endif  // RMW__NAMES_AND_TYPES_H_
