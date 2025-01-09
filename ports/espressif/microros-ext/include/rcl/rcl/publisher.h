// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__PUBLISHER_H_
#define RCL__PUBLISHER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rosidl_runtime_c/message_type_support_struct.h"

#include "rcl/macros.h"
#include "rcl/node.h"
#include "rcl/visibility_control.h"
#include "rcl/time.h"

/// Internal rcl publisher implementation struct.
typedef struct rcl_publisher_impl_s rcl_publisher_impl_t;

/// Structure which encapsulates a ROS Publisher.
typedef struct rcl_publisher_s
{
  /// Pointer to the publisher implementation
  rcl_publisher_impl_t * impl;
} rcl_publisher_t;

/// Options available for a rcl publisher.
typedef struct rcl_publisher_options_s
{
  /// Middleware quality of service settings for the publisher.
  rmw_qos_profile_t qos;
  /// Custom allocator for the publisher, used for incidental allocations.
  /** For default behavior (malloc/free), use: rcl_get_default_allocator() */
  rcl_allocator_t allocator;
  /// rmw specific publisher options, e.g. the rmw implementation specific payload.
  rmw_publisher_options_t rmw_publisher_options;
  /// Disable flag to LoanedMessage, initialized via environmental variable.
  bool disable_loaned_message;
} rcl_publisher_options_t;

