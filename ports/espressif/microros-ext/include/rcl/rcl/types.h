// Copyright 2014 Open Source Robotics Foundation, Inc.
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

/// @file

#ifndef RCL__TYPES_H_
#define RCL__TYPES_H_

#include <rmw/types.h>
#include <rcutils/types.h>

/// The type that holds an rcl return code.
typedef rmw_ret_t rcl_ret_t;

/// Success return code.
#define RCL_RET_OK RMW_RET_OK
/// Unspecified error return code.
#define RCL_RET_ERROR RMW_RET_ERROR
/// Timeout occurred return code.
#define RCL_RET_TIMEOUT RMW_RET_TIMEOUT
/// Failed to allocate memory return code.
#define RCL_RET_BAD_ALLOC RMW_RET_BAD_ALLOC
/// Invalid argument return code.
#define RCL_RET_INVALID_ARGUMENT RMW_RET_INVALID_ARGUMENT
/// Unsupported return code.
#define RCL_RET_UNSUPPORTED RMW_RET_UNSUPPORTED

// rcl specific ret codes start at 100
/// rcl_init() already called return code.
#define RCL_RET_ALREADY_INIT 100
/// rcl_init() not yet called return code.
#define RCL_RET_NOT_INIT 101
/// Mismatched rmw identifier return code.
#define RCL_RET_MISMATCHED_RMW_ID 102
/// Topic name does not pass validation.
#define RCL_RET_TOPIC_NAME_INVALID 103
/// Service name (same as topic name) does not pass validation.
#define RCL_RET_SERVICE_NAME_INVALID 104
/// Topic name substitution is unknown.
#define RCL_RET_UNKNOWN_SUBSTITUTION 105
/// rcl_shutdown() already called return code.
#define RCL_RET_ALREADY_SHUTDOWN 106
/// Resource not found
#define RCL_RET_NOT_FOUND 107

// rcl node specific ret codes in 2XX
/// Invalid rcl_node_t given return code.
#define RCL_RET_NODE_INVALID 200
/// Invalid node name return code.
#define RCL_RET_NODE_INVALID_NAME 201
/// Invalid node namespace return code.
#define RCL_RET_NODE_INVALID_NAMESPACE 202
/// Failed to find node name
#define RCL_RET_NODE_NAME_NON_EXISTENT 203

// rcl publisher specific ret codes in 3XX
/// Invalid rcl_publisher_t given return code.
#define RCL_RET_PUBLISHER_INVALID 300

// rcl subscription specific ret codes in 4XX
/// Invalid rcl_subscription_t given return code.
#define RCL_RET_SUBSCRIPTION_INVALID 400
/// Failed to take a message from the subscription return code.
#define RCL_RET_SUBSCRIPTION_TAKE_FAILED 401

// rcl service client specific ret codes in 5XX
/// Invalid rcl_client_t given return code.
#define RCL_RET_CLIENT_INVALID 500
/// Failed to take a response from the client return code.
#define RCL_RET_CLIENT_TAKE_FAILED 501

// rcl service server specific ret codes in 6XX
/// Invalid rcl_service_t given return code.
#define RCL_RET_SERVICE_INVALID 600
/// Failed to take a request from the service return code.
#define RCL_RET_SERVICE_TAKE_FAILED 601

// rcl guard condition specific ret codes in 7XX

// rcl timer specific ret codes in 8XX
/// Invalid rcl_timer_t given return code.
#define RCL_RET_TIMER_INVALID 800
/// Given timer was canceled return code.
#define RCL_RET_TIMER_CANCELED 801

// rcl wait and wait set specific ret codes in 9XX
/// Invalid rcl_wait_set_t given return code.
#define RCL_RET_WAIT_SET_INVALID 900
/// Given rcl_wait_set_t is empty return code.
#define RCL_RET_WAIT_SET_EMPTY 901
/// Given rcl_wait_set_t is full return code.
#define RCL_RET_WAIT_SET_FULL 902

// rcl argument parsing specific ret codes in 1XXX
/// Argument is not a valid remap rule
#define RCL_RET_INVALID_REMAP_RULE 1001
/// Expected one type of lexeme but got another
#define RCL_RET_WRONG_LEXEME 1002
/// Found invalid ros argument while parsing
#define RCL_RET_INVALID_ROS_ARGS 1003
/// Argument is not a valid parameter rule
#define RCL_RET_INVALID_PARAM_RULE 1010
/// Argument is not a valid log level rule
#define RCL_RET_INVALID_LOG_LEVEL_RULE 1020

// rcl event specific ret codes in 20XX
/// Invalid rcl_event_t given return code.
#define RCL_RET_EVENT_INVALID 2000
/// Failed to take an event from the event handle
#define RCL_RET_EVENT_TAKE_FAILED 2001

/// rcl_lifecycle state register ret codes in 30XX
/// rcl_lifecycle state registered
#define RCL_RET_LIFECYCLE_STATE_REGISTERED 3000
/// rcl_lifecycle state not registered
#define RCL_RET_LIFECYCLE_STATE_NOT_REGISTERED 3001

/// typedef for rmw_serialized_message_t;
typedef rmw_serialized_message_t rcl_serialized_message_t;

#endif  // RCL__TYPES_H_
