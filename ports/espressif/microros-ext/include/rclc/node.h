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

#ifndef RCLC__NODE_H_
#define RCLC__NODE_H_

#if __cplusplus
extern "C"
{
#endif
#include <rcl/node.h>
#include <rclc/types.h>

#include <rclc/init.h>
#include "rclc/visibility_control.h"
/**
 *  Creates a default RCL node.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in this function and in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] name the name of the node
 * \param[in] namespace the namespace of the node
 * \param[in] support the rclc_support_t object
 * \return rcl_node_t if successful
 * \return NULL if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_node_init_default(
  rcl_node_t * node,
  const char * name,
  const char * namespace_,
  rclc_support_t * support);

/**
 *  Creates a RCL node with options.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in this function and in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] name the name of the node
 * \param[in] namespace the namespace of the node
 * \param[in] support the rclc_support_t object
 * \param[in] node_ops node options
 * \return rcl_node_t if successful
 * \return NULL if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_node_init_with_options(
  rcl_node_t * node,
  const char * name,
  const char * namespace_,
  rclc_support_t * support,
  rcl_node_options_t * node_ops);


#if __cplusplus
}
#endif

#endif  // RCLC__NODE_H_
