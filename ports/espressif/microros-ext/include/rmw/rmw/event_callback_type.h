// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__EVENT_CALLBACK_TYPE_H_
#define RMW__EVENT_CALLBACK_TYPE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Common event callback type signature.
/**
 * Event callbacks of this type can be called in various scenarios, e.g.
 * data becomes available on a subscription, a QoS event has occurred, or
 * something similar.
 *
 * The user_data argument is given by the user when registering the callback,
 * and is given back to the callback each time so it can have associated,
 * user-defined state.
 *
 * The number_of_events argument indicates the number of events since the
 * callback was called.
 * This is most often 1, but can be > 1 when events occur before the callback
 * is registered.
 * It should never be 0.
 *
 * \sa rmw_subscription_set_on_new_message_callback()
 * \sa rmw_service_set_on_new_request_callback()
 * \sa rmw_client_set_on_new_response_callback()
 * \sa rmw_event_set_callback()
 */
typedef void (* rmw_event_callback_t)(const void * user_data, size_t number_of_events);

#ifdef __cplusplus
}
#endif

#endif  // RMW__EVENT_CALLBACK_TYPE_H_
