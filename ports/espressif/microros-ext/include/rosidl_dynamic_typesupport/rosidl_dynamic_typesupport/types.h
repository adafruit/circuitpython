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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT__TYPES_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT__TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <rosidl_runtime_c/type_description/field__struct.h>
#include <rosidl_runtime_c/type_description/field_type__struct.h>
#include <rosidl_runtime_c/type_description/individual_type_description__struct.h>
#include <rosidl_runtime_c/type_description/type_description__struct.h>
#include "rosidl_dynamic_typesupport/visibility_control.h"
#include "rosidl_dynamic_typesupport/uchar.h"

#include "rcutils/allocator.h"
#include "rcutils/types/uint8_array.h"

// =================================================================================================
// DYNAMIC TYPESUPPORT
// =================================================================================================
// ID for accessing specific members of dynamic type or dynamic data
typedef size_t rosidl_dynamic_typesupport_member_id_t;

// Forward Declarations ============================================================================
typedef struct \
  rosidl_dynamic_typesupport_serialization_support_s \
  rosidl_dynamic_typesupport_serialization_support_t;
typedef struct \
  rosidl_dynamic_typesupport_serialization_support_impl_s \
  rosidl_dynamic_typesupport_serialization_support_impl_t;
typedef struct \
  rosidl_dynamic_typesupport_serialization_support_interface_s \
  rosidl_dynamic_typesupport_serialization_support_interface_t;

typedef struct \
  rosidl_dynamic_typesupport_dynamic_type_builder_s \
  rosidl_dynamic_typesupport_dynamic_type_builder_t;
typedef struct \
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_s \
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t;

typedef struct \
  rosidl_dynamic_typesupport_dynamic_type_s \
  rosidl_dynamic_typesupport_dynamic_type_t;
typedef struct \
  rosidl_dynamic_typesupport_dynamic_type_impl_s \
  rosidl_dynamic_typesupport_dynamic_type_impl_t;

typedef struct \
  rosidl_dynamic_typesupport_dynamic_data_s \
  rosidl_dynamic_typesupport_dynamic_data_t;
typedef struct \
  rosidl_dynamic_typesupport_dynamic_data_impl_s \
  rosidl_dynamic_typesupport_dynamic_data_impl_t;


// =================================================================================================
// FIELD TYPE INDICES
// =================================================================================================
// This mapping must match the constants defined in type_description_interfaces/msgs/FieldType.msg

// Layout of constants across the 0-255 decimal values in the uint8:
//
// - 000    : Reserved for "not set"
// - 001-048: Primitive types, strings, and reserved space for future primitive types
// - 049-096: Fixed sized array of primitive and string types
// - 097-144: Bounded Sequences of primitive and string types
// - 145-192: Unbounded Sequences of primitive and string types
// - 193-255: Reserved space for future array/sequence-like types

// SPECIALS ========================================================================================
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NOT_SET \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NOT_SET  // 0

// Nested type defined in other .msg/.idl files.
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE  // 1

// PRIMITIVES ======================================================================================
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8  // 2
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8  // 3
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16  // 4
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16  // 5
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32  // 6
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32  // 7
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64  // 8
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64  // 9

// NOTE(methylDragon): Unsure how to implement these. Are each pair equivalent?
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT  // 10
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT  // 10
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_DOUBLE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE  // 11
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE  // 11
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_LONG_DOUBLE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE  // 12
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT128 \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE  // 12

#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_CHAR  // 13
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WCHAR \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WCHAR  // 14
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN  // 15
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE  // 16

#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING  // 17
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING  // 18
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_STRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_STRING  // 19
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_WSTRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_WSTRING  // 20
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING  // 21
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_WSTRING  // 22

