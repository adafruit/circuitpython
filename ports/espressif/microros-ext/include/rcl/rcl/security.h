// Copyright 2018-2020 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__SECURITY_H_
#define RCL__SECURITY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#include "rcl/allocator.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rmw/security_options.h"

#ifndef ROS_SECURITY_ENCLAVE_OVERRIDE
/// The name of the environment variable containing the security enclave override.
# define ROS_SECURITY_ENCLAVE_OVERRIDE "ROS_SECURITY_ENCLAVE_OVERRIDE"
#endif

#ifndef ROS_SECURITY_KEYSTORE_VAR_NAME
/// The name of the environment variable containing the path to the keystore.
# define ROS_SECURITY_KEYSTORE_VAR_NAME "ROS_SECURITY_KEYSTORE"
#endif

#ifndef ROS_SECURITY_STRATEGY_VAR_NAME
/// The name of the environment variable containing the security strategy.
# define ROS_SECURITY_STRATEGY_VAR_NAME "ROS_SECURITY_STRATEGY"
#endif

#ifndef ROS_SECURITY_ENABLE_VAR_NAME
/// The name of the environment variable controlling whether security is enabled.
# define ROS_SECURITY_ENABLE_VAR_NAME "ROS_SECURITY_ENABLE"
#endif

/// Initialize security options from values in the environment variables and given names.
/**
 * Initialize the given security options based on the environment.
 * For more details:
 *  \sa rcl_security_enabled()
 *  \sa rcl_get_enforcement_policy()
 *  \sa rcl_get_secure_root()
 *
 * \param[in] name name used to find the security root path.
 * \param[in] allocator used to do allocations.
 * \param[out] security_options security options that will be configured according to
 *  the environment.
 * \return #RCL_RET_OK If the security options are returned properly, or
 * \return #RCL_RET_INVALID_ARGUMENT if an argument is not valid, or
 * \return #RCL_RET_ERROR if an unexpected error happened
 */
RCL_PUBLIC
rcl_ret_t
rcl_get_security_options_from_environment(
  const char * name,
  const rcutils_allocator_t * allocator,
  rmw_security_options_t * security_options);

/// Check if security has to be used, according to the environment.
/**
 * If the `ROS_SECURITY_ENABLE` environment variable is set to "true", `use_security` will be set to
 * true.
 *
 * \param[out] use_security Must not be NULL.
 * \return #RCL_RET_INVALID_ARGUMENT if an argument is not valid, or
 * \return #RCL_RET_ERROR if an unexpected error happened, or
 * \return #RCL_RET_OK.
 */
RCL_PUBLIC
rcl_ret_t
rcl_security_enabled(bool * use_security);

/// Get security enforcement policy from the environment.
/**
 * Sets `policy` based on the value of the `ROS_SECURITY_STRATEGY` environment variable.
 * If `ROS_SECURITY_STRATEGY` is "Enforce", `policy` will be `RMW_SECURITY_ENFORCEMENT_ENFORCE`.
 * If not, `policy` will be `RMW_SECURITY_ENFORCEMENT_PERMISSIVE`.
 *
 * \param[out] policy Must not be NULL.
 * \return #RCL_RET_INVALID_ARGUMENT if an argument is not valid, or
 * \return #RCL_RET_ERROR if an unexpected error happened, or
 * \return #RCL_RET_OK.
 */
RCL_PUBLIC
rcl_ret_t
rcl_get_enforcement_policy(rmw_security_enforcement_policy_t * policy);

/// Return the secure root given a enclave name.
/**
 * Return the security directory associated with the enclave name.
 *
 * The value of the environment variable `ROS_SECURITY_KEYSTORE` is used as a root.
 * The specific directory to be used is found from that root using the `name` passed.
 * E.g. for a context named "/a/b/c" and root "/r", the secure root path will be
 * "/r/a/b/c", where the delimiter "/" is native for target file system (e.g. "\\" for _WIN32).
 *
 * However, this expansion can be overridden by setting the secure enclave override environment
 * (`ROS_SECURITY_ENCLAVE_OVERRIDE`) variable, allowing users to explicitly specify the exact enclave
 * `name` to be utilized.
 * Such an override is useful for applications where the enclave is non-deterministic
 * before runtime, or when testing and using additional tools that may not otherwise be easily
 * provisioned.
 *
 * \param[in] name validated name (a single token)
 * \param[in] allocator the allocator to use for allocation
 * \return Machine specific (absolute) enclave directory path or NULL on failure.
 *  Returned pointer must be deallocated by the caller of this function
 */
RCL_PUBLIC
char *
rcl_get_secure_root(const char * name, const rcl_allocator_t * allocator);

#ifdef __cplusplus
}
#endif

#endif  // RCL__SECURITY_H_
