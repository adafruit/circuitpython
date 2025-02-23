// Copyright 2016-2017 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__GRAPH_H_
#define RCL__GRAPH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rmw/names_and_types.h>
#include <rmw/get_topic_names_and_types.h>
#include <rmw/topic_endpoint_info_array.h>

#include "rcutils/time.h"
#include "rcutils/types.h"

#include "rosidl_runtime_c/service_type_support_struct.h"

#include "rcl/macros.h"
#include "rcl/client.h"
#include "rcl/node.h"
#include "rcl/visibility_control.h"

/// A structure that contains topic names and types.
typedef rmw_names_and_types_t rcl_names_and_types_t;

/// A structure that encapsulates the node name, node namespace,
/// topic type, gid, and qos_profile or publishers and subscriptions
/// for a topic.
typedef rmw_topic_endpoint_info_t rcl_topic_endpoint_info_t;

/// An array of topic endpoint information.
typedef rmw_topic_endpoint_info_array_t rcl_topic_endpoint_info_array_t;

/// Return a zero-initialized rcl_names_and_types_t structure.
#define rcl_get_zero_initialized_names_and_types rmw_get_zero_initialized_names_and_types

/// Return a zero-initialized rcl_topic_endpoint_info_t structure.
#define rcl_get_zero_initialized_topic_endpoint_info_array \
  rmw_get_zero_initialized_topic_endpoint_info_array

/// Finalize a topic_endpoint_info_array_t structure.
#define rcl_topic_endpoint_info_array_fini rmw_topic_endpoint_info_array_fini

/// Return a list of topic names and types for publishers associated with a node.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `topic_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * There may be some demangling that occurs when listing the names from the middleware
 * implementation.
 * If the `no_demangle` argument is set to `true`, then this will be avoided and the names will be
 * returned as they appear to the middleware.
 *
 * \see rmw_get_topic_names_and_types for more details on no_demangle
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create publishers or subscribers using names returned by this function may not
 * result in the desired topic name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[in] no_demangle if true, list all topics without any demangling
 * \param[in] node_name the node name of the topics to return
 * \param[in] node_namespace the node namespace of the topics to return
 * \param[out] topic_names_and_types list of topic names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if the node name is invalid, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if the node namespace is invalid, or
 * \return #RCL_RET_NODE_NAME_NON_EXISTENT if the node name wasn't found, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_publisher_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  bool no_demangle,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * topic_names_and_types);

/// Return a list of topic names and types for subscriptions associated with a node.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `topic_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * \see rcl_get_publisher_names_and_types_by_node for details on the `no_demangle` parameter.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create publishers or subscribers using names returned by this function may not
 * result in the desired topic name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[in] no_demangle if true, list all topics without any demangling
 * \param[in] node_name the node name of the topics to return
 * \param[in] node_namespace the node namespace of the topics to return
 * \param[out] topic_names_and_types list of topic names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if the node name is invalid, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if the node namespace is invalid, or
 * \return #RCL_RET_NODE_NAME_NON_EXISTENT if the node name wasn't found, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_subscriber_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  bool no_demangle,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * topic_names_and_types);

/// Return a list of service names and types associated with a node.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `service_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `service_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * \see rcl_get_publisher_names_and_types_by_node for details on the `no_demangle` parameter.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create service clients using names returned by this function may not
 * result in the desired service name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[in] node_name the node name of the services to return
 * \param[in] node_namespace the node namespace of the services to return
 * \param[out] service_names_and_types list of service names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if the node name is invalid, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if the node namespace is invalid, or
 * \return #RCL_RET_NODE_NAME_NON_EXISTENT if the node name wasn't found, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_service_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * service_names_and_types);

/// Return a list of service client names and types associated with a node.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `service_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `service_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * \see rcl_get_publisher_names_and_types_by_node for details on the `no_demangle` parameter.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create service servers using names returned by this function may not
 * result in the desired service name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[in] node_name the node name of the services to return
 * \param[in] node_namespace the node namespace of the services to return
 * \param[out] service_names_and_types list of service client names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if the node name is invalid, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if the node namespace is invalid, or
 * \return #RCL_RET_NODE_NAME_NON_EXISTENT if the node name wasn't found, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_client_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * service_names_and_types);

/// Return a list of topic names and their types.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `topic_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * \see rcl_get_publisher_names_and_types_by_node for details on the `no_demangle` parameter.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create publishers or subscribers using names returned by this function may not
 * result in the desired topic name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[in] no_demangle if true, list all topics without any demangling
 * \param[out] topic_names_and_types list of topic names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if the node name is invalid, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if the node namespace is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_topic_names_and_types(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  bool no_demangle,
  rcl_names_and_types_t * topic_names_and_types);

