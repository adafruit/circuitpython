// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/WStrings.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/w_strings__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__WStrings__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5a, 0x2c, 0x8d, 0xdb, 0x20, 0x54, 0x60, 0x00,
      0x23, 0xb9, 0xa1, 0x80, 0x65, 0x2f, 0x2b, 0xa8,
      0x95, 0x37, 0x32, 0x17, 0xca, 0xd4, 0xb1, 0xf0,
      0xfc, 0x12, 0x58, 0xc9, 0xe4, 0x42, 0x4a, 0x79,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__WStrings__TYPE_NAME[] = "test_msgs/msg/WStrings";

// Define type names, field names, and default values
static char test_msgs__msg__WStrings__FIELD_NAME__wstring_value[] = "wstring_value";
static char test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default1[] = "wstring_value_default1";
static char test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default1[] = "Hello world!";
static char test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default2[] = "wstring_value_default2";
static char test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default2[] = "Hell\\xc3\\xb6 w\\xc3\\xb6rld!";
static char test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default3[] = "wstring_value_default3";
static char test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default3[] = "\\xe3\\x83\\x8f\\xe3\\x83\\xad\\xe3\\x83\\xbc\\xe3\\x83\\xaf\\xe3\\x83\\xbc\\xe3\\x83\\xab\\xe3\\x83\\x89";
static char test_msgs__msg__WStrings__FIELD_NAME__array_of_wstrings[] = "array_of_wstrings";
static char test_msgs__msg__WStrings__FIELD_NAME__bounded_sequence_of_wstrings[] = "bounded_sequence_of_wstrings";
static char test_msgs__msg__WStrings__FIELD_NAME__unbounded_sequence_of_wstrings[] = "unbounded_sequence_of_wstrings";

static rosidl_runtime_c__type_description__Field test_msgs__msg__WStrings__FIELDS[] = {
  {
    {test_msgs__msg__WStrings__FIELD_NAME__wstring_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default1, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default1, 12, 12},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default2, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default2, 12, 12},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__wstring_value_default3, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__msg__WStrings__DEFAULT_VALUE__wstring_value_default3, 7, 7},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__array_of_wstrings, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__bounded_sequence_of_wstrings, 28, 28},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_BOUNDED_SEQUENCE,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__WStrings__FIELD_NAME__unbounded_sequence_of_wstrings, 30, 30},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_WSTRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__WStrings__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__WStrings__TYPE_NAME, 22, 22},
      {test_msgs__msg__WStrings__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "wstring wstring_value\n"
  "wstring wstring_value_default1 \"Hello world!\"\n"
  "wstring wstring_value_default2 \"Hell\\xc3\\xb6 w\\xc3\\xb6rld!\"\n"
  "wstring wstring_value_default3 \"\\xe3\\x83\\x8f\\xe3\\x83\\xad\\xe3\\x83\\xbc\\xe3\\x83\\xaf\\xe3\\x83\\xbc\\xe3\\x83\\xab\\xe3\\x83\\x89\"\n"
  "#wstring WSTRING_CONST=\"Hello world!\"\n"
  "#wstring<=22 bounded_wstring_value\n"
  "#wstring<=22 bounded_wstring_value_default1 \"Hello world!\"\n"
  "wstring[3] array_of_wstrings\n"
  "wstring[<=3] bounded_sequence_of_wstrings\n"
  "wstring[] unbounded_sequence_of_wstrings";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__WStrings__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__WStrings__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 399, 399},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__WStrings__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__WStrings__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
