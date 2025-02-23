// Copyright 2014-2017 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__TYPES_H_
#define RMW__TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// map rcutils specific log levels to rmw speicfic type
#include <rcutils/logging.h>

#include "rmw/events_statuses/events_statuses.h"
#include "rmw/init.h"
#include "rmw/init_options.h"
#include "rmw/ret_types.h"
#include "rmw/security_options.h"
#include "rmw/serialized_message.h"
#include "rmw/subscription_content_filter_options.h"
#include "rmw/time.h"
#include "rmw/visibility_control.h"

// We define the GID as 16 bytes (128 bits).  This should be enough to ensure
// uniqueness amongst all entities in the system.  It is up to the individual
// RMW implementations to fill that in, either from the underlying middleware
// or from the RMW layer itself.
#define RMW_GID_STORAGE_SIZE 16u

/// Structure which encapsulates an rmw node
typedef struct RMW_PUBLIC_TYPE rmw_node_s
{
  /// Name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this node's data
  void * data;

  /// A concise name of this rmw node for identification
  const char * name;

  /// The namespace of this rmw node
  const char * namespace_;

  /// Context information about node's init specific information
  rmw_context_t * context;
} rmw_node_t;

/// Endpoint enumeration type
typedef enum RMW_PUBLIC_TYPE rmw_endpoint_type_e
{
  /// Endpoint type has not yet been set
  RMW_ENDPOINT_INVALID = 0,

  /// Creates and publishes messages to the ROS topic
  RMW_ENDPOINT_PUBLISHER,

  /// Listens for and receives messages from a topic
  RMW_ENDPOINT_SUBSCRIPTION
} rmw_endpoint_type_t;

/// Unique network flow endpoints requirement enumeration
typedef enum RMW_PUBLIC_TYPE rmw_unique_network_flow_endpoints_requirement_e
{
  /// Unique network flow endpoints not required
  RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_NOT_REQUIRED = 0,

  /// Unique network flow endpoins strictly required.
  /// Error if not provided by RMW implementation.
  RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_STRICTLY_REQUIRED,

  /// Unique network flow endpoints optionally required.
  /// No error if not provided RMW implementation.
  RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_OPTIONALLY_REQUIRED,

  /// Unique network flow endpoints requirement decided by system.
  RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_SYSTEM_DEFAULT
} rmw_unique_network_flow_endpoints_requirement_t;

/// Options that can be used to configure the creation of a publisher in rmw.
typedef struct RMW_PUBLIC_TYPE rmw_publisher_options_s
{
  /// Used to pass rmw implementation specific resources during publisher creation.
  /**
   * This field is type erased (rather than forward declared) because it will
   * usually be a non-owned reference to an language specific object, e.g.
   * C++ it may be a polymorphic class that only the rmw implementation can use.
   *
   * The resource pointed to here needs to outlive this options structure, and
   * any rmw_publisher objects that are created using it, as they copy this
   * structure and may use this payload throughout their lifetime.
   */
  void * rmw_specific_publisher_payload;

  /// Require middleware to generate unique network flow endpoints.
  /**
   * Unique network flow endpoints are required to differentiate the QoS provided by
   * networks for flows between publishers and subscribers in communicating
   * nodes.
   * Default value is RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_NOT_REQUIRED.
   */
  rmw_unique_network_flow_endpoints_requirement_t require_unique_network_flow_endpoints;
} rmw_publisher_options_t;

/// Structure which encapsulates an rmw publisher
typedef struct RMW_PUBLIC_TYPE rmw_publisher_s
{
  /// Name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this publisher's data
  void * data;

  /// The name of the ROS topic this publisher publishes to
  const char * topic_name;

  /// Publisher options.
  /**
   * The options structure passed to rmw_create_publisher() should be
   * assigned to this field by the rmw implementation.
   * The fields should not be modified after creation, but
   * the contents of the options structure may or may not be const, i.e.
   * shallow const-ness.
   * This field is not marked const to avoid any const casting during setup.
   */
  rmw_publisher_options_t options;

  /// Indicate whether this publisher supports loaning messages
  bool can_loan_messages;
} rmw_publisher_t;

