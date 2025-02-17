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

#ifndef RCL_LIFECYCLE__DATA_TYPES_H_
#define RCL_LIFECYCLE__DATA_TYPES_H_

#include "rcl/rcl.h"

#include "rcl_lifecycle/visibility_control.h"

#include "lifecycle_msgs/msg/transition_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct rcl_lifecycle_transition_s rcl_lifecycle_transition_t;

/// It contains the state of the lifecycle state machine
typedef struct rcl_lifecycle_state_s
{
  /// String with state name: Unconfigured, Inactive, Active or Finalized
  const char * label;
  /// Identifier of the state
  uint8_t id;

  /// Pointer to a struct with the valid transitions
  rcl_lifecycle_transition_t * valid_transitions;
  /// Number of valid transitions
  unsigned int valid_transition_size;
} rcl_lifecycle_state_t;

/// It contains the transitions of the lifecycle state machine
typedef struct rcl_lifecycle_transition_s
{
  /// String with transition name: configuring, cleaningup, activating, deactivating,
  /// errorprocessing or shuttingdown.
  const char * label;
  /// Identifier of the transition
  unsigned int id;
  /// The value where the transition is initialized
  rcl_lifecycle_state_t * start;
  /// The objetive of the transition
  rcl_lifecycle_state_t * goal;
} rcl_lifecycle_transition_t;

/// It contains the transition map states and transitions
typedef struct rcl_lifecycle_transition_map_s
{
  /// States used to generate the transition map
  rcl_lifecycle_state_t * states;
  /// Number of states
  unsigned int states_size;
  /// Transitions used to generate the transition map
  rcl_lifecycle_transition_t * transitions;
  /// Number of transitions
  unsigned int transitions_size;
} rcl_lifecycle_transition_map_t;

/// It contains the communication interfac with the ROS world
typedef struct rcl_lifecycle_com_interface_s
{
  /// Handle to the node used to create the publisher and the services
  rcl_node_t * node_handle;
  /// Event used to publish the transitions
  rcl_publisher_t pub_transition_event;
  /// Service that allows to trigger changes on the state
  rcl_service_t srv_change_state;
  /// Service that allows to get the current state
  rcl_service_t srv_get_state;
  /// Service that allows to get the available states
  rcl_service_t srv_get_available_states;
  /// Service that allows to get the available transitions
  rcl_service_t srv_get_available_transitions;
  /// Service that allows to get transitions from the graph
  rcl_service_t srv_get_transition_graph;
  /// Cached transition event message.
  lifecycle_msgs__msg__TransitionEvent msg;
} rcl_lifecycle_com_interface_t;

/// It contains various options to configure the rcl_lifecycle_state_machine_t instance
typedef struct rcl_lifecycle_state_machine_options_s
{
  /// Flag indicating whether the state machine shall be initialized with default states
  bool initialize_default_states;
  /// Flag indicating whether the com interface shall be used or not
  bool enable_com_interface;
  /// Allocator used for allocating states and transitions
  rcl_allocator_t allocator;
} rcl_lifecycle_state_machine_options_t;

/// It contains the state machine data
typedef struct rcl_lifecycle_state_machine_s
{
  /// Current state of the state machine
  const rcl_lifecycle_state_t * current_state;
  /// Map/Associated array of registered states and transitions
  rcl_lifecycle_transition_map_t transition_map;
  /// Communication interface into a ROS world
  rcl_lifecycle_com_interface_t com_interface;
  /// Options struct with which the state machine was initialized
  rcl_lifecycle_state_machine_options_t options;
} rcl_lifecycle_state_machine_t;

#ifdef __cplusplus
}
#endif

#endif  // RCL_LIFECYCLE__DATA_TYPES_H_
