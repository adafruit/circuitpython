// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__CLIENT_H_
#define RCL__CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"

#include "rcl/allocator.h"
#include "rcl/event_callback.h"
#include "rcl/macros.h"
#include "rcl/node.h"
#include "rcl/publisher.h"
#include "rcl/service_introspection.h"
#include "rcl/time.h"
#include "rcl/visibility_control.h"

#include "rmw/types.h"

/// Internal rcl client implementation struct.
typedef struct rcl_client_impl_s rcl_client_impl_t;

/// Structure which encapsulates a ROS Client.
typedef struct rcl_client_s
{
  /// Pointer to the client implementation
  rcl_client_impl_t * impl;
} rcl_client_t;

/// Options available for a rcl_client_t.
typedef struct rcl_client_options_s
{
  /// Middleware quality of service settings for the client.
  rmw_qos_profile_t qos;
  /// Custom allocator for the client, used for incidental allocations.
  /** For default behavior (malloc/free), use: rcl_get_default_allocator() */
  rcl_allocator_t allocator;
} rcl_client_options_t;

/// Return a rcl_client_t struct with members set to `NULL`.
/**
 * Should be called to get a null rcl_client_t before passing to
 * rcl_client_init().
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_client_t
rcl_get_zero_initialized_client(void);

/// Initialize a rcl client.
/**
 * After calling this function on a rcl_client_t, it can be used to send
 * requests of the given type by calling rcl_send_request().
 * If the request is received by a (possibly remote) service and if the service
 * sends a response, the client can access the response through
 * rcl_take_response() once the response is available to the client.
 *
 * The given rcl_node_t must be valid and the resulting rcl_client_t is only
 * valid as long as the given rcl_node_t remains valid.
 *
 * The rosidl_service_type_support_t is obtained on a per `.srv` type basis.
 * When the user defines a ROS service, code is generated which provides the
 * required rosidl_service_type_support_t object.
 * This object can be obtained using a language appropriate mechanism.
 * \todo TODO(wjwwood) write these instructions once and link to it instead
 *
 * For C, a macro can be used (for example `example_interfaces/AddTwoInts`):
 *
 * ```c
 * #include <rosidl_runtime_c/service_type_support_struct.h>
 * #include <example_interfaces/srv/add_two_ints.h>
 *
 * const rosidl_service_type_support_t * ts =
 *   ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
 * ```
 *
 * For C++, a template function is used:
 *
 * ```cpp
 * #include <rosidl_typesupport_cpp/service_type_support.hpp>
 * #include <example_interfaces/srv/add_two_ints.hpp>
 *
 * using rosidl_typesupport_cpp::get_service_type_support_handle;
 * const rosidl_service_type_support_t * ts =
 *   get_service_type_support_handle<example_interfaces::srv::AddTwoInts>();
 * ```
 *
 * The rosidl_service_type_support_t object contains service type specific
 * information used to send or take requests and responses.
 *
 * The topic name must be a c string which follows the topic and service name
 * format rules for unexpanded names, also known as non-fully qualified names:
 *
 * \see rcl_expand_topic_name
 *
 * The options struct allows the user to set the quality of service settings as
 * well as a custom allocator which is used when initializing/finalizing the
 * client to allocate space for incidentals, e.g. the service name string.
 *
 * Expected usage (for C services):
 *
 * ```c
 * #include <rcl/rcl.h>
 * #include <rosidl_runtime_c/service_type_support_struct.h>
 * #include <example_interfaces/srv/add_two_ints.h>
 *
 * rcl_node_t node = rcl_get_zero_initialized_node();
 * rcl_node_options_t node_ops = rcl_node_get_default_options();
 * rcl_ret_t ret = rcl_node_init(&node, "node_name", "/my_namespace", &node_ops);
 * // ... error handling
 * const rosidl_service_type_support_t * ts =
 *   ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
 * rcl_client_t client = rcl_get_zero_initialized_client();
 * rcl_client_options_t client_ops = rcl_client_get_default_options();
 * ret = rcl_client_init(&client, &node, ts, "add_two_ints", &client_ops);
 * // ... error handling, and on shutdown do finalization:
 * ret = rcl_client_fini(&client, &node);
 * // ... error handling for rcl_client_fini()
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
 * \param[inout] client preallocated rcl_client_t structure
 * \param[in] node valid rcl_node_t
 * \param[in] type_support type support object for the service's type
 * \param[in] service_name the name of the service to request
 * \param[in] options client options, including quality of service settings
 * \return #RCL_RET_OK if the client was initialized successfully, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_ALREADY_INIT if the client is already initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory fails, or
 * \return #RCL_RET_SERVICE_NAME_INVALID if the given service name is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_client_init(
  rcl_client_t * client,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name,
  const rcl_client_options_t * options);

/// Finalize a rcl_client_t.
/**
 * After calling this function, calls to rcl_send_request() and
 * rcl_take_response() will fail when using this client.
 * However, the given node handle is still valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] client handle to the client to be finalized
 * \param[in] node a valid (not finalized) handle to the node used to create the client
 * \return #RCL_RET_OK if client was finalized successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_client_fini(rcl_client_t * client, rcl_node_t * node);

/// Return the default client options in a rcl_client_options_t.
/**
 * The defaults are:
 *
 * - qos = rmw_qos_profile_services_default
 * - allocator = rcl_get_default_allocator()
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_client_options_t
rcl_client_get_default_options(void);

/// Send a ROS request using a client.
/**
 * It is the job of the caller to ensure that the type of the `ros_request`
 * parameter and the type associate with the client (via the type support)
 * match.
 * Passing a different type to `send_request` produces undefined behavior and
 * cannot be checked by this function and therefore no deliberate error will
 * occur.
 *
 * rcl_send_request() is an non-blocking call.
 *
 * The ROS request message given by the `ros_request` void pointer is always
 * owned by the calling code, but should remain constant during `send_request`.
 *
 * This function is thread safe so long as access to both the client and the
 * `ros_request` is synchronized.
 * That means that calling rcl_send_request() from multiple threads is allowed,
 * but calling rcl_send_request() at the same time as non-thread safe client
 * functions is not, e.g. calling rcl_send_request() and rcl_client_fini()
 * concurrently is not allowed.
 * The message cannot change during the rcl_send_request() call.
 * Before calling rcl_send_request() the message can change but after calling
 * rcl_send_request() it depends on RMW implementation behavior.
 * The same `ros_request`, however, can be passed to multiple calls of
 * rcl_send_request() simultaneously, even if the clients differ.
 * The `ros_request` is unmodified by rcl_send_request().
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] for unique pairs of clients and requests, see above for more</i>
 *
 * \param[in] client handle to the client which will make the response
 * \param[in] ros_request type-erased pointer to the ROS request message
 * \param[out] sequence_number the sequence number
 * \return #RCL_RET_OK if the request was sent successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_CLIENT_INVALID if the client is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_send_request(const rcl_client_t * client, const void * ros_request, int64_t * sequence_number);


/// Take a ROS response using a client
/**
 * It is the job of the caller to ensure that the type of the `ros_response`
 * parameter and the type associate with the client (via the type support)
 * match.
 * Passing a different type to take_response produces undefined behavior and
 * cannot be checked by this function and therefore no deliberate error will
 * occur.
 * The request_header is an rmw struct for meta-information about the request
 * sent (e.g. the sequence number).
 * The caller must provide a pointer to an allocated struct.
 * This function will populate the struct's fields.
 * `ros_response` should point to an already allocated ROS response message
 * struct of the correct type, into which the response from the service will be
 * copied.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Maybe [1]
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] only if required when filling the message, avoided for fixed sizes</i>
 *
 * \param[in] client handle to the client which will take the response
 * \param[inout] request_header pointer to the request header
 * \param[inout] ros_response type-erased pointer to the ROS response message
 * \return #RCL_RET_OK if the response was taken successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_CLIENT_INVALID if the client is invalid, or
 * \return #RCL_RET_CLIENT_TAKE_FAILED if take failed but no error occurred
 *         in the middleware, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_take_response_with_info(
  const rcl_client_t * client,
  rmw_service_info_t * request_header,
  void * ros_response);

/// backwards compatibility function that takes a rmw_request_id_t only
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_take_response(
  const rcl_client_t * client,
  rmw_request_id_t * request_header,
  void * ros_response);

/// Get the name of the service that this client will request a response from.
/**
 * This function returns the client's internal service name string.
 * This function can fail, and therefore return `NULL`, if the:
 *   - client is `NULL`
 *   - client is invalid (never called init, called fini, or invalid node)
 *
 * The returned string is only valid as long as the rcl_client_t is valid.
 * The value of the string may change if the service name changes, and therefore
 * copying the string is recommended if this is a concern.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] client pointer to the client
 * \return name string if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const char *
rcl_client_get_service_name(const rcl_client_t * client);

/// Return the rcl client options.
/**
 * This function returns the client's internal options struct.
 * This function can fail, and therefore return `NULL`, if the:
 *   - client is `NULL`
 *   - client is invalid (never called init, called fini, or invalid node)
 *
 * The returned struct is only valid as long as the rcl_client_t is valid.
 * The values in the struct may change if the options of the client change,
 * and therefore copying the struct is recommended if this is a concern.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] client pointer to the client
 * \return options struct if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rcl_client_options_t *
rcl_client_get_options(const rcl_client_t * client);

/// Return the rmw client handle.
/**
 * The handle returned is a pointer to the internally held rmw handle.
 * This function can fail, and therefore return `NULL`, if the:
 *   - client is `NULL`
 *   - client is invalid (never called init, called fini, or invalid node)
 *
 * The returned handle is made invalid if the client is finalized or if
 * rcl_shutdown() is called.
 * The returned handle is not guaranteed to be valid for the life time of the
 * client as it may be finalized and recreated itself.
 * Therefore it is recommended to get the handle from the client using
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
 * \param[in] client pointer to the rcl client
 * \return rmw client handle if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rmw_client_t *
rcl_client_get_rmw_handle(const rcl_client_t * client);

/// Check that the client is valid.
/**
 * The bool returned is `false` if client is invalid.
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
 * \param[in] client pointer to the rcl client
 * \return `true` if `client` is valid, otherwise `false`
 */
