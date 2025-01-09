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

#ifndef RCL_ACTION__GOAL_STATE_MACHINE_H_
#define RCL_ACTION__GOAL_STATE_MACHINE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/types.h"
#include "rcl_action/visibility_control.h"


/// Transition a goal from one state to the next.
/**
 * Given a goal state and a goal event, return the next state.
 *
 * \param[in] state the state to transition from
 * \param[in] event the event triggering a transition
 * \return the next goal state if the transition is valid, or
 * \return `GOAL_STATE_UNKNOWN` if the transition is invalid or an error occured
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_goal_state_t
rcl_action_transition_goal_state(
  const rcl_action_goal_state_t state,
  const rcl_action_goal_event_t event);

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__GOAL_STATE_MACHINE_H_
