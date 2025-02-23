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

#ifndef RCL__NODE_OPTIONS_H_
#define RCL__NODE_OPTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/allocator.h"
#ifdef RCL_MICROROS_COMPLETE_IMPL
#include "rcl/arguments.h"
#endif // RCL_MICROROS_COMPLETE_IMPL
#include "rcl/macros.h"

#include "rcl/domain_id.h"

/// Constant which indicates that the default domain id should be used.
#define RCL_NODE_OPTIONS_DEFAULT_DOMAIN_ID RCL_DEFAULT_DOMAIN_ID

/// Structure which encapsulates the options for creating a rcl_node_t.
typedef struct rcl_node_options_s
{
  // bool anonymous_name;

  // rmw_qos_profile_t parameter_qos;

  /// If true, no parameter infrastructure will be setup.
  // bool no_parameters;

  /// Custom allocator used for internal allocations.
  rcl_allocator_t allocator;

  /// If false then only use arguments in this struct, otherwise use global arguments also.
  bool use_global_arguments;

#ifdef RCL_MICROROS_COMPLETE_IMPL
  /// Command line arguments that apply only to this node.
  rcl_arguments_t arguments;
#endif // RCL_MICROROS_COMPLETE_IMPL

  /// Flag to enable rosout for this node
  bool enable_rosout;

  /// Middleware quality of service settings for /rosout.
  rmw_qos_profile_t rosout_qos;
} rcl_node_options_t;

/// Return the default node options in a rcl_node_options_t.
/**
 * The default values are:
 *
 * - allocator = rcl_get_default_allocator()
 * - use_global_arguments = true
 * - enable_rosout = true
 * - arguments = rcl_get_zero_initialized_arguments()
 * - rosout_qos = rcl_qos_profile_rosout_default
 *
 * \return A structure with the default node options.
 */
RCL_PUBLIC
rcl_node_options_t
rcl_node_get_default_options(void);

/// Copy one options structure into another.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] options The structure to be copied.
 *   Its allocator is used to copy memory into the new structure.
 * \param[out] options_out An options structure containing default values.
 * \return #RCL_RET_OK if the structure was copied successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any function arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_node_options_copy(
  const rcl_node_options_t * options,
  rcl_node_options_t * options_out);

/// Finalize the given node_options.
/**
 * The given node_options must be non-`NULL` and valid, i.e. had
 * rcl_node_get_default_options() called on it but not this function yet.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Yes
 * Lock-Free          | Yes
 *
 * \param[inout] options object to be finalized
 * \return #RCL_RET_OK if setup is successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_node_options_fini(rcl_node_options_t * options);

#ifdef __cplusplus
}
#endif

#endif  // RCL__NODE_OPTIONS_H_
