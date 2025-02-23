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

/// \file

#ifndef RCUTILS__ERROR_HANDLING_H_
#define RCUTILS__ERROR_HANDLING_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1  // indicate we would like strnlen_s if available
#endif
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"
#include "rcutils/macros.h"
#include "rcutils/snprintf.h"
#include "rcutils/testing/fault_injection.h"
#include "rcutils/types/rcutils_ret.h"
#include "rcutils/visibility_control.h"
#include "rcutils/configuration_flags.h"

#if defined(__STDC_LIB_EXT1__) && !defined(RCUTILS_NO_FILESYSTEM)
/// Write the given msg out to stderr, limiting the buffer size in the `fwrite`.
/**
 * This ensures that there is an upper bound to a buffer overrun if `msg` is
 * non-null terminated.
 */
#define RCUTILS_SAFE_FWRITE_TO_STDERR(msg) \
  do {fwrite(msg, sizeof(char), strnlen_s(msg, 4096), stderr);} while (0)
#elif !defined(RCUTILS_NO_FILESYSTEM)
#define RCUTILS_SAFE_FWRITE_TO_STDERR(msg) \
  do {fwrite(msg, sizeof(char), strlen(msg), stderr);} while (0)
#else
  #define RCUTILS_SAFE_FWRITE_TO_STDERR(msg)
#endif

/// Set the error message to stderr using a format string and format arguments.
/**
 * This function sets the error message to stderr using the given format string.
 * The resulting formatted string is silently truncated at
 * RCUTILS_ERROR_MESSAGE_MAX_LENGTH.
 *
 * \param[in] format_string The string to be used as the format of the error message.
 * \param[in] ... Arguments for the format string.
 */

