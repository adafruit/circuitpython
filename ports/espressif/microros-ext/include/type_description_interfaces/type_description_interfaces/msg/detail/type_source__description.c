// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from type_description_interfaces:msg/TypeSource.idl
// generated code does not contain a copyright notice

#include "type_description_interfaces/msg/detail/type_source__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__msg__TypeSource__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xfa, 0xea, 0xec, 0x75, 0x96, 0xc0, 0x4e, 0xcf,
      0x5b, 0x6e, 0x99, 0xad, 0x22, 0x5e, 0x4c, 0x7c,
      0xbb, 0x99, 0x7a, 0xd5, 0x43, 0x5f, 0x79, 0x35,
      0x26, 0xfb, 0x39, 0x84, 0xd0, 0x11, 0xaa, 0xe5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char type_description_interfaces__msg__TypeSource__TYPE_NAME[] = "type_description_interfaces/msg/TypeSource";

// Define type names, field names, and default values
static char type_description_interfaces__msg__TypeSource__FIELD_NAME__type_name[] = "type_name";
static char type_description_interfaces__msg__TypeSource__FIELD_NAME__encoding[] = "encoding";
static char type_description_interfaces__msg__TypeSource__FIELD_NAME__raw_file_contents[] = "raw_file_contents";

static rosidl_runtime_c__type_description__Field type_description_interfaces__msg__TypeSource__FIELDS[] = {
  {
    {type_description_interfaces__msg__TypeSource__FIELD_NAME__type_name, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeSource__FIELD_NAME__encoding, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeSource__FIELD_NAME__raw_file_contents, 17, 17},
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
type_description_interfaces__msg__TypeSource__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
      {type_description_interfaces__msg__TypeSource__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Represents the original source of a ROS 2 interface definition.\n"
  "\n"
  "# ROS interface type name, in PACKAGE/NAMESPACE/TYPENAME format.\n"
  "string type_name\n"
  "\n"
  "# The type of the original source file, typically matching the file extension.\n"
  "# Well-known encodings: \"idl\", \"msg\", \"srv\", \"action\", \"dynamic\", \"implicit\".\n"
  "# \"dynamic\" specifies a type created programmatically by a user, thus having no source.\n"
  "# \"implicit\" specifies a type created automatically as a subtype of a\n"
  "# complex type (service or action) - such as the request message for a service.\n"
  "# Implicit types will have no contents, the full source will be available on the parent srv/action.\n"
  "string encoding\n"
  "\n"
  "# Dumped contents of the interface definition source file.\n"
  "# If `encoding` is \"dynamic\" or \"implicit\", this field will be empty.\n"
  "string raw_file_contents";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__msg__TypeSource__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 816, 816},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__msg__TypeSource__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__msg__TypeSource__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
