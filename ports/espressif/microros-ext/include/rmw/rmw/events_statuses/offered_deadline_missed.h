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

#ifndef RMW__EVENTS_STATUSES__OFFERED_DEADLINE_MISSED_H_
#define RMW__EVENTS_STATUSES__OFFERED_DEADLINE_MISSED_H_

#include <stddef.h>

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// QoS Deadline Missed information provided by a publisher.
typedef struct RMW_PUBLIC_TYPE rmw_offered_deadline_missed_status_s
{
  /**
   * Lifetime cumulative number of offered deadline periods elapsed during which a Publisher failed
   * to provide data.
   * Missed deadlines accumulate; that is, each deadline period the total_count will be incremented
   * by one.
   */
  int32_t total_count;
  /// The change in total_count since the last time the status was last read.
  int32_t total_count_change;
} rmw_offered_deadline_missed_status_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENTS_STATUSES__OFFERED_DEADLINE_MISSED_H_
