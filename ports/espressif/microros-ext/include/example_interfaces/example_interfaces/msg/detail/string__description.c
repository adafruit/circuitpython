// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/String.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/string__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__String__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x55, 0x09, 0xd8, 0x66, 0xa5, 0x79, 0x95, 0x1f,
      0x2f, 0xc6, 0xc1, 0x95, 0x77, 0xc3, 0x26, 0x05,
      0xba, 0x16, 0xf3, 0x08, 0xca, 0xe7, 0xb4, 0x98,
      0x34, 0x1d, 0x79, 0x53, 0x6d, 0x4e, 0xb0, 0x6b,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__String__TYPE_NAME[] = "example_interfaces/msg/String";

// Define type names, field names, and default values
static char example_interfaces__msg__String__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__String__FIELDS[] = {
  {
    {example_interfaces__msg__String__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__String__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__String__TYPE_NAME, 29, 29},
      {example_interfaces__msg__String__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example message of using a primitive datatype, string.\n"
  "# If you want to test with this that's fine, but if you are deploying\n"
  "# it into a system you should create a semantically meaningful message type.\n"
  "# If you want to embed it in another message, use the primitive data type instead.\n"
  "string data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__String__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__String__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 310, 310},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__String__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__String__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