/// Options that can be used to configure the creation of a subscription in rmw.
typedef struct RMW_PUBLIC_TYPE rmw_subscription_options_s
{
  /// Used to pass rmw implementation specific resources during subscription creation.
  /**
   * All the same details and restrictions of this field in
   * rmw_publisher_options_t apply to this struct as well.
   *
   * \sa rmw_publisher_options_t.rmw_specific_publisher_payload
   */
  void * rmw_specific_subscription_payload;

  /// If true then the middleware should not deliver data from local publishers.
  /**
   * This setting is most often used when data should only be received from
   * remote nodes, especially to avoid "double delivery" when both intra- and
   * inter- process communication is taking place.
   *
   * The definition of local means that in the same context.
   */
  bool ignore_local_publications;

  /// Require middleware to generate unique network flow endpoints.
  /**
   * Unique network flow endpoints are required to differentiate the QoS provided by
   * networks for flows between publishers and subscribers in communicating
   * nodes.
   * Default value is RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_NOT_REQUIRED.
   */
  rmw_unique_network_flow_endpoints_requirement_t require_unique_network_flow_endpoints;

  /// Used to create a content filter options during subscription creation.
  rmw_subscription_content_filter_options_t * content_filter_options;
} rmw_subscription_options_t;

typedef struct RMW_PUBLIC_TYPE rmw_subscription_s
{
  /// Name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this subscription
  void * data;

  /// Name of the ros topic this subscription listens to
  const char * topic_name;

  /// Subscription options.
  /**
   * The options structure passed to rmw_create_subscription() should be
   * assigned to this field by the rmw implementation.
   * The fields should not be modified after creation, but
   * the contents of the options structure may or may not be const, i.e.
   * shallow const-ness.
   * This field is not marked const to avoid any const casting during setup.
   */
  rmw_subscription_options_t options;

  /// Indicates whether this subscription can loan messages
  bool can_loan_messages;

  /// Indicates whether content filtered topic of this subscription is enabled
  bool is_cft_enabled;
} rmw_subscription_t;

/// A handle to an rmw service
typedef struct RMW_PUBLIC_TYPE rmw_service_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this service
  void * data;

  /// The name of this service as exposed to the ros graph
  const char * service_name;
} rmw_service_t;

/// A handle to an rmw service client
typedef struct RMW_PUBLIC_TYPE rmw_client_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this service client
  void * data;

  /// The name of this service as exposed to the ros graph
  const char * service_name;
} rmw_client_t;

/// Handle for an rmw guard condition
typedef struct RMW_PUBLIC_TYPE rmw_guard_condition_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this guard condition
  void * data;

  /// rmw context associated with this guard condition
  rmw_context_t * context;
} rmw_guard_condition_t;

/// Allocation of memory for an rmw publisher
typedef struct RMW_PUBLIC_TYPE rmw_publisher_allocation_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this allocation
  void * data;
} rmw_publisher_allocation_t;

/// Allocation of memory for an rmw subscription
typedef struct RMW_PUBLIC_TYPE rmw_subscription_allocation_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// Type erased pointer to this allocation
  void * data;
} rmw_subscription_allocation_t;

/// Array of subscriber handles.
/**
 * An array of void * pointers representing type-erased middleware-specific subscriptions.
 * The number of non-null entries may be smaller than the allocated size of the array.
 * The number of subscriptions represented may be smaller than the allocated size of the array.
 * The creator of this struct is responsible for allocating and deallocating the array.
 */
typedef struct RMW_PUBLIC_TYPE rmw_subscriptions_s
{
  /// The number of subscribers represented by the array.
  size_t subscriber_count;
  /// Pointer to an array of void * pointers of subscriptions.
  void ** subscribers;
} rmw_subscriptions_t;

/// Array of service handles.
/**
 * An array of void * pointers representing type-erased middleware-specific services.
 * The number of non-null entries may be smaller than the allocated size of the array.
 * The number of services represented may be smaller than the allocated size of the array.
 * The creator of this struct is responsible for allocating and deallocating the array.
 */
typedef struct RMW_PUBLIC_TYPE rmw_services_s
{
  /// The number of services represented by the array.
  size_t service_count;
  /// Pointer to an array of void * pointers of services.
  void ** services;
} rmw_services_t;

/// Array of client handles.
/**
 * An array of void * pointers representing type-erased middleware-specific clients.
 * The number of non-null entries may be smaller than the allocated size of the array.
 * The number of clients represented may be smaller than the allocated size of the array.
 * The creator of this struct is responsible for allocating and deallocating the array.
 */
typedef struct RMW_PUBLIC_TYPE rmw_clients_s
{
  /// The number of clients represented by the array.
  size_t client_count;
  /// Pointer to an array of void * pointers of clients.
  void ** clients;
} rmw_clients_t;