#if !defined(RCUTILS_AVOID_DYNAMIC_ALLOCATION)
#define RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(format_string, ...) \
  do { \
    char output_msg[RCUTILS_ERROR_MESSAGE_MAX_LENGTH]; \
    int ret = rcutils_snprintf(output_msg, sizeof(output_msg), format_string, __VA_ARGS__); \
    if (ret < 0) { \
      RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to call snprintf for error message formatting\n"); \
    } else { \
      RCUTILS_SAFE_FWRITE_TO_STDERR(output_msg); \
    } \
  } while (0)
#else
#define RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(format_string, ...)
#endif

#if !defined(RCUTILS_AVOID_DYNAMIC_ALLOCATION)
/// The maximum length a formatted number is allowed to have.
#define RCUTILS_ERROR_STATE_LINE_NUMBER_STR_MAX_LENGTH 20  // "18446744073709551615"

/// The maximum number of formatting characters allowed.
#define RCUTILS_ERROR_FORMATTING_CHARACTERS 6  // ', at ' + ':'

/// The maximum formatted string length.
#define RCUTILS_ERROR_MESSAGE_MAX_LENGTH 1024

/// The maximum length for user defined error message
/**
 * Remember that "chained" errors will include previously specified file paths
 * e.g. "some error, at /path/to/a.c:42, at /path/to/b.c:42"
 */
#define RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH 768

/// The calculated maximum length for the filename.
/**
 * With RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH = 768, RCUTILS_ERROR_STATE_FILE_MAX_LENGTH == 229
 */
#define RCUTILS_ERROR_STATE_FILE_MAX_LENGTH ( \
    RCUTILS_ERROR_MESSAGE_MAX_LENGTH - \
    RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH - \
    RCUTILS_ERROR_STATE_LINE_NUMBER_STR_MAX_LENGTH - \
    RCUTILS_ERROR_FORMATTING_CHARACTERS - \
    1)
#else
#define RCUTILS_ERROR_STATE_LINE_NUMBER_STR_MAX_LENGTH 1
#define RCUTILS_ERROR_FORMATTING_CHARACTERS 1
#define RCUTILS_ERROR_MESSAGE_MAX_LENGTH 1
#define RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH 1
#define RCUTILS_ERROR_STATE_FILE_MAX_LENGTH 1
#endif  // RCUTILS_AVOID_DYNAMIC_ALLOCATION

/// Struct wrapping a fixed-size c string used for returning the formatted error string.
typedef struct rcutils_error_string_s
{
  /// The fixed-size C string used for returning the formatted error string.
  char str[RCUTILS_ERROR_MESSAGE_MAX_LENGTH];
} rcutils_error_string_t;

/// Struct which encapsulates the error state set by RCUTILS_SET_ERROR_MSG().
typedef struct rcutils_error_state_s
{
  /// User message storage, limited to RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH characters.
  char message[RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH];
  /// File name, limited to what's left from RCUTILS_ERROR_STATE_MAX_SIZE characters
  /// after subtracting storage for others.
  char file[RCUTILS_ERROR_STATE_FILE_MAX_LENGTH];
  /// Line number of error.
  uint64_t line_number;
} rcutils_error_state_t;

// make sure our math is right...
#if __STDC_VERSION__ >= 201112L && !defined(RCUTILS_AVOID_DYNAMIC_ALLOCATION)
static_assert(
  sizeof(rcutils_error_string_t) == (
    RCUTILS_ERROR_STATE_MESSAGE_MAX_LENGTH +
    RCUTILS_ERROR_STATE_FILE_MAX_LENGTH +
    RCUTILS_ERROR_STATE_LINE_NUMBER_STR_MAX_LENGTH +
    RCUTILS_ERROR_FORMATTING_CHARACTERS +
    1 /* null terminating character */),
  "Maximum length calculations incorrect");
#endif

/// Forces initialization of thread-local storage if called in a newly created thread.
/**
 * If this function is not called beforehand, then the first time the error
 * state is set or the first time the error message is retrieved, the default
 * allocator will be used to allocate thread-local storage.
 *
 * This function may or may not allocate memory.
 * The system's thread-local storage implementation may need to allocate
 * memory, since it usually has no way of knowing how much storage is needed
 * without knowing how many threads will be created.
 * Most implementations (e.g. C11, C++11, and pthread) do not have ways to
 * specify how this memory is allocated, but if the implementation allows, the
 * given allocator to this function will be used, but is otherwise unused.
 * This only occurs when creating and destroying threads, which can be avoided
 * in the "steady" state by reusing pools of threads.
 *
 * It is worth considering that repeated thread creation and destruction will
 * result in repeated memory allocations and could result in memory
 * fragmentation.
 * This is typically avoided anyways by using pools of threads.
 *
 * In case an error is indicated by the return code, no error message will have
 * been set.
 *
 * If called more than once in a thread, or after implicitly initialized by
 * setting the error state, it will still return `RCUTILS_RET_OK`, even
 * if the given allocator is invalid.
 * Essentially this function does nothing if thread-local storage has already
 * been called.
 * If already initialized, the given allocator is ignored, even if it does not
 * match the allocator used originally to initialize the thread-local storage.
 *
 * \param[in] allocator to be used to allocate and deallocate memory
 * \return #RCUTILS_RET_OK if successful, or
 * \return #RCUTILS_RET_INVALID_ARGUMENT if the allocator is invalid, or
 * \return #RCUTILS_RET_BAD_ALLOC if allocating memory fails, or
 * \return #RCUTILS_RET_ERROR if an unspecified error occurs.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t
rcutils_initialize_error_handling_thread_local_storage(rcutils_allocator_t allocator);

/// Set the error message, as well as the file and line on which it occurred.
/**
 * This is not meant to be used directly, but instead via the
 * RCUTILS_SET_ERROR_MSG(msg) macro.
 *
 * The error_msg parameter is copied into the internal error storage and must
 * be null terminated.
 * The file parameter is copied into the internal error storage and must
 * be null terminated.
 *
 * \param[in] error_string The error message to set.
 * \param[in] file The path to the file in which the error occurred.
 * \param[in] line_number The line number on which the error occurred.
 */
RCUTILS_PUBLIC
void
rcutils_set_error_state(const char * error_string, const char * file, size_t line_number);

/// Check an argument for a null value.
/**
 * If the argument's value is `NULL`, set the error message saying so and
 * return the `error_return_type`.
 *
 * \param[in] argument The argument to test.
 * \param[in] error_return_type The type to return if the argument is `NULL`.
 */
#define RCUTILS_CHECK_ARGUMENT_FOR_NULL(argument, error_return_type) \
  RCUTILS_CHECK_FOR_NULL_WITH_MSG( \
    argument, #argument " argument is null", \
    return error_return_type)

/// Check a value for null, with an error message and error statement.
/**
 * If `value` is `NULL`, the error statement will be evaluated after
 * setting the error message.
 *
 * \param[in] value The value to test.
 * \param[in] msg The error message if `value` is `NULL`.
 * \param[in] error_statement The statement to evaluate if `value` is `NULL`.
 */
#define RCUTILS_CHECK_FOR_NULL_WITH_MSG(value, msg, error_statement) \
  do { \
    if (NULL == value) { \
      RCUTILS_SET_ERROR_MSG(msg); \
      error_statement; \
    } \
  } while (0)

/// Set the error message, as well as append the current file and line number.
/**
 * If an error message was previously set, and rcutils_reset_error() was not called
 * afterwards, and this library was built with RCUTILS_REPORT_ERROR_HANDLING_ERRORS
 * turned on, then the previously set error message will be printed to stderr.
 * Error state storage is thread local and so all error related functions are
 * also thread local.
 *
 * \param[in] msg The error message to be set.
 */
#ifdef RCUTILS_AVOID_DYNAMIC_ALLOCATION
  #define RCUTILS_SET_ERROR_MSG(msg)
#else
#define RCUTILS_SET_ERROR_MSG(msg) \
  do {rcutils_set_error_state(msg, __FILE__, __LINE__);} while (0)
#endif // RCUTILS_AVOID_DYNAMIC_ALLOCATION

/// Set the error message using a format string and format arguments.
/**
 * This function sets the error message using the given format string.
 * The resulting formatted string is silently truncated at
 * RCUTILS_ERROR_MESSAGE_MAX_LENGTH.
 *
 * \param[in] format_string The string to be used as the format of the error message.
 * \param[in] ... Arguments for the format string.
 */
#ifdef RCUTILS_AVOID_DYNAMIC_ALLOCATION
  #define RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(format_string, ...)
#else
#define RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(format_string, ...) \
  do { \
    char output_msg[RCUTILS_ERROR_MESSAGE_MAX_LENGTH]; \
    int ret = rcutils_snprintf(output_msg, sizeof(output_msg), format_string, __VA_ARGS__); \
    if (ret < 0) { \
      RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to call snprintf for error message formatting\n"); \
    } else { \
      RCUTILS_SET_ERROR_MSG(output_msg); \
    } \
  } while (0)
#endif // RCUTILS_AVOID_DYNAMIC_ALLOCATION


/// Indicate that the function intends to set an error message and return an error value.
/**
 * \def RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF
 * Indicating macro similar to RCUTILS_CAN_RETURN_WITH_ERROR_OF, that also sets an error
 * message.
 *
 * For now, this macro simply relies on `RCUTILS_CAN_FAIL_WITH` to set a generic error
 * message and return the given `error_return_value` if fault injection is enabled.
 *
 * \param error_return_value the value returned as a result of a given error.
 */
#define RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(error_return_value) \
  RCUTILS_CAN_FAIL_WITH( \
  { \
    RCUTILS_SET_ERROR_MSG("Injecting " RCUTILS_STRINGIFY(error_return_value)); \
    return error_return_value; \
  })

/// Return `true` if the error is set, otherwise `false`.
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
bool
rcutils_error_is_set(void);

/// Return an rcutils_error_state_t which was set with rcutils_set_error_state().
/**
 * The returned pointer will be NULL if no error has been set in this thread.
 *
 * The returned pointer is valid until RCUTILS_SET_ERROR_MSG, rcutils_set_error_state,
 * or rcutils_reset_error are called in the same thread.
 *
 * \return A pointer to the current error state struct.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
const rcutils_error_state_t *
rcutils_get_error_state(void);

/// Return the error message followed by `, at <file>:<line>` if set, else "error not set".
/**
 * This function is "safe" because it returns a copy of the current error
 * string or one containing the string "error not set" if no error was set.
 * This ensures that the copy is owned by the calling thread and is therefore
 * never invalidated by other error handling calls, and that the C string
 * inside is always valid and null terminated.
 *
 * \return The current error string, with file and line number, or "error not set" if not set.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_error_string_t
rcutils_get_error_string(void);

/// Reset the error state by clearing any previously set error state.
RCUTILS_PUBLIC
void
rcutils_reset_error(void);

/// Set the error message using RCUTILS_SET_ERROR_MSG and append the previous error.
/**
 * If there is no previous error, has same behavior as RCUTILS_SET_ERROR_MSG.
 * \param[in] msg The error message to be set.
 */
#define RCUTILS_SET_ERROR_MSG_AND_APPEND_PREV_ERROR(msg) \
  do { \
    rcutils_error_string_t error_string = rcutils_get_error_string(); \
    rcutils_reset_error(); \
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING( \
      RCUTILS_EXPAND(msg ": %s"), error_string.str); \
  } while (0)

/// Set the error message with RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING and append the previous
/// error.
/**
 * This function sets the error message using the given format string, and appends and resets the
 * latest error string.
 * The resulting formatted string is silently truncated at RCUTILS_ERROR_MESSAGE_MAX_LENGTH.
 *
 * If there is no previous error, has same behavior as RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING.
 *
 * \param[in] format_string The string to be used as the format of the error message.
 * \param[in] ... Arguments for the format string.
 */
#define RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING_AND_APPEND_PREV_ERROR(format_string, ...) \
  do { \
    rcutils_error_string_t error_string = rcutils_get_error_string(); \
    rcutils_reset_error(); \
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING( \
      RCUTILS_EXPAND(format_string ": %s"), __VA_ARGS__, error_string.str); \
  } while (0)

/// Write the given msg out to stderr, limiting the buffer size in the `fwrite`, appending the
/// previous error.
/**
 * This will reset the previous error, if it exists.
 * If there is no previous error, has same behavior as RCUTILS_SAFE_FWRITE_TO_STDERR.
 */
#define RCUTILS_SAFE_FWRITE_TO_STDERR_AND_APPEND_PREV_ERROR(msg) \
  do { \
    rcutils_error_string_t error_string = rcutils_get_error_string(); \
    rcutils_reset_error(); \
    RCUTILS_SAFE_FWRITE_TO_STDERR(msg); \
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(": %s", error_string.str); \
  } while (0)

/// Set the error message to stderr using a format string and format arguments, appending the
/// previous error.
/**
 * This function sets the error message to stderr using the given format string, appending and
 * resetting the previous error.
 * The resulting formatted string is silently truncated at RCUTILS_ERROR_MESSAGE_MAX_LENGTH.
 *
 * This will reset the previous error, if it exists.
 * If there is no previous error, has same behavior as
 * RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING.
 *
 * \param[in] format_string The string to be used as the format of the error message.
 * \param[in] ... Arguments for the format string.
 */
#define RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING_AND_APPEND_PREV_ERROR(format_string, ...) \
  do { \
    rcutils_error_string_t error_string = rcutils_get_error_string(); \
    rcutils_reset_error(); \
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(format_string, __VA_ARGS__); \
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(": %s", error_string.str); \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__ERROR_HANDLING_H_
