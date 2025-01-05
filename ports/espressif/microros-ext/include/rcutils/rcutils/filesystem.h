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

/// \file

#ifndef RCUTILS__FILESYSTEM_H_
#define RCUTILS__FILESYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rcutils/allocator.h"
#include "rcutils/macros.h"
#include "rcutils/visibility_control.h"

/// Return current working directory.
/**
 * \param[in] buffer Allocated string to store current directory path to
 * \param[in] max_length maximum length to be stored in buffer
 * \return `true` if success, or
 * \return `false` if buffer is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
bool
rcutils_get_cwd(char * buffer, size_t max_length);

/// Check if the provided path points to a directory.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if provided path is a directory, or
 * \return `false` if abs_path is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_is_directory(const char * abs_path);

/// Check if the provided path points to a file.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if provided path is a file, or
 * \return `false` if abs_path is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_is_file(const char * abs_path);

/// Check if the provided path points to an existing file/folder.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if the path exists, or
 * \return `false` if abs_path is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_exists(const char * abs_path);

/// Check if the provided path points to a file/folder readable by current user.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if the file is readable, or
 * \return `false` if abs_path is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_is_readable(const char * abs_path);

/// Check if the provided path points to a file/folder writable by current user.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if the file is writable, or
 * \return `false` if abs_path is NULL, or
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_is_writable(const char * abs_path);

/// Check if the provided path points to a file/folder both readable and writable by current user.
/**
 * \param[in] abs_path Absolute path to check.
 * \return `true` if the file is readable and writable, or
 * \return `false` if abs_path is NULL
 * \return `false` on failure.
 */
RCUTILS_PUBLIC
bool
rcutils_is_readable_and_writable(const char * abs_path);

/// Return newly allocated string with arguments separated by correct delimiter for the platform.
/**
 * This function allocates memory and returns it to the caller.
 * It is up to the caller to release the memory once it is done with it by
 * calling `deallocate` on the same allocator passed here.
 *
 * \param[in] left_hand_path
 * \param[in] right_hand_path
 * \param[in] allocator
 * \return concatenated path on success
 * \return `NULL` on invalid arguments
 * \return `NULL` on failure
 */
RCUTILS_PUBLIC
char *
rcutils_join_path(
  const char * left_hand_path,
  const char * right_hand_path,
  rcutils_allocator_t allocator);

/// Return newly allocated string with all argument's "/" replaced by platform specific separator.
/**
 * This function allocates memory and returns it to the caller.
 * It is up to the caller to release the memory once it is done with it by
 * calling `deallocate` on the same allocator passed here.
 *
 * \param[in] path
 * \param[in] allocator
 * \return path using platform specific delimiters on success
 * \return `NULL` on invalid arguments
 * \return `NULL` on failure
 */
RCUTILS_PUBLIC
char *
rcutils_to_native_path(
  const char * path,
  rcutils_allocator_t allocator);

/// Expand user directory in path.
/**
 * This function expands an initial '~' to the current user's home directory.
 * The home directory is fetched using `rcutils_get_home_dir()`.
 * This function returns a newly allocated string on success.
 * It is up to the caller to release the memory once it is done with it by
 * calling `deallocate` on the same allocator passed here.
 *
 * \param[in] path A null-terminated C string representing a path.
 * \param[in] allocator
 * \return path with expanded home directory on success, or
 * \return `NULL` on invalid arguments, or
 * \return `NULL` on failure.
 */
RCUTILS_PUBLIC
char *
rcutils_expand_user(const char * path, rcutils_allocator_t allocator);

/// Create the specified directory.
/**
 * This function creates an absolutely-specified directory.
 * If any of the intermediate directories do not exist, this function will
 * return False.
 * If the abs_path already exists, and is a directory, this function will
 * return true.
 *
 * This function is not thread-safe due to mkdir races as described in the
 * openat(2) documentation.
 *
 * \param[in] abs_path
 * \return `true` if making the directory was successful, or
 * \return `false` if path is NULL, or
 * \return `false` if path is empty, or
 * \return `false` if path is not absolute, or
 * \return `false` if any intermediate directories don't exist.
 */
RCUTILS_PUBLIC
bool
rcutils_mkdir(const char * abs_path);