RCL_PUBLIC
bool
rcl_client_is_valid(const rcl_client_t * client);

/// Get the actual qos settings of the client's request publisher.
/**
 * Used to get the actual qos settings of the client's request publisher.
 * The actual configuration applied when using RMW_*_SYSTEM_DEFAULT
 * can only be resolved after the creation of the client, and it
 * depends on the underlying rmw implementation.
 * If the underlying setting in use can't be represented in ROS terms,
 * it will be set to RMW_*_UNKNOWN.
 * The returned struct is only valid as long as the rcl_client_t is valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] client pointer to the rcl client
 * \return qos struct if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rmw_qos_profile_t *
rcl_client_request_publisher_get_actual_qos(const rcl_client_t * client);

/// Get the actual qos settings of the client's response subscription.
/**
 * Used to get the actual qos settings of the client's response subscription.
 * The actual configuration applied when using RMW_*_SYSTEM_DEFAULT
 * can only be resolved after the creation of the client, and it
 * depends on the underlying rmw implementation.
 * If the underlying setting in use can't be represented in ROS terms,
 * it will be set to RMW_*_UNKNOWN.
 * The returned struct is only valid as long as the rcl_client_t is valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] client pointer to the rcl client
 * \return qos struct if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rmw_qos_profile_t *
rcl_client_response_subscription_get_actual_qos(const rcl_client_t * client);

/// Set the on new response callback function for the client.
/**
 * This API sets the callback function to be called whenever the
 * client is notified about a new response.
 *
 * \sa rmw_client_set_on_new_response_callback for details about this function.
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
 * \param[in] client The client on which to set the callback
 * \param[in] callback The callback to be called when new responses arrive, may be NULL
 * \param[in] user_data Given to the callback when called later, may be NULL
 * \return `RCL_RET_OK` if callback was set to the listener, or
 * \return `RCL_RET_INVALID_ARGUMENT` if `client` is NULL, or
 * \return `RCL_RET_UNSUPPORTED` if the API is not implemented in the dds implementation
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_client_set_on_new_response_callback(
  const rcl_client_t * client,
  rcl_event_callback_t callback,
  const void * user_data);

/// Configures service introspection features for the client.
/**
 * Enables or disables service introspection features for this client.
 * If the introspection state is RCL_SERVICE_INTROSPECTION_OFF, then introspection will
 * be disabled.  If the state is RCL_SERVICE_INTROSPECTION_METADATA, the client metadata
 * will be published.  If the state is RCL_SERVICE_INTROSPECTION_CONTENTS, then the client
 * metadata and service request and response contents will be published.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | Maybe [1]
 * Lock-Free          | Maybe [1]
 * <i>[1] rmw implementation defined</i>
 *
 * \param[in] client client on which to configure service introspection
 * \param[in] node valid rcl_node_t to use to create the introspection publisher
 * \param[in] clock valid rcl_clock_t to use to generate the introspection timestamps
 * \param[in] type_support type support library associated with this client
 * \param[in] publisher_options options to use when creating the introspection publisher
 * \param[in] introspection_state rcl_service_introspection_state_t describing whether
 *            introspection should be OFF, METADATA, or CONTENTS
 * \return #RCL_RET_OK if the call was successful, or
 * \return #RCL_RET_ERROR if the event publisher is invalid, or
 * \return #RCL_RET_NODE_INVALID if the given node is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if the client or node structure is invalid,
 * \return #RCL_RET_BAD_ALLOC if a memory allocation failed
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_client_configure_service_introspection(
  rcl_client_t * client,
  rcl_node_t * node,
  rcl_clock_t * clock,
  const rosidl_service_type_support_t * type_support,
  const rcl_publisher_options_t publisher_options,
  rcl_service_introspection_state_t introspection_state);

#ifdef __cplusplus
}
#endif

#endif  // RCL__CLIENT_H_
