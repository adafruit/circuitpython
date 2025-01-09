// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from shape_msgs:msg/MeshTriangle.idl
// generated code does not contain a copyright notice

#include "shape_msgs/msg/detail/mesh_triangle__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_type_hash_t *
shape_msgs__msg__MeshTriangle__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x61, 0x8e, 0x5c, 0x07, 0x3e, 0xeb, 0x72, 0x9e,
      0x43, 0x3e, 0xf6, 0x22, 0x6e, 0x72, 0xc0, 0x1d,
      0x99, 0x5c, 0x45, 0x9f, 0xb7, 0xd7, 0x63, 0x48,
      0xc9, 0x70, 0x04, 0x09, 0xa5, 0x02, 0x0b, 0xd0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char shape_msgs__msg__MeshTriangle__TYPE_NAME[] = "shape_msgs/msg/MeshTriangle";

// Define type names, field names, and default values
static char shape_msgs__msg__MeshTriangle__FIELD_NAME__vertex_indices[] = "vertex_indices";

static rosidl_runtime_c__type_description__Field shape_msgs__msg__MeshTriangle__FIELDS[] = {
  {
    {shape_msgs__msg__MeshTriangle__FIELD_NAME__vertex_indices, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
shape_msgs__msg__MeshTriangle__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {shape_msgs__msg__MeshTriangle__TYPE_NAME, 27, 27},
      {shape_msgs__msg__MeshTriangle__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Definition of a triangle's vertices.\n"
  "\n"
  "uint32[3] vertex_indices";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
shape_msgs__msg__MeshTriangle__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {shape_msgs__msg__MeshTriangle__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 65, 65},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
shape_msgs__msg__MeshTriangle__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *shape_msgs__msg__MeshTriangle__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
