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

#ifndef RMW__MACROS_H_
#define RMW__MACROS_H_

#include "rcutils/macros.h"

/// Expand the argument to its literal text
#define RMW_STRINGIFY(x) RCUTILS_STRINGIFY(x)

/// Indicate that the caller of a method must check the return value,
/// otherwise the compiler will issue a warning.
#define RMW_WARN_UNUSED RCUTILS_WARN_UNUSED

#endif  // RMW__MACROS_H_
