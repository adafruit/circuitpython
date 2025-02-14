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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__API__SERIALIZATION_SUPPORT_INTERFACE_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__API__SERIALIZATION_SUPPORT_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <rcutils/allocator.h>
#include <rcutils/types/rcutils_ret.h>
#include <rcutils/types/uint8_array.h>
#include <rosidl_runtime_c/type_description/type_description__struct.h>

#include "rosidl_dynamic_typesupport/types.h"
#include "rosidl_dynamic_typesupport/uchar.h"

/// This interface must be adopted by all downstream serialization library implementations

// =================================================================================================
// Interface
// =================================================================================================
struct rosidl_dynamic_typesupport_serialization_support_interface_s
{
  /// Interfaces mimicking the XTypes spec (Section 7.5: Language Binding)
  /// https://www.omg.org/spec/DDS-XTypes/1.1/PDF
  ///
  /// Luckily for us, FastRTPS mimics the spec quite well

  /* TODOS??? (though these are just bonuses...)
   *   - DynamicType::get_type_descriptor / DynamicType::get_descriptor (and TypeDescriptor class)
   *
   * I'm not sure if these are necessary, given the fact we will have the type description message
   * to guide the traversal? Also it's ambiguous what type we should be returning...:
   *   - DynamicType::get_all_members (Returns map of member ID to member)
   *   - DynamicType::get_all_members_by_name (Returns map of member name to member)
   */

  // CORE
  rcutils_allocator_t allocator;
  const char * serialization_library_identifier;

