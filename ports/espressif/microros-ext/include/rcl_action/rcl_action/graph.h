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

#ifndef RCL_ACTION__GRAPH_H_
#define RCL_ACTION__GRAPH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/graph.h"
#include "rcl/node.h"

#include "rcl_action/visibility_control.h"

/// Get a list of action names and types for action clients associated with a node.
/**
 * The `node` parameter must point to a valid node.
 *
 * The `action_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `action_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create action clients or action servers with names returned by this function may
 * not result in the desired action name depending on the remap rules in use.
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
 * \param[in] allocator allocator for allocating space for strings
 * \param[in] node_name the node name of the actions to return
 * \param[in] node_namespace the node namespace of the actions to return
 * \param[out] action_names_and_types list of action names and their types
 * \return `RCL_RET_OK` if the query was successful, or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_client_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * action_names_and_types);

/// Get a list of action names and types for action servers associated with a node.
/**
 * This function returns a list of action names and types for action servers associated with
 * the provided node name.
 *
 * The `node` parameter must point to a valid node.
 *
 * The `action_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `action_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create action clients or action servers with names returned by this function may
 * not result in the desired action name depending on the remap rules in use.
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
 * \param[in] allocator allocator for allocating space for strings
 * \param[in] node_name the node name of the actions to return
 * \param[in] node_namespace the node namespace of the actions to return
 * \param[out] action_names_and_types list of action names and their types
 * \return `RCL_RET_OK` if the query was successful, or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_server_names_and_types_by_node(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  const char * node_name,
  const char * node_namespace,
  rcl_names_and_types_t * action_names_and_types);

/// Return a list of action names and their types.
/**
 * This function returns a list of action names and types in the ROS graph.
 *
 * The `node` parameter must point to a valid node.
 *
 * The `action_names_and_types` parameter must be allocated and zero initialized.
 * This function allocates memory for the returned list of names and types and so it is the
 * callers responsibility to pass `action_names_and_types` to rcl_names_and_types_fini()
 * when it is no longer needed.
 * Failing to do so will result in leaked memory.
 *
 * The returned names are not automatically remapped by this function.
 * Attempting to create action clients or action servers with names returned by this function may
 * not result in the desired action name depending on the remap rules in use.
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
 * \param[in] allocator allocator for allocating space for strings
 * \param[out] action_names_and_types list of action names and types
 * \return `RCL_RET_OK` if the query was successful, or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_names_and_types(
  const rcl_node_t * node,
  rcl_allocator_t * allocator,
  rcl_names_and_types_t * action_names_and_types);

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__GRAPH_H_