/// Calculate the size of the specified directory.
/**
 * Calculates the size of a directory by summarizing the file size of all files.
 * \note This operation is not recursive.
 * \param[in] directory_path The directory path to calculate the size of.
 * \param[out] size The size of the directory in bytes on success.
 * \param[in] allocator Allocator being used for internal file path composition.
 * \return #RCUTILS_RET_OK if successful, or
 * \return #RCUTILS_RET_INVALID_ARGUMENT for invalid arguments, or
 * \return #RCUTILS_RET_BAD_ALLOC if memory allocation fails
 * \return #RCUTILS_RET_ERROR if other error occurs
 */
RCUTILS_PUBLIC
rcutils_ret_t
rcutils_calculate_directory_size(
  const char * directory_path,
  uint64_t * size,
  rcutils_allocator_t allocator);

/// Calculate the size of the specified directory with recursion.
/**
 * Calculates the size of a directory and subdirectory by summarizing the file size of all files.
 * If necessary, you can specify the maximum directory depth to recurse into.
 * Depth definition as below.
 * \code
 * directory_path  <= depth 1
 *    |- subdirectory <= depth 2
 *            |- subdirectory <= depth 3
 *                    ...
 * \endcode
 *
 * \note This API does not follow symlinks to files or directories.
 * \param[in] directory_path The directory path to calculate the size of.
 * \param[in] max_depth The maximum depth of subdirectory. 0 means no limitation.
 * \param[out] size The size of the directory in bytes on success.
 * \param[in] allocator Allocator being used for internal file path composition.
 * \return #RCUTILS_RET_OK if successful, or
 * \return #RCUTILS_RET_INVALID_ARGUMENT for invalid arguments, or
 * \return #RCUTILS_RET_BAD_ALLOC if memory allocation fails
 * \return #RCUTILS_RET_ERROR if other error occurs
 */
RCUTILS_PUBLIC
rcutils_ret_t
rcutils_calculate_directory_size_with_recursion(
  const char * directory_path,
  const size_t max_depth,
  uint64_t * size,
  rcutils_allocator_t allocator);

/// Calculate the size of the specifed file.
/**
 * \param[in] file_path The path of the file to obtain its size of.
 * \return The size of the file in bytes.
 */
RCUTILS_PUBLIC
size_t
rcutils_get_file_size(const char * file_path);

/// An iterator used for enumerating directory contents
typedef struct rcutils_dir_iter_s
{
  /// The name of the enumerated file or directory
  const char * entry_name;
  /// The allocator used internally by iteration functions
  rcutils_allocator_t allocator;
  /// The platform-specific iteration state
  void * state;
} rcutils_dir_iter_t;

/// Begin iterating over the contents of the specified directory.
/**
 * This function is used to list the files and directories that are contained in
 * a specified directory. The structure returned by it must be deallocated using
 * ::rcutils_dir_iter_end when the iteration is completed. The name of the
 * enumerated entry is stored in the `entry_name` member of the returned object,
 * and the first entry is already populated upon completion of this function. To
 * populate the entry with the name of the next entry, use the
 * ::rcutils_dir_iter_next function. Note that the "." and ".." entries are
 * typically among the entries enumerated.
 * \param[in] directory_path The directory path to iterate over the contents of.
 * \param[in] allocator Allocator used to create the returned structure.
 * \return An iterator object used to continue iterating directory contents
 * \return NULL if an error occurred
 */
RCUTILS_PUBLIC
rcutils_dir_iter_t *
rcutils_dir_iter_start(const char * directory_path, const rcutils_allocator_t allocator);

/// Continue iterating over the contents of a directory.
/**
 * \param[in] iter An iterator created by ::rcutils_dir_iter_start.
 * \return `true` if another entry was found, or
 * \return `false` if there are no more entries in the directory.
 */
RCUTILS_PUBLIC
bool
rcutils_dir_iter_next(rcutils_dir_iter_t * iter);

/// Finish iterating over the contents of a directory.
/**
 * \param[in] iter An iterator created by ::rcutils_dir_iter_start.
 */
RCUTILS_PUBLIC
void
rcutils_dir_iter_end(rcutils_dir_iter_t * iter);

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__FILESYSTEM_H_
