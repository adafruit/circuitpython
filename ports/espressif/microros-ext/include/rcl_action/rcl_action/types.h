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

#ifndef RCL_ACTION__TYPES_H_
#define RCL_ACTION__TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/visibility_control.h"

#include "action_msgs/msg/goal_info.h"
#include "action_msgs/msg/goal_status.h"
#include "action_msgs/msg/goal_status_array.h"
#include "action_msgs/srv/cancel_goal.h"

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/types.h"

#include "rosidl_runtime_c/action_type_support_struct.h"

// rcl action specific ret codes in 2XXX
/// Action name does not pass validation return code.
#define RCL_RET_ACTION_NAME_INVALID 2000
/// Action goal accepted return code.
#define RCL_RET_ACTION_GOAL_ACCEPTED 2100
/// Action goal rejected return code.
#define RCL_RET_ACTION_GOAL_REJECTED 2101
/// Action client is invalid return code.
#define RCL_RET_ACTION_CLIENT_INVALID 2102
/// Action client failed to take response return code.
#define RCL_RET_ACTION_CLIENT_TAKE_FAILED 2103
/// Action server is invalid return code.
#define RCL_RET_ACTION_SERVER_INVALID 2200
/// Action server failed to take request return code.
#define RCL_RET_ACTION_SERVER_TAKE_FAILED 2201
/// Action goal handle invalid return code.
#define RCL_RET_ACTION_GOAL_HANDLE_INVALID 2300
/// Action invalid event return code.
#define RCL_RET_ACTION_GOAL_EVENT_INVALID 2301

// TODO(jacobperron): Move these to a common place for UUIDs
#define UUID_SIZE 16
#define uuidcmp(uuid0, uuid1) (0 == memcmp(uuid0, uuid1, UUID_SIZE))
#define zerouuid (uint8_t[UUID_SIZE]) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define uuidcmpzero(uuid) uuidcmp(uuid, (zerouuid))

// Typedef generated messages for convenience
typedef action_msgs__msg__GoalInfo rcl_action_goal_info_t;
typedef action_msgs__msg__GoalStatus rcl_action_goal_status_t;
/// Struct with the action goal status array
typedef struct rcl_action_goal_status_array_s
{
  /// Goal status array message
  action_msgs__msg__GoalStatusArray msg;
  /// Allocator used to initialize this struct.
  rcl_allocator_t allocator;
} rcl_action_goal_status_array_t;
typedef action_msgs__srv__CancelGoal_Request rcl_action_cancel_request_t;
/// Struct with the action cancel response
typedef struct rcl_action_cancel_response_s
{
  /// Cancel goal response message
  action_msgs__srv__CancelGoal_Response msg;
  /// Allocator used to initialize this struct.
  rcl_allocator_t allocator;
} rcl_action_cancel_response_t;

/// Goal states
// TODO(jacobperron): Let states be defined by action_msgs/msg/goal_status.h
// Ideally, we could use an enum type directly from the message when the feature
// is available. Issue: https://github.com/ros2/rosidl/issues/260
typedef int8_t rcl_action_goal_state_t;
#define GOAL_STATE_UNKNOWN action_msgs__msg__GoalStatus__STATUS_UNKNOWN
#define GOAL_STATE_ACCEPTED action_msgs__msg__GoalStatus__STATUS_ACCEPTED
#define GOAL_STATE_EXECUTING action_msgs__msg__GoalStatus__STATUS_EXECUTING
#define GOAL_STATE_CANCELING action_msgs__msg__GoalStatus__STATUS_CANCELING
#define GOAL_STATE_SUCCEEDED action_msgs__msg__GoalStatus__STATUS_SUCCEEDED
#define GOAL_STATE_CANCELED action_msgs__msg__GoalStatus__STATUS_CANCELED
#define GOAL_STATE_ABORTED action_msgs__msg__GoalStatus__STATUS_ABORTED
#define GOAL_STATE_NUM_STATES 7

/// User friendly error messages for invalid trasntions
// Description variables in types.c should be changed if enum values change
extern const char * goal_state_descriptions[];
extern const char * goal_event_descriptions[];

/// Goal state transition events
typedef enum rcl_action_goal_event_e
{
  GOAL_EVENT_EXECUTE = 0,
  GOAL_EVENT_CANCEL_GOAL,
  GOAL_EVENT_SUCCEED,
  GOAL_EVENT_ABORT,
  GOAL_EVENT_CANCELED,
  GOAL_EVENT_NUM_EVENTS
} rcl_action_goal_event_t;