typedef struct RMW_PUBLIC_TYPE rmw_events_s
{
  /// The number of events represented by the array.
  size_t event_count;
  /// Pointer to an array of void * pointers of events.
  void ** events;
} rmw_events_t;

/// Array of guard condition handles.
/**
 * An array of void * pointers representing type-erased middleware-specific guard conditions.
 * The number of non-null entries may be smaller than the allocated size of the array.
 * The number of guard conditions represented may be smaller than the allocated size of the array.
 * The creator of this struct is responsible for allocating and deallocating the array.
 */
typedef struct RMW_PUBLIC_TYPE rmw_guard_conditions_s
{
  /// The number of guard conditions represented by the array.
  size_t guard_condition_count;
  /// Pointer to an array of void * pointers of guard conditions.
  void ** guard_conditions;
} rmw_guard_conditions_t;

/// Container for guard conditions to be waited on
typedef struct RMW_PUBLIC_TYPE rmw_wait_set_s
{
  /// The name of the rmw implementation
  const char * implementation_identifier;

  /// The guard condition to be waited on
  rmw_guard_conditions_t * guard_conditions;

  /// Type erased pointer to this wait set's data
  void * data;
} rmw_wait_set_t;

/// An rmw service request identifier
typedef struct RMW_PUBLIC_TYPE rmw_request_id_s
{
  /// The guid of the writer associated with this request
  uint8_t writer_guid[RMW_GID_STORAGE_SIZE];

  /// Sequence number of this service
  int64_t sequence_number;
} rmw_request_id_t;

/// Meta-data for a service-related take.
typedef struct RMW_PUBLIC_TYPE rmw_service_info_s
{
  rmw_time_point_value_t source_timestamp;
  rmw_time_point_value_t received_timestamp;
  rmw_request_id_t request_id;
} rmw_service_info_t;

typedef enum RMW_PUBLIC_TYPE rmw_qos_reliability_policy_e
{
  /// Implementation specific default
  RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT,

  /// Guarantee that samples are delivered, may retry multiple times.
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,

  /// Attempt to deliver samples, but some may be lost if the network is not robust
  RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,

  /// Reliability policy has not yet been set
  RMW_QOS_POLICY_RELIABILITY_UNKNOWN,

  /// Will match the majority of endpoints and use a reliable policy if possible
  /**
   * A policy will be chosen at the time of creating a subscription or publisher.
   * A reliable policy will by chosen if it matches with all discovered endpoints,
   * otherwise a best effort policy will be chosen.
   *
   * The QoS policy reported by functions like `rmw_subscription_get_actual_qos` or
   * `rmw_publisher_get_actual_qos` may be best available, reliable, or best effort.
   *
   * Services and clients are not supported and default to the reliability value in
   * `rmw_qos_profile_services_default`.
   *
   * The middleware is not expected to update the policy after creating a subscription or
   * publisher, even if the chosen policy is incompatible with newly discovered endpoints.
   * Therefore, this policy should be used with care since non-deterministic behavior
   * can occur due to races with discovery.
   */
  RMW_QOS_POLICY_RELIABILITY_BEST_AVAILABLE
} rmw_qos_reliability_policy_t;

/// QoS history enumerations describing how samples endure
typedef enum RMW_PUBLIC_TYPE rmw_qos_history_policy_e
{
  /// Implementation default for history policy
  RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,

  /// Only store up to a maximum number of samples, dropping oldest once max is exceeded
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,

  /// Store all samples, subject to resource limits
  RMW_QOS_POLICY_HISTORY_KEEP_ALL,

  /// History policy has not yet been set
  RMW_QOS_POLICY_HISTORY_UNKNOWN
} rmw_qos_history_policy_t;

/// QoS durability enumerations describing how samples persist
typedef enum RMW_PUBLIC_TYPE rmw_qos_durability_policy_e
{
  /// Impplementation specific default
  RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,

  /// The rmw publisher is responsible for persisting samples for “late-joining” subscribers
  RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL,

  /// Samples are not persistent
  RMW_QOS_POLICY_DURABILITY_VOLATILE,

  /// Durability policy has not yet been set
  RMW_QOS_POLICY_DURABILITY_UNKNOWN,

  /// Will match the majority of endpoints and use a transient local policy if possible
  /**
   * A policy will be chosen at the time of creating a subscription or publisher.
   * A transient local policy will by chosen if it matches with all discovered endpoints,
   * otherwise a volatile policy will be chosen.
   *
   * In the case that a volatile policy is chosen for a subscription, any messages sent before
   * the subscription was created by transient local publishers will not be received.
   *
   * The QoS policy reported by functions like `rmw_subscription_get_actual_qos` or
   * `rmw_publisher_get_actual_qos` may be best available, transient local, or volatile.
   *
   * Services and clients are not supported and default to the durability value in
   * `rmw_qos_profile_services_default`.
   *
   * The middleware is not expected to update the policy after creating a subscription or
   * publisher, even if the chosen policy is incompatible with newly discovered endpoints.
   * Therefore, this policy should be used with care since non-deterministic behavior
   * can occur due to races with discovery.
   */
  RMW_QOS_POLICY_DURABILITY_BEST_AVAILABLE
} rmw_qos_durability_policy_t;

