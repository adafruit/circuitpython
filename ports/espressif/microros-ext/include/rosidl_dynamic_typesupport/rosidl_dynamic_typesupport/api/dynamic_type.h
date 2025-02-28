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

/// Polymorphic serialization support interface
/// Downstream middlewares should populate this interface as appropriate

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_TYPE_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rcutils/allocator.h>
#include <rcutils/types/rcutils_ret.h>

#include "rosidl_dynamic_typesupport/api/serialization_support.h"
#include "rosidl_dynamic_typesupport/api/serialization_support_interface.h"
#include "rosidl_dynamic_typesupport/types.h"
#include "rosidl_dynamic_typesupport/visibility_control.h"

// Dynamic Type Builder Impl
struct rosidl_dynamic_typesupport_dynamic_type_builder_impl_s
{
  rcutils_allocator_t allocator;
  void * handle;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_type_builder_impl_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type_builder_impl(void);

// Dynamic Type Builder
struct rosidl_dynamic_typesupport_dynamic_type_builder_s
{
  rcutils_allocator_t allocator;
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t impl;
  // !!! Lifetime is NOT managed by this struct
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_type_builder_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type_builder(void);

// Dynamic Type Impl
struct rosidl_dynamic_typesupport_dynamic_type_impl_s
{
  rcutils_allocator_t allocator;
  void * handle;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_type_impl_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type_impl(void);

// Dynamic Type
struct rosidl_dynamic_typesupport_dynamic_type_s
{
  rcutils_allocator_t allocator;
  rosidl_dynamic_typesupport_dynamic_type_impl_t impl;
  // !!! Lifetime is NOT managed by this struct
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_type_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type(void);

// =================================================================================================
// DYNAMIC TYPE
// =================================================================================================

// DYNAMIC TYPE UTILS ==============================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_equals(
  const rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type,
  const rosidl_dynamic_typesupport_dynamic_type_t * other,
  bool * equals);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_get_member_count(
  const rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type,
  size_t * member_count);  // OUT


// DYNAMIC TYPE CONSTRUCTION =======================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_init(
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support, const char * name,
  size_t name_length,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_clone(
  const rosidl_dynamic_typesupport_dynamic_type_builder_t * other,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_init_from_description(
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support,
  const rosidl_runtime_c__type_description__TypeDescription * description,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_fini(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_destroy(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_init_from_dynamic_type_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_init_from_description(
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support,
  const rosidl_runtime_c__type_description__TypeDescription * description,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_clone(
  const rosidl_dynamic_typesupport_dynamic_type_t * other,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_fini(
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_destroy(
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_get_name(
  const rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type,
  const char ** name,  // OUT
  size_t * name_length);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_get_name(
  const rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  const char ** name,  // OUT
  size_t * name_length);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_set_name(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder, const char * name,
  size_t name_length);


// DYNAMIC TYPE PRIMITIVE MEMBERS ==================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bool_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_byte_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_char_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wchar_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float32_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float64_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float128_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int8_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint8_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int16_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint16_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int32_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint32_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int64_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint64_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_string_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wstring_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_string_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_wstring_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_string_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_wstring_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length);


// DYNAMIC TYPE STATIC ARRAY MEMBERS ===============================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bool_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_byte_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_char_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wchar_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float32_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float64_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float128_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int8_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint8_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int16_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint16_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int32_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint32_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int64_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint64_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_string_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wstring_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_string_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound, size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_wstring_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound, size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_string_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length, size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_wstring_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length, size_t array_length);


// DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =========================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bool_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_byte_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_char_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wchar_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float128_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length);


// DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS ===========================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bool_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_byte_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_char_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wchar_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float32_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float64_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_float128_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int8_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint8_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int16_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint16_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int32_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint32_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_int64_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_uint64_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound, size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound, size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length, size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_fixed_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length, size_t sequence_bound);


// DYNAMIC TYPE NESTED MEMBERS =====================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_t * nested_struct);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_array_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_t * nested_struct, size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_unbounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_t * nested_struct);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_bounded_sequence_member(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_t * nested_struct, size_t sequence_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_member_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * nested_struct_builder);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_array_member_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * nested_struct_builder, size_t array_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_unbounded_sequence_member_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * nested_struct_builder);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_type_builder_add_complex_bounded_sequence_member_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_t * nested_struct_builder, size_t sequence_bound);


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_TYPE_H_
