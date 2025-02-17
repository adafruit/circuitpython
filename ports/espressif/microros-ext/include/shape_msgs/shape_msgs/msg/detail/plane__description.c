// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from shape_msgs:msg/Plane.idl
// generated code does not contain a copyright notice

#include "shape_msgs/msg/detail/plane__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_type_hash_t *
shape_msgs__msg__Plane__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdf, 0xbf, 0xe8, 0x31, 0x46, 0x89, 0xc8, 0x50,
      0x61, 0x5d, 0x4a, 0x72, 0x7a, 0xf0, 0x17, 0xe9,
      0xaa, 0x86, 0xc1, 0x0e, 0x36, 0x9a, 0x60, 0x6c,
      0x8c, 0x85, 0x1e, 0xf8, 0xf1, 0x6c, 0x58, 0xc8,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char shape_msgs__msg__Plane__TYPE_NAME[] = "shape_msgs/msg/Plane";

// Define type names, field names, and default values
static char shape_msgs__msg__Plane__FIELD_NAME__coef[] = "coef";

static rosidl_runtime_c__type_description__Field shape_msgs__msg__Plane__FIELDS[] = {
  {
    {shape_msgs__msg__Plane__FIELD_NAME__coef, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      4,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
shape_msgs__msg__Plane__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {shape_msgs__msg__Plane__TYPE_NAME, 20, 20},
      {shape_msgs__msg__Plane__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Representation of a plane, using the plane equation ax + by + cz + d = 0.\n"
  "#\n"
  "# a := coef[0]\n"
  "# b := coef[1]\n"
  "# c := coef[2]\n"
  "# d := coef[3]\n"
  "float64[4] coef";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
shape_msgs__msg__Plane__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {shape_msgs__msg__Plane__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 154, 154},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
shape_msgs__msg__Plane__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *shape_msgs__msg__Plane__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
