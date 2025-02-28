// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from type_description_interfaces:msg/Field.idl
// generated code does not contain a copyright notice

#include "type_description_interfaces/msg/detail/field__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__msg__Field__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc0, 0xb0, 0x13, 0x79, 0xcd, 0x42, 0x26, 0x28,
      0x12, 0x85, 0xcc, 0xaf, 0x6b, 0xe4, 0x66, 0x53,
      0x96, 0x8f, 0x85, 0x5f, 0x7c, 0x5e, 0x41, 0x61,
      0x4f, 0xf5, 0xd7, 0xa8, 0x54, 0xef, 0xef, 0x7c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "type_description_interfaces/msg/detail/field_type__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t type_description_interfaces__msg__FieldType__EXPECTED_HASH = {1, {
    0xa7, 0x0b, 0x6d, 0xd9, 0x19, 0x64, 0x5a, 0x03,
    0xa3, 0x58, 0x6f, 0x7f, 0x82, 0x1d, 0xef, 0xbc,
    0x88, 0x6e, 0xa3, 0xe5, 0x31, 0xa1, 0xd9, 0x5c,
    0xc0, 0xf3, 0x80, 0xa3, 0x97, 0x3c, 0xca, 0xa6,
  }};
#endif

static char type_description_interfaces__msg__Field__TYPE_NAME[] = "type_description_interfaces/msg/Field";
static char type_description_interfaces__msg__FieldType__TYPE_NAME[] = "type_description_interfaces/msg/FieldType";

// Define type names, field names, and default values
static char type_description_interfaces__msg__Field__FIELD_NAME__name[] = "name";
static char type_description_interfaces__msg__Field__FIELD_NAME__type[] = "type";
static char type_description_interfaces__msg__Field__FIELD_NAME__default_value[] = "default_value";

static rosidl_runtime_c__type_description__Field type_description_interfaces__msg__Field__FIELDS[] = {
  {
    {type_description_interfaces__msg__Field__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__Field__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__Field__FIELD_NAME__default_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription type_description_interfaces__msg__Field__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__msg__Field__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__msg__Field__TYPE_NAME, 37, 37},
      {type_description_interfaces__msg__Field__FIELDS, 3, 3},
    },
    {type_description_interfaces__msg__Field__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&type_description_interfaces__msg__FieldType__EXPECTED_HASH, type_description_interfaces__msg__FieldType__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = type_description_interfaces__msg__FieldType__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Represents a single field in a type.\n"
  "\n"
  "# Name of the field.\n"
  "string name\n"
  "# Type of the field, including details about the type like length, nested name, etc.\n"
  "FieldType type\n"
  "# Literal default value of the field as a string, as it appeared in the original\n"
  "# message description file, whether that be .msg/.srv/.action or .idl.\n"
  "string default_value";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__msg__Field__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__msg__Field__TYPE_NAME, 37, 37},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 346, 346},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__msg__Field__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__msg__Field__get_individual_type_description_source(NULL),
    sources[1] = *type_description_interfaces__msg__FieldType__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
