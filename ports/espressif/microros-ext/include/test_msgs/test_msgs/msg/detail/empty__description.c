// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/Empty.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/empty__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Empty__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x65, 0x87, 0xce, 0xac, 0xfd, 0xb9, 0x7a, 0x89,
      0x5b, 0x60, 0x77, 0x4d, 0x02, 0x10, 0x11, 0xc6,
      0x4f, 0x8c, 0x72, 0x99, 0xf5, 0x33, 0x22, 0x40,
      0x4a, 0xf4, 0x03, 0x6d, 0xec, 0x61, 0x42, 0xdc,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__Empty__TYPE_NAME[] = "test_msgs/msg/Empty";

// Define type names, field names, and default values
static char test_msgs__msg__Empty__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field test_msgs__msg__Empty__FIELDS[] = {
  {
    {test_msgs__msg__Empty__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
test_msgs__msg__Empty__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__Empty__TYPE_NAME, 19, 19},
      {test_msgs__msg__Empty__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}


static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Empty__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__Empty__TYPE_NAME, 19, 19},
    {msg_encoding, 3, 3},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Empty__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__Empty__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
