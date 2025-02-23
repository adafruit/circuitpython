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

#ifndef RCL__SERVICE_INTROSPECTION_H_
#define RCL__SERVICE_INTROSPECTION_H_

#define RCL_SERVICE_INTROSPECTION_TOPIC_POSTFIX "/_service_event"

/// The introspection state for a client or service.
typedef enum rcl_service_introspection_state_e
{
  /// Introspection disabled
  RCL_SERVICE_INTROSPECTION_OFF,
  /// Introspect metadata only
  RCL_SERVICE_INTROSPECTION_METADATA,
  /// Introspection metadata and contents
  RCL_SERVICE_INTROSPECTION_CONTENTS,
} rcl_service_introspection_state_t;

#endif  // RCL__SERVICE_INTROSPECTION_H_
