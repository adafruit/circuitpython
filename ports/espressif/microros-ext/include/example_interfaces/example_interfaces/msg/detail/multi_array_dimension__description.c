// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/MultiArrayDimension.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/multi_array_dimension__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__MultiArrayDimension__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa7, 0x85, 0xcb, 0x98, 0x39, 0xe1, 0x77, 0xe3,
      0xeb, 0x76, 0x02, 0x60, 0x13, 0x9a, 0x91, 0x9f,
      0xec, 0x87, 0x82, 0x1e, 0xdc, 0x33, 0x14, 0xc5,
      0x92, 0xf2, 0x72, 0x5a, 0xbb, 0xf0, 0xbf, 0xcd,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char example_interfaces__msg__MultiArrayDimension__TYPE_NAME[] = "example_interfaces/msg/MultiArrayDimension";

// Define type names, field names, and default values
static char example_interfaces__msg__MultiArrayDimension__FIELD_NAME__label[] = "label";
static char example_interfaces__msg__MultiArrayDimension__FIELD_NAME__size[] = "size";
static char example_interfaces__msg__MultiArrayDimension__FIELD_NAME__stride[] = "stride";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__MultiArrayDimension__FIELDS[] = {
  {
    {example_interfaces__msg__MultiArrayDimension__FIELD_NAME__label, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__msg__MultiArrayDimension__FIELD_NAME__size, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__msg__MultiArrayDimension__FIELD_NAME__stride, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__MultiArrayDimension__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__MultiArrayDimension__TYPE_NAME, 42, 42},
      {example_interfaces__msg__MultiArrayDimension__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example of using complex datatypes.\n"
  "# It is not recommended to use directly.\n"
  "# To use a similar datastruct please define a custom message with appropriate semantic meaning.\n"
  "\n"
  "string label   # label of given dimension\n"
  "uint32 size    # size of given dimension (in type units)\n"
  "uint32 stride  # stride of given dimension";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__MultiArrayDimension__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__MultiArrayDimension__TYPE_NAME, 42, 42},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 329, 329},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__MultiArrayDimension__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__MultiArrayDimension__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
