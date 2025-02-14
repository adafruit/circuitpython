// Copyright 2023 Sony Group Corporation.
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

#ifndef RMW__EVENTS_STATUSES__MATCHED_H_
#define RMW__EVENTS_STATUSES__MATCHED_H_

#include <stdint.h>
#include <stddef.h>

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RMW_PUBLIC_TYPE rmw_matched_status_s
{
  /**
   * For publisher, the total cumulative count of subscribers matched to the concerned publisher.
   *
   * For subscriber, the total cumulative count of publishers matched to the concerned subscriber.
   */
  size_t total_count;

  /**
   * total_count change since last time the status was read.
   */
  size_t total_count_change;

  /**
   * For publisher, the number of subscribers currently matched to the concerned publisher.
   *
   * For subscriber, the number of publishers currently matched to the concerned subscriber.
   */
  size_t current_count;

  /**
   * The current_count change since last time the status was read.
   */
  int32_t current_count_change;
} rmw_matched_status_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENTS_STATUSES__MATCHED_H_