/// Return a rcl_action_goal_info_t with members set to zero values.
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_goal_info_t
rcl_action_get_zero_initialized_goal_info(void);

/// Return a rcl_action_goal_status_array_t with members set to `NULL`.
/**
 * Should be called to get a null rcl_action_goal_status_array_t before passing to
 * rcl_action_server_get_goal_status_array().
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_goal_status_array_t
rcl_action_get_zero_initialized_goal_status_array(void);

/// Return a rcl_action_cancel_request_t with members set to `NULL`.
/**
 * Should be called to get a null rcl_action_cancel_request_t before passing to
 *
 * rcl_action_cancel_request_init().
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_cancel_request_t
rcl_action_get_zero_initialized_cancel_request(void);

/// Return a rcl_action_cancel_response_t with members set to `NULL`.
/**
 * Should be called to get a null rcl_action_cancel_response_t before passing to
 * rcl_action_cancel_response_init().
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_cancel_response_t
rcl_action_get_zero_initialized_cancel_response(void);

/// Initialize a rcl_action_goal_status_array_t.
/**
 * After calling this function on a rcl_action_goal_status_array_t, it can be populated
 * and used to get and send status array messages with an action server using
 * rcl_action_get_goal_status_array() and rcl_action_publish_status() respectively.
 *
 * Example usage:
 *
 * ```c
 * #include <rcl/rcl.h>
 * #include <rcl_action/rcl_action.h>
 *
 * rcl_action_goal_status_array_t goal_status_array =
 *   rcl_action_get_zero_initialized_goal_status_array();
 * size_t num_status = 42;
 * ret = rcl_action_goal_status_array_init(
 *   &goal_status_array,
 *   num_status,
 *   rcl_get_default_allocator());
 * // ... error handling, and when done with message, finalize
 * ret = rcl_action_goal_status_array_fini(&goal_status_array, rcl_get_default_allocator());
 * // ... error handling
 * ```
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[out] status_array a preallocated, zero-initialized, goal status array message
 *   to be initialized.
 * \param[in] num_status the number of status messages to allocate space for.
 *   Must be greater than zero
 * \param[in] allocator a valid allocator
 * \return `RCL_RET_OK` if cancel response was initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ALREADY_INIT` if the status array has already been initialized, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_goal_status_array_init(
  rcl_action_goal_status_array_t * status_array,
  const size_t num_status,
  const rcl_allocator_t allocator);

/// Finalize a rcl_action_goal_status_array_t.
/**
 * After calling, the goal status array message will no longer be valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] status_array the goal status array message to be deinitialized
 * \return `RCL_RET_OK` if the goal status array was deinitialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_goal_status_array_fini(rcl_action_goal_status_array_t * status_array);

/// Initialize a rcl_action_cancel_response_t.
/**
 * After calling this function on a rcl_action_cancel_response_t, it can be populated
 * and used to process cancel requests with an action server using
 * rcl_action_process_cancel_request().
 *
 * Example usage:
 *
 * ```c
 * #include <rcl/rcl.h>
 * #include <rcl_action/rcl_action.h>
 *
 * rcl_action_cancel_response_t cancel_response =
 *   rcl_action_get_zero_initialized_cancel_response();
 * size_t num_goals_canceling = 10;
 * ret = rcl_action_cancel_response_init(
 *   &cancel_response,
 *   num_goals_canceling,
 *   rcl_get_default_allocator());
 * // ... error handling, and when done processing response, finalize
 * ret = rcl_action_cancel_response_fini(&cancel_response, rcl_get_default_allocator());
 * // ... error handling
 * ```
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[out] cancel_response a preallocated, zero-initialized, cancel response message
 *   to be initialized.
 * \param[in] num_goals_canceling the number of goals that are canceling to add to the response
 *   Must be greater than zero
 * \param[in] allocator a valid allocator
 * \return `RCL_RET_OK` if cancel response was initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ALREADY_INIT` if the cancel response has already been initialized, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_cancel_response_init(
  rcl_action_cancel_response_t * cancel_response,
  const size_t num_goals_canceling,
  const rcl_allocator_t allocator);

/// Finalize a rcl_action_cancel_response_t.
/**
 * After calling, the cancel response message will no longer be valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] cancel_response the cancel response message to be deinitialized
 * \return `RCL_RET_OK` if the cancel response was deinitialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_cancel_response_fini(rcl_action_cancel_response_t * cancel_response);

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__TYPES_H_
