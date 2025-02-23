// Copyright (c) 2019 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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
#ifndef RCLC__SUBSCRIPTION_H_
#define RCLC__SUBSCRIPTION_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl/subscription.h>
#include <rclc/types.h>
#include <rclc/init.h>
#include "rclc/visibility_control.h"

/**
 *  Creates an rcl subscription.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] subscription -  a zero-initialized rcl_subscription_t
 * \param[in] node the rcl node
 * \param[in] type_support the message data type
 * \param[in] topic_name the name of subscribed topic
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_subscription_init_default(
  rcl_subscription_t * subscription,
  rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name);

/**
 *  Creates an rcl subscription with quality of service option best effort
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] subscription -  a zero-initialized rcl_subscription_t
 * \param[in] node the rcl node
 * \param[in] type_support the message data type
 * \param[in] topic_name the name of subscribed topic
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_subscription_init_best_effort(
  rcl_subscription_t * subscription,
  rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name);


/**
 *  Creates an rcl subscription with defined QoS
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] subscription -  a zero-initialized rcl_subscription_t
 * \param[in] node the rcl node
 * \param[in] type_support the message data type
 * \param[in] topic_name the name of subscribed topic
 * \param[in] qos_profile the qos of the topic
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_subscription_init(
  rcl_subscription_t * subscription,
  rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rmw_qos_profile_t * qos_profile);

#if __cplusplus
}
#endif

#endif  // RCLC__SUBSCRIPTION_H_
