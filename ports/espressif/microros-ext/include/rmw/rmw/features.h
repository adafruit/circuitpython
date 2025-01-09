// Copyright 2022 Open Source Robotics Foundation, Inc.
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


#ifndef RMW__FEATURES_H_
#define RMW__FEATURES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "rcutils/allocator.h"

#include "rmw/macros.h"
#include "rmw/types.h"
#include "rmw/ret_types.h"
#include "rmw/visibility_control.h"

/// List of optional rmw features.
/**
 * Some of the features listed here might become mandatory in the feature, in which case all rmw
 * implementations should return `true`.
 *
 * There might be some optional features that are not listed here, but the goal is to have all of
 * them added.
 */
typedef enum RMW_PUBLIC_TYPE rmw_feature_e
{
  /// `rmw_message_info_t.publication_sequence_number` is filled correctly
  /// by the rmw implementation.
  RMW_FEATURE_MESSAGE_INFO_PUBLICATION_SEQUENCE_NUMBER = 0,
  /// `rmw_message_info_t.reception_sequence_number` is filled correctly
  /// by the rmw implementation.
  RMW_FEATURE_MESSAGE_INFO_RECEPTION_SEQUENCE_NUMBER = 1,
  /// deferred description dynamic type rosidl_message_type_support_t structs are allowed, and the
  /// middleware MUST populate them on type discovery
  RMW_MIDDLEWARE_SUPPORTS_TYPE_DISCOVERY = 2,
  /// dynamic type subscriptions will use take_dynamic_message_with_info()
  RMW_MIDDLEWARE_CAN_TAKE_DYNAMIC_MESSAGE = 3,
} rmw_feature_t;

/// Query if a feature is supported by the rmw implementation.
/**
 * \return `true` if the rmw implementation supports the feature, `false` if not.
 */
RMW_PUBLIC
bool
rmw_feature_supported(rmw_feature_t feature);

#ifdef __cplusplus
}
#endif

#endif  // RMW__FEATURES_H_
