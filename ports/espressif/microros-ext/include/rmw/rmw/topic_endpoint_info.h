// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef RMW__TOPIC_ENDPOINT_INFO_H_
#define RMW__TOPIC_ENDPOINT_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcutils/allocator.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rmw/types.h"
#include "rmw/visibility_control.h"

/// A data structure that encapsulates the node name, node namespace,
/// topic_type, gid, and qos_profile of publishers and subscriptions
/// for a topic.
typedef struct RMW_PUBLIC_TYPE rmw_topic_endpoint_info_s
{
  /// Name of the node
  const char * node_name;
  /// Namespace of the node
  const char * node_namespace;
  /// The associated topic type's name
  const char * topic_type;
  /// Hashed value for topic type's description
  rosidl_type_hash_t topic_type_hash;
  /// The endpoint type
  rmw_endpoint_type_t endpoint_type;
  /// The GID of the endpoint
  uint8_t endpoint_gid[RMW_GID_STORAGE_SIZE];
  /// QoS profile of the endpoint
  rmw_qos_profile_t qos_profile;
} rmw_topic_endpoint_info_t;

/// Return zero initialized topic endpoint info data structure.
/**
 * Endpoint type will be invalid.
 * Endpoint QoS profile will be the system default.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_topic_endpoint_info_t
rmw_get_zero_initialized_topic_endpoint_info(void);

/// Finalize a topic endpoint info data structure.
/**
 * Deallocates all allocated members of the given data structure,
 * and then zero initializes it.
 * If a logical error, such as `RMW_RET_INVALID_ARGUMENT`, ensues, this function
 * will return early, leaving the given data structure unchanged.
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
 *   Finalization is a reentrant procedure, but:
 *   - Access to the topic endpoint info data structure is not synchronized.
 *     It is not safe to read or write `topic_endpoint` during finalization.
 *   - The default allocators are thread-safe objects, but any custom `allocator` may not be.
 *     Check your allocator documentation for further reference.
 *
 * \param[inout] topic_endpoint_info Data structure to be finalized.
 * \param[in] allocator Allocator used to populate the given `topic_endpoint_info`.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `allocator` is invalid,
 *   by rcutils_allocator_is_valid() definition, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_fini(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rcutils_allocator_t * allocator);

/// Set the topic type in the given topic endpoint info data structure.
/**
 * Allocates memory and copies the value of the `topic_type`
 * argument to set the data structure's `topic_type` member.
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
 *   Setting a member is a reentrant procedure, but:
 *   - Access to the topic endpoint info data structure is not synchronized.
 *     It is not safe to read or write the `topic_type` member of the given `topic_endpoint`
 *     while setting it.
 *   - Access to C-style string arguments is read-only but it is not synchronized.
 *     Concurrent `topic_type` reads are safe, but concurrent reads and writes are not.
 *   - The default allocators are thread-safe objects, but any custom `allocator` may not be.
 *     Check your allocator documentation for further reference.
 *
 * \pre Given `topic_type` is a valid C-style string i.e. NULL terminated.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] topic_type Type name to be set.
 * \param[in] allocator Allocator to be used.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_type` is NULL, or
 * \returns `RMW_RET_BAD_ALLOC` if memory allocation fails, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_topic_type(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * topic_type,
  rcutils_allocator_t * allocator);

/// Set the topic type hash in the given topic endpoint info data structure.
/**
 * Assigns the value of the `topic_type_hash` argument to the data structure's
 * `topic_type_hash` member.
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
 *   Setting a member is a reentrant procedure, but:
 *   - Access to the topic endpoint info data structure is not synchronized.
 *     It is not safe to read or write the `topic_type_hash` member of the given `topic_endpoint`
 *     while setting it.
 *     Concurrent `topic_type_hash` reads are safe, but concurrent reads and writes are not.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] topic_type_hash Topic type hash to be copied.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_type_hash` is NULL, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_topic_type_hash(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const rosidl_type_hash_t * type_hash);

/// Set the node name in the given topic endpoint info data structure.
/**
 * Allocates memory and copies the value of the `node_name`
 * argument to set the data structure's `node_name` member.
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
 *   Setting a member is a reentrant procedure, but:
 *   - Access to the topic endpoint info data structure is not synchronized.
 *     It is not safe to read or write the `node_name` member of the given `topic_endpoint`
 *     while setting it.
 *   - Access to C-style string arguments is read-only but it is not synchronized.
 *     Concurrent `node_name` reads are safe, but concurrent reads and writes are not.
 *   - The default allocators are thread-safe objects, but any custom `allocator` may not be.
 *     Check your allocator documentation for further reference.
 *
 * \pre Given `node_name` is a valid C-style string i.e. NULL terminated.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] node_name Node name to be set.
 * \param[in] allocator Allocator to be used.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `node_name` is NULL, or
 * \returns `RMW_RET_BAD_ALLOC` if memory allocation fails, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_node_name(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * node_name,
  rcutils_allocator_t * allocator);

/// Set the node namespace in the given topic endpoint info data structure.
/**
 * Allocates memory and copies the value of the `node_namespace`
 * argument to set the data structure's `node_namespace` member.
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
 *   Setting a member is a reentrant procedure, but:
 *   - Access to the topic endpoint info data structure is not synchronized.
 *     It is not safe to read or write the `node_namespace` member of the given `topic_endpoint`
 *     while setting it.
 *   - Access to C-style string arguments is read-only but it is not synchronized.
 *     Concurrent `node_namespace` reads are safe, but concurrent reads and writes are not.
 *   - The default allocators are thread-safe objects, but any custom `allocator` may not be.
 *     Check your allocator documentation for further reference.
 *
 * \pre Given `node_namespace` is a valid C-style string i.e. NULL terminated.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] node_namespace Node namespace to be set.
 * \param[in] allocator Allocator to be used.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `node_namespace` is NULL, or
 * \returns `RMW_RET_BAD_ALLOC` if memory allocation fails, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_node_namespace(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * node_namespace,
  rcutils_allocator_t * allocator);

/// Set the endpoint type in the given topic endpoint info data structure.
/**
 * Assigns the value of the `type` argument to the data structure's
 * `endpoint_type` member.
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
 *   Setting a member is a reentrant procedure, but access to the
 *   topic endpoint info data structure is not synchronized.
 *   It is not safe to read or write the `endpoint_type` member of the
 *   given `topic_endpoint` while setting it.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] type Endpoint type to be set.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_endpoint_type(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rmw_endpoint_type_t type);

/// Set the endpoint gid in the given topic endpoint info data structure.
/**
 * Copies the value of the `gid` argument to the data structure's
 * `endpoint_gid` member.
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
 *   Setting a member is a reentrant procedure, but access to the
 *   topic endpoint info data structure is not synchronized.
 *   It is not safe to read or write the `gid` member of the
 *   given `topic_endpoint` while setting it.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] gid Endpoint gid to be set.
 * \param[in] size Size of the given `gid`.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `gid` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `size` is greater than RMW_GID_STORAGE_SIZE, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_gid(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const uint8_t * gid,
  size_t size);

/// Set the endpoint QoS profile in the given topic endpoint info data structure.
/**
 * Assigns the value of the `qos_profile` argument to the data structure's
 * `qos_profile` member.
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
 *   Setting a member is a reentrant procedure, but access to the
 *   topic endpoint info data structure is not synchronized.
 *   It is not safe to read or write the `qos_profile` member of the
 *   given `topic_endpoint` while setting it.
 *
 * \param[inout] topic_endpoint_info Data structure to be populated.
 * \param[in] qos_profile QoS profile to be set.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `topic_endpoint_info` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `qos_profile` is NULL, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_topic_endpoint_info_set_qos_profile(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const rmw_qos_profile_t * qos_profile);

#ifdef __cplusplus
}
#endif

#endif    // RMW__TOPIC_ENDPOINT_INFO_H_
