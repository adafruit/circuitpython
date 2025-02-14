// Copyright 2020 Open Source Robotics Foundation, Inc.
// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef RMW__QOS_POLICY_KIND_H_
#define RMW__QOS_POLICY_KIND_H_

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// QoS Policy Kinds
typedef enum RMW_PUBLIC_TYPE rmw_qos_policy_kind_e
{
  RMW_QOS_POLICY_INVALID = 1 << 0,
  RMW_QOS_POLICY_DURABILITY = 1 << 1,
  RMW_QOS_POLICY_DEADLINE = 1 << 2,
  RMW_QOS_POLICY_LIVELINESS = 1 << 3,
  RMW_QOS_POLICY_RELIABILITY = 1 << 4,
  RMW_QOS_POLICY_HISTORY = 1 << 5,
  RMW_QOS_POLICY_LIFESPAN = 1 << 6,
  RMW_QOS_POLICY_DEPTH = 1 << 7,
  RMW_QOS_POLICY_LIVELINESS_LEASE_DURATION = 1 << 8,
  RMW_QOS_POLICY_AVOID_ROS_NAMESPACE_CONVENTIONS = 1 << 9,
} rmw_qos_policy_kind_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__QOS_POLICY_KIND_H_