#define RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_NODE_DEPRECATED_MSG \
  "RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_NODE is deprecated. " \
  "Use RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC if manually asserted liveliness is needed."

#ifndef _WIN32
# define RMW_DECLARE_DEPRECATED(name, msg) name __attribute__((deprecated(msg)))
#else
# define RMW_DECLARE_DEPRECATED(name, msg) name __pragma(deprecated(name))
#endif

/// QoS liveliness enumerations that describe a publisher's reporting policy for its alive status.
/// For a subscriber, these are its requirements for its topic's publishers.
// Suppress syntax errors, as cppcheck does not seem to handle enumerator attributes.
// cppcheck-suppress syntaxError
typedef enum RMW_PUBLIC_TYPE rmw_qos_liveliness_policy_e
{
  /// Implementation specific default
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT = 0,

  /// The signal that establishes a Topic is alive comes from the ROS rmw layer.
  RMW_QOS_POLICY_LIVELINESS_AUTOMATIC = 1,

  /// Explicitly asserting node liveliness is required in this case.
  /// This option is deprecated, use RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC if your application
  /// requires to assert liveliness manually.
  RMW_DECLARE_DEPRECATED(
    RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_NODE,
    RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_NODE_DEPRECATED_MSG) = 2,

  /// The signal that establishes a Topic is alive is at the Topic level. Only publishing a message
  /// on the Topic or an explicit signal from the application to assert liveliness on the Topic
  /// will mark the Topic as being alive.
  // Using `3` for backwards compatibility.
  RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC = 3,

  /// Liveliness policy has not yet been set
  RMW_QOS_POLICY_LIVELINESS_UNKNOWN = 4,

  /// Will match the majority of endpoints and use a manual by topic policy if possible
  /**
   * A policy will be chosen at the time of creating a subscription or publisher.
   * A manual by topic policy will by chosen if it matches with all discovered endpoints,
   * otherwise an automatic policy will be chosen.
   *
   * The QoS policy reported by functions like `rmw_subscription_get_actual_qos` or
   * `rmw_publisher_get_actual_qos` may be best available, automatic, or manual by topic.
   *
   * Services and clients are not supported and default to the liveliness value in
   * `rmw_qos_profile_services_default`.
   *
   * The middleware is not expected to update the policy after creating a subscription or
   * publisher, even if the chosen policy is incompatible with newly discovered endpoints.
   * Therefore, this policy should be used with care since non-deterministic behavior
   * can occur due to races with discovery.
   */
  RMW_QOS_POLICY_LIVELINESS_BEST_AVAILABLE = 5
} rmw_qos_liveliness_policy_t;

/// QoS Deadline default.
#define RMW_QOS_DEADLINE_DEFAULT RMW_DURATION_UNSPECIFIED
/// Will match the majority of endpoints while maintaining as strict a policy as possible
/**
 * Value is RMW_DURATION_INFINITE - 1.
 *
 * A policy will be chosen at the time of creating a subscription or publisher.
 * For a subscription, the deadline will be the maximum value of all discovered publisher
 * deadlines.
 * For a publisher, the deadline will be the minimum value of all discovered subscription
 * deadlines.
 *
 * The QoS policy reported by functions like `rmw_subscription_get_actual_qos` or
 * `rmw_publisher_get_actual_qos` may be best available or the actual deadline value.
 *
 * Services and clients are not supported and default to the deadline value in
 * `rmw_qos_profile_services_default`.
 *
 * The middleware is not expected to update the policy after creating a subscription or
 * publisher, even if the chosen policy is incompatible with newly discovered endpoints.
 * Therefore, this policy should be used with care since non-deterministic behavior
 * can occur due to races with discovery.
 */
#define RMW_QOS_DEADLINE_BEST_AVAILABLE {9223372036LL, 854775806LL}

