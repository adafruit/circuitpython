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

#ifndef RMW__QOS_PROFILES_H_
#define RMW__QOS_PROFILES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rmw/types.h"

static const rmw_qos_profile_t rmw_qos_profile_sensor_data =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  5,
  RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,
  RMW_QOS_POLICY_DURABILITY_VOLATILE,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_parameters =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  1000,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_VOLATILE,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_default =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  10,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_VOLATILE,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_services_default =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  10,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_VOLATILE,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_parameter_events =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  1000,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_VOLATILE,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_system_default =
{
  RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT,
  RMW_QOS_POLICY_DEPTH_SYSTEM_DEFAULT,
  RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT,
  RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

/// Match majority of endpoints currently available while maintaining the highest level of service
/**
 * Reliability, durability, deadline, liveliness, and liveliness lease duration policies will be
 * chosen at the time of creating a subscription or publisher.
 *
 * The actual QoS policy can be retrieved after the endpoint is created with
 * `rmw_get_subscriptions_info_by_topic` or `rmw_get_publishers_info_by_topic`.
 *
 * The middleware is not expected to update policies after creating a subscription or
 * publisher, even if one or more policies are incompatible with newly discovered endpoints.
 * Therefore, this profile should be used with care since non-deterministic behavior
 * can occur due to races with discovery.
 */
static const rmw_qos_profile_t rmw_qos_profile_best_available =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  10,
  RMW_QOS_POLICY_RELIABILITY_BEST_AVAILABLE,
  RMW_QOS_POLICY_DURABILITY_BEST_AVAILABLE,
  RMW_QOS_DEADLINE_BEST_AVAILABLE,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_BEST_AVAILABLE,
  RMW_QOS_LIVELINESS_LEASE_DURATION_BEST_AVAILABLE,
  false
};

static const rmw_qos_profile_t rmw_qos_profile_unknown =
{
  RMW_QOS_POLICY_HISTORY_UNKNOWN,
  RMW_QOS_POLICY_DEPTH_SYSTEM_DEFAULT,
  RMW_QOS_POLICY_RELIABILITY_UNKNOWN,
  RMW_QOS_POLICY_DURABILITY_UNKNOWN,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_UNKNOWN,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

typedef enum RMW_PUBLIC_TYPE rmw_qos_compatibility_type_e
{
  /// QoS policies are compatible
  RMW_QOS_COMPATIBILITY_OK = 0,

  /// QoS policies may not be compatible
  RMW_QOS_COMPATIBILITY_WARNING,

  /// QoS policies are not compatible
  RMW_QOS_COMPATIBILITY_ERROR
} rmw_qos_compatibility_type_t;


/// Check if two QoS profiles are compatible.
/**
 * Two QoS profiles are compatible if a publisher and subcription
 * using the QoS policies can communicate with each other.
 *
 * If any of the profile policies has the value "system default" or "unknown", then it may not be
 * possible to determine the compatibilty.
 * In this case, the output parameter `compatibility` is set to `RMW_QOS_COMPATIBILITY_WARNING`
 * and `reason` is populated.
 *
 * If there is a compatibility warning or error, and a buffer is provided for `reason`, then an
 * explanation of all warnings and errors will be populated into the buffer, separated by
 * semi-colons (`;`).
 * Errors will appear before warnings in the string buffer.
 * If the provided buffer is not large enough, this function will still write to the buffer, up to
 * the `reason_size` number of characters.
 * Therefore, it is possible that not all errors and warnings are communicated if the buffer size limit
 * is reached.
 * A buffer size of 2048 should be more than enough to capture all possible errors and warnings.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] publisher_profile: The QoS profile used for a publisher.
 * \param[in] subscription_profile: The QoS profile used for a subscription.
 * \param[out] compatibility: `RMW_QOS_COMPATIBILITY_OK` if the QoS profiles are compatible, or
 *   `RMW_QOS_COMPATIBILITY_WARNING` if the QoS profiles might be compatible, or
 *   `RMW_QOS_COMPATIBILITY_ERROR` if the QoS profiles are not compatible.
 * \param[out] reason: A detailed reason for a QoS incompatibility or potential incompatibility.
 *   Must be pre-allocated by the caller.
 *   This parameter is optional and may be set to `NULL` if the reason information is not
 *   desired.
 * \param[in] reason_size: Size of the string buffer `reason`, if one is provided.
 *   If `reason` is `nullptr`, then this parameter must be zero.
 * \return `RMW_RET_OK` if the check was successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if `compatibility` is `nullptr`, or
 * \return `RMW_RET_INVALID_ARGUMENT` if `reason` is `NULL` and  `reason_size` is not zero, or
 * \return `RMW_RET_ERROR` if there is an unexpected error.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_qos_profile_check_compatible(
  const rmw_qos_profile_t publisher_profile,
  const rmw_qos_profile_t subscription_profile,
  rmw_qos_compatibility_type_t * compatibility,
  char * reason,
  size_t reason_size);

#ifdef __cplusplus
}
#endif

#endif  // RMW__QOS_PROFILES_H_
