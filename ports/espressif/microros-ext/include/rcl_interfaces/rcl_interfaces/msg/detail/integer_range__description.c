// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/IntegerRange.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/integer_range__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__IntegerRange__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf7, 0xb7, 0xfd, 0xc0, 0xf6, 0x5f, 0x07, 0x70,
      0x2e, 0x09, 0x92, 0x18, 0xe1, 0x32, 0x88, 0xc3,
      0x96, 0x3b, 0xcb, 0x93, 0x45, 0xbd, 0xe7, 0x8b,
      0x56, 0x0e, 0x6c, 0xd1, 0x98, 0x00, 0xfc, 0x5a,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__IntegerRange__TYPE_NAME[] = "rcl_interfaces/msg/IntegerRange";

// Define type names, field names, and default values
static char rcl_interfaces__msg__IntegerRange__FIELD_NAME__from_value[] = "from_value";
static char rcl_interfaces__msg__IntegerRange__FIELD_NAME__to_value[] = "to_value";
static char rcl_interfaces__msg__IntegerRange__FIELD_NAME__step[] = "step";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__IntegerRange__FIELDS[] = {
  {
    {rcl_interfaces__msg__IntegerRange__FIELD_NAME__from_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__FIELD_NAME__to_value, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__FIELD_NAME__step, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__IntegerRange__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
      {rcl_interfaces__msg__IntegerRange__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Represents bounds and a step value for an integer typed parameter.\n"
  "\n"
  "# Start value for valid values, inclusive.\n"
  "int64 from_value\n"
  "\n"
  "# End value for valid values, inclusive.\n"
  "int64 to_value\n"
  "\n"
  "# Size of valid steps between the from and to bound.\n"
  "#\n"
  "# A step value of zero implies a continuous range of values. Ideally, the step\n"
  "# would be less than or equal to the distance between the bounds, as well as an\n"
  "# even multiple of the distance between the bounds, but neither are required.\n"
  "#\n"
  "# If the absolute value of the step is larger than or equal to the distance\n"
  "# between the two bounds, then the bounds will be the only valid values. e.g. if\n"
  "# the range is defined as {from_value: 1, to_value: 2, step: 5} then the valid\n"
  "# values will be 1 and 2.\n"
  "# \n"
  "# If the step is less than the distance between the bounds, but the distance is\n"
  "# not a multiple of the step, then the \"to\" bound will always be a valid value,\n"
  "# e.g. if the range is defined as {from_value: 2, to_value: 5, step: 2} then\n"
  "# the valid values will be 2, 4, and 5.\n"
  "uint64 step";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1036, 1036},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__IntegerRange__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
