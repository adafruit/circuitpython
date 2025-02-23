// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from std_msgs:msg/Empty.idl
// generated code does not contain a copyright notice

#include "std_msgs/msg/detail/empty__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_std_msgs
const rosidl_type_hash_t *
std_msgs__msg__Empty__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x20, 0xb6, 0x25, 0x25, 0x6f, 0x32, 0xd5, 0xdb,
      0xc0, 0xd0, 0x4f, 0xee, 0x44, 0xf4, 0x3c, 0x41,
      0xe5, 0x1c, 0x70, 0xd3, 0x50, 0x2f, 0x84, 0xb4,
      0xa0, 0x8e, 0x7a, 0x9c, 0x26, 0xa9, 0x63, 0x12,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char std_msgs__msg__Empty__TYPE_NAME[] = "std_msgs/msg/Empty";

// Define type names, field names, and default values
static char std_msgs__msg__Empty__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field std_msgs__msg__Empty__FIELDS[] = {
  {
    {std_msgs__msg__Empty__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
std_msgs__msg__Empty__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {std_msgs__msg__Empty__TYPE_NAME, 18, 18},
      {std_msgs__msg__Empty__FIELDS, 1, 1},
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
std_msgs__msg__Empty__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {std_msgs__msg__Empty__TYPE_NAME, 18, 18},
    {msg_encoding, 3, 3},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
std_msgs__msg__Empty__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *std_msgs__msg__Empty__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
