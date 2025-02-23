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


#ifndef RCL_LIFECYCLE__TRANSITION_MAP_H_
#define RCL_LIFECYCLE__TRANSITION_MAP_H_

#include "rcl/macros.h"

#include "rcl_lifecycle/data_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Initialize a rcl_lifecycle_state_init.
/*
 * Should be called to get a null rcl_lifecycle_transition_map_t before passing to
 * rcl_lifecycle_register_state() or rcl_lifecycle_register_transition().
 *
 * \return rcl_lifecycle_transition_map_t a initilized struct
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_lifecycle_transition_map_t
rcl_lifecycle_get_zero_initialized_transition_map(void);

/// Check if a transition map is active using a rcl_lifecycle_state_machine_t.
/*
 * The function checks if the transition map is initialized. It returns `RCL_RET_OK`
 * if the transition map is initialized successfully or `RCL_RET_ERROR` if the transition
 * map is not initialized.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition_map pointer to the transition map struct to check
 * \return `RCL_RET_OK` if the transition map is initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if the transition map is not initialized.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_transition_map_is_initialized(
  const rcl_lifecycle_transition_map_t * transition_map);

/// Finalize a rcl_lifecycle_transition_map_t.
/*
 * Calling this will set the rcl_lifecycle_transition_map_t struct into the an unitialized state that is
 * functionally the same as before rcl_lifecycle_register_state or
 * rcl_lifecycle_register_transition was called. This function make the rcl_lifecycle_transition_map_t invalid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] transition_map struct to be deinitialized
 * \param[in] allocator a valid allocator used to deinitialized the state machine
 * \return `RCL_RET_OK` if the state was deinitialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_transition_map_fini(
  rcl_lifecycle_transition_map_t * transition_map,
  const rcl_allocator_t * allocator);

/// Register a state
/*
 * This function registers a new state in the transition map.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition_map to be modified
 * \param[in] state the state to register
 * \param[in] allocator a valid allocator used to register the state machine
 * \return `RCL_RET_OK` if the state was registered successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_LIFECYCLE_STATE_REGISTERED` if state is already registered.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_register_state(
  rcl_lifecycle_transition_map_t * transition_map,
  rcl_lifecycle_state_t state,
  const rcl_allocator_t * allocator);

/// Register a transition
/*
 * This function registers a new transition in the transition map.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition_map to be modified
 * \param[in] transition the transition to register
 * \param[in] allocator a valid allocator used to register the state machine
 * \return `RCL_RET_OK` if the state was deinitialized successfully, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_LIFECYCLE_STATE_NOT_REGISTERED` if state is not registered, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_register_transition(
  rcl_lifecycle_transition_map_t * transition_map,
  rcl_lifecycle_transition_t transition,
  const rcl_allocator_t * allocator);

/// Get the state from a transition map based on the state id
/*
 * A pointer to the internally lifecycle state struct is returned based on the `id`.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition_map
 * \param[in] state_id
 * \return pointer to a rcl_lifecycle_state_t or NULL if the state id doesn't exist
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_lifecycle_state_t *
rcl_lifecycle_get_state(
  rcl_lifecycle_transition_map_t * transition_map,
  unsigned int state_id);

/// Get the state from a transition map based on the state id
/*
 * A pointer to the internally lifecycle transition struct is returned based on the `label`.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] transition_map to be modified
 * \param[in] state_id used to get the label to search
 * \return pointer to a rcl_lifecycle_state_t or NULL if the state id doesn't exist
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_lifecycle_transition_t *
rcl_lifecycle_get_transitions(
  rcl_lifecycle_transition_map_t * transition_map,
  unsigned int transition_id);

#ifdef __cplusplus
}
#endif

#endif  // RCL_LIFECYCLE__TRANSITION_MAP_H_
