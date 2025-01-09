// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/Point.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/point__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__Point__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x69, 0x63, 0x08, 0x48, 0x42, 0xa9, 0xb0, 0x44,
      0x94, 0xd6, 0xb2, 0x94, 0x1d, 0x11, 0x44, 0x47,
      0x08, 0xd8, 0x92, 0xda, 0x2f, 0x4b, 0x09, 0x84,
      0x3b, 0x9c, 0x43, 0xf4, 0x2a, 0x7f, 0x68, 0x81,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";

// Define type names, field names, and default values
static char geometry_msgs__msg__Point__FIELD_NAME__x[] = "x";
static char geometry_msgs__msg__Point__FIELD_NAME__y[] = "y";
static char geometry_msgs__msg__Point__FIELD_NAME__z[] = "z";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__Point__FIELDS[] = {
  {
    {geometry_msgs__msg__Point__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Point__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Point__FIELD_NAME__z, 1, 1},
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
geometry_msgs__msg__Point__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
      {geometry_msgs__msg__Point__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This contains the position of a point in free space\n"
  "float64 x\n"
  "float64 y\n"
  "float64 z";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__Point__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 84, 84},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__Point__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
