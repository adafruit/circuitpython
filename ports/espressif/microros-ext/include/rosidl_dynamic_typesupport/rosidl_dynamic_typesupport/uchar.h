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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
// Nothing to do here, C++11 and beyond have char16_t as a keyword:
// https://en.cppreference.com/w/cpp/keyword/char16_t
#else
// According to https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005finclude.html,
// short-circuit style use of __has_include is not supported, so split the conditional
#  if defined(__has_include)
#    if __has_include(<uchar.h>)
// If the compiler has __has_include, and uchar.h exists, include that as it will have char16_t
// as a typedef.
#      include <uchar.h>
#    else
// Otherwise assume that char16_t isn't defined anywhere, and define it ourselves as uint_least16_t.
#      define __ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H__NEEDS_CHAR16_T_DECL
#    endif
#  else
#    define __ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H__NEEDS_CHAR16_T_DECL
#  endif
#endif

#if defined(__ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H__NEEDS_CHAR16_T_DECL)
#  undef __ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H__NEEDS_CHAR16_T_DECL
#  include <stdint.h>
typedef uint_least16_t char16_t;
#endif


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__UCHAR_H_
