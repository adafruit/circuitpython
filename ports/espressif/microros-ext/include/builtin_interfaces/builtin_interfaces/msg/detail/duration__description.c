// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from builtin_interfaces:msg/Duration.idl
// generated code does not contain a copyright notice

#include "builtin_interfaces/msg/detail/duration__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_builtin_interfaces
const rosidl_type_hash_t *
builtin_interfaces__msg__Duration__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe8, 0xd0, 0x09, 0xf6, 0x59, 0x81, 0x6f, 0x75,
      0x8b, 0x75, 0x33, 0x4e, 0xe1, 0xa9, 0xca, 0x5b,
      0x5c, 0x0b, 0x85, 0x98, 0x43, 0x26, 0x1f, 0x14,
      0xc7, 0xf9, 0x37, 0x34, 0x95, 0x99, 0xd9, 0x3b,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char builtin_interfaces__msg__Duration__TYPE_NAME[] = "builtin_interfaces/msg/Duration";

// Define type names, field names, and default values
static char builtin_interfaces__msg__Duration__FIELD_NAME__sec[] = "sec";
static char builtin_interfaces__msg__Duration__FIELD_NAME__nanosec[] = "nanosec";

static rosidl_runtime_c__type_description__Field builtin_interfaces__msg__Duration__FIELDS[] = {
  {
    {builtin_interfaces__msg__Duration__FIELD_NAME__sec, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Duration__FIELD_NAME__nanosec, 7, 7},
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
builtin_interfaces__msg__Duration__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
      {builtin_interfaces__msg__Duration__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Duration defines a period between two time points.\n"
  "# Messages of this datatype are of ROS Time following this design:\n"
  "# https://design.ros2.org/articles/clock_and_time.html\n"
  "\n"
  "# Seconds component, range is valid over any possible int32 value.\n"
  "int32 sec\n"
  "\n"
  "# Nanoseconds component in the range of [0, 1e9).\n"
  "uint32 nanosec";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
builtin_interfaces__msg__Duration__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 319, 319},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
builtin_interfaces__msg__Duration__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
