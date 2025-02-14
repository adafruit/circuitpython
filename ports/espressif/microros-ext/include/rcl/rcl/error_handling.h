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

#ifndef RCL__ERROR_HANDLING_H_
#define RCL__ERROR_HANDLING_H_

#include "rcutils/error_handling.h"

/// The error handling in RCL is just an alias to the error handling in rcutils.

typedef rcutils_error_state_t rcl_error_state_t;
typedef rcutils_error_string_t rcl_error_string_t;

#define rcl_initialize_error_handling_thread_local_storage \
  rcutils_initialize_error_handling_thread_local_storage

#define rcl_set_error_state rcutils_set_error_state

#define RCL_CHECK_ARGUMENT_FOR_NULL(argument, error_return_type) \
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(argument, error_return_type)

#define RCL_CHECK_FOR_NULL_WITH_MSG(value, msg, error_statement) \
  RCUTILS_CHECK_FOR_NULL_WITH_MSG(value, msg, error_statement)

#define RCL_SET_ERROR_MSG(msg) RCUTILS_SET_ERROR_MSG(msg)

#define RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(fmt_str, ...) \
  RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(fmt_str, __VA_ARGS__)

#define rcl_error_is_set rcutils_error_is_set

#define rcl_get_error_state rcutils_get_error_state

#define rcl_get_error_string rcutils_get_error_string

#define rcl_reset_error rcutils_reset_error

#endif  // RCL__ERROR_HANDLING_H_
