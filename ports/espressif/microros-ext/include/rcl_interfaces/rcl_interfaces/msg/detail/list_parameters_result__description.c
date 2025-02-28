// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/ListParametersResult.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/list_parameters_result__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__ListParametersResult__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x23, 0x7a, 0xe3, 0x42, 0x84, 0x13, 0xdc, 0xbc,
      0xfb, 0x45, 0x2b, 0x51, 0x0c, 0x42, 0x35, 0x5f,
      0x3a, 0x2b, 0x02, 0x1d, 0xc0, 0x91, 0xaf, 0xa3,
      0xe1, 0x85, 0x26, 0xd5, 0x70, 0x22, 0xf1, 0xcd,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__ListParametersResult__TYPE_NAME[] = "rcl_interfaces/msg/ListParametersResult";

// Define type names, field names, and default values
static char rcl_interfaces__msg__ListParametersResult__FIELD_NAME__names[] = "names";
static char rcl_interfaces__msg__ListParametersResult__FIELD_NAME__prefixes[] = "prefixes";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__ListParametersResult__FIELDS[] = {
  {
    {rcl_interfaces__msg__ListParametersResult__FIELD_NAME__names, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ListParametersResult__FIELD_NAME__prefixes, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__ListParametersResult__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
      {rcl_interfaces__msg__ListParametersResult__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# The resulting parameters under the given prefixes.\n"
  "string[] names\n"
  "\n"
  "# The resulting prefixes under the given prefixes.\n"
  "# TODO(wjwwood): link to prefix definition and rules.\n"
  "string[] prefixes";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__ListParametersResult__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 192, 192},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__ListParametersResult__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__ListParametersResult__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
