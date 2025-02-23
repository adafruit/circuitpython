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

#ifndef RCLC__VISIBILITY_CONTROL_H_
#define RCLC__VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define RCLC_EXPORT __attribute__ ((dllexport))
    #define RCLC_IMPORT __attribute__ ((dllimport))
  #else
    #define RCLC_EXPORT __declspec(dllexport)
    #define RCLC_IMPORT __declspec(dllimport)
  #endif
  #ifdef RCLC_BUILDING_LIBRARY
    #define RCLC_PUBLIC RCLC_EXPORT
  #else
    #define RCLC_PUBLIC RCLC_IMPORT
  #endif
  #define RCLC_PUBLIC_TYPE RCLC_PUBLIC
  #define RCLC_LOCAL
#else
  #define RCLC_EXPORT __attribute__ ((visibility("default")))
  #define RCLC_IMPORT
  #if __GNUC__ >= 4
    #define RCLC_PUBLIC __attribute__ ((visibility("default")))
    #define RCLC_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define RCLC_PUBLIC
    #define RCLC_LOCAL
  #endif
  #define RCLC_PUBLIC_TYPE
#endif

#ifdef __cplusplus
}
#endif

#endif  // RCLC__VISIBILITY_CONTROL_H_
