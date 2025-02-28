// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__EVENT_H_
#define RMW__EVENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "rcutils/allocator.h"

#include "rmw/macros.h"
#include "rmw/types.h"
#include "rmw/ret_types.h"
#include "rmw/visibility_control.h"

/// Define publisher/subscription events
typedef enum rmw_event_type_e
{
  // subscription events
  RMW_EVENT_LIVELINESS_CHANGED,
  RMW_EVENT_REQUESTED_DEADLINE_MISSED,
  RMW_EVENT_REQUESTED_QOS_INCOMPATIBLE,
  RMW_EVENT_MESSAGE_LOST,
  RMW_EVENT_SUBSCRIPTION_INCOMPATIBLE_TYPE,
  RMW_EVENT_SUBSCRIPTION_MATCHED,

  // publisher events
  RMW_EVENT_LIVELINESS_LOST,
  RMW_EVENT_OFFERED_DEADLINE_MISSED,
  RMW_EVENT_OFFERED_QOS_INCOMPATIBLE,
  RMW_EVENT_PUBLISHER_INCOMPATIBLE_TYPE,
  RMW_EVENT_PUBLICATION_MATCHED,

  // sentinel value
  RMW_EVENT_INVALID
} rmw_event_type_t;

/// Encapsulate the RMW event implementation, data, and type.
typedef struct RMW_PUBLIC_TYPE rmw_event_s
{
  /// Implementation identifier, used to ensure two different implementations are not being mixed.
  const char * implementation_identifier;
  /// Data specific to this event type from either the publisher or subscriber.
  void * data;
  /// The event type that occurred.
  rmw_event_type_t event_type;
} rmw_event_t;

/// Return a zero initialized event structure.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_event_t
rmw_get_zero_initialized_event(void);

/// Initialize a rmw publisher event.
/**
 * \param[inout] rmw_event to initialize
 * \param[in] publisher to initialize with
 * \param[inout] event_type for the event to initialize
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if invalid argument, or
 * \return `RMW_RET_UNSUPPORTED` if event_type is not supported, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_publisher_event_init(
  rmw_event_t * rmw_event,
  const rmw_publisher_t * publisher,
  rmw_event_type_t event_type);

/// Initialize a rmw subscription event.
/**
 * \param[inout] rmw_event to initialize
 * \param[in] subscription to initialize with
 * \param[inout] event_type for the event to handle
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if invalid argument, or
 * \return `RMW_RET_UNSUPPORTED` if event_type is not supported, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_subscription_event_init(
  rmw_event_t * rmw_event,
  const rmw_subscription_t * subscription,
  rmw_event_type_t event_type);

/// Take an event from the event handle.
/**
 * \param[in] event_handle event object to take from
 * \param[inout] event_info event info object to write taken data into
 * \param[out] taken boolean flag indicating if an event was taken or not
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_BAD_ALLOC` if memory allocation failed, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_take_event(
  const rmw_event_t * event_handle,
  void * event_info,
  bool * taken);

/// Finalize an rmw_event_t.
/**
 * \param[in] event to finalize
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_event_fini(rmw_event_t * event);

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENT_H_
