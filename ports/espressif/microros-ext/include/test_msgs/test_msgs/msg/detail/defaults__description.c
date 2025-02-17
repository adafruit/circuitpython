// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Defaults.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/defaults__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Defaults__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa7, 0x25, 0xee, 0x02, 0xbf, 0x25, 0xa2, 0x24,
      0xcb, 0x7d, 0x14, 0x01, 0x5a, 0x02, 0x45, 0xb6,
      0x65, 0x4f, 0xf5, 0x91, 0xae, 0x1a, 0xc2, 0x77,
      0xe2, 0xa2, 0x67, 0x3f, 0x01, 0x3e, 0xa6, 0xd2,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__Defaults__TYPE_NAME[] = "test_msgs/msg/Defaults";

// Define type names, field names, and default values
static char test_msgs__msg__Defaults__FIELD_NAME__bool_value[] = "bool_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__bool_value[] = "True";
static char test_msgs__msg__Defaults__FIELD_NAME__byte_value[] = "byte_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__byte_value[] = "50";
static char test_msgs__msg__Defaults__FIELD_NAME__char_value[] = "char_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__char_value[] = "100";
static char test_msgs__msg__Defaults__FIELD_NAME__float32_value[] = "float32_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__float32_value[] = "1.125";
static char test_msgs__msg__Defaults__FIELD_NAME__float64_value[] = "float64_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__float64_value[] = "1.125";
static char test_msgs__msg__Defaults__FIELD_NAME__int8_value[] = "int8_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__int8_value[] = "-50";
static char test_msgs__msg__Defaults__FIELD_NAME__uint8_value[] = "uint8_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__uint8_value[] = "200";
static char test_msgs__msg__Defaults__FIELD_NAME__int16_value[] = "int16_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__int16_value[] = "-1000";
static char test_msgs__msg__Defaults__FIELD_NAME__uint16_value[] = "uint16_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__uint16_value[] = "2000";
static char test_msgs__msg__Defaults__FIELD_NAME__int32_value[] = "int32_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__int32_value[] = "-30000";
static char test_msgs__msg__Defaults__FIELD_NAME__uint32_value[] = "uint32_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__uint32_value[] = "60000";
static char test_msgs__msg__Defaults__FIELD_NAME__int64_value[] = "int64_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__int64_value[] = "-40000000";
static char test_msgs__msg__Defaults__FIELD_NAME__uint64_value[] = "uint64_value";
static char test_msgs__msg__Defaults__DEFAULT_VALUE__uint64_value[] = "50000000";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Defaults__FIELDS[] = {
  {
    {test_msgs__msg__Defaults__FIELD_NAME__bool_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__bool_value, 4, 4},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__byte_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__byte_value, 2, 2},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__char_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__char_value, 3, 3},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__float32_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__float32_value, 5, 5},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__float64_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__float64_value, 5, 5},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__int8_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__int8_value, 3, 3},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__uint8_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__uint8_value, 3, 3},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__int16_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__int16_value, 5, 5},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__uint16_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__uint16_value, 4, 4},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__int32_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__int32_value, 6, 6},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__uint32_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__uint32_value, 5, 5},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__int64_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__int64_value, 9, 9},
  },
  {
    {test_msgs__msg__Defaults__FIELD_NAME__uint64_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Defaults__DEFAULT_VALUE__uint64_value, 8, 8},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Defaults__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
      {test_msgs__msg__Defaults__FIELDS, 13, 13},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "bool bool_value true\n"
  "byte byte_value 50\n"
  "char char_value 100\n"
  "float32 float32_value 1.125\n"
  "float64 float64_value 1.125\n"
  "int8 int8_value -50\n"
  "uint8 uint8_value 200\n"
  "int16 int16_value -1000\n"
  "uint16 uint16_value 2000\n"
  "int32 int32_value -30000\n"
  "uint32 uint32_value 60000\n"
  "int64 int64_value -40000000\n"
  "uint64 uint64_value 50000000";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Defaults__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 315, 315},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Defaults__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
