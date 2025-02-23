// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef RCL_ACTION__ACTION_CLIENT_H_
#define RCL_ACTION__ACTION_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/types.h"
#include "rcl_action/visibility_control.h"
#include "rcl/event_callback.h"
#include "rcl/macros.h"
#include "rcl/node.h"


/// Internal action client implementation struct.
typedef struct rcl_action_client_impl_s rcl_action_client_impl_t;

/// Structure which encapsulates a ROS action client.
typedef struct rcl_action_client_s
{
  /// Pointer to the action client implementation
  rcl_action_client_impl_t * impl;
} rcl_action_client_t;

/// Options available for a rcl_action_client_t.
typedef struct rcl_action_client_options_s
{
  /// Middleware quality of service settings for the action client.
  /// Goal service quality of service
  rmw_qos_profile_t goal_service_qos;
  /// Result service quality of service
  rmw_qos_profile_t result_service_qos;
  /// Cancel service quality of service
  rmw_qos_profile_t cancel_service_qos;
  /// Feedback topic quality of service
  rmw_qos_profile_t feedback_topic_qos;
  /// Status topic quality of service
  rmw_qos_profile_t status_topic_qos;
  /// Custom allocator for the action client, used for incidental allocations.
  /** For default behavior (malloc/free), see: rcl_get_default_allocator() */
  rcl_allocator_t allocator;
} rcl_action_client_options_t;

