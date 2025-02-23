// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Arrays.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/arrays__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Arrays__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x8a, 0x32, 0x1b, 0x80, 0xda, 0xe8, 0xc4, 0x4d,
      0xfd, 0x1c, 0xe5, 0x58, 0xea, 0xf1, 0xb1, 0xcd,
      0x5e, 0xbb, 0xe5, 0x95, 0x08, 0x63, 0xed, 0x67,
      0x58, 0x36, 0x53, 0x55, 0x11, 0xa7, 0xf9, 0x1a,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "test_msgs/msg/detail/constants__functions.h"
#include "test_msgs/msg/detail/defaults__functions.h"
#include "test_msgs/msg/detail/basic_types__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t test_msgs__msg__BasicTypes__EXPECTED_HASH = {1, {
    0x7c, 0x30, 0x0a, 0xfd, 0x4e, 0x79, 0x67, 0x98,
    0xd4, 0x9b, 0xdd, 0x6c, 0xda, 0xa0, 0xfa, 0x87,
    0xfa, 0x0e, 0xd2, 0xba, 0x32, 0x17, 0xd9, 0x77,
    0xe1, 0xfa, 0xa8, 0x70, 0x70, 0xd7, 0x97, 0xab,
  }};
static const rosidl_type_hash_t test_msgs__msg__Constants__EXPECTED_HASH = {1, {
    0x98, 0x3e, 0x7a, 0xe7, 0x09, 0x9d, 0x65, 0xb3,
    0x03, 0x0c, 0x31, 0xc2, 0x89, 0x0b, 0x0e, 0xc2,
    0xb3, 0x32, 0x48, 0x4f, 0xae, 0x9f, 0x28, 0xc6,
    0x8e, 0x01, 0x34, 0x0c, 0x8f, 0x61, 0xed, 0xcf,
  }};
static const rosidl_type_hash_t test_msgs__msg__Defaults__EXPECTED_HASH = {1, {
    0xa7, 0x25, 0xee, 0x02, 0xbf, 0x25, 0xa2, 0x24,
    0xcb, 0x7d, 0x14, 0x01, 0x5a, 0x02, 0x45, 0xb6,
    0x65, 0x4f, 0xf5, 0x91, 0xae, 0x1a, 0xc2, 0x77,
    0xe2, 0xa2, 0x67, 0x3f, 0x01, 0x3e, 0xa6, 0xd2,
  }};
#endif

static char test_msgs__msg__Arrays__TYPE_NAME[] = "test_msgs/msg/Arrays";
static char test_msgs__msg__BasicTypes__TYPE_NAME[] = "test_msgs/msg/BasicTypes";
static char test_msgs__msg__Constants__TYPE_NAME[] = "test_msgs/msg/Constants";
static char test_msgs__msg__Defaults__TYPE_NAME[] = "test_msgs/msg/Defaults";

