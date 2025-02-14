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

/// @file

#ifndef RCL__EVENT_H_
#define RCL__EVENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <rmw/event.h>

#include "rcl/client.h"
#include "rcl/event_callback.h"
#include "rcl/macros.h"
#include "rcl/publisher.h"
#include "rcl/service.h"
#include "rcl/subscription.h"
#include "rcl/visibility_control.h"

/// Enumeration of all of the publisher events that may fire.
typedef enum rcl_publisher_event_type_e
{
  RCL_PUBLISHER_OFFERED_DEADLINE_MISSED,
  RCL_PUBLISHER_LIVELINESS_LOST,
  RCL_PUBLISHER_OFFERED_INCOMPATIBLE_QOS,
  RCL_PUBLISHER_INCOMPATIBLE_TYPE,
  RCL_PUBLISHER_MATCHED,
} rcl_publisher_event_type_t;

/// Enumeration of all of the subscription events that may fire.
typedef enum rcl_subscription_event_type_e
{
  RCL_SUBSCRIPTION_REQUESTED_DEADLINE_MISSED,
  RCL_SUBSCRIPTION_LIVELINESS_CHANGED,
  RCL_SUBSCRIPTION_REQUESTED_INCOMPATIBLE_QOS,
  RCL_SUBSCRIPTION_MESSAGE_LOST,
  RCL_SUBSCRIPTION_INCOMPATIBLE_TYPE,
  RCL_SUBSCRIPTION_MATCHED,
} rcl_subscription_event_type_t;

/// Internal rcl implementation struct.
typedef struct rcl_event_impl_s rcl_event_impl_t;

/// Structure which encapsulates a ROS QoS event handle.
typedef struct rcl_event_s
{
  /// Pointer to the event implementation
  rcl_event_impl_t * impl;
} rcl_event_t;

/// Return a rcl_event_t struct with members set to `NULL`.
/**
 * Should be called to get a null rcl_event_t before passing to
 * rcl_event_init().
 *
 * \return Zero initialized rcl_event_t.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_event_t
rcl_get_zero_initialized_event(void);

/// Initialize an rcl_event_t with a publisher.
/**
 * Fill the rcl_event_t with the publisher and desired event_type.
 *
 * \param[in,out] event pointer to fill
 * \param[in] publisher to get events from
 * \param[in] event_type to listen for
 * \return #RCL_RET_OK if the rcl_event_t is filled, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory fails, or
 * \return #RCL_RET_UNSUPPORTED if event_type is not supported, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_event_init(
  rcl_event_t * event,
  const rcl_publisher_t * publisher,
  const rcl_publisher_event_type_t event_type);

/// Initialize an rcl_event_t with a subscription.
/**
 * Fill the rcl_event_t with the subscription and desired event_type.
 *
 * \param[in,out] event pointer to fill
 * \param[in] subscription to get events from
 * \param[in] event_type to listen for
 * \return #RCL_RET_OK if the rcl_event_t is filled, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory fails, or
 * \return #RCL_RET_UNSUPPORTED if event_type is not supported, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_subscription_event_init(
  rcl_event_t * event,
  const rcl_subscription_t * subscription,
  const rcl_subscription_event_type_t event_type);

// Take event using the event handle.
/**
 * Take an event from the event handle.
 *
 * \param[in] event event object to take from
 * \param[in, out] event_info event info object to write taken data into
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if memory allocation failed, or
 * \return #RCL_RET_EVENT_TAKE_FAILED if the take event failed, or
 * \return #RCL_RET_ERROR if an unexpected error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_take_event(
  const rcl_event_t * event,
  void * event_info);

// Finalize an event.
/**
 * Finalize an event.
 *
 * \param[in] event to finalize
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_EVENT_INVALID if event is null, or
 * \return #RCL_RET_ERROR if an unexpected error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_event_fini(rcl_event_t * event);

/// Return the rmw event handle.
/**
 * The handle returned is a pointer to the internally held rmw handle.
 * This function can fail, and therefore return `NULL`, if the:
 *   - event is `NULL`
 *   - event is invalid (never called init, called fini, or invalid node)
 *
 * The returned handle is made invalid if the event is finalized or if
 * rcl_shutdown() is called.
 * The returned handle is not guaranteed to be valid for the life time of the
 * event as it may be finalized and recreated itself.
 * Therefore it is recommended to get the handle from the event using
 * this function each time it is needed and avoid use of the handle
 * concurrently with functions that might change it.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] event pointer to the rcl event
 * \return rmw event handle if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rmw_event_t *
rcl_event_get_rmw_handle(const rcl_event_t * event);

/// Check that the event is valid.
/**
 * The bool returned is `false` if `event` is invalid.
 * The bool returned is `true` otherwise.
 * In the case where `false` is to be returned, an error message is set.
 * This function cannot fail.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] event pointer to the rcl event
 * \return `true` if `event` is valid, otherwise `false`
 */
RCL_PUBLIC
bool
rcl_event_is_valid(const rcl_event_t * event);

/// Set the callback function for the event.
/**
 * This API sets the callback function to be called whenever the
 * event is notified about a new instance of the event.
 *
 * \sa rmw_event_set_callback for more details about this function.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | Maybe [1]
 * Lock-Free          | Maybe [1]
 * <i>[1] rmw implementation defined</i>
 *
 * \param[in] event The event on which to set the callback
 * \param[in] callback The callback to be called when new events occur, may be NULL
 * \param[in] user_data Given to the callback when called later, may be NULL
 * \return `RCL_RET_OK` if callback was set to the listener, or
 * \return `RCL_RET_INVALID_ARGUMENT` if `event` is NULL, or
 * \return `RCL_RET_UNSUPPORTED` if the API is not implemented in the dds implementation
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_event_set_callback(
  const rcl_event_t * event,
  rcl_event_callback_t callback,
  const void * user_data);

#ifdef __cplusplus
}
#endif

#endif  // RCL__EVENT_H_