/// QoS Lifespan default.
#define RMW_QOS_LIFESPAN_DEFAULT RMW_DURATION_UNSPECIFIED

/// QoS Liveliness lease duration default.
#define RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT RMW_DURATION_UNSPECIFIED
/// Will match the majority of endpoints while maintaining as strict a policy as possible
/**
 * Value is RMW_DURATION_INFINITE - 1.
 *
 * A policy will be chosen at the time of creating a subscription or publisher.
 * For a subscription, the lease duration will be the maximum value of all discovered publisher
 * lease durations.
 * For a publisher, the lease duration will be the minimum value of all discovered subscription
 * lease durations.
 *
 * The QoS policy reported by functions like `rmw_subscription_get_actual_qos` or
 * `rmw_publisher_get_actual_qos` may be best available or the actual lease duration value.
 *
 * Services and clients are not supported and default to the lease duration value in
 * `rmw_qos_profile_services_default`.
 *
 * The middleware is not expected to update the policy after creating a subscription or
 * publisher, even if the chosen policy is incompatible with newly discovered endpoints.
 * Therefore, this policy should be used with care since non-deterministic behavior
 * can occur due to races with discovery.
 */
#define RMW_QOS_LIVELINESS_LEASE_DURATION_BEST_AVAILABLE {9223372036LL, 854775806LL}

/// ROS MiddleWare quality of service profile.
typedef struct RMW_PUBLIC_TYPE rmw_qos_profile_s
{
  enum rmw_qos_history_policy_e history;
  /// Size of the message queue.
  size_t depth;
  /// Reliabiilty QoS policy setting
  enum rmw_qos_reliability_policy_e reliability;
  /// Durability QoS policy setting
  enum rmw_qos_durability_policy_e durability;
  /// The period at which messages are expected to be sent/received
  /**
    * RMW_DURATION_UNSPECIFIED will use the RMW implementation's default value,
    *   which may or may not be infinite.
    * RMW_DURATION_INFINITE explicitly states that messages never miss a deadline expectation.
    */
  struct rmw_time_s deadline;
  /// The age at which messages are considered expired and no longer valid
  /**
    * RMW_DURATION_UNSPEFICIED will use the RMW implementation's default value,
    *   which may or may not be infinite.
    * RMW_DURATION_INFINITE explicitly states that messages do not expire.
    */
  struct rmw_time_s lifespan;
  /// Liveliness QoS policy setting
  enum rmw_qos_liveliness_policy_e liveliness;
  /// The time within which the RMW node or publisher must show that it is alive
  /**
    * RMW_DURATION_UNSPEFICIED will use the RMW implementation's default value,
    *   which may or may not be infinite.
    * RMW_DURATION_INFINITE explicitly states that liveliness is not enforced.
    */
  struct rmw_time_s liveliness_lease_duration;

  /// If true, any ROS specific namespacing conventions will be circumvented.
  /**
   * In the case of DDS and topics, for example, this means the typical
   * ROS specific prefix of `rt` would not be applied as described here:
   *
   *   http://design.ros2.org/articles/topic_and_service_names.html#ros-specific-namespace-prefix
   *
   * This might be useful when trying to directly connect a native DDS topic
   * with a ROS 2 topic.
   */
  bool avoid_ros_namespace_conventions;
} rmw_qos_profile_t;

/// Globally unique identifier for a ROS graph entity
/**
 * This is expected to be globally unique within a ROS domain.
 * The identifier should be the same when reported both locally (where the entity was created)
 * and on remote hosts or processes.
 */
typedef struct RMW_PUBLIC_TYPE rmw_gid_s
{
  /// Name of the rmw implementation
  const char * implementation_identifier;

  /// Byte data GID value
  uint8_t data[RMW_GID_STORAGE_SIZE];
} rmw_gid_t;

#define RMW_MESSAGE_INFO_SEQUENCE_NUMBER_UNSUPPORTED UINT64_MAX

