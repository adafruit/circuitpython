// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from type_description_interfaces:msg/FieldType.idl
// generated code does not contain a copyright notice

#include "type_description_interfaces/msg/detail/field_type__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__msg__FieldType__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa7, 0x0b, 0x6d, 0xd9, 0x19, 0x64, 0x5a, 0x03,
      0xa3, 0x58, 0x6f, 0x7f, 0x82, 0x1d, 0xef, 0xbc,
      0x88, 0x6e, 0xa3, 0xe5, 0x31, 0xa1, 0xd9, 0x5c,
      0xc0, 0xf3, 0x80, 0xa3, 0x97, 0x3c, 0xca, 0xa6,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char type_description_interfaces__msg__FieldType__TYPE_NAME[] = "type_description_interfaces/msg/FieldType";

// Define type names, field names, and default values
static char type_description_interfaces__msg__FieldType__FIELD_NAME__type_id[] = "type_id";
static char type_description_interfaces__msg__FieldType__DEFAULT_VALUE__type_id[] = "0";
static char type_description_interfaces__msg__FieldType__FIELD_NAME__capacity[] = "capacity";
static char type_description_interfaces__msg__FieldType__FIELD_NAME__string_capacity[] = "string_capacity";
static char type_description_interfaces__msg__FieldType__FIELD_NAME__nested_type_name[] = "nested_type_name";

