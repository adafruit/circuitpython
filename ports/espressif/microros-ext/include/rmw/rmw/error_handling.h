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

/**
 * @file error_handling.h
 * @brief Functions and macros to interact with error handling mechanisms
 *
 * This logic pretty much renames many `rcutils` functions and macros. Also check out that
 * package's documentation for more information.
 */

#ifndef RMW__ERROR_HANDLING_H_
#define RMW__ERROR_HANDLING_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rcutils/error_handling.h>

/// Struct wrapping a fixed-size c string used for returning the formatted error string.
typedef rcutils_error_string_t rmw_error_string_t;

/// Struct which encapsulates the error state set by RMW_SET_ERROR_MSG().
typedef rcutils_error_state_t rmw_error_state_t;

/// Limit the buffer size in the `fwrite` call to give an upper bound to buffer overrun in the case
/// of non-null terminated `msg`.
#define RMW_SAFE_FWRITE_TO_STDERR RCUTILS_SAFE_FWRITE_TO_STDERR

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
 * \return `RCUTILS_RET_OK` if successful, or
 * \return `RCUTILS_RET_INVALID_ARGUMENT` if the allocator is invalid, or
 * \return `RCUTILS_RET_BAD_ALLOC` if allocating memory fails, or
 * \return `RCUTILS_RET_ERROR` if an unspecified error occurs.
 */
#define rmw_initialize_error_handling_thread_local_storage \
  rcutils_initialize_error_handling_thread_local_storage

/// Set the error message, as well as the file and line on which it occurred.
/**
 * This is not meant to be used directly, but instead via the
 * RMW_SET_ERROR_MSG(msg) macro.
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
#define rmw_set_error_state rcutils_set_error_state

/// Check an argument for a null value.
/**
 * If the argument's value is `NULL`, set the error message saying so and
 * return the `error_return_type`.
 *
 * \param[in] argument The argument to test.
 * \param[in] error_return_type The type to return if the argument is `NULL`.
 */
#define RMW_CHECK_ARGUMENT_FOR_NULL(argument, error_return_type) \
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(argument, error_return_type)

/// Check a value for null, with an error message and error statement.
/**
 * If `value` is `NULL`, the error statement will be evaluated after
 * setting the error message.
 *
 * \param[in] value The value to test.
 * \param[in] msg The error message if `value` is `NULL`.
 * \param[in] error_statement The statement to evaluate if `value` is `NULL`.
 */
#define RMW_CHECK_FOR_NULL_WITH_MSG(value, msg, error_statement) \
  RCUTILS_CHECK_FOR_NULL_WITH_MSG(value, msg, error_statement)

/// Set the error message, as well as append the current file and line number.
/**
 * If an error message was previously set, and rmw_reset_error() was not called
 * afterwards, and this library was built with RCUTILS_REPORT_ERROR_HANDLING_ERRORS
 * turned on, then the previously set error message will be printed to stderr.
 * Error state storage is thread local and so all error related functions are
 * also thread local.
 *
 * \param[in] msg The error message to be set.
 */
#define RMW_SET_ERROR_MSG(msg) RCUTILS_SET_ERROR_MSG(msg)

/// Set the error message using a format string and format arguments.
/**
 * This function sets the error message using the given format string.
 * The resulting formatted string is silently truncated at
 * RCUTILS_ERROR_MESSAGE_MAX_LENGTH.
 *
 * \param[in] format_string The string to be used as the format of the error message.
 * \param[in] ... Arguments for the format string.
 */
#define RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(format_string, ...) \
  RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(format_string, __VA_ARGS__)

/// Return `true` if the error is set, otherwise `false`.
/**
 * This is currently defines as `rcutils_error_is_set`
 */
#define rmw_error_is_set rcutils_error_is_set

/// Return an rcutils_error_state_t which was set with rcutils_set_error_state().
/**
 * The returned pointer will be NULL if no error has been set in this thread.
 *
 * The returned pointer is valid until RCUTILS_SET_ERROR_MSG, rcutils_set_error_state,
 * or rcutils_reset_error are called in the same thread.
 *
 * \return A pointer to the current error state struct.
 */
#define rmw_get_error_state rcutils_get_error_state

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
#define rmw_get_error_string rcutils_get_error_string

/// Reset the error state by clearing any previously set error state.
#define rmw_reset_error rcutils_reset_error

/// Set the error message using RCUTILS_SET_ERROR_MSG and append the previous error.
/**
 * If there is no previous error, has same behavior as RCUTILS_SET_ERROR_MSG.
 * \param[in] msg The error message to be set.
 */
#define RMW_SET_ERROR_MSG_AND_APPEND_PREV_ERROR(msg) \
  RCUTILS_SET_ERROR_MSG_AND_APPEND_PREV_ERROR(msg)

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
#define RMW_SET_ERROR_MSG_WITH_FORMAT_STRING_AND_APPEND_PREV_ERROR(format_string, ...) \
  RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING_AND_APPEND_PREV_ERROR(format_string, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif  // RMW__ERROR_HANDLING_H_