  rcutils_ret_t (* serialization_support_impl_fini)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * impl);

  rcutils_ret_t (* serialization_support_interface_fini)(
    rosidl_dynamic_typesupport_serialization_support_interface_t * methods);

  // ===============================================================================================
  // DYNAMIC TYPE
  // ===============================================================================================

  // DYNAMIC TYPE UTILS
  rcutils_ret_t (* dynamic_type_equals)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_impl_t * type,
    const rosidl_dynamic_typesupport_dynamic_type_impl_t * other,
    bool * equals);  // OUT

  // "member" from XTypes spec
  rcutils_ret_t (* dynamic_type_get_member_count)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_impl_t * dynamic_type,
    size_t * member_count);  // OUT


  // DYNAMIC TYPE CONSTRUCTION
  rcutils_ret_t (* dynamic_type_builder_init)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const char * name, size_t name_length,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder);  // OUT

  rcutils_ret_t (* dynamic_type_builder_clone)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * other,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder);  // OUT

  rcutils_ret_t (* dynamic_type_builder_fini)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder);


  rcutils_ret_t (* dynamic_type_init_from_dynamic_type_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * dynamic_type);  // OUT

  rcutils_ret_t (* dynamic_type_clone)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_impl_t * other,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * dynamic_type);  // OUT

  rcutils_ret_t (* dynamic_type_fini)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * dynamic_type);


  rcutils_ret_t (* dynamic_type_get_name)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_impl_t * dynamic_type,
    const char ** name,  // OUT
    size_t * name_length);  // OUT

  rcutils_ret_t (* dynamic_type_builder_get_name)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    const char ** name,  // OUT
    size_t * name_length);  // OUT

  rcutils_ret_t (* dynamic_type_builder_set_name)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    const char * name, size_t name_length);


  // DYNAMIC TYPE PRIMITIVE MEMBERS
  rcutils_ret_t (* dynamic_type_builder_add_bool_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_byte_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_char_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_wchar_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float32_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float64_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float128_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int8_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint8_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int16_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint16_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int32_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint32_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int64_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint64_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_string_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_wstring_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_string_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_wstring_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_length);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_string_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_bound);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_wstring_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_bound);


  // DYNAMIC TYPE STATIC ARRAY MEMBERS
  rcutils_ret_t (* dynamic_type_builder_add_bool_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_byte_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_char_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_wchar_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_float32_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_float64_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_float128_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_int8_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint8_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_int16_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint16_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_int32_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint32_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_int64_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint64_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_string_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_wstring_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_string_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_length, size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_wstring_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_length, size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_string_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_bound, size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_wstring_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_bound, size_t array_length);


  // DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS
  rcutils_ret_t (* dynamic_type_builder_add_bool_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_byte_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_char_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_wchar_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float32_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float64_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_float128_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int8_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint8_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int16_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint16_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int32_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint32_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_int64_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_uint64_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_string_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_wstring_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_string_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_length);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_wstring_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_length);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_string_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_bound);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_bound);


  // DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS
  rcutils_ret_t (* dynamic_type_builder_add_bool_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_byte_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_char_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_wchar_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_float32_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_float64_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_float128_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_int8_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_uint8_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_int16_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_uint16_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_int32_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_uint32_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_int64_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_uint64_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_string_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_wstring_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_string_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_length, size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_fixed_wstring_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_length, size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_string_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t string_bound, size_t sequence_bound);

  rcutils_ret_t (* dynamic_type_builder_add_bounded_wstring_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    size_t wstring_bound, size_t sequence_bound);


  // DYNAMIC TYPE NESTED MEMBERS
  rcutils_ret_t (* dynamic_type_builder_add_complex_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct);

  rcutils_ret_t (* dynamic_type_builder_add_complex_array_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_complex_unbounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct);

  rcutils_ret_t (* dynamic_type_builder_add_complex_bounded_sequence_member)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t sequence_bound);


  rcutils_ret_t (* dynamic_type_builder_add_complex_member_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder);

  rcutils_ret_t (* dynamic_type_builder_add_complex_array_member_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
    size_t array_length);

  rcutils_ret_t (* dynamic_type_builder_add_complex_unbounded_sequence_member_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder);

  rcutils_ret_t (* dynamic_type_builder_add_complex_bounded_sequence_member_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * name, size_t name_length,
    const char * default_value, size_t default_value_length,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
    size_t sequence_bound);


  // ===============================================================================================
  // DYNAMIC DATA
  // ===============================================================================================

  // DYNAMIC DATA UTILS
  rcutils_ret_t (* dynamic_data_clear_all_values)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);

  rcutils_ret_t (* dynamic_data_clear_nonkey_values)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);

  rcutils_ret_t (* dynamic_data_clear_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id);


  rcutils_ret_t (* dynamic_data_equals)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * other,
    bool * equals);  // OUT

  // "item" from XTypes
  rcutils_ret_t (* dynamic_data_get_item_count)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    size_t * item_count);  // OUT

  rcutils_ret_t (* dynamic_data_get_member_id_by_name)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char * name, size_t name_length,
    rosidl_dynamic_typesupport_member_id_t * member_id);  // OUT

  rcutils_ret_t (* dynamic_data_get_member_id_at_index)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    size_t index,
    rosidl_dynamic_typesupport_member_id_t * member_id);  // OUT

  rcutils_ret_t (* dynamic_data_get_array_index)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    size_t index,
    rosidl_dynamic_typesupport_member_id_t * array_index);  // OUT


  rcutils_ret_t (* dynamic_data_loan_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * loaned_dynamic_data);  // OUT

  rcutils_ret_t (* dynamic_data_return_loaned_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * inner_data);

  rcutils_ret_t (* dynamic_data_get_name)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char ** name,  // OUT
    size_t * name_length);  // OUT


  // DYNAMIC DATA CONSTRUCTION
  rcutils_ret_t (* dynamic_data_init_from_dynamic_type_builder)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * dynamic_type_builder,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);  // OUT

  rcutils_ret_t (* dynamic_data_init_from_dynamic_type)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_type_impl_t * type,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);  // OUT

  rcutils_ret_t (* dynamic_data_clone)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * other,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);  // OUT

  rcutils_ret_t (* dynamic_data_fini)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);

  // DYNAMIC DATA SERIALIZATION
  // NOTE(methylDragon): I'm not sure if rcutils_uint8_array_t is the right type to pass...
  //                     On the other hand it plays well with rmw and stores the buffer, length, and
  //                     capacity...
  //
  //                     I also considered using a void * instead, but I like the idea of forcing
  //                     serialization support libraries to play with uint8_t *s instead of their
  //                     own native type.
  //
  //                     ... Though I'm betting that any of their types can be accurately
  //                     represented as such a byte array
  //
  // NOTE(methylDragon): rmw_serialized_message_t is a typedef of rcutils_uint8_array_t
  rcutils_ret_t (* dynamic_data_serialize)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rcutils_uint8_array_t * buffer);  // OUT

  rcutils_ret_t (* dynamic_data_deserialize)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,  // OUT
    rcutils_uint8_array_t * buffer);


  // DYNAMIC DATA PRIMITIVE MEMBER GETTERS
  rcutils_ret_t (* dynamic_data_get_bool_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    bool * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_byte_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    uint8_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_char_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_wchar_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char16_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_float32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    float * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_float64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    double * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_float128_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    long double * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_int8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    int8_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_uint8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    uint8_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_int16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    int16_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_uint16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    uint16_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_int32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    int32_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_uint32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    uint32_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_int64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    int64_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_uint64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    uint64_t * value);  // OUT

  rcutils_ret_t (* dynamic_data_get_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char ** value,  // OUT
    size_t * value_length);  // OUT

  rcutils_ret_t (* dynamic_data_get_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char16_t ** value,  // OUT
    size_t * value_length);  // OUT

  rcutils_ret_t (* dynamic_data_get_fixed_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char ** value,  // OUT
    size_t * value_length,  //  OUT
    size_t string_length);

  rcutils_ret_t (* dynamic_data_get_fixed_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char16_t ** value,  // OUT
    size_t * value_length,  //  OUT
    size_t wstring_length);

  rcutils_ret_t (* dynamic_data_get_bounded_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char ** value,  // OUT
    size_t * value_length,  //  OUT
    size_t string_bound);

  rcutils_ret_t (* dynamic_data_get_bounded_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    char16_t ** value,  // OUT
    size_t * value_length,  //  OUT
    size_t wstring_bound);


  // DYNAMIC DATA PRIMITIVE MEMBER SETTERS
  rcutils_ret_t (* dynamic_data_set_bool_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, bool value);

  rcutils_ret_t (* dynamic_data_set_byte_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, uint8_t value);

  rcutils_ret_t (* dynamic_data_set_char_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, char value);

  rcutils_ret_t (* dynamic_data_set_wchar_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, char16_t value);

  rcutils_ret_t (* dynamic_data_set_float32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, float value);

  rcutils_ret_t (* dynamic_data_set_float64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, double value);

  rcutils_ret_t (* dynamic_data_set_float128_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, long double value);

  rcutils_ret_t (* dynamic_data_set_int8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, int8_t value);

  rcutils_ret_t (* dynamic_data_set_uint8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, uint8_t value);

  rcutils_ret_t (* dynamic_data_set_int16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, int16_t value);

  rcutils_ret_t (* dynamic_data_set_uint16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, uint16_t value);

  rcutils_ret_t (* dynamic_data_set_int32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, int32_t value);

  rcutils_ret_t (* dynamic_data_set_uint32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, uint32_t value);

  rcutils_ret_t (* dynamic_data_set_int64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, int64_t value);

  rcutils_ret_t (* dynamic_data_set_uint64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id, uint64_t value);

  rcutils_ret_t (* dynamic_data_set_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * value, size_t value_length);

  rcutils_ret_t (* dynamic_data_set_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char16_t * value, size_t value_length);

  rcutils_ret_t (* dynamic_data_set_fixed_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * value, size_t value_length,
    size_t string_length);

  rcutils_ret_t (* dynamic_data_set_fixed_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char16_t * value, size_t value_length,
    size_t wstring_length);

  rcutils_ret_t (* dynamic_data_set_bounded_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char * value, size_t value_length,
    size_t string_bound);

  rcutils_ret_t (* dynamic_data_set_bounded_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    const char16_t * value, size_t value_length,
    size_t wstring_bound);


  // DYNAMIC DATA SEQUENCES
  rcutils_ret_t (* dynamic_data_clear_sequence_data)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data);

  rcutils_ret_t (* dynamic_data_remove_sequence_data)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id);

  rcutils_ret_t (* dynamic_data_insert_sequence_data)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT


  rcutils_ret_t (* dynamic_data_insert_bool_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    bool value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_byte_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    uint8_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_char_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    char value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_wchar_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    char16_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_float32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    float value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_float64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    double value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_float128_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    long double value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_int8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    int8_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_uint8_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    uint8_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_int16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    int16_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_uint16_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    uint16_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_int32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    int32_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_uint32_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    uint32_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_int64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    int64_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_uint64_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    uint64_t value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char * value, size_t value_length,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char16_t * value, size_t value_length,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_fixed_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char * value, size_t value_length, size_t string_length,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_fixed_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char16_t * value, size_t value_length, size_t wstring_length,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_bounded_string_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char * value, size_t value_length, size_t string_bound,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_bounded_wstring_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const char16_t * value, size_t value_length, size_t wstring_bound,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT


  // DYNAMIC DATA NESTED
  rcutils_ret_t (* dynamic_data_get_complex_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    rcutils_allocator_t * allocator,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * value);  // OUT (copies)

  rcutils_ret_t (* dynamic_data_set_complex_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_member_id_t id,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * value);


  rcutils_ret_t (* dynamic_data_insert_complex_value_copy)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT

  rcutils_ret_t (* dynamic_data_insert_complex_value)(
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * dynamic_data,
    rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
    rosidl_dynamic_typesupport_member_id_t * out_id);  // OUT
};

ROSIDL_DYNAMIC_TYPESUPPORT_PUBLIC
rosidl_dynamic_typesupport_serialization_support_interface_t
rosidl_dynamic_typesupport_get_zero_initialized_serialization_support_interface(void);

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__API__SERIALIZATION_SUPPORT_INTERFACE_H_