// Define type names, field names, and default values
static char test_msgs__msg__Arrays__FIELD_NAME__bool_values[] = "bool_values";
static char test_msgs__msg__Arrays__FIELD_NAME__byte_values[] = "byte_values";
static char test_msgs__msg__Arrays__FIELD_NAME__char_values[] = "char_values";
static char test_msgs__msg__Arrays__FIELD_NAME__float32_values[] = "float32_values";
static char test_msgs__msg__Arrays__FIELD_NAME__float64_values[] = "float64_values";
static char test_msgs__msg__Arrays__FIELD_NAME__int8_values[] = "int8_values";
static char test_msgs__msg__Arrays__FIELD_NAME__uint8_values[] = "uint8_values";
static char test_msgs__msg__Arrays__FIELD_NAME__int16_values[] = "int16_values";
static char test_msgs__msg__Arrays__FIELD_NAME__uint16_values[] = "uint16_values";
static char test_msgs__msg__Arrays__FIELD_NAME__int32_values[] = "int32_values";
static char test_msgs__msg__Arrays__FIELD_NAME__uint32_values[] = "uint32_values";
static char test_msgs__msg__Arrays__FIELD_NAME__int64_values[] = "int64_values";
static char test_msgs__msg__Arrays__FIELD_NAME__uint64_values[] = "uint64_values";
static char test_msgs__msg__Arrays__FIELD_NAME__string_values[] = "string_values";
static char test_msgs__msg__Arrays__FIELD_NAME__basic_types_values[] = "basic_types_values";
static char test_msgs__msg__Arrays__FIELD_NAME__constants_values[] = "constants_values";
static char test_msgs__msg__Arrays__FIELD_NAME__defaults_values[] = "defaults_values";
static char test_msgs__msg__Arrays__FIELD_NAME__bool_values_default[] = "bool_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__bool_values_default[] = "(False, True, False)";
static char test_msgs__msg__Arrays__FIELD_NAME__byte_values_default[] = "byte_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__byte_values_default[] = "(0, 1, 255)";
static char test_msgs__msg__Arrays__FIELD_NAME__char_values_default[] = "char_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__char_values_default[] = "(0, 1, 127)";
static char test_msgs__msg__Arrays__FIELD_NAME__float32_values_default[] = "float32_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__float32_values_default[] = "(1.125, 0.0, -1.125)";
static char test_msgs__msg__Arrays__FIELD_NAME__float64_values_default[] = "float64_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__float64_values_default[] = "(3.1415, 0.0, -3.1415)";
static char test_msgs__msg__Arrays__FIELD_NAME__int8_values_default[] = "int8_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__int8_values_default[] = "(0, 127, -128)";
static char test_msgs__msg__Arrays__FIELD_NAME__uint8_values_default[] = "uint8_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__uint8_values_default[] = "(0, 1, 255)";
static char test_msgs__msg__Arrays__FIELD_NAME__int16_values_default[] = "int16_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__int16_values_default[] = "(0, 32767, -32768)";
static char test_msgs__msg__Arrays__FIELD_NAME__uint16_values_default[] = "uint16_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__uint16_values_default[] = "(0, 1, 65535)";
static char test_msgs__msg__Arrays__FIELD_NAME__int32_values_default[] = "int32_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__int32_values_default[] = "(0, 2147483647, -2147483648)";
static char test_msgs__msg__Arrays__FIELD_NAME__uint32_values_default[] = "uint32_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__uint32_values_default[] = "(0, 1, 4294967295)";
static char test_msgs__msg__Arrays__FIELD_NAME__int64_values_default[] = "int64_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__int64_values_default[] = "(0, 9223372036854775807, -9223372036854775808)";
static char test_msgs__msg__Arrays__FIELD_NAME__uint64_values_default[] = "uint64_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__uint64_values_default[] = "(0, 1, 18446744073709551615)";
static char test_msgs__msg__Arrays__FIELD_NAME__string_values_default[] = "string_values_default";
static char test_msgs__msg__Arrays__DEFAULT_VALUE__string_values_default[] = "('', 'max value', 'min value')";
static char test_msgs__msg__Arrays__FIELD_NAME__alignment_check[] = "alignment_check";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Arrays__FIELDS[] = {
  {
    {test_msgs__msg__Arrays__FIELD_NAME__bool_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__byte_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__char_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__float32_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__float64_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int8_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint8_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int16_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint16_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int32_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint32_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int64_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint64_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__string_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__basic_types_values, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__constants_values, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__defaults_values, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__bool_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__bool_values_default, 20, 20},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__byte_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__byte_values_default, 11, 11},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__char_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__char_values_default, 11, 11},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__float32_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__float32_values_default, 20, 20},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__float64_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__float64_values_default, 22, 22},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int8_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__int8_values_default, 14, 14},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint8_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__uint8_values_default, 11, 11},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int16_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__int16_values_default, 18, 18},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint16_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__uint16_values_default, 13, 13},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int32_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__int32_values_default, 28, 28},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint32_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__uint32_values_default, 18, 18},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__int64_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__int64_values_default, 46, 46},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__uint64_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__uint64_values_default, 28, 28},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__string_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Arrays__DEFAULT_VALUE__string_values_default, 30, 30},
  },
  {
    {test_msgs__msg__Arrays__FIELD_NAME__alignment_check, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__msg__Arrays__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Arrays__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
      {test_msgs__msg__Arrays__FIELDS, 32, 32},
    },
    {test_msgs__msg__Arrays__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Arrays of different types\n"
  "bool[3] bool_values\n"
  "byte[3] byte_values\n"
  "char[3] char_values\n"
  "float32[3] float32_values\n"
  "float64[3] float64_values\n"
  "int8[3] int8_values\n"
  "uint8[3] uint8_values\n"
  "int16[3] int16_values\n"
  "uint16[3] uint16_values\n"
  "int32[3] int32_values\n"
  "uint32[3] uint32_values\n"
  "int64[3] int64_values\n"
  "uint64[3] uint64_values\n"
  "string[3] string_values\n"
  "BasicTypes[3] basic_types_values\n"
  "Constants[3] constants_values\n"
  "Defaults[3] defaults_values\n"
  "bool[3] bool_values_default [false, true, false]\n"
  "byte[3] byte_values_default [0, 1, 255]\n"
  "char[3] char_values_default [0, 1, 127]\n"
  "float32[3] float32_values_default [1.125, 0.0, -1.125]\n"
  "float64[3] float64_values_default [3.1415, 0.0, -3.1415]\n"
  "int8[3] int8_values_default [0, 127, -128]\n"
  "uint8[3] uint8_values_default [0, 1, 255]\n"
  "int16[3] int16_values_default [0, 32767, -32768]\n"
  "uint16[3] uint16_values_default [0, 1, 65535]\n"
  "int32[3] int32_values_default [0, 2147483647, -2147483648]\n"
  "uint32[3] uint32_values_default [0, 1, 4294967295]\n"
  "int64[3] int64_values_default [0, 9223372036854775807, -9223372036854775808]\n"
  "uint64[3] uint64_values_default [0, 1, 18446744073709551615]\n"
  "string[3] string_values_default [\"\", \"max value\", \"min value\"]\n"
  "# Regression test: check alignment of basic field after an array field is correct\n"
  "int32 alignment_check";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Arrays__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1271, 1271},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Arrays__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Arrays__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[2] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
