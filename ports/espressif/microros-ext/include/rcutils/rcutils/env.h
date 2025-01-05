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

/// \file

#ifndef RCUTILS__ENV_H_
#define RCUTILS__ENV_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

#include "rcutils/macros.h"
#include "rcutils/visibility_control.h"

/// Set or un-set a process-scoped environment variable.
/**
 * This function modifies the environment variables for the current process by
 * copying given string values into the process' global environment variable
 * store.
 *
 * \par Thread Safety:
 * This function is not thread-safe. Take care not to modify the environment variables while
 * another thread might be reading or writing environment variables.
 *
 * \par Platform Consistency:
 * The behavior when setting a variable to an empty string (`""`) differs
 * between platforms. On Windows, the variable is un-set (as if \p env_value was
 * `NULL`), while on other platforms the variable is set to an empty string as
 * expected.
 *
 * \param[in] env_name Name of the environment variable to modify.
 * \param[in] env_value Value to set the environment variable to, or `NULL` to
 *   un-set.
 * \return `true` if success, or
 * \return `false` if env_name is invalid or NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
bool
rcutils_set_env(const char * env_name, const char * env_value);

/// Retrieve the value of the given environment variable if it exists, or "".
/** The c-string which is returned in the env_value output parameter is only
 * valid until the next time this function is called, because it is a direct
 * pointer to the static storage.
 * The variable env_value populated by this function should never have free()
 * called on it.
 * If the environment variable is not set, an empty string will be returned.
 *
 * In both cases, environment variable set or unset, NULL is returned unless
 * an exception has occurred, in which case the error string is returned.
 * For example:
 *
 * ```c
 * #include <stdio.h>
 * #include <rcutils/env.h>
 * const char * env_value;
 * const char * error_str;
 * error_str = rcutils_get_env("SOME_ENV_VAR", &env_value);
 * if (error_str != NULL) {
 *   fprintf(stderr, "Error getting env var: %s\n", error_str);
 * }
 * printf("Valued of 'SOME_ENV_VAR': %s\n", env_value);
 * ```
 *
 * This function cannot be concurrently called together with rcutils_set_env (or any platform specific equivalent)
 * on different threads, but multiple concurrent calls to this function are thread safe.
 *
 * \param[in] env_name the name of the environment variable
 * \param[out] env_value pointer to the value cstring, or "" if unset
 * \return NULL on success (success can be returning an empty string), or
 * \return an error string on failure.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
const char *
rcutils_get_env(const char * env_name, const char ** env_value);

/// Retrieve the full path to the home directory.
/**
 * The c-string which is returned is only valid until the next time this
 * function is called, because it is a direct pointer to the static storage.
 * Also note that the string returned here should *not* be freed.
 *
 * The function first tries to get the HOME environment variable.
 * If that variable exists and is non-empty, that will be returned.
 * Otherwise, on Windows, the function tries to get the USERPROFILE environment variable.
 * If that variable exists and is non-empty, that will be returned.
 * Otherwise, NULL will be returned.
 *
 * This function cannot be thread-safely called together with rcutils_set_env
 * (or any platform specific equivalent), but multiple calls to this function are thread safe.
 *
 * \return The home directory on success, or
 * \return `NULL` on failure.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
const char *
rcutils_get_home_dir(void);

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__ENV_H_
