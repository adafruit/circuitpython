// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from shape_msgs:msg/Mesh.idl
// generated code does not contain a copyright notice

#include "shape_msgs/msg/detail/mesh__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_type_hash_t *
shape_msgs__msg__Mesh__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf2, 0x15, 0x0b, 0x82, 0xd8, 0xee, 0x7e, 0x8b,
      0xc3, 0xf3, 0x96, 0xa2, 0xb1, 0x58, 0xae, 0xfb,
      0x4b, 0x9a, 0x55, 0x10, 0xa4, 0x74, 0xbe, 0x27,
      0x1b, 0xa1, 0x26, 0x8a, 0xeb, 0xb5, 0x52, 0x89,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/point__functions.h"
#include "shape_msgs/msg/detail/mesh_triangle__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Point__EXPECTED_HASH = {1, {
    0x69, 0x63, 0x08, 0x48, 0x42, 0xa9, 0xb0, 0x44,
    0x94, 0xd6, 0xb2, 0x94, 0x1d, 0x11, 0x44, 0x47,
    0x08, 0xd8, 0x92, 0xda, 0x2f, 0x4b, 0x09, 0x84,
    0x3b, 0x9c, 0x43, 0xf4, 0x2a, 0x7f, 0x68, 0x81,
  }};
static const rosidl_type_hash_t shape_msgs__msg__MeshTriangle__EXPECTED_HASH = {1, {
    0x61, 0x8e, 0x5c, 0x07, 0x3e, 0xeb, 0x72, 0x9e,
    0x43, 0x3e, 0xf6, 0x22, 0x6e, 0x72, 0xc0, 0x1d,
    0x99, 0x5c, 0x45, 0x9f, 0xb7, 0xd7, 0x63, 0x48,
    0xc9, 0x70, 0x04, 0x09, 0xa5, 0x02, 0x0b, 0xd0,
  }};
#endif

static char shape_msgs__msg__Mesh__TYPE_NAME[] = "shape_msgs/msg/Mesh";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char shape_msgs__msg__MeshTriangle__TYPE_NAME[] = "shape_msgs/msg/MeshTriangle";

// Define type names, field names, and default values
static char shape_msgs__msg__Mesh__FIELD_NAME__triangles[] = "triangles";
static char shape_msgs__msg__Mesh__FIELD_NAME__vertices[] = "vertices";

static rosidl_runtime_c__type_description__Field shape_msgs__msg__Mesh__FIELDS[] = {
  {
    {shape_msgs__msg__Mesh__FIELD_NAME__triangles, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {shape_msgs__msg__MeshTriangle__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {shape_msgs__msg__Mesh__FIELD_NAME__vertices, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription shape_msgs__msg__Mesh__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {shape_msgs__msg__MeshTriangle__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
shape_msgs__msg__Mesh__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {shape_msgs__msg__Mesh__TYPE_NAME, 19, 19},
      {shape_msgs__msg__Mesh__FIELDS, 2, 2},
    },
    {shape_msgs__msg__Mesh__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&shape_msgs__msg__MeshTriangle__EXPECTED_HASH, shape_msgs__msg__MeshTriangle__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = shape_msgs__msg__MeshTriangle__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Definition of a mesh.\n"
  "\n"
  "# List of triangles; the index values refer to positions in vertices[].\n"
  "MeshTriangle[] triangles\n"
  "\n"
  "# The actual vertices that make up the mesh.\n"
  "geometry_msgs/Point[] vertices";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
shape_msgs__msg__Mesh__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {shape_msgs__msg__Mesh__TYPE_NAME, 19, 19},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 199, 199},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
shape_msgs__msg__Mesh__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *shape_msgs__msg__Mesh__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[2] = *shape_msgs__msg__MeshTriangle__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
