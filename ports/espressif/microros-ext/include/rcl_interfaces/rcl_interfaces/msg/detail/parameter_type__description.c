// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/ParameterType.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/parameter_type__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__ParameterType__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdf, 0x29, 0xed, 0x05, 0x7a, 0x83, 0x48, 0x62,
      0x18, 0x7b, 0xe2, 0x4d, 0xd1, 0x87, 0xd9, 0x81,
      0x79, 0x0f, 0xf3, 0xea, 0x65, 0x02, 0xf4, 0xcd,
      0x27, 0xb4, 0x32, 0xcb, 0xc4, 0x2c, 0x6d, 0x46,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__ParameterType__TYPE_NAME[] = "rcl_interfaces/msg/ParameterType";

// Define type names, field names, and default values
static char rcl_interfaces__msg__ParameterType__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__ParameterType__FIELDS[] = {
  {
    {rcl_interfaces__msg__ParameterType__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
rcl_interfaces__msg__ParameterType__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__ParameterType__TYPE_NAME, 32, 32},
      {rcl_interfaces__msg__ParameterType__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# These types correspond to the value that is set in the ParameterValue message.\n"
  "\n"
  "# Default value, which implies this is not a valid parameter.\n"
  "uint8 PARAMETER_NOT_SET=0\n"
  "\n"
  "uint8 PARAMETER_BOOL=1\n"
  "uint8 PARAMETER_INTEGER=2\n"
  "uint8 PARAMETER_DOUBLE=3\n"
  "uint8 PARAMETER_STRING=4\n"
  "uint8 PARAMETER_BYTE_ARRAY=5\n"
  "uint8 PARAMETER_BOOL_ARRAY=6\n"
  "uint8 PARAMETER_INTEGER_ARRAY=7\n"
  "uint8 PARAMETER_DOUBLE_ARRAY=8\n"
  "uint8 PARAMETER_STRING_ARRAY=9";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__ParameterType__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__ParameterType__TYPE_NAME, 32, 32},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 422, 422},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__ParameterType__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__ParameterType__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
