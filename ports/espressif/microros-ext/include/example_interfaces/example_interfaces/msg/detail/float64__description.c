// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/Float64.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/float64__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__Float64__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x74, 0xc1, 0x37, 0xb7, 0x93, 0x0c, 0x26, 0x33,
      0x94, 0x25, 0xa9, 0x5f, 0xcf, 0xab, 0x44, 0x11,
      0x99, 0xbc, 0x41, 0xe0, 0xe5, 0x72, 0xd3, 0xa0,
      0xc9, 0xe9, 0x5b, 0xad, 0xd7, 0x2b, 0x50, 0xda,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__Float64__TYPE_NAME[] = "example_interfaces/msg/Float64";

// Define type names, field names, and default values
static char example_interfaces__msg__Float64__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__Float64__FIELDS[] = {
  {
    {example_interfaces__msg__Float64__FIELD_NAME__data, 4, 4},
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
example_interfaces__msg__Float64__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__Float64__TYPE_NAME, 30, 30},
      {example_interfaces__msg__Float64__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example message of using a primitive datatype, float64.\n"
  "# If you want to test with this that's fine, but if you are deploying\n"
  "# it into a system you should create a semantically meaningful message type.\n"
  "# If you want to embed it in another message, use the primitive data type instead.\n"
  "float64 data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__Float64__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__Float64__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 312, 312},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__Float64__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__Float64__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
