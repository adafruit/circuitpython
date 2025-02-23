// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/FloatingPointRange.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/floating_point_range__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__FloatingPointRange__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe6, 0xaf, 0x23, 0xa2, 0x3c, 0x17, 0x7f, 0xee,
      0x5f, 0x30, 0x75, 0xc8, 0xb1, 0xe4, 0x35, 0x16,
      0x2a, 0x9b, 0x63, 0xc8, 0x63, 0xd7, 0x8c, 0x06,
      0x01, 0x74, 0x60, 0xb4, 0x96, 0x84, 0x26, 0x2d,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__FloatingPointRange__TYPE_NAME[] = "rcl_interfaces/msg/FloatingPointRange";

// Define type names, field names, and default values
static char rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__from_value[] = "from_value";
static char rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__to_value[] = "to_value";
static char rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__step[] = "step";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__FloatingPointRange__FIELDS[] = {
  {
    {rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__from_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__to_value, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__FloatingPointRange__FIELD_NAME__step, 4, 4},
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
rcl_interfaces__msg__FloatingPointRange__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
      {rcl_interfaces__msg__FloatingPointRange__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Represents bounds and a step value for a floating point typed parameter.\n"
  "\n"
  "# Start value for valid values, inclusive.\n"
  "float64 from_value\n"
  "\n"
  "# End value for valid values, inclusive.\n"
  "float64 to_value\n"
  "\n"
  "# Size of valid steps between the from and to bound.\n"
  "# \n"
  "# Step is considered to be a magnitude, therefore negative values are treated\n"
  "# the same as positive values, and a step value of zero implies a continuous\n"
  "# range of values.\n"
  "#\n"
  "# Ideally, the step would be less than or equal to the distance between the\n"
  "# bounds, as well as an even multiple of the distance between the bounds, but\n"
  "# neither are required.\n"
  "#\n"
  "# If the absolute value of the step is larger than or equal to the distance\n"
  "# between the two bounds, then the bounds will be the only valid values. e.g. if\n"
  "# the range is defined as {from_value: 1.0, to_value: 2.0, step: 5.0} then the\n"
  "# valid values will be 1.0 and 2.0.\n"
  "#\n"
  "# If the step is less than the distance between the bounds, but the distance is\n"
  "# not a multiple of the step, then the \"to\" bound will always be a valid value,\n"
  "# e.g. if the range is defined as {from_value: 2.0, to_value: 5.0, step: 2.0}\n"
  "# then the valid values will be 2.0, 4.0, and 5.0.\n"
  "float64 step";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1186, 1186},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__FloatingPointRange__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
