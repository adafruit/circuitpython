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

#ifndef RMW__SECURITY_OPTIONS_H_
#define RMW__SECURITY_OPTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#include "rcutils/allocator.h"

#include "rmw/ret_types.h"
#include "rmw/visibility_control.h"

typedef enum RMW_PUBLIC_TYPE rmw_security_enforcement_policy_e
{
  RMW_SECURITY_ENFORCEMENT_PERMISSIVE,
  RMW_SECURITY_ENFORCEMENT_ENFORCE,
} rmw_security_enforcement_policy_t;

typedef struct RMW_PUBLIC_TYPE rmw_security_options_s
{
  rmw_security_enforcement_policy_t enforce_security;
  char * security_root_path;
} rmw_security_options_t;

/// Get zero initialized security options.
RMW_PUBLIC
rmw_security_options_t
rmw_get_zero_initialized_security_options(void);

/// Get default initialized security options.
RMW_PUBLIC
rmw_security_options_t
rmw_get_default_security_options(void);

/// Copy the given security options.
/**
 * \param[in] src security options to be copied.
 * \param[in] allocator allocator used when copying data to the new security options.
 * \param[out] dst security options to be set.
 * \returns RMW_RET_BAD_ALLOC, or
 * \returns RMW_RET_OK
 */
RMW_PUBLIC
rmw_ret_t
rmw_security_options_copy(
  const rmw_security_options_t * src,
  const rcutils_allocator_t * allocator,
  rmw_security_options_t * dst);

/// Set the security root path for the given security options.
/**
 * The provided `security_root_path` will be copied into allocated memory.
 *
 * \param[in] security_root_path path to be set.
 * \param[in] allocator allocator used to allocate the new path.
 * \param[inout] security_options security options to be set.
 * \returns RMW_RET_BAD_ALLOC, or
 * \returns RMW_RET_OK
 */
RMW_PUBLIC
rmw_ret_t
rmw_security_options_set_root_path(
  const char * security_root_path,
  const rcutils_allocator_t * allocator,
  rmw_security_options_t * security_options);

/// Finalize the given security_options.
/**
 * \param[in] security_options security options to be finalized.
 * \param[in] allocator allocator used to deallocate the root path.
 * \returns RMW_RET_ERROR, or
 * \returns RMW_RET_OK
 */
RMW_PUBLIC
rmw_ret_t
rmw_security_options_fini(
  rmw_security_options_t * security_options,
  const rcutils_allocator_t * allocator);

#ifdef __cplusplus
}
#endif

#endif  // RMW__SECURITY_OPTIONS_H_
