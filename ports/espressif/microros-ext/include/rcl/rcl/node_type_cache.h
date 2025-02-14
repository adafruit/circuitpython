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

#ifndef RCL__NODE_TYPE_CACHE_H_
#define RCL__NODE_TYPE_CACHE_H_

#include "rcl/node.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "type_description_interfaces/msg/type_description.h"
#include "type_description_interfaces/msg/type_source.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rcl_type_info_t
{
  type_description_interfaces__msg__TypeDescription * type_description;
  type_description_interfaces__msg__TypeSource__Sequence * type_sources;
} rcl_type_info_t;

/// Initialize the node's type cache.
/**
 * This function initializes hash map of the node's type cache such that types
 * can be registered and retrieved.
 * Note that to correctly capture all types used by a node, this needs to be called
 * before any "builtin" publishers or services are created.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] node the handle to the node whose type cache should be initialized
 * \return #RCL_RET_OK if the node's type cache was successfully initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the given `node` is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_init(rcl_node_t * node);

/// Finalize the node's type cache.
/**
 * This function clears the hash map of the node's type cache and deallocates
 * used memory.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] node the handle to the node whose type cache should be finalized
 * \return #RCL_RET_OK if the node's type cache was successfully finalized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the given `node` is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_fini(rcl_node_t * node);

/// Register a type with the node's type cache.
/**
 * This function registers the given type, uniquely identified by the type_hash,
 * with the node with the node's type cache. Multiple registrations of the same
 * type will increment its registration count.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] node the handle to the node whose type cache should be finalized
 * \param[in] type_hash hash of the type
 * \param[in] type_description type description struct
 * \param[in] type_description_sources type description sources struct
 * \return #RCL_RET_OK if the type was successfully registered, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the given `node` is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_register_type(
  const rcl_node_t * node, const rosidl_type_hash_t * type_hash,
  const rosidl_runtime_c__type_description__TypeDescription * type_description,
  const rosidl_runtime_c__type_description__TypeSource__Sequence * type_description_sources
);

/// Unregister a message type from the node's type cache.
/**
 * This function uses the given `type_hash` to unregister the associated type in
 * the node's type cache. If the type has been registered multiple times, the
 * type will only be removed if its registration count reaches 0.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] node the handle to the node whose type cache should be finalized
 * \param[in] type_hash type hash
 * \return #RCL_RET_OK if the type was successfully registered, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the given `node` is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_unregister_type(
  const rcl_node_t * node, const rosidl_type_hash_t * type_hash);

/// Retrieve type information from the node's type cache.
/**
 * This function returns the desired type information from the node's type cache
 *
 * The `type_info` field must point to an allocated `rcl_type_info_t` object to
 * which the type information will be written.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] node the handle to the node whose type cache should be queried
 * \param[in] type_hash type hash
 * \param[out] type_info pointer to the type info struct that will be populated
 * \return #RCL_RET_OK if type information was retrieved successfully
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the given `node` is invalid, or
 * \return #RCL_RET_NOT_INIT if node's type cache has not been initialized, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_get_type_info(
  const rcl_node_t * node,
  const rosidl_type_hash_t * type_hash,
  rcl_type_info_t * type_info);

#ifdef __cplusplus
}
#endif

#endif  // RCL__NODE_TYPE_CACHE_H_
