// Copyright 2016 Open Source Robotics Foundation, Inc.
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

/** \mainpage rcl: Common functionality ROS lifecycle
 *
 * `rcl_lifecycle` provides a pure C implementation of the ROS concept of lifecycle.
 * It builds on top of the implementation of topics and services in `rcl`.
 *
 * `rcl_lifecycle` consists of functions and structs for the following ROS lifecycle entities:
 *
 * - Lifecycle states
 * - Lifecycle transitions
 * - Lifecycle state machine
 * - Lifecycle triggers
 *
 * Some useful abstractions:
 *
 * - Return codes and other types
 *   - rcl_lifecycle/data_types.h
 */

#ifndef RCL_LIFECYCLE__RCL_LIFECYCLE_H_
#define RCL_LIFECYCLE__RCL_LIFECYCLE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#include "rcl_lifecycle/data_types.h"
#include "rcl_lifecycle/default_state_machine.h"
#include "rcl_lifecycle/visibility_control.h"

/// Return a rcl_lifecycle_state_t struct with members set to `NULL` or 0.
/**
 * Should be called to get a null rcl_lifecycle_state_t before passing to
 * rcl_lifecycle_state_init().
 *
 * \return rcl_lifecycle_state_t a initilized struct
 */
RCL_LIFECYCLE_PUBLIC
rcl_lifecycle_state_t
rcl_lifecycle_get_zero_initialized_state(void);

/// Initialize a rcl_lifecycle_state_init.
/**
 * This function initializes a state based on an `id` and a `label`.
 *
 * The given `rcl_lifecycle_state_t` must be zero initialized with the function
 * `rcl_lifecycle_get_zero_initialized_state()` and must not be already initialized
 * by this function.
 * The allocator will be used to allocate the label string.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] state pointer to the state struct to be initialized
 * \param[in] id identifier of the state
 * \param[in] label label of the state
 * \param[in] allocator a valid allocator used to initialized the lifecycle state
 * \return `RCL_RET_OK` if state was initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_state_init(
  rcl_lifecycle_state_t * state,
  uint8_t id,
  const char * label,
  const rcl_allocator_t * allocator);

/// Finalize a rcl_lifecycle_state_t.
/**
 *
 * Calling this will set the rcl_lifecycle_state_t struct into the an unitialized state that is
 * functionally the same as before rcl_lifecycle_state_init was called. This function make the
 * rcl_lifecycle_state_t invalid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] state struct to be finalized
 * \param[in] allocator a valid allocator used to finalize the lifecycle state
 * \return `RCL_RET_OK` if the state was finalized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_state_fini(
  rcl_lifecycle_state_t * state,
  const rcl_allocator_t * allocator);

/// Return a rcl_lifecycle_transition_t struct with members set to `NULL` or 0.
/**
 * Should be called to get a null rcl_lifecycle_transition_t before passing to
 * rcl_lifecycle_transition_init().
 */
RCL_LIFECYCLE_PUBLIC
rcl_lifecycle_transition_t
rcl_lifecycle_get_zero_initialized_transition(void);

/// Initialize a transition from a start state to the goal state.
/**
 * The given `rcl_lifecycle_transition_t` must be zero initialized with the function
 * `rcl_lifecycle_get_zero_initialized_transition()` and must not be already initialized
 * by this function.
 * The allocator will be used to allocate the label string and the rcl_lifecycle_state_t structs.
 *
 * Note: the transition pointer will take ownership
 * of the start and goal state. When calling
 * rcl_lifecycle_transition_fini(), the two states
 * will be freed.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition to a preallocated, zero-initialized transition structure
 *    to be initialized.
 * \param[in] id identifier of the transition
 * \param[in] label label of the transition
 * \param[in] start the value where the transition is initialized
 * \param[in] goal the objetive of the transition
 * \param[in] allocator a valid allocator used to finalize the lifecycle state
 * \return `RCL_RET_OK` if the transition is initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_transition_init(
  rcl_lifecycle_transition_t * transition,
  unsigned int id,
  const char * label,
  rcl_lifecycle_state_t * start,
  rcl_lifecycle_state_t * goal,
  const rcl_allocator_t * allocator);

/// Finalize a rcl_lifecycle_transition_t.
/**
 * Calling this will set the rcl_lifecycle_transition_t struct into the an unitialized state that is
 * functionally the same as before rcl_lifecycle_transition_init was called. This function make the
 * rcl_lifecycle_transition_t invalid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] transition struct to be finalized
 * \param[in] allocator a valid allocator used to finalize the transition
 * \return `RCL_RET_OK` if the state was finalized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_transition_fini(
  rcl_lifecycle_transition_t * transition,
  const rcl_allocator_t * allocator);

/// Return a default initialized state machine options struct.
RCL_LIFECYCLE_PUBLIC
rcl_lifecycle_state_machine_options_t
rcl_lifecycle_get_default_state_machine_options(void);

/// Return a rcl_lifecycle_state_machine_t struct with members set to `NULL` or 0.
/**
 * Should be called to get a null rcl_lifecycle_state_machine_t before passing to
 * rcl_lifecycle_state_machine_init().
 */
RCL_LIFECYCLE_PUBLIC
rcl_lifecycle_state_machine_t
rcl_lifecycle_get_zero_initialized_state_machine(void);

