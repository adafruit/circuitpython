// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/Int64.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/int64__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__Int64__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1b, 0x3b, 0x9a, 0x65, 0x02, 0xf5, 0x60, 0xd0,
      0x79, 0x52, 0x0c, 0x73, 0xc6, 0x85, 0xa9, 0x55,
      0x0e, 0x5a, 0x18, 0x38, 0xd2, 0xce, 0xfd, 0x53,
      0x7f, 0xe0, 0xab, 0xa7, 0x5a, 0x36, 0x39, 0xa0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__Int64__TYPE_NAME[] = "example_interfaces/msg/Int64";

// Define type names, field names, and default values
static char example_interfaces__msg__Int64__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__Int64__FIELDS[] = {
  {
    {example_interfaces__msg__Int64__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__Int64__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__Int64__TYPE_NAME, 28, 28},
      {example_interfaces__msg__Int64__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example message of using a primitive datatype, int64.\n"
  "# If you want to test with this that's fine, but if you are deploying\n"
  "# it into a system you should create a semantically meaningful message type.\n"
  "# If you want to embed it in another message, use the primitive data type instead.\n"
  "int64 data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__Int64__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__Int64__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 308, 308},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__Int64__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__Int64__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
