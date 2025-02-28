// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Strings.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/strings__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Strings__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xee, 0xe5, 0x55, 0x33, 0x63, 0x62, 0x93, 0xac,
      0x7c, 0x94, 0xc6, 0x24, 0x32, 0xee, 0xdb, 0x57,
      0x77, 0xeb, 0x84, 0x98, 0x47, 0xd2, 0xa1, 0x7b,
      0x4d, 0xce, 0xad, 0x8c, 0x4a, 0x20, 0xab, 0x32,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__Strings__TYPE_NAME[] = "test_msgs/msg/Strings";

// Define type names, field names, and default values
static char test_msgs__msg__Strings__FIELD_NAME__string_value[] = "string_value";
static char test_msgs__msg__Strings__FIELD_NAME__string_value_default1[] = "string_value_default1";
static char test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default1[] = "Hello world!";
static char test_msgs__msg__Strings__FIELD_NAME__string_value_default2[] = "string_value_default2";
static char test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default2[] = "Hello'world!";
static char test_msgs__msg__Strings__FIELD_NAME__string_value_default3[] = "string_value_default3";
static char test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default3[] = "Hello\"world!";
static char test_msgs__msg__Strings__FIELD_NAME__string_value_default4[] = "string_value_default4";
static char test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default4[] = "Hello'world!";
static char test_msgs__msg__Strings__FIELD_NAME__string_value_default5[] = "string_value_default5";
static char test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default5[] = "Hello\"world!";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value[] = "bounded_string_value";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default1[] = "bounded_string_value_default1";
static char test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default1[] = "Hello world!";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default2[] = "bounded_string_value_default2";
static char test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default2[] = "Hello'world!";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default3[] = "bounded_string_value_default3";
static char test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default3[] = "Hello\"world!";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default4[] = "bounded_string_value_default4";
static char test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default4[] = "Hello'world!";
static char test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default5[] = "bounded_string_value_default5";
static char test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default5[] = "Hello\"world!";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Strings__FIELDS[] = {
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value_default1, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default1, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value_default2, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default2, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value_default3, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default3, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value_default4, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default4, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__string_value_default5, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__string_value_default5, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default1, 29, 29},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default1, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default2, 29, 29},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default2, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default3, 29, 29},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default3, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default4, 29, 29},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default4, 12, 12},
  },
  {
    {test_msgs__msg__Strings__FIELD_NAME__bounded_string_value_default5, 29, 29},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      22,
      {NULL, 0, 0},
    },
    {test_msgs__msg__Strings__DEFAULT_VALUE__bounded_string_value_default5, 12, 12},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Strings__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Strings__TYPE_NAME, 21, 21},
      {test_msgs__msg__Strings__FIELDS, 12, 12},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string string_value\n"
  "string string_value_default1 \"Hello world!\"\n"
  "string string_value_default2 \"Hello\\'world!\"\n"
  "string string_value_default3 \\'Hello\"world!\\'\n"
  "string string_value_default4 'Hello\\\\'world!'\n"
  "string string_value_default5 \"Hello\\\\\"world!\"\n"
  "string STRING_CONST=\"Hello world!\"\n"
  "string<=22 bounded_string_value\n"
  "string<=22 bounded_string_value_default1 \"Hello world!\"\n"
  "string<=22 bounded_string_value_default2 \"Hello\\'world!\"\n"
  "string<=22 bounded_string_value_default3 \\'Hello\"world!\\'\n"
  "string<=22 bounded_string_value_default4 'Hello\\\\'world!'\n"
  "string<=22 bounded_string_value_default5 \"Hello\\\\\"world!\"";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Strings__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Strings__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 591, 591},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Strings__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Strings__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
