// Copyright 2023 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__IDENTIFIER_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__IDENTIFIER_H_

#include "rosidl_dynamic_typesupport/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
extern const char * const rosidl_dynamic_typesupport_c__identifier;

/// Get the name of the rosidl_dynamic_typesupport_c identifier
/**
 * \return Name of rosidl_dynamic_typesupport_c identifier
 */
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
const char *
rosidl_get_dynamic_typesupport_identifier(void);

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__IDENTIFIER_H_
