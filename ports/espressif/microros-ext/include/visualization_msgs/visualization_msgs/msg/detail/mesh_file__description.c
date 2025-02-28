// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:msg/MeshFile.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/msg/detail/mesh_file__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__MeshFile__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x77, 0x10, 0xec, 0xe1, 0x5a, 0x14, 0x8f, 0xb7,
      0xc9, 0xb5, 0x46, 0x36, 0x4c, 0xfb, 0x21, 0x5b,
      0xb0, 0x60, 0x98, 0x08, 0x7b, 0xd6, 0x39, 0x4f,
      0xe5, 0xb7, 0x3a, 0x49, 0x35, 0x08, 0xf8, 0xc4,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char visualization_msgs__msg__MeshFile__TYPE_NAME[] = "visualization_msgs/msg/MeshFile";

// Define type names, field names, and default values
static char visualization_msgs__msg__MeshFile__FIELD_NAME__filename[] = "filename";
static char visualization_msgs__msg__MeshFile__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field visualization_msgs__msg__MeshFile__FIELDS[] = {
  {
    {visualization_msgs__msg__MeshFile__FIELD_NAME__filename, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MeshFile__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__MeshFile__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__msg__MeshFile__TYPE_NAME, 31, 31},
      {visualization_msgs__msg__MeshFile__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Used to send raw mesh files.\n"
  "\n"
  "# The filename is used for both debug purposes and to provide a file extension\n"
  "# for whatever parser is used.\n"
  "string filename\n"
  "\n"
  "# This stores the raw text of the mesh file.\n"
  "uint8[] data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__MeshFile__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__msg__MeshFile__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 217, 217},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__MeshFile__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__msg__MeshFile__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
