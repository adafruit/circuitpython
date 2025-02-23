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
#ifndef RCLC__TIMER_H_
#define RCLC__TIMER_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl/timer.h>
#include <rclc/types.h>
#include "rclc/visibility_control.h"

/**
 *  Creates an rcl timer.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes (in RCL)
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] timer a zero-initialized rcl_timer_t
 * \param[in] support the rclc_support_t object
 * \param[in] timeout_ns the time out in nanoseconds of the timer
 * \param[in] callback the callback of the timer
 * \param[in] autostart the state of the timer at initialization
 * \return `RCL_RET_OK` if successful
 * \return `RCL_ERROR` (or other error code) if an error occurred
 */
RCLC_PUBLIC
rcl_ret_t
rclc_timer_init_default2(
  rcl_timer_t * timer,
  rclc_support_t * support,
  const uint64_t timeout_ns,
  const rcl_timer_callback_t callback,
  bool autostart);

/**
 * \deprecated `rclc_timer_init_default` implementation was removed.
 *   Refer to `rclc_timer_init_default2`.
 */
RCL_PUBLIC
RCUTILS_DEPRECATED_WITH_MSG("Call rclc_timer_init_default2 instead")
rcl_ret_t
rclc_timer_init_default(
  rcl_timer_t * timer,
  rclc_support_t * support,
  const uint64_t timeout_ns,
  const rcl_timer_callback_t callback);

#if __cplusplus
}
#endif

#endif  // RCLC__TIMER_H_
