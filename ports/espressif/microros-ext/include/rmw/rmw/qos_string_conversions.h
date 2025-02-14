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

#ifndef RMW__QOS_STRING_CONVERSIONS_H_
#define RMW__QOS_STRING_CONVERSIONS_H_

#include "rmw/types.h"
#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Return a string representing the policy kind.
/**
 * Returns `NULL` when `kind` is `RMW_QOS_POLICY_INVALID` or an undefined kind.
 *
 * The stringified version of the policy kind can be obtained doing the follwing conversion:
 * RMW_QOS_POLICY_<POLICY_KIND> -> lower_case(<POLICY_KIND>)
 *
 * For example, the stringified version of `RMW_QOS_POLICY_DURABILITY` is
 * "durability" and `RMW_QOS_POLICY_DEADLINE` is "deadline".
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] kind qos policy kind to be stringified.
 * \return a null terminated string representing the policy kind, or
 * \return `NULL` if kind is `RMW_QOS_POLICY_INVALID` or an undefined kind.
 */
RMW_PUBLIC
const char *
rmw_qos_policy_kind_to_str(rmw_qos_policy_kind_t kind);

/// Return a string representing the policy value.
/**
 * Returns `NULL` when `value` is `RMW_QOS_POLICY_*_UNKNOWN` or an undefined enum value.
 *
 * The stringified version of the policy value can be obtained doing the follwing conversion:
 * RMW_QOS_POLICY_<POLICY_KIND>_<POLICY_VALUE> -> lower_case(<POLICY_VALUE>)
 *
 * For example, the stringified version of `RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC` is
 * "manual_by_topic" and `RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT` is "best_effort".
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] value qos policy value to be stringified.
 * \return a null terminated string representing the policy value, or
 * \return `NULL` if value is `RMW_QOS_POLICY_*_UNKNOWN` or an undefined enum value.
 */
RMW_PUBLIC
const char *
rmw_qos_durability_policy_to_str(rmw_qos_durability_policy_t value);

/// Return a string representing the policy value.
/**
 * See \ref rmw_qos_durability_policy_to_str() for more details.
 */
RMW_PUBLIC
const char *
rmw_qos_history_policy_to_str(rmw_qos_history_policy_t value);

/// Return a string representing the policy value.
/**
 * See \ref rmw_qos_durability_policy_to_str() for more details.
 */
RMW_PUBLIC
const char *
rmw_qos_liveliness_policy_to_str(rmw_qos_liveliness_policy_t value);

/// Return a string representing the policy value.
/**
 * See \ref rmw_qos_durability_policy_to_str() for more details.
 */
RMW_PUBLIC
const char *
rmw_qos_reliability_policy_to_str(rmw_qos_reliability_policy_t value);

/// Return a policy kind based on the provided string.
/**
 * Returns the policy kind represented by the provided string, or
 * `RMW_QOS_POLICY_INVALID` when the provided string doesn't represent any policy kind.
 *
 * How policy kinds are stringified is explained in \ref rmw_qos_policy_kind_to_str.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str string identifying a qos policy kind.
 * \return the policy kind represented by the string, or
 * \return `RMW_QOS_POLICY_INVALID` if the string doesn't represent any policy kind.
 */
RMW_PUBLIC
rmw_qos_policy_kind_t
rmw_qos_policy_kind_from_str(const char * str);

/// Return a enum value based on the provided string.
/**
 * Returns the enum value based on the provided string, or
 * `RMW_QOS_POLICY_*_UNKNOWN` when the provided string is unexpected.
 *
 * How policy values are stringified is explained in \ref rmw_qos_durability_policy_to_str.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | Yes
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] str string identifying a qos policy value.
 * \return the policy value represented by the string, or
 * \return `RMW_QOS_POLICY_*_UNKNOWN` if the string doesn't represent any value.
 */
RMW_PUBLIC
rmw_qos_durability_policy_t
rmw_qos_durability_policy_from_str(const char * str);

/// Return a enum value based on the provided string.
/**
 * See \ref rmw_qos_durability_policy_from_str() for more details.
 */
RMW_PUBLIC
rmw_qos_history_policy_t
rmw_qos_history_policy_from_str(const char * str);

/// Return a enum value based on the provided string.
/**
 * See \ref rmw_qos_durability_policy_from_str() for more details.
 */
RMW_PUBLIC
rmw_qos_liveliness_policy_t
rmw_qos_liveliness_policy_from_str(const char * str);


/// Return a enum value based on the provided string.
/**
 * See \ref rmw_qos_durability_policy_from_str() for more details.
 */
RMW_PUBLIC
rmw_qos_reliability_policy_t
rmw_qos_reliability_policy_from_str(const char * str);

#ifdef __cplusplus
}
#endif

#endif  // RMW__QOS_STRING_CONVERSIONS_H_
