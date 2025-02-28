// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from std_msgs:msg/ColorRGBA.idl
// generated code does not contain a copyright notice

#include "std_msgs/msg/detail/color_rgba__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_std_msgs
const rosidl_type_hash_t *
std_msgs__msg__ColorRGBA__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x77, 0xa7, 0xa5, 0xb9, 0xae, 0x47, 0x73, 0x06,
      0x09, 0x76, 0x65, 0x10, 0x6e, 0x04, 0x13, 0xba,
      0x74, 0x44, 0x02, 0x45, 0xb1, 0xf3, 0xd0, 0xc6,
      0xd6, 0x40, 0x5f, 0xe5, 0xc7, 0x81, 0x3f, 0xe8,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char std_msgs__msg__ColorRGBA__TYPE_NAME[] = "std_msgs/msg/ColorRGBA";

// Define type names, field names, and default values
static char std_msgs__msg__ColorRGBA__FIELD_NAME__r[] = "r";
static char std_msgs__msg__ColorRGBA__FIELD_NAME__g[] = "g";
static char std_msgs__msg__ColorRGBA__FIELD_NAME__b[] = "b";
static char std_msgs__msg__ColorRGBA__FIELD_NAME__a[] = "a";

static rosidl_runtime_c__type_description__Field std_msgs__msg__ColorRGBA__FIELDS[] = {
  {
    {std_msgs__msg__ColorRGBA__FIELD_NAME__r, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__ColorRGBA__FIELD_NAME__g, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__ColorRGBA__FIELD_NAME__b, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__ColorRGBA__FIELD_NAME__a, 1, 1},
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
std_msgs__msg__ColorRGBA__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {std_msgs__msg__ColorRGBA__TYPE_NAME, 22, 22},
      {std_msgs__msg__ColorRGBA__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float32 r\n"
  "float32 g\n"
  "float32 b\n"
  "float32 a";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
std_msgs__msg__ColorRGBA__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {std_msgs__msg__ColorRGBA__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 40, 40},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
std_msgs__msg__ColorRGBA__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *std_msgs__msg__ColorRGBA__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
