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

/** \mainpage rcl: Common functionality ROS actions
 *
 * `rcl_action` provides a pure C implementation of the ROS concept of an \b action.
 * It builds on top of the implementation of topics and services in `rcl`.
 *
 * `rcl_action` consists of functions and structs for the following ROS action entities:
 *
 * - Action client
 *   - rcl_action/action_client.h
 * - Action server
 *   - rcl_action/action_server.h
 * - Goal handle
 *   - rcl_action/goal_handle.h
 * - Goal state machine
 *   - rcl_action/goal_state_machine.h
 *
 * It also has some machinery that is necessary to wait on and act on these entities:
 *
 * - Wait sets for waiting on actions clients and action servers to be ready
 *   - rcl_action/wait.h
 *
 * Some useful abstractions and utilities:
 *
 * - Return codes and other types
 *   - rcl_action/types.h
 */

#ifndef RCL_ACTION__RCL_ACTION_H_
#define RCL_ACTION__RCL_ACTION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/action_client.h"
#include "rcl_action/action_server.h"
#include "rcl_action/default_qos.h"
#include "rcl_action/goal_handle.h"
#include "rcl_action/goal_state_machine.h"
#include "rcl_action/graph.h"
#include "rcl_action/types.h"
#include "rcl_action/wait.h"

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__RCL_ACTION_H_
