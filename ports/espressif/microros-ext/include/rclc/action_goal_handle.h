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
#ifndef RCLC__ACTION_GOAL_HANDLE_H_
#define RCLC__ACTION_GOAL_HANDLE_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl_action/rcl_action.h>

struct rclc_generic_entity_t;
struct rclc_action_client_t;

typedef struct rclc_action_goal_handle_t
{
  struct rclc_action_goal_handle_t * next;

  union {
    struct rclc_action_server_t * action_server;
    struct rclc_action_client_t * action_client;
  };

  rcl_action_goal_state_t status;

  unique_identifier_msgs__msg__UUID goal_id;
  struct Generic_SendGoal_Request * ros_goal_request;

  bool available_goal_response;
  bool goal_accepted;
  bool available_feedback;
  bool available_result_response;
  bool available_cancel_response;
  bool goal_cancelled;

  // Goal requests header
  union {
    rmw_request_id_t goal_request_header;
    int64_t goal_request_sequence_number;
  };

  union {
    rmw_request_id_t result_request_header;
    int64_t result_request_sequence_number;
  };

  union {
    rmw_request_id_t cancel_request_header;
    int64_t cancel_request_sequence_number;
  };
} rclc_action_goal_handle_t;

#define DECLARE_GOAL_HANDLE_POOL \
  rclc_action_goal_handle_t * goal_handles_memory; \
  size_t goal_handles_memory_size; \
  rclc_action_goal_handle_t * free_goal_handles; \
  rclc_action_goal_handle_t * used_goal_handles;

#if __cplusplus
}
#endif

#endif  // RCLC__ACTION_GOAL_HANDLE_H_
