// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from unique_identifier_msgs:msg/UUID.idl
// generated code does not contain a copyright notice

#include "unique_identifier_msgs/msg/detail/uuid__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_unique_identifier_msgs
const rosidl_type_hash_t *
unique_identifier_msgs__msg__UUID__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1b, 0x8e, 0x8a, 0xca, 0x95, 0x8c, 0xbe, 0xa2,
      0x8f, 0xe6, 0xef, 0x60, 0xbf, 0x6c, 0x19, 0xb6,
      0x83, 0xc9, 0x7a, 0x9e, 0xf6, 0x0b, 0xb3, 0x47,
      0x52, 0x06, 0x7d, 0x0f, 0x2f, 0x7a, 0xb4, 0x37,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char unique_identifier_msgs__msg__UUID__TYPE_NAME[] = "unique_identifier_msgs/msg/UUID";

// Define type names, field names, and default values
static char unique_identifier_msgs__msg__UUID__FIELD_NAME__uuid[] = "uuid";

static rosidl_runtime_c__type_description__Field unique_identifier_msgs__msg__UUID__FIELDS[] = {
  {
    {unique_identifier_msgs__msg__UUID__FIELD_NAME__uuid, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      16,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
unique_identifier_msgs__msg__UUID__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
      {unique_identifier_msgs__msg__UUID__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A universally unique identifier (UUID).\n"
  "#\n"
  "#  http://en.wikipedia.org/wiki/Universally_unique_identifier\n"
  "#  http://tools.ietf.org/html/rfc4122.html\n"
  "\n"
  "uint8[16] uuid";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
unique_identifier_msgs__msg__UUID__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 165, 165},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
unique_identifier_msgs__msg__UUID__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
