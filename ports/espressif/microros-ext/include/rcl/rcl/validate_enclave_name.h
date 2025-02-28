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

#ifndef RCL__VALIDATE_ENCLAVE_NAME_H_
#define RCL__VALIDATE_ENCLAVE_NAME_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

/// The enclave name is valid.
#define RCL_ENCLAVE_NAME_VALID RMW_NAMESPACE_VALID

/// The enclave name is invalid because it is an empty string.
#define RCL_ENCLAVE_NAME_INVALID_IS_EMPTY_STRING RMW_NAMESPACE_INVALID_IS_EMPTY_STRING

/// The enclave name is invalid because it is not absolute.
#define RCL_ENCLAVE_NAME_INVALID_NOT_ABSOLUTE RMW_NAMESPACE_INVALID_NOT_ABSOLUTE

/// The enclave name is invalid because it ends with a forward slash.
#define RCL_ENCLAVE_NAME_INVALID_ENDS_WITH_FORWARD_SLASH \
  RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH

/// The enclave name is invalid because it has characters that are not allowed.
#define RCL_ENCLAVE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS \
  RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS

/// The enclave name is invalid because it contains repeated forward slashes.
#define RCL_ENCLAVE_NAME_INVALID_CONTAINS_REPEATED_FORWARD_SLASH \
  RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH

/// The enclave name is invalid because one of the tokens starts with a number.
#define RCL_ENCLAVE_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER \
  RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER

/// The enclave name is invalid because the name is too long.
#define RCL_ENCLAVE_NAME_INVALID_TOO_LONG RMW_NAMESPACE_INVALID_TOO_LONG

/// The maximum length of an enclave name.
#define RCL_ENCLAVE_NAME_MAX_LENGTH RMW_NODE_NAME_MAX_NAME_LENGTH

/// Determine if a given enclave name is valid.
/**
 * The same rules as rmw_validate_namespace() are used.
 * The only difference is in the maximum allowed length, which can be up to 255 characters.
 *
 * \param[in] enclave enclave to be validated
 * \param[out] validation_result int in which the result of the check is stored
 * \param[out] invalid_index index of the input string where an error occurred
 * \return #RCL_RET_OK on successfully running the check, or
 * \return #RCL_RET_INVALID_ARGUMENT on invalid parameters, or
 * \return #RCL_RET_ERROR when an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_validate_enclave_name(
  const char * enclave,
  int * validation_result,
  size_t * invalid_index);

/// Deterimine if a given enclave name is valid.
/**
 * This is an overload of rcl_validate_enclave_name() with an extra parameter
 * for the length of enclave.
 *
 * \param[in] enclave enclave to be validated
 * \param[in] enclave_length The number of characters in enclave
 * \param[out] validation_result int in which the result of the check is stored
 * \param[out] invalid_index index of the input string where an error occurred
 * \return #RCL_RET_OK on successfully running the check, or
 * \return #RCL_RET_INVALID_ARGUMENT on invalid parameters, or
 * \return #RCL_RET_ERROR when an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_validate_enclave_name_with_size(
  const char * enclave,
  size_t enclave_length,
  int * validation_result,
  size_t * invalid_index);

/// Return a validation result description, or NULL if unknown or RCL_ENCLAVE_NAME_VALID.
/**
 * \param[in] validation_result The validation result to get the string for
 * \return A string description of the validation result if successful, or
 * \return NULL if the validation result is invalid.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const char *
rcl_enclave_name_validation_result_string(int validation_result);

#ifdef __cplusplus
}
#endif

#endif  // RCL__VALIDATE_ENCLAVE_NAME_H_
