// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__VALIDATE_TOPIC_NAME_H_
#define RCL__VALIDATE_TOPIC_NAME_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

/// The topic name is valid.
#define RCL_TOPIC_NAME_VALID 0

/// The topic name is invalid because it is an empty string.
#define RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING 1

/// The topic name is invalid because it ends with a forward slash.
#define RCL_TOPIC_NAME_INVALID_ENDS_WITH_FORWARD_SLASH 2

/// The topic name is invalid because it has characters that are not allowed.
#define RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS 3

/// The topic name is invalid because one of the tokens starts with a number.
#define RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER 4

/// The topic name is invalid because it has an unmatched curly brace.
#define RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE 5

/// The topic name is invalid because it has a misplaced tilde in it.
#define RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE 6

/// The topic name is invalid because a tilde is not directly followed by a forward slash.
#define RCL_TOPIC_NAME_INVALID_TILDE_NOT_FOLLOWED_BY_FORWARD_SLASH 7

/// The topic name is invalid because one of the substitutions has characters that are not allowed.
#define RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS 8

/// The topic name is invalid because one of the substitutions starts with a number.
#define RCL_TOPIC_NAME_INVALID_SUBSTITUTION_STARTS_WITH_NUMBER 9

/// Validate a given topic name.
/**
 * The topic name does not need to be a full qualified name, but it should
 * follow some of the rules in this document:
 *
 *   http://design.ros2.org/articles/topic_and_service_names.html
 *
 * Note that this function expects any URL suffixes as described in the above
 * document to have already been removed.
 *
 * If the input topic is valid, RCL_TOPIC_NAME_VALID will be stored
 * into validation_result.
 * If the input topic violates any of the rules then one of these values will
 * be stored into validation_result:
 *
 * - RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING
 * - RCL_TOPIC_NAME_INVALID_ENDS_WITH_FORWARD_SLASH
 * - RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS
 * - RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER
 * - RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE
 * - RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE
 * - RCL_TOPIC_NAME_INVALID_TILDE_NOT_FOLLOWED_BY_FORWARD_SLASH
 * - RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS
 * - RCL_TOPIC_NAME_INVALID_SUBSTITUTION_STARTS_WITH_NUMBER
 *
 * Some checks, like the check for illegal repeated forward slashes, are not
 * checked in this function because they would need to be checked again after
 * expansion anyways.
 * The purpose of this subset of checks is to try to catch issues with content
 * that will be expanded in some way by rcl_expand_topic_name(), like `~` or
 * substitutions inside of `{}`, or with other issues that would obviously
 * prevent expansion, like RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING.
 *
 * This function does not check that the substitutions are known substitutions,
 * only that the contents of the `{}` follow the rules outline in the document
 * which was linked to above.
 *
 * Stricter validation can be done with rmw_validate_full_topic_name() after using
 * rcl_expand_topic_name().
 *
 * Additionally, if the invalid_index argument is not NULL, then it will be
 * assigned the index in the topic_name string where the violation occurs.
 * The invalid_index is not set if the validation passes.
 *
 * \param[in] topic_name the topic name to be validated, must be null terminated
 * \param[out] validation_result the reason for validation failure, if any
 * \param[out] invalid_index index of violation if the input topic is invalid
 * \return #RCL_RET_OK if the topic name was expanded successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_validate_topic_name(
  const char * topic_name,
  int * validation_result,
  size_t * invalid_index);

/// Validate a given topic name.
/**
 * This is an overload with an extra parameter for the length of topic_name.
 * \param[in] topic_name the topic name to be validated, must be null terminated
 * \param[in] topic_name_length The number of characters in topic_name.
 * \param[out] validation_result the reason for validation failure, if any
 * \param[out] invalid_index index of violation if the input topic is invalid
 * \return #RCL_RET_OK if the topic name was expanded successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 *
 * \sa rcl_validate_topic_name()
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_validate_topic_name_with_size(
  const char * topic_name,
  size_t topic_name_length,
  int * validation_result,
  size_t * invalid_index);

/// Return a validation result description, or NULL if unknown or RCL_TOPIC_NAME_VALID.
/**
 * \param[in] validation_result The validation result to get the string for
 * \return A string description of the validation result if successful, or
 * \return NULL if the validation result is invalid.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const char *
rcl_topic_name_validation_result_string(int validation_result);

#ifdef __cplusplus
}
#endif

#endif  // RCL__VALIDATE_TOPIC_NAME_H_
