// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:msg/UVCoordinate.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/msg/detail/uv_coordinate__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__UVCoordinate__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf2, 0x7f, 0x7e, 0xd2, 0x1f, 0xe3, 0x60, 0xc6,
      0x06, 0x69, 0x44, 0xf8, 0x56, 0xb8, 0x01, 0xa0,
      0xc0, 0xd1, 0xe9, 0x4e, 0x81, 0x5b, 0x68, 0x86,
      0x44, 0x4b, 0x42, 0xd9, 0x0b, 0x19, 0x6a, 0x26,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char visualization_msgs__msg__UVCoordinate__TYPE_NAME[] = "visualization_msgs/msg/UVCoordinate";

// Define type names, field names, and default values
static char visualization_msgs__msg__UVCoordinate__FIELD_NAME__u[] = "u";
static char visualization_msgs__msg__UVCoordinate__FIELD_NAME__v[] = "v";

static rosidl_runtime_c__type_description__Field visualization_msgs__msg__UVCoordinate__FIELDS[] = {
  {
    {visualization_msgs__msg__UVCoordinate__FIELD_NAME__u, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__UVCoordinate__FIELD_NAME__v, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__UVCoordinate__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__msg__UVCoordinate__TYPE_NAME, 35, 35},
      {visualization_msgs__msg__UVCoordinate__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Location of the pixel as a ratio of the width of a 2D texture.\n"
  "# Values should be in range: [0.0-1.0].\n"
  "float32 u\n"
  "float32 v";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__msg__UVCoordinate__TYPE_NAME, 35, 35},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 125, 125},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__UVCoordinate__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
