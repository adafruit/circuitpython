// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/ParameterDescriptor.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/parameter_descriptor__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__ParameterDescriptor__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x52, 0x17, 0x5d, 0xbf, 0xda, 0x6c, 0x51, 0x15,
      0x31, 0x01, 0xd3, 0x3d, 0x2a, 0x9d, 0xa0, 0x57,
      0x43, 0xf6, 0x6f, 0x02, 0xd5, 0xab, 0x2c, 0xa9,
      0xec, 0x47, 0x09, 0xb4, 0x6b, 0x73, 0xd7, 0x04,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "rcl_interfaces/msg/detail/integer_range__functions.h"
#include "rcl_interfaces/msg/detail/floating_point_range__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH = {1, {
    0xe6, 0xaf, 0x23, 0xa2, 0x3c, 0x17, 0x7f, 0xee,
    0x5f, 0x30, 0x75, 0xc8, 0xb1, 0xe4, 0x35, 0x16,
    0x2a, 0x9b, 0x63, 0xc8, 0x63, 0xd7, 0x8c, 0x06,
    0x01, 0x74, 0x60, 0xb4, 0x96, 0x84, 0x26, 0x2d,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__IntegerRange__EXPECTED_HASH = {1, {
    0xf7, 0xb7, 0xfd, 0xc0, 0xf6, 0x5f, 0x07, 0x70,
    0x2e, 0x09, 0x92, 0x18, 0xe1, 0x32, 0x88, 0xc3,
    0x96, 0x3b, 0xcb, 0x93, 0x45, 0xbd, 0xe7, 0x8b,
    0x56, 0x0e, 0x6c, 0xd1, 0x98, 0x00, 0xfc, 0x5a,
  }};
#endif

static char rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME[] = "rcl_interfaces/msg/ParameterDescriptor";
static char rcl_interfaces__msg__FloatingPointRange__TYPE_NAME[] = "rcl_interfaces/msg/FloatingPointRange";
static char rcl_interfaces__msg__IntegerRange__TYPE_NAME[] = "rcl_interfaces/msg/IntegerRange";

// Define type names, field names, and default values
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__name[] = "name";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__type[] = "type";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__description[] = "description";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__additional_constraints[] = "additional_constraints";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__read_only[] = "read_only";
static char rcl_interfaces__msg__ParameterDescriptor__DEFAULT_VALUE__read_only[] = "False";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__dynamic_typing[] = "dynamic_typing";
static char rcl_interfaces__msg__ParameterDescriptor__DEFAULT_VALUE__dynamic_typing[] = "False";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__floating_point_range[] = "floating_point_range";
static char rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__integer_range[] = "integer_range";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__ParameterDescriptor__FIELDS[] = {
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__description, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__additional_constraints, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__read_only, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {rcl_interfaces__msg__ParameterDescriptor__DEFAULT_VALUE__read_only, 5, 5},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__dynamic_typing, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {rcl_interfaces__msg__ParameterDescriptor__DEFAULT_VALUE__dynamic_typing, 5, 5},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__floating_point_range, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__FIELD_NAME__integer_range, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__msg__ParameterDescriptor__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__ParameterDescriptor__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
      {rcl_interfaces__msg__ParameterDescriptor__FIELDS, 8, 8},
    },
    {rcl_interfaces__msg__ParameterDescriptor__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH, rcl_interfaces__msg__FloatingPointRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__FloatingPointRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__IntegerRange__EXPECTED_HASH, rcl_interfaces__msg__IntegerRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__IntegerRange__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is the message to communicate a parameter's descriptor.\n"
  "\n"
  "# The name of the parameter.\n"
  "string name\n"
  "\n"
  "# Enum values are defined in the `ParameterType.msg` message.\n"
  "uint8 type\n"
  "\n"
  "# Description of the parameter, visible from introspection tools.\n"
  "string description\n"
  "\n"
  "# Parameter constraints\n"
  "\n"
  "# Plain English description of additional constraints which cannot be expressed\n"
  "# with the available constraints, e.g. \"only prime numbers\".\n"
  "#\n"
  "# By convention, this should only be used to clarify constraints which cannot\n"
  "# be completely expressed with the parameter constraints below.\n"
  "string additional_constraints\n"
  "\n"
  "# If 'true' then the value cannot change after it has been initialized.\n"
  "bool read_only false\n"
  "\n"
  "# If true, the parameter is allowed to change type.\n"
  "bool dynamic_typing false\n"
  "\n"
  "# If any of the following sequences are not empty, then the constraint inside of\n"
  "# them apply to this parameter.\n"
  "#\n"
  "# FloatingPointRange and IntegerRange are mutually exclusive.\n"
  "\n"
  "# FloatingPointRange consists of a from_value, a to_value, and a step.\n"
  "FloatingPointRange[<=1] floating_point_range\n"
  "\n"
  "# IntegerRange consists of a from_value, a to_value, and a step.\n"
  "IntegerRange[<=1] integer_range";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1172, 1172},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__ParameterDescriptor__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
