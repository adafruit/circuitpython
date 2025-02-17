// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from std_msgs:msg/Int64.idl
// generated code does not contain a copyright notice

#include "std_msgs/msg/detail/int64__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_std_msgs
const rosidl_type_hash_t *
std_msgs__msg__Int64__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x8c, 0xd1, 0x04, 0x8c, 0x2f, 0x18, 0x6b, 0x6b,
      0xd9, 0xa9, 0x24, 0x72, 0xdc, 0x1c, 0xe5, 0x17,
      0x23, 0xc0, 0x83, 0x3a, 0x22, 0x1e, 0x2b, 0x7a,
      0xec, 0xff, 0xf1, 0x11, 0x77, 0x4f, 0x4b, 0x49,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char std_msgs__msg__Int64__TYPE_NAME[] = "std_msgs/msg/Int64";

// Define type names, field names, and default values
static char std_msgs__msg__Int64__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field std_msgs__msg__Int64__FIELDS[] = {
  {
    {std_msgs__msg__Int64__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
std_msgs__msg__Int64__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {std_msgs__msg__Int64__TYPE_NAME, 18, 18},
      {std_msgs__msg__Int64__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This was originally provided as an example message.\n"
  "# It is deprecated as of Foxy\n"
  "# It is recommended to create your own semantically meaningful message.\n"
  "# However if you would like to continue using this please use the equivalent in example_msgs.\n"
  "\n"
  "int64 data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
std_msgs__msg__Int64__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {std_msgs__msg__Int64__TYPE_NAME, 18, 18},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 262, 262},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
std_msgs__msg__Int64__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *std_msgs__msg__Int64__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
