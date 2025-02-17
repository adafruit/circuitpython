// Copyright 2023 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__MACROS_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__MACROS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rcutils/types/rcutils_ret.h>
#include <rcutils/error_handling.h>


/// Check return code, and return it if not ok.
/**
 * \param[in] statement_with_return_code The statement to test.
 */
#define ROSIDL_DYNAMIC_TYPESUPPORT_CHECK_RET_FOR_NOT_OK(statement_with_return_code) \
  do { \
    rcutils_ret_t macro_ret_ = statement_with_return_code; \
    if (macro_ret_ != RCUTILS_RET_OK) { \
      return macro_ret_; \
    } \
  } while (0)


/// Check return code, and if not ok, run a cleanup statement and return the return code.
/**
 * \param[in] statement_with_return_code The statement to test.
 * \param[in] cleanup_statement The statement to run before returning the non-ok return code.
 */
#define ROSIDL_DYNAMIC_TYPESUPPORT_CHECK_RET_FOR_NOT_OK_WITH_CLEANUP( \
    statement_with_return_code, cleanup_statement) \
  do { \
    rcutils_ret_t macro_ret_ = statement_with_return_code; \
    if (macro_ret_ != RCUTILS_RET_OK) { \
      cleanup_statement; \
      return macro_ret_; \
    } \
  } while (0)

#define ROSIDL_DYNAMIC_TYPESUPPORT_STRINGIFY(x) ROSIDL_DYNAMIC_TYPESUPPORT_STRINGIFY2(x)
#define ROSIDL_DYNAMIC_TYPESUPPORT_STRINGIFY2(x) #x

#define ROSIDL_DYNAMIC_TYPESUPPORT_EXPAND(x) ROSIDL_DYNAMIC_TYPESUPPORT_EXPAND2(x)
#define ROSIDL_DYNAMIC_TYPESUPPORT_EXPAND2(x) x

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__MACROS_H_
