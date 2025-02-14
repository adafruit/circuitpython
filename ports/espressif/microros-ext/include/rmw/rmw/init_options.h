// Copyright 2014-2018 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__INIT_OPTIONS_H_
#define RMW__INIT_OPTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "rcutils/allocator.h"
#include "rmw/discovery_options.h"
#include "rmw/domain_id.h"
#include "rmw/localhost.h"
#include "rmw/macros.h"
#include "rmw/ret_types.h"
#include "rmw/security_options.h"
#include "rmw/visibility_control.h"

/// Implementation defined options structure used during rmw_init().
/**
 * This should be defined by the rmw implementation.
 */
typedef struct rmw_init_options_impl_s rmw_init_options_impl_t;

/// Options structure used during rmw_init().
typedef struct RMW_PUBLIC_TYPE rmw_init_options_s
{
  /// Locally (process local) unique ID that represents this init/shutdown cycle.
  /**
   * This should be set by the caller of `rmw_init()` to a number that is
   * unique within this process.
   * It is designed to be used with `rcl_init()` and `rcl_get_instance_id()`.
   */
  uint64_t instance_id;
  /// Implementation identifier, used to ensure two different implementations are not being mixed.
  const char * implementation_identifier;
  /// ROS domain id
  size_t domain_id;
  /// Security options
  rmw_security_options_t security_options;
  /// Enable localhost only
  rmw_localhost_only_t localhost_only;
  /// Configure discovery
  rmw_discovery_options_t discovery_options;
  /// Enclave, name used to find security artifacts in a sros2 keystore.
  char * enclave;

  // TODO(wjwwood): replace with rmw_allocator_t when that refactor happens
  /// Allocator used during internal allocation of init options, if needed.
  rcutils_allocator_t allocator;
  /// Implementation defined init options.
  /** May be NULL if there are no implementation defined options. */
  rmw_init_options_impl_t * impl;
} rmw_init_options_t;

/// Return a zero initialized init options structure.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_init_options_t
rmw_get_zero_initialized_init_options(void);

/// Initialize given init options with the default values and implementation specific values.
/**
 * The given allocator is used, if required, during setup of the init options,
 * but is also used during initialization.
 *
 * In either case the given allocator is stored in the returned init options.
 *
 * The `impl` pointer should not be changed manually.
 *
 * \pre The given init options must be zero initialized.
 *
 * \post If initialization fails, init options will remain zero initialized.
 *
 * \remark Giving an already initialized init options will result
 *   in a failure with return code `RMW_RET_INVALID_ARGUMENT`.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * This should be defined by the rmw implementation.
 *
 * \param[inout] init_options object to be setup
 * \param[in] allocator to be used during setup and during initialization
 * \return `RMW_RET_OK` if setup is successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RMW_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RMW_RET_ERROR` if an unspecified error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_init_options_init(rmw_init_options_t * init_options, rcutils_allocator_t allocator);

/// Copy the given source init options to the destination init options.
/**
 * The allocator from the source is used for any allocations and stored in the
 * destination.
 *
 * \pre The source init options must have been initialized
 *   i.e. had `rmw_init_options_init()` called on.
 * \pre The destination init options must be zero initialized.
 *
 * \post If copy fails, destination init options will remain zero initialized.
 *
 * \remark Giving an zero initialized init options as a source will result
 *   in a failure with return code `RMW_RET_INVALID_ARGUMENT`.
 * \remark Giving an already initialized init options for the destination will result
 *   in a failure with return code `RMW_RET_INVALID_ARGUMENT`.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * This should be defined by the rmw implementation.
 *
 * \param[in] src rcl_init_options_t object to be copied from
 * \param[out] dst rcl_init_options_t object to be copied into
 * \return `RMW_RET_OK` if the copy is successful, or
 * \return `RMW_RET_INCORRECT_RMW_IMPLEMENTATION` if the implementation
 *   identifier for src does not match the implementation of this function, or
 * \return `RMW_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RMW_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RMW_RET_ERROR` if an unspecified error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_init_options_copy(const rmw_init_options_t * src, rmw_init_options_t * dst);

/// Finalize the given init options.
/**
 * This function will return early if a logical error, such as `RMW_RET_INVALID_ARGUMENT`
 * or `RMW_RET_INCORRECT_RMW_IMPLEMENTATION`, ensues, leaving the given init options
 * unchanged.
 * Otherwise, it will proceed despite errors, freeing as much resources as it can and zero
 * initializing the given init options.
 *
 * \pre The given init options must have been initialized
 *   i.e. had `rmw_init_options_init()` called on.
 *
 * \remarks If init options are zero initialized,
 *   then `RMW_RET_INVALID_ARGUMENT` is returned.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * This should be defined by the rmw implementation.
 *
 * \param[inout] init_options object to finalized
 * \return `RMW_RET_OK` if finalization is successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RMW_RET_INCORRECT_RMW_IMPLEMENTATION` if the implementation
 *   identifier does not match the implementation of this function, or
 * \return `RMW_RET_ERROR` if an unspecified error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_init_options_fini(rmw_init_options_t * init_options);

#ifdef __cplusplus
}
#endif

#endif  // RMW__INIT_OPTIONS_H_