/// Return a rcl_publisher_t struct with members set to `NULL`.
/**
 * Should be called to get a null rcl_publisher_t before passing to
 * rcl_publisher_init().
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_publisher_t
rcl_get_zero_initialized_publisher(void);

/// Initialize a rcl publisher.
/**
 * After calling this function on a rcl_publisher_t, it can be used to publish
 * messages of the given type to the given topic using rcl_publish().
 *
 * The given rcl_node_t must be valid and the resulting rcl_publisher_t is only
 * valid as long as the given rcl_node_t remains valid.
 *
 * The rosidl_message_type_support_t is obtained on a per .msg type basis.
 * When the user defines a ROS message, code is generated which provides the
 * required rosidl_message_type_support_t object.
 * This object can be obtained using a language appropriate mechanism.
 * \todo TODO(wjwwood) write these instructions once and link to it instead
 *
 * For C, a macro can be used (for example `std_msgs/String`):
 *
 * ```c
 * #include <rosidl_runtime_c/message_type_support_struct.h>
 * #include <std_msgs/msg/string.h>
 * const rosidl_message_type_support_t * string_ts =
 *   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
 * ```
 *
 * For C++, a template function is used:
 *
 * ```cpp
 * #include <rosidl_typesupport_cpp/message_type_support.hpp>
 * #include <std_msgs/msg/string.hpp>
 * const rosidl_message_type_support_t * string_ts =
 *   rosidl_typesupport_cpp::get_message_type_support_handle<std_msgs::msg::String>();
 * ```
 *
 * The rosidl_message_type_support_t object contains message type specific
 * information used to publish messages.
 *
 * The topic name must be a c string which follows the topic and service name
 * format rules for unexpanded names, also known as non-fully qualified names:
 *
 * \see rcl_expand_topic_name
 *
 * The options struct allows the user to set the quality of service settings as
 * well as a custom allocator which is used when initializing/finalizing the
 * publisher to allocate space for incidentals, e.g. the topic name string.
 *
 * Expected usage (for C messages):
 *
 * ```c
 * #include <rcl/rcl.h>
 * #include <rosidl_runtime_c/message_type_support_struct.h>
 * #include <std_msgs/msg/string.h>
 *
 * rcl_node_t node = rcl_get_zero_initialized_node();
 * rcl_node_options_t node_ops = rcl_node_get_default_options();
 * rcl_ret_t ret = rcl_node_init(&node, "node_name", "/my_namespace", &node_ops);
 * // ... error handling
 * const rosidl_message_type_support_t * ts = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
 * rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
 * rcl_publisher_options_t publisher_ops = rcl_publisher_get_default_options();
 * ret = rcl_publisher_init(&publisher, &node, ts, "chatter", &publisher_ops);
 * // ... error handling, and on shutdown do finalization:
 * ret = rcl_publisher_fini(&publisher, &node);
 * // ... error handling for rcl_publisher_fini()
 * ret = rcl_node_fini(&node);
 * // ... error handling for rcl_deinitialize_node()
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
 * \param[inout] publisher preallocated publisher structure
 * \param[in] node valid rcl node handle
 * \param[in] type_support type support object for the topic's type
 * \param[in] topic_name the name of the topic to publish on
 * \param[in] options publisher options, including quality of service settings
 * \return #RCL_RET_OK if the publisher was initialized successfully, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_ALREADY_INIT if the publisher is already initialized, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory fails, or
 * \return #RCL_RET_TOPIC_NAME_INVALID if the given topic name is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_init(
  rcl_publisher_t * publisher,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rcl_publisher_options_t * options);

/// Finalize a rcl_publisher_t.
/**
 * After calling, the node will no longer be advertising that it is publishing
 * on this topic (assuming this is the only publisher on this topic).
 *
 * After calling, calls to rcl_publish will fail when using this publisher.
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
 * \param[inout] publisher handle to the publisher to be finalized
 * \param[in] node a valid (not finalized) handle to the node used to create the publisher
 * \return #RCL_RET_OK if publisher was finalized successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_NODE_INVALID if the node is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_fini(rcl_publisher_t * publisher, rcl_node_t * node);

/// Return the default publisher options in a rcl_publisher_options_t.
/**
 * The defaults are:
 *
 * - qos = rmw_qos_profile_default
 * - allocator = rcl_get_default_allocator()
 * - rmw_publisher_options = rmw_get_default_publisher_options()
 * - disable_loaned_message = false, true only if ROS_DISABLE_LOANED_MESSAGES=1
 *
 * \return A structure with the default publisher options.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_publisher_options_t
rcl_publisher_get_default_options(void);

/// Borrow a loaned message.
/**
 * The memory allocated for the ros message belongs to the middleware and must not be deallocated
 * other than by a call to \sa rcl_return_loaned_message_from_publisher.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No [0]
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * [0] the underlying middleware might allocate new memory or returns an existing chunk form a pool.
 * The function in rcl however does not allocate any additional memory.
 *
 * \param[in] publisher Publisher to which the allocated message is associated.
 * \param[in] type_support Typesupport to which the internal ros message is allocated.
 * \param[out] ros_message The pointer to be filled to a valid ros message by the middleware.
 * \return #RCL_RET_OK if the ros message was correctly initialized, or
 * \return #RCL_RET_PUBLISHER_INVALID if the passed publisher is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if an argument other than the ros message is null, or
 * \return #RCL_RET_BAD_ALLOC if the ros message could not be correctly created, or
 * \return #RCL_RET_UNSUPPORTED if the middleware does not support that feature, or
 * \return #RCL_RET_ERROR if an unexpected error occured.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_borrow_loaned_message(
  const rcl_publisher_t * publisher,
  const rosidl_message_type_support_t * type_support,
  void ** ros_message);

/// Return a loaned message previously borrowed from a publisher.
/**
 * The ownership of the passed in ros message will be transferred back to the middleware.
 * The middleware might deallocate and destroy the message so that the pointer is no longer
 * guaranteed to be valid after that call.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher Publisher to which the loaned message is associated.
 * \param[in] loaned_message Loaned message to be deallocated and destroyed.
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if an argument is null, or
 * \return #RCL_RET_UNSUPPORTED if the middleware does not support that feature, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_ERROR if an unexpected error occurs and no message can be initialized.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_return_loaned_message_from_publisher(
  const rcl_publisher_t * publisher,
  void * loaned_message);

/// Publish a ROS message on a topic using a publisher.
/**
 * It is the job of the caller to ensure that the type of the ros_message
 * parameter and the type associate with the publisher (via the type support)
 * match.
 * Passing a different type to publish produces undefined behavior and cannot
 * be checked by this function and therefore no deliberate error will occur.
 *
 * \todo TODO(wjwwood):
 *   The blocking behavior of publish is a still a point of dispute.
 *   This section should be updated once the behavior is clearly defined.
 *   See: https://github.com/ros2/ros2/issues/255
 *
 * Calling rcl_publish() is a potentially blocking call.
 * When called rcl_publish() will immediately do any publishing related work,
 * including, but not limited to, converting the message into a different type,
 * serializing the message, collecting publish statistics, etc.
 * The last thing it will do is call the underlying middleware's publish
 * function which may or may not block based on the quality of service settings
 * given via the publisher options in rcl_publisher_init().
 * For example, if the reliability is set to reliable, then a publish may block
 * until space in the publish queue is available, but if the reliability is set
 * to best effort then it should not block.
 *
 * The ROS message given by the `ros_message` void pointer is always owned by
 * the calling code, but should remain constant during publish.
 *
 * This function is thread safe so long as access to both the publisher and the
 * `ros_message` is synchronized.
 * That means that calling rcl_publish() from multiple threads is allowed, but
 * calling rcl_publish() at the same time as non-thread safe publisher
 * functions is not, e.g. calling rcl_publish() and rcl_publisher_fini()
 * concurrently is not allowed.
 * The message cannot change during the rcl_publish() call.
 * Before calling rcl_publish() the message can change but after calling
 * rcl_publish() it depends on RMW implementation behavior.
 * The same `ros_message`, however, can be passed to multiple calls of
 * rcl_publish() simultaneously, even if the publishers differ.
 * The `ros_message` is unmodified by rcl_publish().
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] for unique pairs of publishers and messages, see above for more</i>
 *
 * \param[in] publisher handle to the publisher which will do the publishing
 * \param[in] ros_message type-erased pointer to the ROS message
 * \param[in] allocation structure pointer, used for memory preallocation (may be NULL)
 * \return #RCL_RET_OK if the message was published successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publish(
  const rcl_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation);

/// Publish a serialized message on a topic using a publisher.
/**
 * It is the job of the caller to ensure that the type of the serialized message
 * parameter and the type associate with the publisher (via the type support)
 * match.
 * Even though this call to publish takes an already serialized serialized message,
 * the publisher has to register its type as a ROS known message type.
 * Passing a serialized message from a different type leads to undefined behavior on the subscriber side.
 * The publish call might be able to send any abitrary serialized message, it is however
 * not garantueed that the subscriber side successfully deserializes this byte stream.
 *
 * Apart from this, the `publish_serialized` function has the same behavior as rcl_publish()
 * expect that no serialization step is done.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[1] for unique pairs of publishers and messages, see above for more</i>
 *
 * \param[in] publisher handle to the publisher which will do the publishing
 * \param[in] serialized_message  pointer to the already serialized message in raw form
 * \param[in] allocation structure pointer, used for memory preallocation (may be NULL)
 * \return #RCL_RET_OK if the message was published successfully, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publish_serialized_message(
  const rcl_publisher_t * publisher,
  const rcl_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation);

/// Publish a loaned message on a topic using a publisher.
/**
 * A previously borrowed loaned message can be sent via this call to rcl_publish_loaned_message().
 * By calling this function, the ownership of the loaned message is getting transferred back
 * to the middleware.
 * The pointer to the `ros_message` is not guaranteed to be valid after as the middleware
 * migth deallocate the memory for this message internally.
 * It is thus recommended to call this function only in combination with
 * \sa rcl_borrow_loaned_message().
 *
 * Apart from this, the `publish_loaned_message` function has the same behavior as rcl_publish()
 * except that no serialization step is done.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No [0]
 * Thread-Safe        | Yes [1]
 * Uses Atomics       | No
 * Lock-Free          | Yes
 * <i>[0] the middleware might deallocate the loaned message.
 * The RCL function however does not allocate any memory.</i>
 * <i>[1] for unique pairs of publishers and messages, see above for more</i>
 *
 * \param[in] publisher handle to the publisher which will do the publishing
 * \param[in] ros_message  pointer to the previously borrow loaned message
 * \param[in] allocation structure pointer, used for memory preallocation (may be NULL)
 * \return #RCL_RET_OK if the message was published successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_UNSUPPORTED if the middleware does not support that feature, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publish_loaned_message(
  const rcl_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation);

/// Manually assert that this Publisher is alive (for RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC)
/**
 * If the rmw Liveliness policy is set to RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC, the creator of
 * this publisher may manually call `assert_liveliness` at some point in time to signal to the rest
 * of the system that this Node is still alive.
 * This function must be called at least as often as the qos_profile's liveliness_lease_duration
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher handle to the publisher that needs liveliness to be asserted
 * \return #RCL_RET_OK if the liveliness assertion was completed successfully, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_assert_liveliness(const rcl_publisher_t * publisher);

/// Wait until all published message data is acknowledged or until the specified timeout elapses.
/**
 * This function waits until all published message data were acknowledged by peer node or timeout.
 *
 * The timeout unit is nanoseconds.
 * If the timeout is negative then this function will block indefinitely until all published message
 * data were acknowledged.
 * If the timeout is 0 then this function will be non-blocking; checking all published message data
 * were acknowledged (If acknowledged, return RCL_RET_OK. Otherwise, return RCL_RET_TIMEOUT), but
 * not waiting.
 * If the timeout is greater than 0 then this function will return after that period of time has
 * elapsed (return RCL_RET_TIMEOUT) or all published message data were acknowledged (return
 * RCL_RET_OK).
 *
 * This function only waits for acknowledgments if the publisher's QOS profile is RELIABLE.
 * Otherwise this function will immediately return RCL_RET_OK.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[in] publisher handle to the publisher that needs to wait for all acked.
 * \param[in] timeout the duration to wait for all published message data were acknowledged, in
 *   nanoseconds.
 * \return #RCL_RET_OK if successful, or
 * \return #RCL_RET_TIMEOUT if timed out, or
 * \return #RCL_RET_PUBLISHER_INVALID if publisher is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs, or
 * \return #RCL_RET_UNSUPPORTED if the middleware does not support that feature.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_wait_for_all_acked(
  const rcl_publisher_t * publisher,
  rcl_duration_value_t timeout);

/// Get the topic name for the publisher.
/**
 * This function returns the publisher's internal topic name string.
 * This function can fail, and therefore return `NULL`, if the:
 *   - publisher is `NULL`
 *   - publisher is invalid (never called init, called fini, or invalid node)
 *
 * The returned string is only valid as long as the rcl_publisher_t is valid.
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
 * \param[in] publisher pointer to the publisher
 * \return name string if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const char *
rcl_publisher_get_topic_name(const rcl_publisher_t * publisher);

/// Return the rcl publisher options.
/**
 * This function returns the publisher's internal options struct.
 * This function can fail, and therefore return `NULL`, if the:
 *   - publisher is `NULL`
 *   - publisher is invalid (never called init, called fini, or invalid node)
 *
 * The returned struct is only valid as long as the rcl_publisher_t is valid.
 * The values in the struct may change if the options of the publisher change,
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
 * \param[in] publisher pointer to the publisher
 * \return options struct if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rcl_publisher_options_t *
rcl_publisher_get_options(const rcl_publisher_t * publisher);

/// Return the rmw publisher handle.
/**
 * The handle returned is a pointer to the internally held rmw handle.
 * This function can fail, and therefore return `NULL`, if the:
 *   - publisher is `NULL`
 *   - publisher is invalid (never called init, called fini, or invalid node)
 *
 * The returned handle is made invalid if the publisher is finalized or if
 * rcl_shutdown() is called.
 * The returned handle is not guaranteed to be valid for the life time of the
 * publisher as it may be finalized and recreated itself.
 * Therefore it is recommended to get the handle from the publisher using
 * this function each time it is needed and avoid use of the handle
 * concurrently with functions that might change it.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher pointer to the rcl publisher
 * \return rmw publisher handle if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rmw_publisher_t *
rcl_publisher_get_rmw_handle(const rcl_publisher_t * publisher);

/// Return the context associated with this publisher.
/**
 * This function can fail, and therefore return `NULL`, if the:
 *   - publisher is `NULL`
 *   - publisher is invalid (never called init, called fini, etc.)
 *
 * The returned context is made invalid if the publisher is finalized or if
 * rcl_shutdown() is called.
 * Therefore it is recommended to get the handle from the publisher using
 * this function each time it is needed and avoid use of the handle
 * concurrently with functions that might change it.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher pointer to the rcl publisher
 * \return context if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_context_t *
rcl_publisher_get_context(const rcl_publisher_t * publisher);

/// Return true if the publisher is valid, otherwise false.
/**
 * The bool returned is `false` if `publisher` is invalid.
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
 * \param[in] publisher pointer to the rcl publisher
 * \return `true` if `publisher` is valid, otherwise `false`
 */
