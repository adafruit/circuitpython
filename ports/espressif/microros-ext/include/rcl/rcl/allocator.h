// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__ALLOCATOR_H_
#define RCL__ALLOCATOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcutils/allocator.h"

/// Encapsulation of an allocator.
/**
 * \sa rcutils_allocator_t
 */
typedef rcutils_allocator_t rcl_allocator_t;

/// Return a properly initialized rcl_allocator_t with default values.
/**
 * \sa rcutils_get_default_allocator()
 */
#define rcl_get_default_allocator rcutils_get_default_allocator

/// Emulate the behavior of [reallocf](https://linux.die.net/man/3/reallocf).
/**
 * \sa rcutils_reallocf()
 */
#define rcl_reallocf rcutils_reallocf

/// Check that the given allocator is initialized.
/**
 * If the allocator is not initialized, run the fail_statement.
 */
#define RCL_CHECK_ALLOCATOR(allocator, fail_statement) \
  RCUTILS_CHECK_ALLOCATOR(allocator, fail_statement)

/// Check that the given allocator is initialized, or fail with a message.
/**
 * If the allocator is not initialized, set the error to msg, and run the fail_statement.
 */
#define RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, msg, fail_statement) \
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(allocator, msg, fail_statement)

#ifdef __cplusplus
}
#endif

#endif  // RCL__ALLOCATOR_H_
