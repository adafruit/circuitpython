// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__INIT_OPTIONS_H_
#define RCL__INIT_OPTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rmw/init.h"

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

typedef struct rcl_init_options_impl_s rcl_init_options_impl_t;

/// Encapsulation of init options and implementation defined init options.
typedef struct rcl_init_options_s
{
  /// Implementation specific pointer.
  rcl_init_options_impl_t * impl;
} rcl_init_options_t;

/// Return a zero initialized rcl_init_options_t struct.
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_init_options_t
rcl_get_zero_initialized_init_options(void);

/// Initialize given init_options with the default values and implementation specific values.
/**
 * The given allocator is used, if required, during setup of the init options,
 * but is also used during initialization.
 *
 * In either case the given allocator is stored in the returned init options.
 *
 * The `impl` pointer should not be changed manually.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * \param[inout] init_options object to be setup
 * \param[in] allocator to be used during setup and during initialization
 * \return #RCL_RET_OK if setup is successful, or
 * \return #RCL_RET_ALREADY_INIT if init_options has already be initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_init_options_init(rcl_init_options_t * init_options, rcl_allocator_t allocator);

/// Copy the given source init_options to the destination init_options.
/**
 * The allocator from the source is used for any allocations and stored in the
 * destination.
 *
 * The destination should either be zero initialized with
 * rcl_get_zero_initialized_init_options() or should have had
 * rcl_init_options_fini() called on it.
 * Giving an already initialized init options for the destination will result
 * in a failure with return code #RCL_RET_ALREADY_INIT.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * \param[in] src rcl_init_options_t object to be copied from
 * \param[out] dst rcl_init_options_t object to be copied into
 * \return #RCL_RET_OK if the copy is successful, or
 * \return #RCL_RET_ALREADY_INIT if the dst has already be initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_init_options_copy(const rcl_init_options_t * src, rcl_init_options_t * dst);

/// Finalize the given init_options.
/**
 * The given init_options must be non-`NULL` and valid, i.e. had
 * rcl_init_options_init() called on it but not this function yet.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * \param[inout] init_options object to be setup
 * \return #RCL_RET_OK if setup is successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_init_options_fini(rcl_init_options_t * init_options);

/// Return the domain_id stored in the init options.
/**
 * Get the domain id from the specified rcl_init_options_t object.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] init_options object from which the domain id should be retrieved.
 * \param[out] domain_id domain id to be set in init_options object.
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_init_options_get_domain_id(const rcl_init_options_t * init_options, size_t * domain_id);

/// Set a domain id in the init options provided.
/**
 * Store the domain id in the specified init_options object.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] init_options objects in which to set the specified domain id.
 * \param[in] domain_id domain id to be set in init_options object.
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_init_options_set_domain_id(rcl_init_options_t * init_options, size_t domain_id);

/// Return the rmw init options which are stored internally.
/**
 * This function can fail and return `NULL` if:
 *   - init_options is NULL
 *   - init_options is invalid, e.g. init_options->impl is NULL
 *
 * If NULL is returned an error message will have been set.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * \param[in] init_options object from which the rmw init options should be retrieved
 * \return pointer to the the rcl init options, or
 * \return `NULL` if there was an error
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rmw_init_options_t *
rcl_init_options_get_rmw_init_options(rcl_init_options_t * init_options);

/// Return the allocator stored in the init_options.
/**
 * This function can fail and return `NULL` if:
 *   - init_options is NULL
 *   - init_options is invalid, e.g. init_options->impl is NULL
 *
 * If NULL is returned an error message will have been set.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] init_options object from which the allocator should be retrieved
 * \return pointer to the rcl allocator, or
 * \return `NULL` if there was an error
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rcl_allocator_t *
rcl_init_options_get_allocator(const rcl_init_options_t * init_options);

#ifdef __cplusplus
}
#endif

#endif  // RCL__INIT_OPTIONS_H_
