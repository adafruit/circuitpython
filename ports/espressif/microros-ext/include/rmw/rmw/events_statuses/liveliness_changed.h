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

#ifndef RMW__EVENTS_STATUSES__LIVELINESS_CHANGED_H_
#define RMW__EVENTS_STATUSES__LIVELINESS_CHANGED_H_

#include <stdint.h>

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// QoS Liveliness Changed information provided by a subscription.
typedef struct RMW_PUBLIC_TYPE rmw_liveliness_changed_status_s
{
  /**
   * The total number of currently active Publishers which publish to the topic associated with
   * the Subscription.
   * This count increases when a newly matched Publisher asserts its liveliness for the first time
   * or when a Publisher previously considered to be not alive reasserts its liveliness.
   * The count decreases when a Publisher considered alive fails to assert its liveliness and
   * becomes not alive, whether because it was deleted normally or for some other reason.
   */
  int32_t alive_count;
  /**
   * The total count of current Publishers which publish to the topic associated with the
   * Subscription that are no longer asserting their liveliness.
   * This count increases when a Publisher considered alive fails to assert its liveliness and
   * becomes not alive for some reason other than the normal deletion of that Publisher.
   * It decreases when a previously not alive Publisher either reasserts its liveliness or is
   * deleted normally.
   */
  int32_t not_alive_count;
  /// The change in the alive_count since the status was last read.
  int32_t alive_count_change;
  /// The change in the not_alive_count since the status was last read.
  int32_t not_alive_count_change;
} rmw_liveliness_changed_status_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENTS_STATUSES__LIVELINESS_CHANGED_H_