/// Information describing an rmw message
typedef struct RMW_PUBLIC_TYPE rmw_message_info_s
{
  /// Time when the message was published by the publisher.
  /**
   * The exact point at which the timestamp is taken is not specified, but
   * it should be taken consistently at the same point in the
   * publishing process each time.
   */
  rmw_time_point_value_t source_timestamp;
  /// Time when the message was received by the subscription.
  /**
   * The exact point at which the timestamp is taken is not specified, but
   * it should be taken consistently at the same point in the
   * process of receiving a message each time.
   */
  rmw_time_point_value_t received_timestamp;
  /// Sequence number of the received message set by the publisher.
  /**
   * This sequence number is set by the publisher and therefore uniquely identifies
   * a message when combined with the publisher GID.
   * For long running applications, the sequence number might wrap around at some point.
   *
   * If the rmw implementation doesn't support sequence numbers, its value will be
   * RMW_MESSAGE_INFO_SEQUENCE_NUMBER_UNSUPPORTED.
   *
   * Requirements:
   *
   * If `psn1` and `psn2` are the publication sequence numbers obtained by
   * calls to `rmw_take*()`, where `psn1` was obtained in a call that happened before `psn2` and both
   * sequence numbers are from the same publisher (i.e. also same publisher gid), then:
   *
   * - psn2 > psn1 (except in the case of a wrap around)
   * - `psn2 - psn1 - 1` is the number of messages the publisher sent in the middle of both
   *   received messages.
   *   Those might have already been taken by other `rmw_take*()` calls that happened in between or lost.
   *   `psn2 - psn1 - 1 = 0` if and only if the messages were sent by the publisher consecutively.
   */
  uint64_t publication_sequence_number;
  /// Sequence number of the received message set by the subscription.
  /**
   * This sequence number is set by the subscription regardless of which
   * publisher sent the message.
   * For long running applications, the sequence number might wrap around at some point.
   *
   * If the rmw implementation doesn't support sequence numbers, its value will be
   * RMW_MESSAGE_INFO_SEQUENCE_NUMBER_UNSUPPORTED.
   *
   * Requirements:
   *
   * If `rsn1` and `rsn2` are the reception sequence numbers obtained by
   * calls to `rmw_take*()`, where `rsn1` was obtained in a call that happened before `rsn2`, then:
   *
   * - rsn2 > rsn1 (except in the case of a wrap around)
   * - `rsn2 = rsn1 + 1` if and only if both `rmw_take*()` calls happened consecutively.
   */
  uint64_t reception_sequence_number;
  /// Global unique identifier of the publisher that sent the message.
  /**
   * The identifier uniquely identifies the publisher for the local context, but
   * it will not necessarily be the same identifier given in other contexts or processes
   * for the same publisher.
   * Therefore the identifier will uniquely identify the publisher within your application
   * but may disagree about the identifier for that publisher when compared to another
   * application.
   * Even with this limitation, when combined with the publisher sequence number it can
   * uniquely identify a message within your local context.
   * Publisher GIDs generated by the rmw implementation could collide at some point, in which
   * case it is not possible to distinguish which publisher sent the message.
   * The details of how GIDs are generated are rmw implementation dependent.
   *
   * It is possible the the rmw implementation needs to reuse a publisher GID,
   * due to running out of unique identifiers or some other constraint, in which case
   * the rmw implementation may document what happens in that case, but that
   * behavior is not defined here.
   * However, this should be avoided, if at all possible, by the rmw implementation,
   * and should be unlikely to happen in practice.
   *
   * \todo In the future we want this to uniquely identify the publisher globally across
   *   contexts, processes, and machines.
   */
  rmw_gid_t publisher_gid;

  /// Whether this message is from intra_process communication or not
  bool from_intra_process;
} rmw_message_info_t;

/// Get zero initialized mesage info.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_message_info_t
rmw_get_zero_initialized_message_info(void);

/// Default size of the rmw queue when history is set to RMW_QOS_POLICY_HISTORY_KEEP_LAST,
/// 0 indicates it is currently not set
enum {RMW_QOS_POLICY_DEPTH_SYSTEM_DEFAULT = 0};

/// Type mapping of rcutils log severity types to rmw specific types.
typedef enum RMW_PUBLIC_TYPE
{
  /// Debug log severity, for pedantic messaging
  RMW_LOG_SEVERITY_DEBUG = RCUTILS_LOG_SEVERITY_DEBUG,

  /// Informational log severity, for reporting expected but not overwhelming information
  RMW_LOG_SEVERITY_INFO = RCUTILS_LOG_SEVERITY_INFO,

  /// Warning log severity, for reporting recoverable issues
  RMW_LOG_SEVERITY_WARN = RCUTILS_LOG_SEVERITY_WARN,

  /// Error log severity, for reporting uncoverable issues
  RMW_LOG_SEVERITY_ERROR = RCUTILS_LOG_SEVERITY_ERROR,

  /// Fatal log severity, for reporting issue causing imminent shutdown
  RMW_LOG_SEVERITY_FATAL = RCUTILS_LOG_SEVERITY_FATAL
} rmw_log_severity_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__TYPES_H_
