// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/Quaternion.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/quaternion__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__Quaternion__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x8a, 0x76, 0x5f, 0x66, 0x77, 0x8c, 0x8f, 0xf7,
      0xc8, 0xab, 0x94, 0xaf, 0xcc, 0x59, 0x0a, 0x2e,
      0xd5, 0x32, 0x5a, 0x1d, 0x9a, 0x07, 0x6f, 0xff,
      0xf3, 0x8f, 0xbc, 0xe3, 0x6f, 0x45, 0x86, 0x84,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";

// Define type names, field names, and default values
static char geometry_msgs__msg__Quaternion__FIELD_NAME__x[] = "x";
static char geometry_msgs__msg__Quaternion__DEFAULT_VALUE__x[] = "0.0";
static char geometry_msgs__msg__Quaternion__FIELD_NAME__y[] = "y";
static char geometry_msgs__msg__Quaternion__DEFAULT_VALUE__y[] = "0.0";
static char geometry_msgs__msg__Quaternion__FIELD_NAME__z[] = "z";
static char geometry_msgs__msg__Quaternion__DEFAULT_VALUE__z[] = "0.0";
static char geometry_msgs__msg__Quaternion__FIELD_NAME__w[] = "w";
static char geometry_msgs__msg__Quaternion__DEFAULT_VALUE__w[] = "1.0";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__Quaternion__FIELDS[] = {
  {
    {geometry_msgs__msg__Quaternion__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {geometry_msgs__msg__Quaternion__DEFAULT_VALUE__x, 3, 3},
  },
  {
    {geometry_msgs__msg__Quaternion__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {geometry_msgs__msg__Quaternion__DEFAULT_VALUE__y, 3, 3},
  },
  {
    {geometry_msgs__msg__Quaternion__FIELD_NAME__z, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {geometry_msgs__msg__Quaternion__DEFAULT_VALUE__z, 3, 3},
  },
  {
    {geometry_msgs__msg__Quaternion__FIELD_NAME__w, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {geometry_msgs__msg__Quaternion__DEFAULT_VALUE__w, 3, 3},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
geometry_msgs__msg__Quaternion__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
      {geometry_msgs__msg__Quaternion__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This represents an orientation in free space in quaternion form.\n"
  "\n"
  "float64 x 0\n"
  "float64 y 0\n"
  "float64 z 0\n"
  "float64 w 1";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__Quaternion__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 116, 116},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__Quaternion__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
