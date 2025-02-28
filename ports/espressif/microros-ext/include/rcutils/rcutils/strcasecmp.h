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

#ifndef RCUTILS__STRCASECMP_H_
#define RCUTILS__STRCASECMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcutils/macros.h"
#include "rcutils/visibility_control.h"

/// Case insensitive string compare.
/**
 * This function compares two strings ignoring case in a portable way.
 * This performs a byte-by-byte comparison of the strings s1 and s2,
 * ignoring the case of the characters.
 *
 * \param[in] s1 Null terminated string to compare.
 * \param[in] s2 Null terminated string to compare.
 * \param[out] value Pointer to comparison result.
 *   An integer less than, equal to, or greater than zero if s1 is, after
 *   ignoring case, found to be less than, to match, or be greater than s2,
 *   respectively.
 * \return 0 if method succeeded, or
 * \return -1 if failed.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
int
rcutils_strcasecmp(
  const char * s1,
  const char * s2,
  int * value);

/// Case insensitive string compare up to count characters.
/**
 * This function compares two strings ignoring case in a portable way.
 * This performs a byte-by-byte comparison of the strings s1 and s2 up to count
 * characters of s1 and s2, ignoring the case of the characters.
 *
 * \param[in] s1 First string to compare.
 * \param[in] s2 Second string to compare.
 * \param[in] n Count of characters to compare.
 * \param[out] value Pointer to comparison result.
 *   An integer less than, equal to, or greater than zero if s1 is, after
 *   ignoring case, found to be less than, to match, or be greater than s2,
 *   respectively.
 * \return 0 if method succeeded, or
 * \return -1 if failed.
 */
RCUTILS_PUBLIC
RCUTILS_WARN_UNUSED
int
rcutils_strncasecmp(
  const char * s1,
  const char * s2,
  size_t n,
  int * value);

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__STRCASECMP_H_
