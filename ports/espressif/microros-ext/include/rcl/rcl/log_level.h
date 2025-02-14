// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__LOG_LEVEL_H_
#define RCL__LOG_LEVEL_H_

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// typedef for RCUTILS_LOG_SEVERITY;
typedef enum RCUTILS_LOG_SEVERITY rcl_log_severity_t;

/// A logger item to specify a name and a log level.
typedef struct rcl_logger_setting_s
{
  /// Name for the logger.
  const char * name;
  /// Minimum log level severity of the logger.
  rcl_log_severity_t level;
} rcl_logger_setting_t;

/// Hold default logger level and other logger setting.
typedef struct rcl_log_levels_s
{
  /// Minimum default logger level severity.
  rcl_log_severity_t default_logger_level;
  /// Array of logger setting.
  rcl_logger_setting_t * logger_settings;
  /// Number of logger settings.
  size_t num_logger_settings;
  /// Capacity of logger settings.
  size_t capacity_logger_settings;
  /// Allocator used to allocate objects in this struct.
  rcl_allocator_t allocator;
} rcl_log_levels_t;

/// Return a rcl_log_levels_t struct with members initialized to zero value.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \return a rcl_log_levels_t struct with members initialized to zero value.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_log_levels_t
rcl_get_zero_initialized_log_levels(void);

/// Initialize a log levels structure.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] log_levels The structure to be initialized.
 * \param[in] allocator Memory allocator to be used and assigned into log_levels.
 * \param[in] logger_count Number of logger settings to be allocated.
 *  This reserves memory for logger_settings, but doesn't initialize it.
 * \return #RCL_RET_OK if the structure was initialized successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels is NULL, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels contains initialized memory, or
 * \return #RCL_RET_INVALID_ARGUMENT if allocator is invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_log_levels_init(
  rcl_log_levels_t * log_levels, const rcl_allocator_t * allocator, size_t logger_count);

/// Copy one log levels structure into another.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] src The structure to be copied.
 *  Its allocator is used to copy memory into the new structure.
 * \param[out] dst A log levels structure to be copied into.
 * \return #RCL_RET_OK if the structure was copied successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if src is NULL, or
 * \return #RCL_RET_INVALID_ARGUMENT if src allocator is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if dst is NULL, or
 * \return #RCL_RET_INVALID_ARGUMENT if dst contains already allocated memory, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_log_levels_copy(const rcl_log_levels_t * src, rcl_log_levels_t * dst);

/// Reclaim resources held inside rcl_log_levels_t structure.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] log_levels The structure which its resources have to be deallocated.
 * \return #RCL_RET_OK if the memory was successfully freed, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels is NULL, or
 * \return #RCL_RET_INVALID_ARGUMENT if the log_levels allocator is invalid and the structure contains initialized memory.
 */
RCL_PUBLIC
rcl_ret_t
rcl_log_levels_fini(rcl_log_levels_t * log_levels);

/// Shrink log levels structure.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] log_levels The structure to be shrunk.
 * \return #RCL_RET_OK if the memory was successfully shrunk, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels is NULL or if its allocator is invalid, or
 * \return #RCL_RET_BAD_ALLOC if reallocating memory failed.
 */
RCL_PUBLIC
rcl_ret_t
rcl_log_levels_shrink_to_size(rcl_log_levels_t * log_levels);

/// Add logger setting with a name and a level.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] log_levels The structure where to set the logger log level.
 * \param[in] logger_name Name for the logger, a copy of it will be stored in the structure.
 * \param[in] log_level Minimum log level severity to be set for logger_name.
 * \return #RCL_RET_OK if add logger setting successfully, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels is NULL, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels was not initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if log_levels allocator is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if logger_name is NULL, or
 * \return #RCL_RET_ERROR if the log_levels structure is already full.
 */
RCL_PUBLIC
rcl_ret_t
rcl_log_levels_add_logger_setting(
  rcl_log_levels_t * log_levels, const char * logger_name, rcl_log_severity_t log_level);

#ifdef __cplusplus
}
#endif

#endif  // RCL__LOG_LEVEL_H_
