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

#ifndef RCL_ACTION__NAMES_H_
#define RCL_ACTION__NAMES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/visibility_control.h"
#include "rcl_action/types.h"

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/types.h"


/// Get the goal service name of an action.
/**
 * This function returns the goal service name for a given action name
 * that must be used by action clients and action servers to successfully
 * communicate with each other.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_name The name of the action whose goal service name is
 *   being returned.
 * \param[in] allocator A valid allocator to be used.
 * \param[out] goal_service_name Either an allocated string with the action
 *   goal service name, or `NULL` if the function failed to allocate memory
 *   for it. Must refer to a `NULL` pointer upon call.
 * \return `RCL_RET_OK` if the action goal service name was returned, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the action name is not valid
 *   (i.e. empty), or
 * \return `RCL_RET_INVALID_ARGUMENT` if the action name is `NULL`, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the goal service name pointer is
 *   `NULL` or points to a non-`NULL` pointer, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_goal_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** goal_service_name);

/// Get the cancel service name of an action.
/**
 * This function returns the cancel service name for a given action name
 * that must be used by action clients and action servers to successfully
 * communicate with each other.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_name The name of the action whose cancel service name
 *   is being returned.
 * \param[in] allocator A valid allocator to be used.
 * \param[out] cancel_service_name Either an allocated string with the action
 *   cancel service name, or `NULL` if the function failed to allocate memory
 *   for it. Must refer to a `NULL` pointer upon call.
 * \return `RCL_RET_OK` if the action cancel service name was returned, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the action name is not valid
 *   (i.e. empty), or
 * \return `RCL_RET_INVALID_ARGUMENT` if the action name is `NULL`, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the cancel service name is `NULL` or
 *   points to a non-`NULL` pointer, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_cancel_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** cancel_service_name);

/// Get the result service name of an action.
/**
 * This function returns the result service name for a given action name
 * that must be used by action clients and action servers to successfully
 * communicate with each other.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_name The name of the action whose result service name
 *   is being returned.
 * \param[in] allocator A valid allocator to be used.
 * \param[out] result_service_name Either an allocated string with the action
 *   result service name, or `NULL` if the function failed to allocate memory
 *   for it. Must refer to a `NULL` pointer upon call.
 * \return `RCL_RET_OK` if the action result service name was returned, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the action name is not valid
 *   (i.e. empty), or
 * \return `RCL_RET_INVALID_ARGUMENT` if the action name is `NULL`, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the result service name pointer is
 *   `NULL` or points to a non-`NULL` pointer, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_result_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** result_service_name);

/// Get the feedback topic name of an action.
/**
 * This function returns the feedback topic name for a given action name
 * that must be used by action clients and action servers to successfully
 * communicate with each other.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_name The name of the action whose feedback topic name
 *   is being returned.
 * \param[in] allocator A valid allocator to be used.
 * \param[out] feedback_topic_name Either an allocated string with the action
 *   feedback topic name, or `NULL` if the function failed to allocate memory
 *   for it. Must refer to a `NULL` pointer upon call.
 * \return `RCL_RET_OK` if the action feedback topic name was returned, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the action name is not valid
 *   (i.e. empty), or
 * \return `RCL_RET_INVALID_ARGUMENT` if the action name is `NULL`, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the feedback topic name pointer is
 *   `NULL` or points to a non-`NULL` pointer, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_feedback_topic_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** feedback_topic_name);

/// Get the status topic name of an action.
/**
 * This function returns the status topic name for a given action name
 * that must be used by action clients and action servers to successfully
 * communicate with each other.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_name The name of the action whose status topic
 *   name is being returned.
 * \param[in] allocator A valid allocator to be used.
 * \param[out] status_topic_name Either an allocated string with the action
 *   status topic name, or `NULL` if the function failed to allocate memory
 *   for it. Must refer to a `NULL` pointer upon call.
 * \return `RCL_RET_OK` if the action status topic name was returned, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the action name is not valid
 *   (i.e. empty), or
 * \return `RCL_RET_INVALID_ARGUMENT` if the action name is `NULL`, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if the status topic name pointer is
 *   `NULL` or points to a non-`NULL` pointer, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_get_status_topic_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** status_topic_name);

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__NAMES_H_
