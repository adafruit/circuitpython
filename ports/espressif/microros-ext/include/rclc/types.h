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

#ifndef RCLC__TYPES_H_
#define RCLC__TYPES_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl/context.h>
#include <rcl/allocator.h>
#include <rcl/time.h>

typedef struct
{
  rcl_context_t context;
  rcl_allocator_t * allocator;
  rcl_clock_t clock;
} rclc_support_t;


/**
 * macro to print errors
 */
#ifndef PRINT_RCLC_ERROR
#define PRINT_RCLC_ERROR(rclc, rcl) \
  do { \
    RCUTILS_LOG_ERROR_NAMED( \
      ROS_PACKAGE_NAME, \
      "[" #rclc "] Error in " #rcl ": %s\n", rcutils_get_error_string().str); \
    rcl_reset_error(); \
  } while (0)
#endif

/**
 * macro to print warnings
 */
#ifndef PRINT_RCLC_WARN
#define PRINT_RCLC_WARN(rclc, rcl) \
  do { \
    RCUTILS_LOG_WARN_NAMED( \
      ROS_PACKAGE_NAME, \
      "[" #rclc "] Warning in " #rcl ": %s\n", rcutils_get_error_string().str); \
    rcl_reset_error(); \
  } while (0)
#endif

#define RCLC_UNUSED(x) (void)x

#if __cplusplus
}
#endif

#endif  // RCLC__TYPES_H_
