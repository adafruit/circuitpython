// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Nested.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/nested__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Nested__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf9, 0x64, 0xf9, 0xdc, 0x32, 0x20, 0x22, 0xb7,
      0xa6, 0x81, 0xee, 0x69, 0x33, 0xee, 0x5c, 0x3c,
      0x78, 0x80, 0x25, 0xb9, 0x42, 0xfe, 0xdb, 0x7f,
      0x4d, 0x7c, 0x01, 0x8e, 0xd8, 0x40, 0xcf, 0x86,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "test_msgs/msg/detail/basic_types__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t test_msgs__msg__BasicTypes__EXPECTED_HASH = {1, {
    0x7c, 0x30, 0x0a, 0xfd, 0x4e, 0x79, 0x67, 0x98,
    0xd4, 0x9b, 0xdd, 0x6c, 0xda, 0xa0, 0xfa, 0x87,
    0xfa, 0x0e, 0xd2, 0xba, 0x32, 0x17, 0xd9, 0x77,
    0xe1, 0xfa, 0xa8, 0x70, 0x70, 0xd7, 0x97, 0xab,
  }};
#endif

static char test_msgs__msg__Nested__TYPE_NAME[] = "test_msgs/msg/Nested";
static char test_msgs__msg__BasicTypes__TYPE_NAME[] = "test_msgs/msg/BasicTypes";

// Define type names, field names, and default values
static char test_msgs__msg__Nested__FIELD_NAME__basic_types_value[] = "basic_types_value";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Nested__FIELDS[] = {
  {
    {test_msgs__msg__Nested__FIELD_NAME__basic_types_value, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__msg__Nested__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Nested__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Nested__TYPE_NAME, 20, 20},
      {test_msgs__msg__Nested__FIELDS, 1, 1},
    },
    {test_msgs__msg__Nested__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "BasicTypes basic_types_value";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Nested__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Nested__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 29, 29},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Nested__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Nested__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
