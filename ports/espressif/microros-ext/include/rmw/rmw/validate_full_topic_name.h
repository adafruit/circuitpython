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

#ifndef RMW__VALIDATE_FULL_TOPIC_NAME_H_
#define RMW__VALIDATE_FULL_TOPIC_NAME_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rmw/macros.h"
#include "rmw/types.h"

#define RMW_TOPIC_VALID 0
#define RMW_TOPIC_INVALID_IS_EMPTY_STRING 1
#define RMW_TOPIC_INVALID_NOT_ABSOLUTE 2
#define RMW_TOPIC_INVALID_ENDS_WITH_FORWARD_SLASH 3
#define RMW_TOPIC_INVALID_CONTAINS_UNALLOWED_CHARACTERS 4
#define RMW_TOPIC_INVALID_CONTAINS_REPEATED_FORWARD_SLASH 5
#define RMW_TOPIC_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER 6
#define RMW_TOPIC_INVALID_TOO_LONG 7

#define RMW_TOPIC_MAX_NAME_LENGTH 255U /* impl constraint */ - 8U /* reserved for prefixes */

/// Determine if a given fully qualified topic name is valid.
/** Validity of a FQN for topic is determined based on rules defined here:
 *
 *   http://design.ros2.org/articles/topic_and_service_names.html
 *
 * Note that this function expects any URL suffixes as described in the above
 * document to have already been removed.
 *
 * If either the C string or validation_result pointer are null, then
 * `RMW_RET_INVALID_ARGUMENT` will be returned.
 * The topic_name should be a valid, null-terminated C string.
 * The validation_result int pointer should point to valid memory so a result
 * can be stored in it as an output variable.
 * The invalid_index size_t pointer should either point NULL or to valid memory
 * so in the event of a validation error, the location in the input string can
 * be stored therein.
 * If NULL is passed in for invalid_index, it will be not be set.
 *
 * The invalid_index will not be assigned a value if the topic is valid.
 *
 * The int which validation_result points to will have a one of a few possible
 * results values (defined with macros) stored into it:
 *
 * - RMW_TOPIC_VALID
 * - RMW_TOPIC_INVALID_IS_EMPTY_STRING
 * - RMW_TOPIC_INVALID_NOT_ABSOLUTE
 * - RMW_TOPIC_INVALID_ENDS_WITH_FORWARD_SLASH
 * - RMW_TOPIC_INVALID_CONTAINS_UNALLOWED_CHARACTERS
 * - RMW_TOPIC_INVALID_CONTAINS_REPEATED_FORWARD_SLASH
 * - RMW_TOPIC_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER
 * - RMW_TOPIC_INVALID_TOO_LONG
 *
 * The result value can be converted to a description with the
 * rmw_full_topic_name_validation_result_string() function.
 *
 * The `RMW_TOPIC_INVALID_TOO_LONG` is guaranteed to be checked last, such
 * that if you get that result, then you can assume all other checks succeeded.
 * This is done so that the length limit can be treated as a warning rather
 * than an error if desired.
 *
 * \param[in] topic_name topic name to be validated
 * \param[out] validation_result int in which the result of the check is stored
 * \param[out] invalid_index size_t index of the input string where an error occurred
 * \returns `RMW_RET_OK` on successfully running the check, or
 * \returns `RMW_RET_INVALID_ARGUMENT` on invalid parameters, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_validate_full_topic_name(
  const char * topic_name,
  int * validation_result,
  size_t * invalid_index);

/// Deterimine if a given topic name is valid.
/**
 * This is an overload with an extra parameter for the length of topic_name.
 *
 * \sa rmw_validate_full_topic_name(const char *, int *, size_t *)
 *
 * \param[in] topic_name topic name to be validated
 * \param[in] topic_name_length The number of characters in topic_name.
 * \param[out] validation_result int in which the result of the check is stored
 * \param[out] invalid_index size_t index of the input string where an error occurred
 * \returns `RMW_RET_OK` on successfully running the check, or
 * \returns `RMW_RET_INVALID_ARGUMENT` on invalid parameters, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_validate_full_topic_name_with_size(
  const char * topic_name,
  size_t topic_name_length,
  int * validation_result,
  size_t * invalid_index);

/// Return a validation result description, or NULL if RMW_TOPIC_VALID.
/**
 * \param[in] validation_result the result of validation
 * \returns `NULL` if the validation result is `RMW_TOPIC_VALID`, or
 * \returns a string representing the validation result.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
const char *
rmw_full_topic_name_validation_result_string(int validation_result);

#ifdef __cplusplus
}
#endif

#endif  // RMW__VALIDATE_FULL_TOPIC_NAME_H_
