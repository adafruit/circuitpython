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

#ifndef RCL_LIFECYCLE__DEFAULT_STATE_MACHINE_H_
#define RCL_LIFECYCLE__DEFAULT_STATE_MACHINE_H_

#include "rcl/macros.h"
#include "rcl/types.h"

#include "rcl_lifecycle/data_types.h"
#include "rcl_lifecycle/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_configure_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_cleanup_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_activate_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_deactivate_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_shutdown_label;

RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_transition_success_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_transition_failure_label;
RCL_LIFECYCLE_PUBLIC extern const char * rcl_lifecycle_transition_error_label;

/// Initialize a default state machine
/*
 * This function initializes a default state machine. It registers all: primary states,
 * transition states, transitions and the initial state. The primary state is unconfigured.
 *
 * States: unknown, unconfigured, inactive, active and finalized.
 * Transition states: configuring, cleaningup, activating, deactivating, errorprocessing
 *                    and shuttingdown.
 * Transitions:
 *    - unconfigured to configuring
 *    - unconfigured to shuttingdown
 *    - configuring to inactive
 *    - configuring to unconfigured
 *    - configuring to errorprocessing
 *    - inactive to activating
 *    - inactive to cleaningup
 *    - inactive to shuttingdown
 *    - cleaningup to unconfigured
 *    - cleaningup to inactive
 *    - cleaniningup to errorprocessing
 *    - activating to active
 *    - activating to inactive
 *    - activating to errorprocessing
 *    - active to deactivating
 *    - active to shuttingdown
 *    - deactivating to inactive
 *    - deactivating to active
 *    - deactivating to errorprocessing
 *    - shutting down to finalized
 *    - shutting down to finalized
 *    - shutting down to errorprocessing
 *    - errorprocessing to uncofigured
 *    - errorprocessing to finalized
 *    - errorprocessing to finalized
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
 * \param[in] allocator a valid allocator used to initialized the state machine
 * \return `RCL_RET_OK` if the state machine was initialized successfully, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_LIFECYCLE_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_lifecycle_init_default_state_machine(
  rcl_lifecycle_state_machine_t * state_machine, const rcl_allocator_t * allocator);

#ifdef __cplusplus
}
#endif

#endif  // RCL_LIFECYCLE__DEFAULT_STATE_MACHINE_H_
