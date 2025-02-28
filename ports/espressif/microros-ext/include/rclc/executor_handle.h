// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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
#ifndef RCLC__EXECUTOR_HANDLE_H_
#define RCLC__EXECUTOR_HANDLE_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl/rcl.h>
#include <rclc/visibility_control.h>

#include <rclc/action_client.h>
#include <rclc/action_server.h>

/// TODO (jst3si) Where is this defined? - in my build environment this variable is not set.
// #define ROS_PACKAGE_NAME "rclc"

/// Enumeration for timer, subscription, guard conditions etc to be waited on.
typedef enum
{
  RCLC_SUBSCRIPTION,
  RCLC_SUBSCRIPTION_WITH_CONTEXT,
  RCLC_TIMER,
  // RCLC_TIMER_WITH_CONTEXT,  // TODO
  RCLC_CLIENT,
  RCLC_CLIENT_WITH_REQUEST_ID,
  // RCLC_CLIENT_WITH_CONTEXT,  // TODO
  RCLC_SERVICE,
  RCLC_SERVICE_WITH_REQUEST_ID,
  RCLC_SERVICE_WITH_CONTEXT,
  RCLC_ACTION_CLIENT,
  RCLC_ACTION_SERVER,
  RCLC_GUARD_CONDITION,
  // RCLC_GUARD_CONDITION_WITH_CONTEXT,  //TODO
  RCLC_NONE
} rclc_executor_handle_type_t;

/// Enumeration for invocation type. ON_NEW_DATA calls a callback only when new data is available
/// ALWAYS calls the callback always, even if no data is available (e.g. for type FUNCTION_CALL)
typedef enum
{
  ON_NEW_DATA,
  ALWAYS
} rclc_executor_handle_invocation_t;

/// Type definition for subscription callback function
/// - incoming message
typedef void (* rclc_subscription_callback_t)(const void *);

/// Type definition (duplicate) for subscription callback function (alias for foxy and galactic).
/// - incoming message
typedef rclc_subscription_callback_t rclc_callback_t;

/// Type definition for subscription callback function
/// - incoming message
/// - additional callback context
typedef void (* rclc_subscription_callback_with_context_t)(const void *, void *);

/// Type definition for client callback function
/// - request message
/// - response message
typedef void (* rclc_service_callback_t)(const void *, void *);

/// Type definition for client callback function
/// - request message
/// - request id
/// - response message
typedef void (* rclc_service_callback_with_request_id_t)(const void *, rmw_request_id_t *, void *);

/// Type definition for client callback function
/// - request message
/// - response message
/// - additional service context
typedef void (* rclc_service_callback_with_context_t)(const void *, void *, void *);

/// Type definition for client callback function
/// - response message
typedef void (* rclc_client_callback_t)(const void *);

/// Type definition for client callback function
/// - response message
/// - request id
typedef void (* rclc_client_callback_with_request_id_t)(const void *, rmw_request_id_t *);

/// Type definition for guard condition callback function.
typedef void (* rclc_gc_callback_t)();


/// Container for a handle.
typedef struct
{
  /// Type of handle
  rclc_executor_handle_type_t type;
  /// Invocation type determines when to execute the callback
  rclc_executor_handle_invocation_t invocation;
  /// Pointer to the handle
  union {
    rcl_subscription_t * subscription;
    rcl_timer_t * timer;
    rcl_client_t * client;
    rcl_service_t * service;
    rcl_guard_condition_t * gc;
    rclc_action_client_t * action_client;
    rclc_action_server_t * action_server;
  };
  /// Storage of data, which holds the message of a subscription, service, etc.
  /// subscription: ptr to message
  /// service: ptr to request message
  void * data;

  /// request-id only for type service/client request/response
  rmw_request_id_t req_id;

  /// only for service - ptr to response message
  void * data_response_msg;

  /// ptr to additional callback context
  void * callback_context;

  // TODO(jst3si) new type to be stored as data for
  //              service/client objects
  //              look at memory allocation for this struct!
  // struct {
  //   void * request_msg
  //   void * response_msg
  //   rmw_request_id_t req_id;
  //} rclc_service_data_type_t

  /// Storage for callbacks
  union {
    rclc_subscription_callback_t subscription_callback;
    rclc_subscription_callback_with_context_t subscription_callback_with_context;
    rclc_service_callback_t service_callback;
    rclc_service_callback_with_request_id_t service_callback_with_reqid;
    rclc_service_callback_with_context_t service_callback_with_context;
    rclc_client_callback_t client_callback;
    rclc_client_callback_with_request_id_t client_callback_with_reqid;
    rclc_gc_callback_t gc_callback;
  };

  /// Internal variable.
  /**  Denotes the index of this handle in the correspoding wait_set entry.
  *    (wait_set_subscriptions[index], wait_set_timers[index], ...
  *    is in the range [0,executor.max_handles), initialization: executor_max_handles
  *    because this value will never be assigned as an index in the wait_set.
  */
  size_t index;
  /// Internal variable. Flag, which is true, if the handle is initialized and therefore initialized
  bool initialized;
  /// Interval variable. Flag, which is true, if new data is available from DDS queue
  /// (is set after calling rcl_take)
  bool data_available;
  /// pointer to custom handle
  void * custom;
} rclc_executor_handle_t;

