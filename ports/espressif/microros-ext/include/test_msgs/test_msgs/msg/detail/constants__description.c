// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Constants.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/constants__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Constants__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x98, 0x3e, 0x7a, 0xe7, 0x09, 0x9d, 0x65, 0xb3,
      0x03, 0x0c, 0x31, 0xc2, 0x89, 0x0b, 0x0e, 0xc2,
      0xb3, 0x32, 0x48, 0x4f, 0xae, 0x9f, 0x28, 0xc6,
      0x8e, 0x01, 0x34, 0x0c, 0x8f, 0x61, 0xed, 0xcf,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__Constants__TYPE_NAME[] = "test_msgs/msg/Constants";

// Define type names, field names, and default values
static char test_msgs__msg__Constants__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Constants__FIELDS[] = {
  {
    {test_msgs__msg__Constants__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Constants__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
      {test_msgs__msg__Constants__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "bool BOOL_CONST=true\n"
  "byte BYTE_CONST=50\n"
  "char CHAR_CONST=100\n"
  "float32 FLOAT32_CONST=1.125\n"
  "float64 FLOAT64_CONST=1.125\n"
  "int8 INT8_CONST=-50\n"
  "uint8 UINT8_CONST=200\n"
  "int16 INT16_CONST=-1000\n"
  "uint16 UINT16_CONST=2000\n"
  "int32 INT32_CONST=-30000\n"
  "uint32 UINT32_CONST=60000\n"
  "int64 INT64_CONST=-40000000\n"
  "uint64 UINT64_CONST=50000000";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Constants__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 315, 315},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Constants__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