/// Return a list of service names and their types.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `service_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `service_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create clients or services using names returned by this function may not result in
 * the desired service name being used depending on the remap rules in use.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for strings
 * \param[out] service_names_and_types list of service names and their types
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_service_names_and_types(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  rcl_names_and_types_t * service_names_and_types);

/// Initialize a rcl_names_and_types_t object.
/**
 * This function initializes the string array for the names and allocates space
 * for all the string arrays for the types according to the given size, but
 * it does not initialize the string array for each set of types.
 * However, the string arrays for each set of types is zero initialized.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] names_and_types object to be initialized
 * \param[in] size the number of names and sets of types to be stored
 * \param[in] allocator to be used to allocate and deallocate memory
 * \return #RCL_RET_OK on success, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if memory allocation fails, or
 * \return #RCL_RET_ERROR when an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_names_and_types_init(
  rcl_names_and_types_t * names_and_types,
  size_t size,
  rcl_allocator_t * allocator);

/// Finalize a rcl_names_and_types_t object.
/**
 * The object is populated when given to one of the rcl_get_*_names_and_types()
 * functions.
 * This function reclaims any resources allocated during population.
 *
 * The `names_and_types` parameter must not be `NULL`, and must point to an
 * already allocated rcl_names_and_types_t struct that was previously
 * passed to a successful rcl_get_*_names_and_types() function call.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] names_and_types struct to be finalized
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_names_and_types_fini(rcl_names_and_types_t * names_and_types);

/// Return a list of node names and their associated namespaces in the ROS graph.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `node_names` and `node_namespaces` parameters must be allocated and zero initialized.
 * `node_names` and `node_namespaces` are the output for this function,
 * and contain allocated memory.
 * Use rcutils_get_zero_initialized_string_array() for initializing an empty
 * rcutils_string_array_t struct.
 * These `node_names` and `node_namespaces` structs should therefore be passed
 * to rcutils_string_array_fini() when they are no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * Example:
 *
 * ```c
 * rcutils_string_array_t node_names =
 *   rcutils_get_zero_initialized_string_array();
 * rcutils_string_array_t node_namespaces =
 *   rcutils_get_zero_initialized_string_array();
 * rcl_ret_t ret = rcl_get_node_names(node, &node_names, &node_namespaces);
 * if (ret != RCL_RET_OK) {
 *   // ... error handling
 * }
 * // ... use the node_names struct, and when done:
 * rcutils_ret_t rcutils_ret = rcutils_string_array_fini(&node_names);
 * if (rcutils_ret != RCUTILS_RET_OK) {
 *   // ... error handling
 * }
 * rcutils_ret_t rcutils_ret = rcutils_string_array_fini(&node_namespaces);
 * if (rcutils_ret != RCUTILS_RET_OK) {
 *   // ... error handling
 * }
 * ```
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator used to control allocation and deallocation of names
 * \param[out] node_names struct storing discovered node names
 * \param[out] node_namespaces struct storing discovered node namespaces
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_BAD_ALLOC if an error occurred while allocating memory, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID_NAME if a node with an invalid name is detected, or
 * \return #RCL_RET_NODE_INVALID_NAMESPACE if a node with an invalid namespace is detected, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_node_names(
  const rcl_node_t * node,
  rcl_allocator_t allocator,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces);

/// Return a list of node names and their associated namespaces and enclaves in the ROS graph.
/**
 * An rcl_get_node_names() equivalent, but including in its output the enclave
 * name the node is using.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] RMW implementation in use may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator used to control allocation and deallocation of names
 * \param[out] node_names struct storing discovered node names
 * \param[out] node_namespaces struct storing discovered node namespaces
 * \param[out] enclaves struct storing discovered node enclaves
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_BAD_ALLOC if an error occurred while allocating memory, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_node_names_with_enclaves(
  const rcl_node_t * node,
  rcl_allocator_t allocator,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces,
  rcutils_string_array_t * enclaves);

/// Return the number of publishers on a given topic.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_name` parameter must not be `NULL`, and must not be an empty string.
 * It should also follow the topic name rules.
 *
 * See: https://design.ros2.org/articles/topic_and_service_names.html
 *
 * The `count` parameter must point to a valid bool.
 * The `count` parameter is the output for this function and will be set.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * The topic name is not automatically remapped by this function.
 * If there is a publisher created with topic name `foo` and remap rule `foo:=bar` then calling
 * this with `topic_name` set to `bar` will return a count of 1, and with `topic_name` set to `foo`
 * will return a count of 0.
 * /sa rcl_remap_topic_name()
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] topic_name the name of the topic in question
 * \param[out] count number of publishers on the given topic
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_count_publishers(
  const rcl_node_t * node,
  const char * topic_name,
  size_t * count);

/// Return the number of subscriptions on a given topic.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_name` parameter must not be `NULL`, and must not be an empty string.
 * It should also follow the topic name rules.
 *
 * See: https://design.ros2.org/articles/topic_and_service_names.html
 *
 * The `count` parameter must point to a valid bool.
 * The `count` parameter is the output for this function and will be set.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * The topic name is not automatically remapped by this function.
 * If there is a subscriber created with topic name `foo` and remap rule `foo:=bar` then calling
 * this with `topic_name` set to `bar` will return a count of 1, and with `topic_name` set to `foo`
 * will return a count of 0.
 * /sa rcl_remap_topic_name()
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] topic_name the name of the topic in question
 * \param[out] count number of subscriptions on the given topic
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_count_subscribers(
  const rcl_node_t * node,
  const char * topic_name,
  size_t * count);

/// Return the number of clients on a given service.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `service_name` parameter must not be `NULL`, and must not be an empty string.
 * It should also follow the service name rules.
 *
 * See: https://design.ros2.org/articles/topic_and_service_names.html
 *
 * The `count` parameter must point to a valid size_t.
 * The `count` parameter is the output for this function and will be set.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * The service name is not automatically remapped by this function.
 * If there is a client created with service name `foo` and remap rule `foo:=bar` then calling
 * this with `service_name` set to `bar` will return a count of 1, and with `service_name` set to `foo`
 * will return a count of 0.
 * /sa rcl_remap_service_name()
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] service_name the name of the service in question
 * \param[out] count number of clients on the given service
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_count_clients(
  const rcl_node_t * node,
  const char * service_name,
  size_t * count);

/// Return the number of servers on a given service.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `service_name` parameter must not be `NULL`, and must not be an empty string.
 * It should also follow the service name rules.
 *
 * See: https://design.ros2.org/articles/topic_and_service_names.html
 *
 * The `count` parameter must point to a valid size_t.
 * The `count` parameter is the output for this function and will be set.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * The service name is not automatically remapped by this function.
 * If there is a server created with service name `foo` and remap rule `foo:=bar` then calling
 * this with `service_name` set to `bar` will return a count of 1, and with `service_name` set to `foo`
 * will return a count of 0.
 * /sa rcl_remap_service_name()
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] service_name the name of the service in question
 * \param[out] count number of services on the given service
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_count_services(
  const rcl_node_t * node,
  const char * service_name,
  size_t * count);

/// Wait for there to be a specified number of publishers on a given topic.
/**
 * The `node` parameter must point to a valid node.
 * The nodes graph guard condition is used by this function, and therefore the caller should
 * take care not to use the guard condition concurrently in any other wait sets.
 *
 * The `allocator` parameter must point to a valid allocator.
 *
 * The `topic_name` parameter must not be `NULL`, and must not be an empty string.
 * It should also follow the topic name rules.
 *
 * This function blocks and will return when the number of publishers for `topic_name`
 * is greater than or equal to the `count` parameter, or the specified `timeout` is reached.
 *
 * The `timeout` parameter is in nanoseconds.
 * The timeout is based on system time elapsed.
 * A negative value disables the timeout (i.e. this function blocks until the number of
 * publishers is greater than or equals to `count`).
 *
 * The `success` parameter must point to a valid bool.
 * The `success` parameter is the output for this function and will be set.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator to allocate space for the rcl_wait_set_t used to wait for graph events
 * \param[in] topic_name the name of the topic in question
 * \param[in] count number of publishers to wait for
 * \param[in] timeout maximum duration to wait for publishers
 * \param[out] success `true` if the number of publishers is equal to or greater than count, or
 *   `false` if a timeout occurred waiting for publishers.
 * \return #RCL_RET_OK if there was no errors, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_TIMEOUT if a timeout occurs before the number of publishers is detected, or
 * \return #RCL_RET_ERROR if an unspecified error occurred.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_for_publishers(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * topic_name,
  const size_t count,
  rcutils_duration_value_t timeout,
  bool * success);

/// Wait for there to be a specified number of subscribers on a given topic.
/**
 * \see rcl_wait_for_publishers
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator to allocate space for the rcl_wait_set_t used to wait for graph events
 * \param[in] topic_name the name of the topic in question
 * \param[in] count number of subscribers to wait for
 * \param[in] timeout maximum duration to wait for subscribers
 * \param[out] success `true` if the number of subscribers is equal to or greater than count, or
 *   `false` if a timeout occurred waiting for subscribers.
 * \return #RCL_RET_OK if there was no errors, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_TIMEOUT if a timeout occurs before the number of subscribers is detected, or
 * \return #RCL_RET_ERROR if an unspecified error occurred.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_for_subscribers(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * topic_name,
  const size_t count,
  rcutils_duration_value_t timeout,
  bool * success);

/// Return a list of all publishers to a topic.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_name` parameter must not be `NULL`.
 *
 * When the `no_mangle` parameter is `true`, the provided `topic_name` should be a valid topic name
 * for the middleware (useful when combining ROS with native middleware (e.g. DDS) apps).
 * When the `no_mangle` parameter is `false`, the provided `topic_name` should follow
 * ROS topic name conventions.
 * In either case, the topic name should always be fully qualified.
 *
 * Each element in the `publishers_info` array will contain the node name, node namespace,
 * topic type, gid and the qos profile of the publisher.
 * It is the responsibility of the caller to ensure that `publishers_info` parameter points
 * to a valid struct of type rcl_topic_endpoint_info_array_t.
 * The `count` field inside the struct must be set to 0 and the `info_array` field inside
 * the struct must be set to null.
 * \see rmw_get_zero_initialized_topic_endpoint_info_array
 *
 * The `allocator` will be used to allocate memory to the `info_array` member
 * inside of `publishers_info`.
 * Moreover, every const char * member inside of
 * rmw_topic_endpoint_info_t will be assigned a copied value on allocated memory.
 * \see rmw_topic_endpoint_info_set_node_name and the likes.
 * However, it is the responsibility of the caller to
 * reclaim any allocated resources to `publishers_info` to avoid leaking memory.
 * \see rmw_topic_endpoint_info_array_fini
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for
 *            the array inside publishers_info
 * \param[in] topic_name the name of the topic in question
 * \param[in] no_mangle if `true`, `topic_name` needs to be a valid middleware topic name,
 *            otherwise it should be a valid ROS topic name
 * \param[out] publishers_info a struct representing a list of publisher information
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if memory allocation fails, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_publishers_info_by_topic(
  const rcl_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rcl_topic_endpoint_info_array_t * publishers_info);

/// Return a list of all subscriptions to a topic.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `topic_name` parameter must not be `NULL`.
 *
 * When the `no_mangle` parameter is `true`, the provided `topic_name` should be a valid topic name
 * for the middleware (useful when combining ROS with native middleware (e.g. DDS) apps).
 * When the `no_mangle` parameter is `false`, the provided `topic_name` should follow
 * ROS topic name conventions.
 * In either case, the topic name should always be fully qualified.
 *
 * Each element in the `subscriptions_info` array will contain the node name, node namespace,
 * topic type, gid and the qos profile of the subscription.
 * It is the responsibility of the caller to ensure that `subscriptions_info` parameter points
 * to a valid struct of type rcl_topic_endpoint_info_array_t.
 * The `count` field inside the struct must be set to 0 and the `info_array` field inside
 * the struct must be set to null.
 * \see rmw_get_zero_initialized_topic_endpoint_info_array
 *
 * The `allocator` will be used to allocate memory to the `info_array` member
 * inside of `subscriptions_info`.
 * Moreover, every const char * member inside of
 * rmw_topic_endpoint_info_t will be assigned a copied value on allocated memory.
 * \see rmw_topic_endpoint_info_set_node_name and the likes.
 * However, it is the responsibility of the caller to
 * reclaim any allocated resources to `subscriptions_info` to avoid leaking memory.
 * \see rmw_topic_endpoint_info_array_fini
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] allocator allocator to be used when allocating space for
 *            the array inside publishers_info
 * \param[in] topic_name the name of the topic in question
 * \param[in] no_mangle if `true`, `topic_name` needs to be a valid middleware topic name,
 *            otherwise it should be a valid ROS topic name
 * \param[out] subscriptions_info a struct representing a list of subscriptions information
 * \return #RCL_RET_OK if the query was successful, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if memory allocation fails, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_get_subscriptions_info_by_topic(
  const rcl_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rcl_topic_endpoint_info_array_t * subscriptions_info);

/// Check if a service server is available for the given service client.
/**
 * This function will return true for `is_available` if there is a service server
 * available for the given client.
 *
 * The `node` parameter must point to a valid node.
 *
 * The `client` parameter must point to a valid client.
 *
 * The given client and node must match, i.e. the client must have been created
 * using the given node.
 *
 * The `is_available` parameter must not be `NULL`, and must point a bool variable.
 * The result of the check will be stored in the `is_available` parameter.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] client the handle to the service client being queried
 * \param[out] is_available set to true if there is a service server available, else false
 * \return #RCL_RET_OK if the check was made successfully (regardless of the service readiness), or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_service_server_is_available(
  const rcl_node_t * node,
  const rcl_client_t * client,
  bool * is_available);

#ifdef __cplusplus
}
#endif

#endif  // RCL__GRAPH_H_
