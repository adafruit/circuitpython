// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/SetParametersResult.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/set_parameters_result__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__SetParametersResult__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xcf, 0xcc, 0x0f, 0xb0, 0x37, 0x1e, 0xe5, 0x15,
      0x9b, 0x40, 0x39, 0x60, 0xef, 0x43, 0x00, 0xf8,
      0xf9, 0xd2, 0xf1, 0xfd, 0x61, 0x17, 0xc8, 0x66,
      0x6b, 0x7f, 0x96, 0x54, 0xd5, 0x28, 0xa9, 0xb1,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__SetParametersResult__TYPE_NAME[] = "rcl_interfaces/msg/SetParametersResult";

// Define type names, field names, and default values
static char rcl_interfaces__msg__SetParametersResult__FIELD_NAME__successful[] = "successful";
static char rcl_interfaces__msg__SetParametersResult__FIELD_NAME__reason[] = "reason";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__SetParametersResult__FIELDS[] = {
  {
    {rcl_interfaces__msg__SetParametersResult__FIELD_NAME__successful, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__SetParametersResult__FIELD_NAME__reason, 6, 6},
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
rcl_interfaces__msg__SetParametersResult__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
      {rcl_interfaces__msg__SetParametersResult__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A true value of the same index indicates that the parameter was set\n"
  "# successfully. A false value indicates the change was rejected.\n"
  "bool successful\n"
  "\n"
  "# Reason why the setting was a failure. On success, the contents of this field\n"
  "# are undefined.  This should only be used for logging and user interfaces.\n"
  "string reason";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__SetParametersResult__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 321, 321},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__SetParametersResult__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__SetParametersResult__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