/// Initialize state machine
/**
 * This function initialize the state machine: one publisher to publish transitions messages
 * and a set of services to get information about states and transitions.
 * If `default_state` is `true` then a new default state machine is initialized.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] state_machine struct to be initialized
 * \param[in] node_handle a valid (not finalized) handle to the node used to create the publisher
 *    and the services
 * \param[in] ts_pub_notify pointer to transition publisher, it used to publish the transitions
 * \param[in] ts_srv_change_state pointer to the service that allows to trigger changes on the state
 * \param[in] ts_srv_get_state pointer to the service that allows to get the current state
 * \param[in] ts_srv_get_available_states pointer to the service that allows to get the available states
 * \param[in] ts_srv_get_available_transitions pointer to the service that allows to get the
 *    available transitions
 * \param[in] ts_srv_get_transition_graph pointer to the service that allows to get transitions from
 *    the graph
 * \param[in] state_machine_options collection of config options for initializing the state machine
 * \return `RCL_RET_OK` if the state machine was initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if input params is NULL, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_state_machine_init(
  rcl_lifecycle_state_machine_t * state_machine,
  rcl_node_t * node_handle,
  const rosidl_message_type_support_t * ts_pub_notify,
  const rosidl_service_type_support_t * ts_srv_change_state,
  const rosidl_service_type_support_t * ts_srv_get_state,
  const rosidl_service_type_support_t * ts_srv_get_available_states,
  const rosidl_service_type_support_t * ts_srv_get_available_transitions,
  const rosidl_service_type_support_t * ts_srv_get_transition_graph,
  const rcl_lifecycle_state_machine_options_t * state_machine_options);

/// Finalize a rcl_lifecycle_state_machine_t.
/**
 * Calling this will set the rcl_lifecycle_state_machine_t struct into the an unitialized state that is
 * functionally the same as before rcl_lifecycle_state_machine_init was called. This function make the
 * rcl_lifecycle_state_machine_t invalid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] state_machine struct to be finalized
 * \param[in] node_handle valid (not finalized) handle to the node
 * \return `RCL_RET_OK` if the state was finalized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_state_machine_fini(
  rcl_lifecycle_state_machine_t * state_machine,
  rcl_node_t * node_handle);

/// Check if a state machine is active.
/**
 * If the state is initialized then returns `RCL_RET_OK`, otherwise returns `RCL_RET_ERROR`
 * In the case where `RCL_RET_ERROR` is to be returned, an error message is set.
 * This function cannot fail.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state_machine pointer to the state machine struct
 * \return `RCL_RET_OK` if the state is initialized, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_state_machine_is_initialized(
  const rcl_lifecycle_state_machine_t * state_machine);

/// Get a state by id.
/**
 * A pointer to the internally transition struct is returned
 * based on the `id`. If the `id` is not set in the state then returns NULL.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state pointer to the state struct
 * \param[in] id identifier to be find in the valid transitions
 * \return a pointer to the lifecycle transition if the `id` exists or otherwise it return NULL
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
const rcl_lifecycle_transition_t *
rcl_lifecycle_get_transition_by_id(
  const rcl_lifecycle_state_t * state,
  uint8_t id);

/// Get a state by id.
/**
 * A pointer to the internally transition struct is returned
 * based on the `label`. If the `label` is not set in the state then returns NULL.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state pointer to the state struct
 * \param[in] label label to be find in the valid transitions
 * \return a pointer to the lifecycle transition if the label exists or otherwise it return NULL
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
const rcl_lifecycle_transition_t *
rcl_lifecycle_get_transition_by_label(
  const rcl_lifecycle_state_t * state,
  const char * label);

/// Trigger a state by id.
/**
 * This function will trigger a transition based on the `id`. If the argument
 * `publish_notification` is `true` then a message will be published in the
 * ROS 2 network notifying the transition, if `false` no message will be published.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state_machine pointer to the state machine struct
 * \param[in] id identifier of the transition to be triggered
 * \param[in] publish_notification if the value is `true` a message will be published
 *    notifying the transition, otherwise no message will be published
 * \return `RCL_RET_OK` if the transition was triggered successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_trigger_transition_by_id(
  rcl_lifecycle_state_machine_t * state_machine,
  uint8_t id,
  bool publish_notification);

/// Trigger a state by label.
/**
 * This function will trigger a transition base on the `label`. If the argument
 * `publish_notification` is `true` then a message will be published in the
 * ROS 2 network notifying the transition, if `false` no message will be published.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state_machine pointer to the state machine struct
 * \param[in] label of the transition to be triggered
 * \param[in] publish_notification if the value is `true` a message will be published
 *    notifying the transition, otherwise no message will be published
 * \return `RCL_RET_OK` if the transition was triggered successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_trigger_transition_by_label(
  rcl_lifecycle_state_machine_t * state_machine,
  const char * label,
  bool publish_notification);

/// Print the state machine data
/**
 * This function will print in the standard output the data in the
 * rcl_lifecycle_state_machine_t struct.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] state_machine pointer to the state machine struct to print
 */
RCL_LIFECYCLE_PUBLIC
void
rcl_print_state_machine(const rcl_lifecycle_state_machine_t * state_machine);

#ifdef __cplusplus
}
#endif  // extern "C"

#endif  // RCL_LIFECYCLE__RCL_LIFECYCLE_H_
