// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/ParameterValue.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/parameter_value__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__ParameterValue__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x11, 0x5f, 0xc0, 0x89, 0xa3, 0x87, 0xe2, 0x3c,
      0x7e, 0xcd, 0x35, 0x25, 0xc9, 0x18, 0x9c, 0x37,
      0x91, 0x09, 0x11, 0x9d, 0x6a, 0xb8, 0x2e, 0x8d,
      0xfb, 0xde, 0x0f, 0xdf, 0x6a, 0x7f, 0x9b, 0x68,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__ParameterValue__TYPE_NAME[] = "rcl_interfaces/msg/ParameterValue";

// Define type names, field names, and default values
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__type[] = "type";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__bool_value[] = "bool_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__integer_value[] = "integer_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__double_value[] = "double_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__string_value[] = "string_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__byte_array_value[] = "byte_array_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__bool_array_value[] = "bool_array_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__integer_array_value[] = "integer_array_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__double_array_value[] = "double_array_value";
static char rcl_interfaces__msg__ParameterValue__FIELD_NAME__string_array_value[] = "string_array_value";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__ParameterValue__FIELDS[] = {
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__bool_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__integer_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__double_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__string_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__byte_array_value, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__bool_array_value, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__integer_array_value, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__double_array_value, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__FIELD_NAME__string_array_value, 18, 18},
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
rcl_interfaces__msg__ParameterValue__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__ParameterValue__TYPE_NAME, 33, 33},
      {rcl_interfaces__msg__ParameterValue__FIELDS, 10, 10},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Used to determine which of the next *_value fields are set.\n"
  "# ParameterType.PARAMETER_NOT_SET indicates that the parameter was not set\n"
  "# (if gotten) or is uninitialized.\n"
  "# Values are enumerated in `ParameterType.msg`.\n"
  "\n"
  "# The type of this parameter, which corresponds to the appropriate field below.\n"
  "uint8 type\n"
  "\n"
  "# \"Variant\" style storage of the parameter value. Only the value corresponding\n"
  "# the type field will have valid information.\n"
  "\n"
  "# Boolean value, can be either true or false.\n"
  "bool bool_value\n"
  "\n"
  "# Integer value ranging from -9,223,372,036,854,775,808 to\n"
  "# 9,223,372,036,854,775,807.\n"
  "int64 integer_value\n"
  "\n"
  "# A double precision floating point value following IEEE 754.\n"
  "float64 double_value\n"
  "\n"
  "# A textual value with no practical length limit.\n"
  "string string_value\n"
  "\n"
  "# An array of bytes, used for non-textual information.\n"
  "byte[] byte_array_value\n"
  "\n"
  "# An array of boolean values.\n"
  "bool[] bool_array_value\n"
  "\n"
  "# An array of 64-bit integer values.\n"
  "int64[] integer_array_value\n"
  "\n"
  "# An array of 64-bit floating point values.\n"
  "float64[] double_array_value\n"
  "\n"
  "# An array of string values.\n"
  "string[] string_array_value";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__ParameterValue__TYPE_NAME, 33, 33},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1098, 1098},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__ParameterValue__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
