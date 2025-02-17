// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/Char.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/char__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__Char__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x32, 0x0d, 0xcd, 0x57, 0xe1, 0x18, 0x3f, 0xb0,
      0x84, 0x63, 0xcc, 0x3a, 0xb5, 0x0b, 0xf7, 0xe5,
      0xce, 0x0e, 0xce, 0xe3, 0x9f, 0x64, 0xd1, 0x5a,
      0x9e, 0x9e, 0xec, 0xa3, 0x38, 0x4c, 0x91, 0xa5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__Char__TYPE_NAME[] = "example_interfaces/msg/Char";

// Define type names, field names, and default values
static char example_interfaces__msg__Char__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__Char__FIELDS[] = {
  {
    {example_interfaces__msg__Char__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__Char__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__Char__TYPE_NAME, 27, 27},
      {example_interfaces__msg__Char__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example message of using a primitive datatype, char.\n"
  "# If you want to test with this that's fine, but if you are deploying\n"
  "# it into a system you should create a semantically meaningful message type.\n"
  "# If you want to embed it in another message, use the primitive data type instead.\n"
  "char data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__Char__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__Char__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 306, 306},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__Char__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__Char__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