/// Return a rcl_action_client_t struct with members set to `NULL`.
/**
 * Should be called to get a null rcl_action_client_t before passing to
 * rcl_action_client_init().
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_client_t
rcl_action_get_zero_initialized_client(void);

/// Initialize a rcl_action_client_t.
/**
 * After calling this function on a rcl_action_client_t, it can be used to send
 * goals of the given type to the given topic using rcl_action_send_goal_request().
 * If a goal request is sent to a (possibly remote) server and if the server
 * sends a response, the client can access the response with
 * rcl_take_goal_response() once the response is available.
 *
 * After a goal request has been accepted, the rcl_action_client_t associated with the
 * goal can perform the following operations:
 *
 * - Send a request for the result with rcl_action_send_result_request().
 * If the server sends a response when the goal terminates, the result can be accessed
 * with rcl_action_take_result_response(), once the response is available.
 * - Send a cancel request for the goal with rcl_action_send_cancel_request().
 * If the server sends a response to a cancel request, the client can access the
 * response with rcl_action_take_cancel_response() once the response is available.
 * - Take feedback about the goal with rcl_action_take_feedback().
 *
 * A rcl_action_client_t can be used to access the current status of all accepted goals
 * communicated by the action server with rcl_action_take_status().
 *
 * The given rcl_node_t must be valid and the resulting rcl_action_client_t is
 * only valid as long as the given rcl_node_t remains valid.
 *
 * The rosidl_action_type_support_t is obtained on a per .action type basis.
 * When the user defines a ROS action, code is generated which provides the
 * required rosidl_action_type_support_t object.
 * This object can be obtained using a language appropriate mechanism.
 *
 * \todo TODO(jacobperron) write these instructions once and link to it instead
 *
 * For C, a macro can be used (for example `example_interfaces/Fibonacci`):
 *
 * ```c
 * #include <rosidl_runtime_c/action_type_support_struct.h>
 * #include <example_interfaces/action/fibonacci.h>
 * const rosidl_action_type_support_t * ts =
 *   ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci);
 * ```
 *
 * For C++, a template function is used:
 *
 * ```cpp
 * #include <rosidl_runtime_cpp/action_type_support.hpp>
 * #include <example_interfaces/action/fibonacci.h>
 * using rosidl_typesupport_cpp::get_action_type_support_handle;
 * const rosidl_action_type_support_t * ts =
 *   get_action_type_support_handle<example_interfaces::action::Fibonacci>();
 * ```
 *
 * The rosidl_action_type_support_t object contains action type specific
 * information used to send or take goals, results, and feedback.
 *
 * The topic name must be a c string that follows the topic and service name
 * format rules for unexpanded names, also known as non-fully qualified names:
 *
 * \see rcl_expand_topic_name
 *
 * The options struct allows the user to set the quality of service settings as
 * well as a custom allocator that is used when initializing/finalizing the
 * client to allocate space for incidentals, e.g. the action client name string.
 *
 * Expected usage (for C action clients):
 *
 * ```c
 * #include <rcl/rcl.h>
 * #include <rcl_action/action_client.h>
 * #include <rosidl_runtime_c/action_type_support_struct.h>
 * #include <example_interfaces/action/fibonacci.h>
 *
 * rcl_node_t node = rcl_get_zero_initialized_node();
 * rcl_node_options_t node_ops = rcl_node_get_default_options();
 * rcl_ret_t ret = rcl_node_init(&node, "node_name", "/my_namespace", &node_ops);
 * // ... error handling
 * const rosidl_action_type_support_t * ts =
 *   ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci);
 * rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
 * rcl_action_client_options_t action_client_ops = rcl_action_client_get_default_options();
 * ret = rcl_action_client_init(&action_client, &node, ts, "fibonacci", &action_client_ops);
 * // ... error handling, and on shutdown do finalization:
 * ret = rcl_action_client_fini(&action_client, &node);
 * // ... error handling for rcl_action_client_fini()
 * ret = rcl_node_fini(&node);
 * // ... error handling for rcl_node_fini()
 * ```
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[out] action_client a preallocated, zero-initialized action client structure
 *   to be initialized
 * \param[in] node valid rcl node handle
 * \param[in] type_support type support object for the action's type
 * \param[in] action_name the name of the action
 * \param[in] options action_client options, including quality of service settings
 * \return `RCL_RET_OK` if action_client was initialized successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_ALREADY_INIT` if the action client is already initialized, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ACTION_NAME_INVALID` if the given action name is invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_init(
  rcl_action_client_t * action_client,
  rcl_node_t * node,
  const rosidl_action_type_support_t * type_support,
  const char * action_name,
  const rcl_action_client_options_t * options);

/// Finalize a rcl_action_client_t.
/**
 * After calling, the node will no longer listen for goals for this action client
 * (assuming this is the only action client of this type in this node).
 *
 * After calling, calls to rcl_wait(), rcl_action_send_goal_request(),
 * rcl_action_take_goal_response(), rcl_action_send_cancel_request(),
 * rcl_action_take_cancel_response(), rcl_action_send_result_request(),
 * rcl_action_take_result_response(), rcl_action_take_feedback(), and
 * rcl_action_take_status(), will fail when using this action client.
 *
 * Additionally, rcl_wait() will be interrupted if currently blocking.
 *
 * The given node handle is still valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] action_client handle to the action_client to be deinitialized
 * \param[in] node handle to the node used to create the action client
 * \return `RCL_RET_OK` if the action client was deinitialized successfully, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_fini(rcl_action_client_t * action_client, rcl_node_t * node);

/// Return the default action client options in a rcl_action_client_options_t.
/**
 * The defaults are:
 *
 * - goal_service_qos = rmw_qos_profile_services_default;
 * - result_service_qos = rmw_qos_profile_services_default;
 * - cancel_service_qos = rmw_qos_profile_services_default;
 * - feedback_topic_qos = rmw_qos_profile_default;
 * - status_topic_qos = rcl_action_qos_profile_status_default;
 * - allocator = rcl_get_default_allocator()
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_action_client_options_t
rcl_action_client_get_default_options(void);

/// Check if an action server is available for the given action client.
/**
 * This function will return true for is_available if there is an action server
 * available for the given action client.
 *
 * The node parameter must not be `NULL`, and must point to a valid node.
 *
 * The client parameter must not be `NULL`, and must point to a valid client.
 *
 * The given client and node must match, i.e. the client must have been created
 * using the given node.
 *
 * The is_available parameter must not be `NULL`, and must point a bool variable.
 * The result of the check will be stored in the is_available parameter.
 *
 * In the event that error handling needs to allocate memory, this function
 * will try to use the node's allocator.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Maybe [1]
 * <i>[1] implementation may need to protect the data structure with a lock</i>
 *
 * \param[in] node the handle to the node being used to query the ROS graph
 * \param[in] client the handle to the action client being queried
 * \param[out] is_available set to true if there is an action server available, else false
 * \return `RCL_RET_OK` if successful (regardless of the action server availability), or
 * \return `RCL_RET_NODE_INVALID` if the node is invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_server_is_available(
  const rcl_node_t * node,
  const rcl_action_client_t * client,
  bool * is_available);

/// Send a ROS goal using a rcl_action_client_t.
/**
 * This is a non-blocking call.
 *
 * The caller is responsible for ensuring that the type of `ros_goal_request`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * The ROS goal message given by the `ros_goal_request` void pointer is always
 * owned by the calling code, but should remain constant during execution of this
 * function. i.e. The message cannot change during the rcl_action_send_goal_request() call.
 * Before calling rcl_action_send_goal_request() the message can change but after calling
 * rcl_action_send_goal_request() it depends on RMW implementation behavior.
 * The same `ros_goal_request` can be passed to multiple calls of this function
 * simultaneously, even if the action clients differ.
 *
 * This function is thread safe so long as access to both the rcl_action_client_t
 * and the `ros_goal_request` are synchronized.
 * That means that calling rcl_action_send_goal_request() from multiple threads is allowed,
 * but calling rcl_action_send_goal_request() at the same time as non-thread safe action
 * client functions is not, e.g. calling rcl_action_send_goal_request() and
 * rcl_action_client_fini() concurrently is not allowed.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] for unique pairs of clients and goals, see above for more</i>
 *
 * \param[in] action_client handle to the client that will make the goal request
 * \param[in] ros_goal_request pointer to the ROS goal message
 * \param[out] sequence_number pointer to the goal request sequence number
 * \return `RCL_RET_OK` if the request was sent successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the client is invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_send_goal_request(
  const rcl_action_client_t * action_client,
  const void * ros_goal_request,
  int64_t * sequence_number);

/// Take a response for a goal request from an action server using a rcl_action_client_t.
/**
 * \todo TODO(jacobperron) blocking of take?
 *
 * \todo TODO(jacobperron) pre-, during-, and post-conditions for message ownership?
 *
 * \todo TODO(jacobperron) is this thread-safe?
 *
 * The caller is responsible for ensuring that the type of `ros_goal_response`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * The caller must provide a pointer to an allocated message for the `ros_goal_response`.
 * If the take is successful, this function will populate the fields of `ros_goal_response`.
 * The ROS message given by the `ros_goal_response` void pointer is always
 * owned by the calling code, but should remain constant during execution of this
 * function. i.e. The message cannot change during the rcl_action_send_goal_response() call.
 * Before calling rcl_action_send_goal_response() the message can change but after calling
 * rcl_action_send_goal_response() it depends on RMW implementation behavior.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client handle to the client that will take the goal response
 * \param[out] response_header pointer to the goal response header
 * \param[out] ros_goal_response pointer to the response of a goal request
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_ACTION_CLIENT_TAKE_FAILED` if take failed but no error occurred
 *         in the middleware, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_take_goal_response(
  const rcl_action_client_t * action_client,
  rmw_request_id_t * response_header,
  void * ros_goal_response);

/// Take a ROS feedback message for an active goal associated with a rcl_action_client_t.
/**
 * The caller is responsible for ensuring that the type of `ros_feedback`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * \todo TODO(jacobperron) blocking of take?
 *
 * \todo TODO(jacobperron) pre-, during-, and post-conditions for message ownership?
 *
 * \todo TODO(jacobperron) is this thread-safe?
 *
 * `ros_feedback` should point to a preallocated ROS message struct of the
 * correct type.
 * If feedback is successfully taken, the feedback message is copied to into the
 * `ros_feedback` struct.
 *
 * If allocation is required when taking the feedback, e.g. if space needs to
 * be allocated for a dynamically sized array in the target message, then the
 * allocator given in the action client options is used.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Maybe [1]
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] only if required when filling the feedback message, avoided for fixed sizes</i>
 *
 * \param[in] action_client handle to the client that will take action feedback
 * \param[out] ros_feedback pointer to the ROS feedback message.
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ACTION_CLIENT_TAKE_FAILED` if take failed but no error occurred
 *         in the middleware, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_take_feedback(
  const rcl_action_client_t * action_client,
  void * ros_feedback);

/// Take a ROS status message using a rcl_action_client_t.
/**
 * The caller is responsible for ensuring that the type of `ros_status_array`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * \todo TODO(jacobperron) blocking of take?
 *
 * \todo TODO(jacobperron) pre-, during-, and post-conditions for message ownership?
 *
 * \todo TODO(jacobperron) is this thread-safe?
 *
 * The caller is responsible for allocating the `ros_status_array` struct with a
 * zero-initialization (the internal array should not be allocated).
 * If there is a successful take, then `ros_status_array` is populated
 * with the allocator given in the action client options.
 * It is the callers responsibility to deallocate the `ros_status_array` struct using
 * the allocator given in the action client options.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client handle to the client that will take status message
 * \param[out] ros_status_array pointer to ROS aciton_msgs/StatusArray message that
 *   will be populated with information about goals that have accepted the cancel request.
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ACTION_CLIENT_TAKE_FAILED` if take failed but no error occurred
 *         in the middleware, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_take_status(
  const rcl_action_client_t * action_client,
  void * ros_status_array);

/// Send a request for the result of a completed goal associated with a rcl_action_client_t.
/**
 * This is a non-blocking call.
 *
 * The caller is responsible for ensuring that the type of `ros_result_request`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * The ROS message given by the `ros_result_request` void pointer is always
 * owned by the calling code, but should remain constant during execution of this
 * function. i.e. The message cannot change during the rcl_action_send_result_request() call.
 * Before calling rcl_action_send_result_request() the message can change but after calling
 * rcl_action_send_result_request() it depends on RMW implementation behavior.
 * The same `ros_result_request` can be passed to multiple calls of this function
 * simultaneously, even if the action clients differ.
 *
 * This function is thread safe so long as access to both the rcl_action_client_t
 * and the `ros_result_request` are synchronized.
 * That means that calling rcl_action_send_result_request() from multiple threads is allowed,
 * but calling rcl_action_send_result_request() at the same time as non-thread safe action
 * client functions is not, e.g. calling rcl_action_send_result_request() and
 * rcl_action_client_fini() concurrently is not allowed.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] for unique pairs of clients and result requests, see above for more</i>

 * \param[in] action_client handle to the client that will send the result request
 * \param[in] ros_result_request pointer to a ROS result request message
 * \param[out] sequence_number pointer to the result request sequence number
 * \return `RCL_RET_OK` if the request was sent successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_send_result_request(
  const rcl_action_client_t * action_client,
  const void * ros_result_request,
  int64_t * sequence_number);

/// Take a ROS result message for a completed goal associated with a rcl_action_client_t.
/**
 * \todo TODO(jacobperron) blocking of take?
 *
 * \todo TODO(jacobperron) pre-, during-, and post-conditions for message ownership?
 *
 * \todo TODO(jacobperron) is this thread-safe?
 *
 * The caller is responsible for ensuring that the type of `ros_result_response`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * The caller must provide a pointer to an allocated message for the `ros_result_response`.
 * If the take is successful, this function will populate the fields of `ros_result_response`.
 * The ROS message given by the `ros_result_response` void pointer is always
 * owned by the calling code, but should remain constant during execution of this
 * function. i.e. The message cannot change during the rcl_action_take_result_response() call.
 * Before calling rcl_action_take_result_response() the message can change but after calling
 * rcl_action_take_result_response() it depends on RMW implementation behavior.
 *
 * If allocation is required when taking the result, e.g. if space needs to
 * be allocated for a dynamically sized array in the target message, then the
 * allocator given in the action client options is used.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Maybe [1]
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] only if required when filling the result response message, avoided for fixed sizes</i>
 *
 * \param[in] action_client handle to the client that will take the result response
 * \param[out] response_header pointer to the result response header
 * \param[out] ros_result preallocated, zero-initialized, struct where the ROS
 *   result message is copied.
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ACTION_CLIENT_TAKE_FAILED` if take failed but no error occurred
 *         in the middleware, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_take_result_response(
  const rcl_action_client_t * action_client,
  rmw_request_id_t * response_header,
  void * ros_result);

/// Send a cancel request for a goal using a rcl_action_client_t.
/**
 * This is a non-blocking call.
 *
 * The caller is responsible for ensuring that the type of `ros_cancel_request`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.
 *
 * The following cancel policy applies based on the goal ID and the timestamp provided
 * by the `ros_cancel_request` message:
 *
 * - If the goal ID is zero and timestamp is zero, cancel all goals.
 * - If the goal ID is zero and timestamp is not zero, cancel all goals accepted
 *   at or before the timestamp.
 * - If the goal ID is not zero and timestamp is zero, cancel the goal with the
 *   given ID regardless of the time it was accepted.
 * - If the goal ID is not zero and timestamp is not zero, cancel the goal with the
 *   given ID and all goals accepted at or before the timestamp.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client handle to the client that will make the cancel request
 * \param[in] ros_cancel_request pointer the ROS cancel request message
 * \param[out] sequence_number pointer to the cancel request sequence number
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_send_cancel_request(
  const rcl_action_client_t * action_client,
  const void * ros_cancel_request,
  int64_t * sequence_number);

/// Take a cancel response using a rcl_action_client_t.
/**
 * \todo TODO(jacobperron) blocking of take?
 *
 * \todo TODO(jacobperron) pre-, during-, and post-conditions for message ownership?
 *
 * \todo TODO(jacobperron) is this thread-safe?
 *
 * The caller is responsible for ensuring that the type of `ros_cancel_response`
 * and the type associate with the client (via the type support) match.
 * Passing a different type produces undefined behavior and cannot be checked
 * by this function and therefore no deliberate error will occur.

 * The caller is responsible for allocating the `ros_cancel_response` message
 * with a zero-initialization (the internal array should not be allocated).
 * If a successful response is taken, then `ros_cancel_response` is populated
 * using the allocator given in the action client options.
 * It is the callers responsibility to deallocate `ros_cancel_response` using
 * the allocator given in the action client options.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client handle to the client that will take the cancel response
 * \param[out] response_header pointer to the cancel response header
 * \param[out] ros_cancel_response a zero-initialized ROS cancel response message where
 *   the cancel response is copied.
 * \return `RCL_RET_OK` if the response was taken successfully, or
 * \return `RCL_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return `RCL_RET_ACTION_CLIENT_INVALID` if the action client is invalid, or
 * \return `RCL_RET_BAD_ALLOC` if allocating memory failed, or
 * \return `RCL_RET_ACTION_CLIENT_TAKE_FAILED` if take failed but no error occurred
 *         in the middleware, or
 * \return `RCL_RET_ERROR` if an unspecified error occurs.
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_take_cancel_response(
  const rcl_action_client_t * action_client,
  rmw_request_id_t * response_header,
  void * ros_cancel_response);

/// Get the name of the action for a rcl_action_client_t.
/**
 * This function returns the action client's name string.
 * This function can fail, and therefore return `NULL`, if the:
 *   - action client is `NULL`
 *   - action client is invalid (never called init, called fini, or invalid)
 *
 * The returned string is only valid as long as the action client is valid.
 * The value of the string may change if the topic name changes, and therefore
 * copying the string is recommended if this is a concern.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client the pointer to the action client
 * \return name string if successful, otherwise `NULL`
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
const char *
rcl_action_client_get_action_name(const rcl_action_client_t * action_client);

/// Return the options for a rcl_action_client_t.
/**
 * This function returns the action client's internal options struct.
 * This function can fail, and therefore return `NULL`, if the:
 *   - action client is `NULL`
 *   - action client is invalid (never called init, called fini, or invalid)
 *
 * The returned struct is only valid as long as the action client is valid.
 * The values in the struct may change if the action client's options change,
 * and therefore copying the struct is recommended if this is a concern.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] action_client pointer to the action client
 * \return options struct if successful, otherwise `NULL`
 */
RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
const rcl_action_client_options_t *
rcl_action_client_get_options(const rcl_action_client_t * action_client);

/// Check that a rcl_action_client_t is valid.
/**
 * The bool returned is `false` if `action_client` is invalid.
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
 * \param[in] action_client pointer to the rcl action client
 * \return `true` if `action_client` is valid, otherwise `false`
 */
RCL_ACTION_PUBLIC
bool
rcl_action_client_is_valid(
  const rcl_action_client_t * action_client);

RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_set_goal_client_callback(
  const rcl_action_client_t * action_client,
  rcl_event_callback_t callback,
  const void * user_data);

RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_set_cancel_client_callback(
  const rcl_action_client_t * action_client,
  rcl_event_callback_t callback,
  const void * user_data);

RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_set_result_client_callback(
  const rcl_action_client_t * action_client,
  rcl_event_callback_t callback,
  const void * user_data);

RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_set_feedback_subscription_callback(
  const rcl_action_client_t * action_client,
  rcl_event_callback_t callback,
  const void * user_data);

RCL_ACTION_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_action_client_set_status_subscription_callback(
  const rcl_action_client_t * action_client,
  rcl_event_callback_t callback,
  const void * user_data);

#ifdef __cplusplus
}
#endif

#endif  // RCL_ACTION__ACTION_CLIENT_H_
