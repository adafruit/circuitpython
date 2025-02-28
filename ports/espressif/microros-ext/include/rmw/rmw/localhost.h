// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__LOCALHOST_H_
#define RMW__LOCALHOST_H_

#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Used to specify if the context can only communicate through localhost.
typedef enum RMW_PUBLIC_TYPE rmw_localhost_only_e
{
  /// Uses ROS_LOCALHOST_ONLY environment variable.
  RMW_LOCALHOST_ONLY_DEFAULT = 0,
  /// Forces using only localhost.
  RMW_LOCALHOST_ONLY_ENABLED = 1,
  /// Forces disabling localhost only.
  RMW_LOCALHOST_ONLY_DISABLED = 2,
} rmw_localhost_only_t;

#ifdef __cplusplus
}
#endif

#endif  // RMW__LOCALHOST_H_