/// Information about total number of subscriptions, guard_conditions, timers, subscription etc.
typedef struct
{
  /// Total number of subscriptions
  size_t number_of_subscriptions;
  /// Total number of timers
  size_t number_of_timers;
  /// Total number of clients
  size_t number_of_clients;
  /// Total number of services
  size_t number_of_services;
  /// Total number of action clients
  size_t number_of_action_clients;
  /// Total number of action servers
  size_t number_of_action_servers;
  /// Total number of guard conditions
  size_t number_of_guard_conditions;
  /// Total number of events
  size_t number_of_events;
} rclc_executor_handle_counters_t;

/**
 * Initializes the counters of each handle type to zero.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] handle_counters preallocated rclc_executor_handle_counters_t
 * \return `RCL_RET_INVALID_ARGUMENT` if `handle_counters` is a null pointer
 */
RCLC_PUBLIC
rcl_ret_t
rclc_executor_handle_counters_zero_init(rclc_executor_handle_counters_t * handle_counters);

/**
 *  Initializes a handle with default values. The {@link rclc_executor_handle_t.index}
 *  is initialized with `max_handles`, which is a non-valid index. Note that, valid indicies
 *  are [0,max-handles-1]. The {@link rclc_executor_handle_t.invocation} is set to `ON_NEW_DATA`,
 *  so that a potential callback is invoced only whenever new data is received. All other member
 *  fields are set appropriate default values, like `none`, `NULL` or `false`.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] handle preallocated rclc_executor_handle_t
 * \param[in] max_handles maximum number of handles
 * \return `RCL_RET_OK` if the \p handle was initialized successfully
 * \return `RCL_RET_INVALID_ARGUMENT` if \p h is a null pointer
 */
RCLC_PUBLIC
rcl_ret_t
rclc_executor_handle_init(
  rclc_executor_handle_t * handle,
  size_t max_handles);

/**
 *  Resets a rclc_executor_handle_t. Compared to the function  {@link rclc_executor_handle_init()}
 *   only the {@link rclc_executor_handle_t.index} and {@link rclc_executor_handle_t.initialized}
 *   variables are reset to default values.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] handle a preallocated rclc_executor_handle_t
 * \param[in] max_handles maximum number of handles
 * \return `RCL_RET_OK` if \p h was cleared successfully
 * \return `RCL_RET_INVALID_ARGUMENT` if \p h is a null pointer
 */
RCLC_PUBLIC
rcl_ret_t
rclc_executor_handle_clear(
  rclc_executor_handle_t * handle,
  size_t max_handles);

/**
 *  Print out type name of a rclc_executor_handle_t.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] handle preallocated rclc_executor_handle_t
 * \return `RCL_RET_OK` if the handle was printed successfully
 * \return `RCL_RET_INVALID_ARGUMENT` if \p h is a null pointer
 */
RCLC_PUBLIC
rcl_ret_t
rclc_executor_handle_print(rclc_executor_handle_t * handle);

/**
 *  Returns a pointer to the rcl-handle stored in the rclc_executor_handle_t.
 *  That can be rcl_subscription_t or rcl_timer_t
 *
 *  If handle is NULL, then NULL is returned.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] handle preallocated rclc_executor_handle_t
 * \return pointer to the rcl-handle (rcl_subscription_t or rcl_timer_t)
 * \return NULL, if handle is a NULL pointer.
 */
RCLC_PUBLIC
void *
rclc_executor_handle_get_ptr(rclc_executor_handle_t * handle);

#if __cplusplus
}
#endif

#endif  // RCLC__EXECUTOR_HANDLE_H_
