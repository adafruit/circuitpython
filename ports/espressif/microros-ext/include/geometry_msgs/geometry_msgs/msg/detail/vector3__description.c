// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/Vector3.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/vector3__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__Vector3__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
      0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
      0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
      0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";

// Define type names, field names, and default values
static char geometry_msgs__msg__Vector3__FIELD_NAME__x[] = "x";
static char geometry_msgs__msg__Vector3__FIELD_NAME__y[] = "y";
static char geometry_msgs__msg__Vector3__FIELD_NAME__z[] = "z";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__Vector3__FIELDS[] = {
  {
    {geometry_msgs__msg__Vector3__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__FIELD_NAME__z, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
geometry_msgs__msg__Vector3__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
      {geometry_msgs__msg__Vector3__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This represents a vector in free space.\n"
  "\n"
  "# This is semantically different than a point.\n"
  "# A vector is always anchored at the origin.\n"
  "# When a transform is applied to a vector, only the rotational component is applied.\n"
  "\n"
  "float64 x\n"
  "float64 y\n"
  "float64 z";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__Vector3__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 251, 251},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__Vector3__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
