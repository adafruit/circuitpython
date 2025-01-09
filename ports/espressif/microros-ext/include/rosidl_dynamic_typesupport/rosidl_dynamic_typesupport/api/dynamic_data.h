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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_DATA_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rcutils/allocator.h>
#include <rcutils/types/rcutils_ret.h>
#include <rcutils/types/uint8_array.h>

#include "rosidl_dynamic_typesupport/api/serialization_support.h"
#include "rosidl_dynamic_typesupport/api/serialization_support_interface.h"
#include "rosidl_dynamic_typesupport/visibility_control.h"
#include "rosidl_dynamic_typesupport/types.h"
#include "rosidl_dynamic_typesupport/uchar.h"

// Dynamic Data Impl
struct rosidl_dynamic_typesupport_dynamic_data_impl_s
{
  rcutils_allocator_t allocator;
  void * handle;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_data_impl_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_data_impl(void);

// Dynamic Data
struct rosidl_dynamic_typesupport_dynamic_data_s
{
  rcutils_allocator_t allocator;
  rosidl_dynamic_typesupport_dynamic_data_impl_t impl;
  // !!! Lifetime is NOT managed by this struct
  rosidl_dynamic_typesupport_serialization_support_t * serialization_support;
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_dynamic_data_t
rosidl_dynamic_typesupport_get_zero_initialized_dynamic_data(void);

// ===============================================================================================
// DYNAMIC DATA
// ===============================================================================================

// DYNAMIC DATA UTILS ==============================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_clear_all_values(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_clear_nonkey_values(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_clear_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_equals(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  const rosidl_dynamic_typesupport_dynamic_data_t * other,
  bool * equals);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_item_count(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  size_t * item_count);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_member_id_by_name(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  const char * name, size_t name_length,
  rosidl_dynamic_typesupport_member_id_t * member_id);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_member_id_at_index(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  size_t index,
  rosidl_dynamic_typesupport_member_id_t * member_id);  // OUT

// You must use this for arrays
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_array_index(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  size_t index,
  rosidl_dynamic_typesupport_member_id_t * array_index);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_loan_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_t * loaned_dynamic_data);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_return_loaned_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_dynamic_data_t * inner_dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_return_and_destroy_loaned_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_dynamic_data_t * inner_dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_name(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  const char ** name,  // OUT
  size_t * name_length);  // OUT


// DYNAMIC DATA CONSTRUCTION =======================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_init_from_dynamic_type_builder(
  rosidl_dynamic_typesupport_dynamic_type_builder_t * dynamic_type_builder,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_init_from_dynamic_type(
  rosidl_dynamic_typesupport_dynamic_type_t * dynamic_type,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_clone(
  const rosidl_dynamic_typesupport_dynamic_data_t * other_dynamic_data,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_fini(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_destroy(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);


// DYNAMIC DATA SERIALIZATION ======================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_serialize(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rcutils_uint8_array_t * buffer);  // OUT

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_deserialize(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,  // OUT
  rcutils_uint8_array_t * buffer);


// DYNAMIC DATA PRIMITIVE MEMBER GETTERS ===========================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_bool_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, bool * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_byte_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, unsigned char * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_char_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_wchar_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char16_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_float32_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, float * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_float64_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, double * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_float128_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, long double * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_int8_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int8_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_uint8_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint8_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_int16_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int16_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_uint16_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint16_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_int32_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int32_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_uint32_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint32_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_int64_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int64_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_uint64_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint64_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_string_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_wstring_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_fixed_string_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_fixed_wstring_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_bounded_string_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_bounded_wstring_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_bound);


// DYNAMIC DATA PRIMITIVE MEMBER SETTERS ===========================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_bool_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, bool value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_byte_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, unsigned char value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_char_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_wchar_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, char16_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_float32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, float value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_float64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, double value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_float128_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, long double value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_int8_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int8_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_uint8_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint8_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_int16_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int16_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_uint16_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint16_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_int32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int32_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_uint32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint32_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_int64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, int64_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_uint64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, uint64_t value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_fixed_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_fixed_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_length);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_bounded_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_bound);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_bounded_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_bound);


// DYNAMIC DATA SEQUENCES ==========================================================================
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_clear_sequence_data(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_remove_sequence_data(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_sequence_data(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_bool_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, bool value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_byte_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, uint8_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_char_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, char value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_wchar_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, char16_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_float32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, float value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_float64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, double value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_float128_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, long double value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_int8_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, int8_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_uint8_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, uint8_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_int16_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, int16_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_uint16_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, uint16_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_int32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, int32_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_uint32_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, uint32_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_int64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, int64_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_uint64_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, uint64_t value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char * value, size_t value_length,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char16_t * value,
  size_t value_length, rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_fixed_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char * value, size_t value_length,
  size_t string_length, rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_fixed_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char16_t * value,
  size_t value_length, size_t wstring_length, rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_bounded_string_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char * value, size_t value_length,
  size_t string_bound, rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_bounded_wstring_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data, const char16_t * value,
  size_t value_length, size_t wstring_bound, rosidl_dynamic_typesupport_member_id_t * out_id);


// DYNAMIC DATA NESTED =============================================================================
// The user is expected to allocate the '** value' outparam outside
// This function will then reassign the '** value''s 'serialization_support' member to match the
// input's
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_get_complex_value(
  const rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_t * value);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_set_complex_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_member_id_t id, rosidl_dynamic_typesupport_dynamic_data_t * value);

// This deep copies the passed data
ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_complex_value_copy(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  const rosidl_dynamic_typesupport_dynamic_data_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id);

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rcutils_ret_t
rosidl_dynamic_typesupport_dynamic_data_insert_complex_value(
  rosidl_dynamic_typesupport_dynamic_data_t * dynamic_data,
  rosidl_dynamic_typesupport_dynamic_data_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id);


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__API__DYNAMIC_DATA_H_
