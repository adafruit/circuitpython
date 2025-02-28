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

#ifndef RMW__EVENTS_STATUSES__MESSAGE_LOST_H_
#define RMW__EVENTS_STATUSES__MESSAGE_LOST_H_

#include <stddef.h>

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RMW_PUBLIC_TYPE rmw_message_lost_status_s
{
  /// Total number of messages lost.
  size_t total_count;
  /// Number of messages lost since last callback.
  size_t total_count_change;
} rmw_message_lost_status_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENTS_STATUSES__MESSAGE_LOST_H_