RCL_PUBLIC
bool
rcl_publisher_is_valid(const rcl_publisher_t * publisher);

/// Return true if the publisher is valid except the context, otherwise false.
/**
 * This is used in clean up functions that need to access the publisher, but do
 * not need use any functions with the context.
 *
 * It is identical to rcl_publisher_is_valid except it ignores the state of the
 * context associated with the publisher.
 * \sa rcl_publisher_is_valid()
 */
RCL_PUBLIC
bool
rcl_publisher_is_valid_except_context(const rcl_publisher_t * publisher);

/// Get the number of subscriptions matched to a publisher.
/**
 * Used to get the internal count of subscriptions matched to a publisher.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | Maybe [1]
 * Lock-Free          | Maybe [1]
 * <i>[1] only if the underlying rmw doesn't make use of this feature </i>
 *
 * \param[in] publisher pointer to the rcl publisher
 * \param[out] subscription_count number of matched subscriptions
 * \return #RCL_RET_OK if the count was retrieved, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_PUBLISHER_INVALID if the publisher is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_publisher_get_subscription_count(
  const rcl_publisher_t * publisher,
  size_t * subscription_count);

/// Get the actual qos settings of the publisher.
/**
 * Used to get the actual qos settings of the publisher.
 * The actual configuration applied when using RMW_*_SYSTEM_DEFAULT
 * can only be resolved after the creation of the publisher, and it
 * depends on the underlying rmw implementation.
 * If the underlying setting in use can't be represented in ROS terms,
 * it will be set to RMW_*_UNKNOWN.
 * The returned struct is only valid as long as the rcl_publisher_t is valid.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher pointer to the rcl publisher
 * \return qos struct if successful, otherwise `NULL`
 */
RCL_PUBLIC
RCL_WARN_UNUSED
const rmw_qos_profile_t *
rcl_publisher_get_actual_qos(const rcl_publisher_t * publisher);


/// Check if publisher instance can loan messages.
/**
 * Depending on the middleware and the message type, this will return true if the middleware
 * can allocate a ROS message instance.
 */
RCL_PUBLIC
bool
rcl_publisher_can_loan_messages(const rcl_publisher_t * publisher);

#ifdef __cplusplus
}
#endif

#endif  // RCL__PUBLISHER_H_
