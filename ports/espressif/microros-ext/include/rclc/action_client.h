// Copyright (c) 2021 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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
#ifndef RCLC__ACTION_CLIENT_H_
#define RCLC__ACTION_CLIENT_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl_action/rcl_action.h>
#include <rcl/allocator.h>
#include <rclc/types.h>
#include <rclc/action_goal_handle.h>
#include "rclc/visibility_control.h"

typedef void (* rclc_action_client_goal_callback_t)(
  rclc_action_goal_handle_t * goal_handle,
  bool accepted,
  void * args);
typedef void (* rclc_action_client_feedback_callback_t)(
  rclc_action_goal_handle_t * goal_handle,
  void * ros_feedback,
  void * args);
typedef void (* rclc_action_client_result_callback_t)(
  rclc_action_goal_handle_t * goal_handle,
  void * ros_result_response,
  void * args);
typedef void (* rclc_action_client_cancel_callback_t)(
  rclc_action_goal_handle_t * goal_handle,
  bool cancelled,
  void * args);

typedef struct rclc_action_client_t
{
  DECLARE_GOAL_HANDLE_POOL

  rcl_action_client_t rcl_handle;
  const rcl_allocator_t * allocator;

  // Callbacks
  rclc_action_client_goal_callback_t goal_callback;
  rclc_action_client_feedback_callback_t feedback_callback;
  rclc_action_client_result_callback_t result_callback;
  rclc_action_client_cancel_callback_t cancel_callback;

  // Action messages
  struct Generic_FeedbackMessage * ros_feedback;
  struct Generic_GetResult_Response * ros_result_response;
  action_msgs__srv__CancelGoal_Response ros_cancel_response;

  // Available flags
  bool feedback_available;
  bool status_available;
  bool goal_response_available;
  bool result_response_available;
  bool cancel_response_available;
} rclc_action_client_t;

/**
 *  Creates an rcl action client.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[inout] action_client the action client of type rclc_action_client_t to be initialized
 * \param[in] node the rcl node
 * \param[in] type_support the message data type
 * \param[in] action_name the name of the action
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error has occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_action_client_init_default(
  rclc_action_client_t * action_client,
  rcl_node_t * node,
  const rosidl_action_type_support_t * type_support,
  const char * action_name);

/**
 *  Send a goal to an action server.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[in] action_client the action client for this action request
 * \param[in] ros_request untyped ros action request
 * \param[inout] goal_handle (optional) returns the goal handle used to track the goal
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error has occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_action_send_goal_request(
  rclc_action_client_t * action_client,
  void * ros_request,
  rclc_action_goal_handle_t ** goal_handle);

/**
 *  Send a cancel request for a goal to an action server.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[in] goal_handle goal handle to be cancelled
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error has occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_action_send_cancel_request(
  rclc_action_goal_handle_t * goal_handle);

/**
 *  Fini a action client and free all resources.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[in] action_client the action client to be finialized
 * \param[in] node owner node
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error has occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_action_client_fini(
  rclc_action_client_t * action_client,
  rcl_node_t * node);

#if __cplusplus
}
#endif

#endif  // RCLC__ACTION_CLIENT_H_
