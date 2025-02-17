// Copyright 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef RMW__TIME_H_
#define RMW__TIME_H_

#ifdef __cplusplus
extern "C"
{
#endif  // __cplusplus

#include <stdint.h>

#include "rcutils/time.h"

#include "rmw/macros.h"
#include "rmw/visibility_control.h"

/// A struct representing a duration or relative time in RMW - does not encode an origin.
typedef struct RMW_PUBLIC_TYPE rmw_time_s
{
  /// Seconds component
  uint64_t sec;

  /// Nanoseconds component
  uint64_t nsec;
} rmw_time_t;

typedef rcutils_time_point_value_t rmw_time_point_value_t;
typedef rcutils_duration_value_t rmw_duration_t;

/// Constant representing an infinite duration. Use rmw_time_equal for comparisons.
/**
  * Different RMW implementations have different representations for infinite durations.
  * This value is reported for QoS policy durations that are left unspecified.
  * Do not directly compare `sec == sec && nsec == nsec`, because we don't want to be sensitive
  * to non-normalized values (nsec > 1 second) - use rmw_time_equal instead.
  * This value is INT64_MAX nanoseconds = 0x7FFF FFFF FFFF FFFF = d 9 223 372 036 854 775 807
  *
  * Note: these constants cannot be `static const rmw_time_t` because in C that can't be used
  * as a compile-time initializer
  */
#define RMW_DURATION_INFINITE {9223372036LL, 854775807LL}
#define RMW_DURATION_UNSPECIFIED {0LL, 0LL}

/// Check whether two rmw_time_t represent the same time.
RMW_PUBLIC
RMW_WARN_UNUSED
bool
rmw_time_equal(const rmw_time_t left, const rmw_time_t right);

/// Return the total nanosecond representation of a time.
/**
  * \return INT64_MAX if input is too large to store in 64 bits
  */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_duration_t
rmw_time_total_nsec(const rmw_time_t time);

/// Construct rmw_time_t from a total nanoseconds representation.
/**
  * rmw_time_t only specifies relative time, so the origin is not relevant for this calculation.
  * \return RMW_DURATION_INFINITE if input is negative, which is not representable in rmw_time_t
  */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_time_t
rmw_time_from_nsec(const rmw_duration_t nanoseconds);

/// Ensure that an rmw_time_t does not have nanoseconds > 1 second.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_time_t
rmw_time_normalize(const rmw_time_t time);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // RMW__TIME_H_