static rosidl_runtime_c__type_description__Field type_description_interfaces__msg__FieldType__FIELDS[] = {
  {
    {type_description_interfaces__msg__FieldType__FIELD_NAME__type_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {type_description_interfaces__msg__FieldType__DEFAULT_VALUE__type_id, 1, 1},
  },
  {
    {type_description_interfaces__msg__FieldType__FIELD_NAME__capacity, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__FieldType__FIELD_NAME__string_capacity, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__FieldType__FIELD_NAME__nested_type_name, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      255,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__msg__FieldType__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
      {type_description_interfaces__msg__FieldType__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Represents the type of a field and related meta-data.\n"
  "\n"
  "# A constant for each type supported according to:\n"
  "#   http://design.ros2.org/articles/legacy_interface_definition.html\n"
  "# and:\n"
  "#   http://design.ros2.org/articles/idl_interface_definition.html\n"
  "# Order is loosely coupled to the order of appearance in the IDL 4.2 spec:\n"
  "#  https://www.omg.org/spec/IDL/4.2\n"
  "\n"
  "# Layout of constants across the 0-255 decimal values in the uint8:\n"
  "#\n"
  "# - 000    : Reserved for \"not set\"\n"
  "# - 001-048: Primitive types, strings, and reserved space for future primitive types\n"
  "# - 049-096: Fixed sized array of primitive and string types\n"
  "# - 097-144: Bounded Sequences of primitive and string types\n"
  "# - 145-192: Unbounded Sequences of primitive and string types\n"
  "# - 193-255: Reserved space for future array/sequence-like types\n"
  "\n"
  "uint8 FIELD_TYPE_NOT_SET = 0\n"
  "\n"
  "# Nested type defined in other .msg/.idl files.\n"
  "uint8 FIELD_TYPE_NESTED_TYPE = 1\n"
  "\n"
  "# Integer Types\n"
  "uint8 FIELD_TYPE_INT8 = 2\n"
  "uint8 FIELD_TYPE_UINT8 = 3\n"
  "uint8 FIELD_TYPE_INT16 = 4\n"
  "uint8 FIELD_TYPE_UINT16 = 5\n"
  "uint8 FIELD_TYPE_INT32 = 6\n"
  "uint8 FIELD_TYPE_UINT32 = 7\n"
  "uint8 FIELD_TYPE_INT64 = 8\n"
  "uint8 FIELD_TYPE_UINT64 = 9\n"
  "\n"
  "# Floating-Point Types\n"
  "uint8 FIELD_TYPE_FLOAT = 10\n"
  "uint8 FIELD_TYPE_DOUBLE = 11\n"
  "uint8 FIELD_TYPE_LONG_DOUBLE = 12\n"
  "\n"
  "# Char and WChar Types\n"
  "uint8 FIELD_TYPE_CHAR = 13\n"
  "uint8 FIELD_TYPE_WCHAR = 14\n"
  "\n"
  "# Boolean Type\n"
  "uint8 FIELD_TYPE_BOOLEAN = 15\n"
  "\n"
  "# Byte/Octet Type\n"
  "uint8 FIELD_TYPE_BYTE = 16\n"
  "\n"
  "# String Types\n"
  "uint8 FIELD_TYPE_STRING = 17\n"
  "uint8 FIELD_TYPE_WSTRING = 18\n"
  "\n"
  "# Fixed String Types\n"
  "uint8 FIELD_TYPE_FIXED_STRING = 19\n"
  "uint8 FIELD_TYPE_FIXED_WSTRING = 20\n"
  "\n"
  "# Bounded String Types\n"
  "uint8 FIELD_TYPE_BOUNDED_STRING = 21\n"
  "uint8 FIELD_TYPE_BOUNDED_WSTRING = 22\n"
  "\n"
  "# Fixed Sized Array Types\n"
  "uint8 FIELD_TYPE_NESTED_TYPE_ARRAY = 49\n"
  "uint8 FIELD_TYPE_INT8_ARRAY = 50\n"
  "uint8 FIELD_TYPE_UINT8_ARRAY = 51\n"
  "uint8 FIELD_TYPE_INT16_ARRAY = 52\n"
  "uint8 FIELD_TYPE_UINT16_ARRAY = 53\n"
  "uint8 FIELD_TYPE_INT32_ARRAY = 54\n"
  "uint8 FIELD_TYPE_UINT32_ARRAY = 55\n"
  "uint8 FIELD_TYPE_INT64_ARRAY = 56\n"
  "uint8 FIELD_TYPE_UINT64_ARRAY = 57\n"
  "uint8 FIELD_TYPE_FLOAT_ARRAY = 58\n"
  "uint8 FIELD_TYPE_DOUBLE_ARRAY = 59\n"
  "uint8 FIELD_TYPE_LONG_DOUBLE_ARRAY = 60\n"
  "uint8 FIELD_TYPE_CHAR_ARRAY = 61\n"
  "uint8 FIELD_TYPE_WCHAR_ARRAY = 62\n"
  "uint8 FIELD_TYPE_BOOLEAN_ARRAY = 63\n"
  "uint8 FIELD_TYPE_BYTE_ARRAY = 64\n"
  "uint8 FIELD_TYPE_STRING_ARRAY = 65\n"
  "uint8 FIELD_TYPE_WSTRING_ARRAY = 66\n"
  "uint8 FIELD_TYPE_FIXED_STRING_ARRAY = 67\n"
  "uint8 FIELD_TYPE_FIXED_WSTRING_ARRAY = 68\n"
  "uint8 FIELD_TYPE_BOUNDED_STRING_ARRAY = 69\n"
  "uint8 FIELD_TYPE_BOUNDED_WSTRING_ARRAY = 70\n"
  "\n"
  "# Bounded Sequence Types\n"
  "uint8 FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE = 97\n"
  "uint8 FIELD_TYPE_INT8_BOUNDED_SEQUENCE = 98\n"
  "uint8 FIELD_TYPE_UINT8_BOUNDED_SEQUENCE = 99\n"
  "uint8 FIELD_TYPE_INT16_BOUNDED_SEQUENCE = 100\n"
  "uint8 FIELD_TYPE_UINT16_BOUNDED_SEQUENCE = 101\n"
  "uint8 FIELD_TYPE_INT32_BOUNDED_SEQUENCE = 102\n"
  "uint8 FIELD_TYPE_UINT32_BOUNDED_SEQUENCE = 103\n"
  "uint8 FIELD_TYPE_INT64_BOUNDED_SEQUENCE = 104\n"
  "uint8 FIELD_TYPE_UINT64_BOUNDED_SEQUENCE = 105\n"
  "uint8 FIELD_TYPE_FLOAT_BOUNDED_SEQUENCE = 106\n"
  "uint8 FIELD_TYPE_DOUBLE_BOUNDED_SEQUENCE = 107\n"
  "uint8 FIELD_TYPE_LONG_DOUBLE_BOUNDED_SEQUENCE = 108\n"
  "uint8 FIELD_TYPE_CHAR_BOUNDED_SEQUENCE = 109\n"
  "uint8 FIELD_TYPE_WCHAR_BOUNDED_SEQUENCE = 110\n"
  "uint8 FIELD_TYPE_BOOLEAN_BOUNDED_SEQUENCE = 111\n"
  "uint8 FIELD_TYPE_BYTE_BOUNDED_SEQUENCE = 112\n"
  "uint8 FIELD_TYPE_STRING_BOUNDED_SEQUENCE = 113\n"
  "uint8 FIELD_TYPE_WSTRING_BOUNDED_SEQUENCE = 114\n"
  "uint8 FIELD_TYPE_FIXED_STRING_BOUNDED_SEQUENCE = 115\n"
  "uint8 FIELD_TYPE_FIXED_WSTRING_BOUNDED_SEQUENCE = 116\n"
  "uint8 FIELD_TYPE_BOUNDED_STRING_BOUNDED_SEQUENCE = 117\n"
  "uint8 FIELD_TYPE_BOUNDED_WSTRING_BOUNDED_SEQUENCE = 118\n"
  "\n"
  "# Unbounded Sequence Types\n"
  "uint8 FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE = 145\n"
  "uint8 FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE = 146\n"
  "uint8 FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE = 147\n"
  "uint8 FIELD_TYPE_INT16_UNBOUNDED_SEQUENCE = 148\n"
  "uint8 FIELD_TYPE_UINT16_UNBOUNDED_SEQUENCE = 149\n"
  "uint8 FIELD_TYPE_INT32_UNBOUNDED_SEQUENCE = 150\n"
  "uint8 FIELD_TYPE_UINT32_UNBOUNDED_SEQUENCE = 151\n"
  "uint8 FIELD_TYPE_INT64_UNBOUNDED_SEQUENCE = 152\n"
  "uint8 FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE = 153\n"
  "uint8 FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE = 154\n"
  "uint8 FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE = 155\n"
  "uint8 FIELD_TYPE_LONG_DOUBLE_UNBOUNDED_SEQUENCE = 156\n"
  "uint8 FIELD_TYPE_CHAR_UNBOUNDED_SEQUENCE = 157\n"
  "uint8 FIELD_TYPE_WCHAR_UNBOUNDED_SEQUENCE = 158\n"
  "uint8 FIELD_TYPE_BOOLEAN_UNBOUNDED_SEQUENCE = 159\n"
  "uint8 FIELD_TYPE_BYTE_UNBOUNDED_SEQUENCE = 160\n"
  "uint8 FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE = 161\n"
  "uint8 FIELD_TYPE_WSTRING_UNBOUNDED_SEQUENCE = 162\n"
  "uint8 FIELD_TYPE_FIXED_STRING_UNBOUNDED_SEQUENCE = 163\n"
  "uint8 FIELD_TYPE_FIXED_WSTRING_UNBOUNDED_SEQUENCE = 164\n"
  "uint8 FIELD_TYPE_BOUNDED_STRING_UNBOUNDED_SEQUENCE = 165\n"
  "uint8 FIELD_TYPE_BOUNDED_WSTRING_UNBOUNDED_SEQUENCE = 166\n"
  "\n"
  "# Identifying number for the type of the field, using one of the above constants.\n"
  "uint8 type_id 0\n"
  "\n"
  "# Only used when the type is an array or a bounded sequence.\n"
  "# In the case of an array, this is the fixed capacity of the array.\n"
  "# In the case of a bounded sequence, this is the maximum capacity of the sequence.\n"
  "# In all other cases this field is unused.\n"
  "uint64 capacity\n"
  "\n"
  "# Only used when the type is a fixed or bounded string/wstring, or a array/sequence of those.\n"
  "# In the case of a fixed string/wstring, it is the fixed length of the string.\n"
  "# In the case of a bounded string/wstring, it is the maximum capacity of the string.\n"
  "# In the case of an array/sequence of fixed string/wstring, it is the fixed length of the strings.\n"
  "# In the case of an array/sequence of bounded string/wstring, it is the maximum capacity of the strings.\n"
  "# It is not currently possible to have different string capacities per element in the array/sequence.\n"
  "uint64 string_capacity\n"
  "\n"
  "# Only used when the type is a nested type or array/sequence of nested types.\n"
  "# This is limited to 255 characters.\n"
  "# TODO(wjwwood): this 255 character limit was chosen due to this being the limit\n"
  "#   for DDSI-RTPS based middlewares, which is the most commonly used right now.\n"
  "#   We lack a ROS 2 specific limit in our design documents, but we should update\n"
  "#   this and/or link to the design doc when that is available.\n"
  "string<=255 nested_type_name";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__msg__FieldType__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 6162, 6162},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__msg__FieldType__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__msg__FieldType__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
