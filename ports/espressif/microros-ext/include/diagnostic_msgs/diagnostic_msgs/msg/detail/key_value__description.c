// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from diagnostic_msgs:msg/KeyValue.idl
// generated code does not contain a copyright notice

#include "diagnostic_msgs/msg/detail/key_value__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__msg__KeyValue__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xd6, 0x80, 0x81, 0xea, 0xa5, 0x40, 0x28, 0x8c,
      0x54, 0x40, 0x75, 0x3b, 0xae, 0xce, 0xf0, 0xc4,
      0xe1, 0x6e, 0x81, 0xa5, 0xf7, 0x8a, 0xd6, 0x89,
      0x02, 0xde, 0xd5, 0x10, 0x04, 0x13, 0xbb, 0x42,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char diagnostic_msgs__msg__KeyValue__TYPE_NAME[] = "diagnostic_msgs/msg/KeyValue";

// Define type names, field names, and default values
static char diagnostic_msgs__msg__KeyValue__FIELD_NAME__key[] = "key";
static char diagnostic_msgs__msg__KeyValue__FIELD_NAME__value[] = "value";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__msg__KeyValue__FIELDS[] = {
  {
    {diagnostic_msgs__msg__KeyValue__FIELD_NAME__key, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__KeyValue__FIELD_NAME__value, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__msg__KeyValue__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
      {diagnostic_msgs__msg__KeyValue__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# What to label this value when viewing.\n"
  "string key\n"
  "# A value to track over time.\n"
  "string value";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 95, 95},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__msg__KeyValue__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
