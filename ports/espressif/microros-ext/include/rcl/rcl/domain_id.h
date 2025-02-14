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

#ifndef RCL__DOMAIN_ID_H_
#define RCL__DOMAIN_ID_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rmw/domain_id.h"

/// The default domain ID used by RCL.
#define RCL_DEFAULT_DOMAIN_ID RMW_DEFAULT_DOMAIN_ID

extern const char * const RCL_DOMAIN_ID_ENV_VAR;

/// Determine the default domain ID, based on the environment.
/**
 * \param[out] domain_id Must not be NULL.
 * \returns #RCL_RET_INVALID_ARGUMENT if an argument is invalid, or,
 * \returns #RCL_RET_ERROR in case of an unexpected error, or,
 * \returns #RCL_RET_OK.
 */
RCL_PUBLIC
rcl_ret_t
rcl_get_default_domain_id(size_t * domain_id);

#ifdef __cplusplus
}
#endif

#endif  // RCL__DOMAIN_ID_H_
