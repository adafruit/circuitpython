// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/UInt32.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/u_int32__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__UInt32__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe8, 0x6c, 0xcc, 0xba, 0x58, 0x6f, 0x30, 0xc1,
      0x64, 0x98, 0xf3, 0xcc, 0xd3, 0x55, 0x0a, 0x76,
      0x4b, 0x25, 0x52, 0x39, 0xa6, 0x14, 0x2b, 0xe3,
      0xa4, 0xd7, 0xa2, 0xfa, 0x9a, 0x43, 0x51, 0x5c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__UInt32__TYPE_NAME[] = "example_interfaces/msg/UInt32";

// Define type names, field names, and default values
static char example_interfaces__msg__UInt32__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__UInt32__FIELDS[] = {
  {
    {example_interfaces__msg__UInt32__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__UInt32__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__UInt32__TYPE_NAME, 29, 29},
      {example_interfaces__msg__UInt32__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example message of using a primitive datatype, uint32.\n"
  "# If you want to test with this that's fine, but if you are deploying\n"
  "# it into a system you should create a semantically meaningful message type.\n"
  "# If you want to embed it in another message, use the primitive data type instead.\n"
  "uint32 data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__UInt32__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__UInt32__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 310, 310},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__UInt32__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__UInt32__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
