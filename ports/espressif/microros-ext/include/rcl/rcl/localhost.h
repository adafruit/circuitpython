// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__LOCALHOST_H_
#define RCL__LOCALHOST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rmw/localhost.h"

extern const char * const RCL_LOCALHOST_ENV_VAR;

/// Determine if the user wants to communicate using loopback only.
/**
 * Checks if localhost should be used for network communication based on environment.
 *
 * \param[out] localhost_only Must not be NULL.
 * \return #RCL_RET_INVALID_ARGUMENT if an argument is invalid, or
 * \return #RCL_RET_ERROR if an unexpected error happened, or
 * \return #RCL_RET_OK.
 */
RCL_PUBLIC
rcl_ret_t
rcl_get_localhost_only(rmw_localhost_only_t * localhost_only);

#ifdef __cplusplus
}
#endif

#endif  // RCL__LOCALHOST_H_
