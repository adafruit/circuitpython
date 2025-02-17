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

/// @file

#ifndef RCL__RMW_IMPLEMENTATION_IDENTIFIER_CHECK_H_
#define RCL__RMW_IMPLEMENTATION_IDENTIFIER_CHECK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/visibility_control.h"

/// The environment variable name to control which RMW implementation is used.
#define RMW_IMPLEMENTATION_ENV_VAR_NAME "RMW_IMPLEMENTATION"

/// The environment variable name to control whether the chosen RMW implementation
/// matches the one that is in use.
#define RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME "RCL_ASSERT_RMW_ID_MATCHES"

/// Check whether the RMW implementation in use matches what the user requested.
/**
 * \return #RCL_RET_OK if the RMW implementation in use matches what the user requested, or
 * \return #RCL_RET_MISMATCHED_RMW_ID if the RMW implementation does not match, or
 * \return #RCL_RET_BAD_ALLOC if memory allocation failed, or
 * \return #RCL_RET_ERROR if some other error occurred.
 */
RCL_PUBLIC
rcl_ret_t rcl_rmw_implementation_identifier_check(void);

#ifdef __cplusplus
}
#endif

#endif  // RCL__RMW_IMPLEMENTATION_IDENTIFIER_CHECK_H_