// SEQUENCES =======================================================================================
#define ROSIDL_DYNAMIC_TYPESUPPORT_SEQUENCE_TYPE_DELIMITER 48
#define ROSIDL_DYNAMIC_TYPESUPPORT_ARRAY_OFFSET 48
#define ROSIDL_DYNAMIC_TYPESUPPORT_UNBOUNDED_SEQUENCE_OFFSET 96
#define ROSIDL_DYNAMIC_TYPESUPPORT_BOUNDED_SEQUENCE_OFFSET 144

#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY  // 49
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY  // 50
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY  // 51
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY  // 52
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY  // 53
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY  // 54
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY  // 55
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY  // 56
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY  // 57
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY  // 58
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY  // 58
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_DOUBLE_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY  // 59
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY  // 59
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_LONG_DOUBLE_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_ARRAY  // 60
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT128_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_ARRAY  // 60
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_CHAR_ARRAY  // 61
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WCHAR_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WCHAR_ARRAY  // 62
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY  // 63
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY  // 64
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY  // 65
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_ARRAY  // 66
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_STRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_STRING_ARRAY  // 67
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_WSTRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_WSTRING_ARRAY  // 68
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING_ARRAY  // 69
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_ARRAY \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_WSTRING_ARRAY  // 70

#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE  // 97
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_BOUNDED_SEQUENCE  // 98
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_BOUNDED_SEQUENCE  // 99
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_BOUNDED_SEQUENCE  // 100
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_BOUNDED_SEQUENCE  // 101
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_BOUNDED_SEQUENCE  // 102
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_BOUNDED_SEQUENCE  // 103
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_BOUNDED_SEQUENCE  // 104
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_BOUNDED_SEQUENCE  // 105
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_BOUNDED_SEQUENCE  // 106
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_BOUNDED_SEQUENCE  // 106
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_DOUBLE_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_BOUNDED_SEQUENCE  // 107
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_BOUNDED_SEQUENCE  // 107
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_LONG_DOUBLE_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_BOUNDED_SEQUENCE  // 108
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT128_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_BOUNDED_SEQUENCE  // 108
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_CHAR_BOUNDED_SEQUENCE  // 109
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WCHAR_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WCHAR_BOUNDED_SEQUENCE  // 110
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_BOUNDED_SEQUENCE  // 111
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_BOUNDED_SEQUENCE  // 112
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_BOUNDED_SEQUENCE  // 113
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_BOUNDED_SEQUENCE  // 114
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_STRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_STRING_BOUNDED_SEQUENCE  // 115
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_WSTRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_WSTRING_BOUNDED_SEQUENCE  // 116
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING_BOUNDED_SEQUENCE  // 117
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_BOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_WSTRING_BOUNDED_SEQUENCE  // 118

#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE  // 145
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE  // 146
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE  // 147
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_UNBOUNDED_SEQUENCE  // 148
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_UNBOUNDED_SEQUENCE  // 149
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_UNBOUNDED_SEQUENCE  // 150
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_UNBOUNDED_SEQUENCE  // 151
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_UNBOUNDED_SEQUENCE  // 152
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE  // 153
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE  // 154
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE  // 154
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE  // 155
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE  // 155
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_LONG_DOUBLE_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_UNBOUNDED_SEQUENCE  // 156
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT128_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_LONG_DOUBLE_UNBOUNDED_SEQUENCE  // 156
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_CHAR_UNBOUNDED_SEQUENCE  // 157
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WCHAR_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WCHAR_UNBOUNDED_SEQUENCE  // 158
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_UNBOUNDED_SEQUENCE  // 159
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_UNBOUNDED_SEQUENCE  // 160
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE  // 161
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_UNBOUNDED_SEQUENCE  // 162
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_STRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_STRING_UNBOUNDED_SEQUENCE  // 163
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FIXED_WSTRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FIXED_WSTRING_UNBOUNDED_SEQUENCE  // 164
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING_UNBOUNDED_SEQUENCE  // 165  // NOLINT
#define ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_UNBOUNDED_SEQUENCE \
  rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_WSTRING_UNBOUNDED_SEQUENCE  // 166  // NOLINT

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT__TYPES_H_
