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

/// @file

#ifndef RCL__LOGGING_H_
#define RCL__LOGGING_H_

#include "rcl/allocator.h"
#include "rcl/arguments.h"
#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

#include "rcutils/logging.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// The function signature to log messages.
typedef rcutils_logging_output_handler_t rcl_logging_output_handler_t;

/// Configure the logging system.
/**
 * This function should be called during the ROS initialization process.
 * It will add the enabled log output appenders to the root logger.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] global_args The global arguments for the system
 * \param[in] allocator Used to allocate memory used by the logging system
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if a general error occurs
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_logging_configure(
  const rcl_arguments_t * global_args,
  const rcl_allocator_t * allocator);

/// Configure the logging system with the provided output handler.
/**
 * Similar to rcl_logging_configure, but it uses the provided output handler.
 * \sa rcl_logging_configure
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] global_args The global arguments for the system
 * \param[in] allocator Used to allocate memory used by the logging system
 * \param[in] output_handler Output handler to be installed
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_ERROR if a general error occurs
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_logging_configure_with_output_handler(
  const rcl_arguments_t * global_args,
  const rcl_allocator_t * allocator,
  rcl_logging_output_handler_t output_handler);

/**
 * This function should be called to tear down the logging setup by the configure function.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \return #RCL_RET_OK if successful.
 * \return #RCL_RET_ERROR if a general error occurs
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_logging_fini(void);

/// See if logging rosout is enabled.
/**
 * This function is meant to be used to check if logging rosout is enabled.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \return `TRUE` if logging rosout is enabled, or
 * \return `FALSE` if logging rosout is disabled.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
bool rcl_logging_rosout_enabled(void);

/// Default output handler used by rcl.
/**
 * This function can be wrapped in a language specific client library,
 * adding the necessary mutual exclusion protection there, and then use
 * rcl_logging_configure_with_output_handler() instead of
 * rcl_logging_configure().
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] location The pointer to the location struct or NULL
 * \param[in] severity The severity level
 * \param[in] name The name of the logger, must be null terminated c string
 * \param[in] timestamp The timestamp for when the log message was made
 * \param[in] format The list of arguments to insert into the formatted log message
 * \param[in] args argument for the string format
 */
RCL_PUBLIC
void
rcl_logging_multiple_output_handler(
  const rcutils_log_location_t * location,
  int severity, const char * name, rcutils_time_point_value_t timestamp,
  const char * format, va_list * args);

#ifdef __cplusplus
}
#endif

#endif  // RCL__LOGGING_H_
