// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from builtin_interfaces:msg/Time.idl
// generated code does not contain a copyright notice

#include "builtin_interfaces/msg/detail/time__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_builtin_interfaces
const rosidl_type_hash_t *
builtin_interfaces__msg__Time__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
      0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
      0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
      0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char builtin_interfaces__msg__Time__FIELD_NAME__sec[] = "sec";
static char builtin_interfaces__msg__Time__FIELD_NAME__nanosec[] = "nanosec";

static rosidl_runtime_c__type_description__Field builtin_interfaces__msg__Time__FIELDS[] = {
  {
    {builtin_interfaces__msg__Time__FIELD_NAME__sec, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__FIELD_NAME__nanosec, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
builtin_interfaces__msg__Time__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
      {builtin_interfaces__msg__Time__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message communicates ROS Time defined here:\n"
  "# https://design.ros2.org/articles/clock_and_time.html\n"
  "\n"
  "# The seconds component, valid over all int32 values.\n"
  "int32 sec\n"
  "\n"
  "# The nanoseconds component, valid in the range [0, 1e9).\n"
  "uint32 nanosec";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
builtin_interfaces__msg__Time__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 245, 245},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
builtin_interfaces__msg__Time__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
