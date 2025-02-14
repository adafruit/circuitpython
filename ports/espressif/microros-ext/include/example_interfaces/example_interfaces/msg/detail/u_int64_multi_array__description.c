// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:msg/UInt64MultiArray.idl
// generated code does not contain a copyright notice

#include "example_interfaces/msg/detail/u_int64_multi_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__msg__UInt64MultiArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc0, 0x5a, 0x34, 0x8b, 0xd8, 0x87, 0xaa, 0xc7,
      0x6a, 0x94, 0x7c, 0x90, 0x1f, 0x67, 0xf9, 0x3b,
      0xf9, 0xa7, 0xfa, 0x14, 0xb2, 0x8d, 0xce, 0x02,
      0xeb, 0x49, 0x5d, 0xad, 0xeb, 0x88, 0x42, 0xe4,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "example_interfaces/msg/detail/multi_array_layout__functions.h"
#include "example_interfaces/msg/detail/multi_array_dimension__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t example_interfaces__msg__MultiArrayDimension__EXPECTED_HASH = {1, {
    0xa7, 0x85, 0xcb, 0x98, 0x39, 0xe1, 0x77, 0xe3,
    0xeb, 0x76, 0x02, 0x60, 0x13, 0x9a, 0x91, 0x9f,
    0xec, 0x87, 0x82, 0x1e, 0xdc, 0x33, 0x14, 0xc5,
    0x92, 0xf2, 0x72, 0x5a, 0xbb, 0xf0, 0xbf, 0xcd,
  }};
static const rosidl_type_hash_t example_interfaces__msg__MultiArrayLayout__EXPECTED_HASH = {1, {
    0xba, 0x42, 0xea, 0x30, 0x07, 0x4e, 0x58, 0x26,
    0xa1, 0xe9, 0x1f, 0x70, 0xf3, 0x66, 0x0d, 0xda,
    0x29, 0x96, 0x93, 0x71, 0x69, 0x48, 0x7f, 0x87,
    0x7f, 0xc2, 0x0a, 0x8a, 0x40, 0x2e, 0x2c, 0x27,
  }};
#endif

static char example_interfaces__msg__UInt64MultiArray__TYPE_NAME[] = "example_interfaces/msg/UInt64MultiArray";
static char example_interfaces__msg__MultiArrayDimension__TYPE_NAME[] = "example_interfaces/msg/MultiArrayDimension";
static char example_interfaces__msg__MultiArrayLayout__TYPE_NAME[] = "example_interfaces/msg/MultiArrayLayout";

// Define type names, field names, and default values
static char example_interfaces__msg__UInt64MultiArray__FIELD_NAME__layout[] = "layout";
static char example_interfaces__msg__UInt64MultiArray__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__msg__UInt64MultiArray__FIELDS[] = {
  {
    {example_interfaces__msg__UInt64MultiArray__FIELD_NAME__layout, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__msg__MultiArrayLayout__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__msg__UInt64MultiArray__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription example_interfaces__msg__UInt64MultiArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {example_interfaces__msg__MultiArrayDimension__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__msg__MultiArrayLayout__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__msg__UInt64MultiArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__msg__UInt64MultiArray__TYPE_NAME, 39, 39},
      {example_interfaces__msg__UInt64MultiArray__FIELDS, 2, 2},
    },
    {example_interfaces__msg__UInt64MultiArray__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&example_interfaces__msg__MultiArrayDimension__EXPECTED_HASH, example_interfaces__msg__MultiArrayDimension__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = example_interfaces__msg__MultiArrayDimension__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&example_interfaces__msg__MultiArrayLayout__EXPECTED_HASH, example_interfaces__msg__MultiArrayLayout__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = example_interfaces__msg__MultiArrayLayout__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example of using complex datatypes.\n"
  "# It is not recommended to use directly.\n"
  "# To use a similar datastruct please define a custom message with appropriate semantic meaning.\n"
  "\n"
  "# Please look at the MultiArrayLayout message definition for\n"
  "# documentation on all multiarrays.\n"
  "\n"
  "MultiArrayLayout  layout        # specification of data layout\n"
  "uint64[]          data          # array of data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__msg__UInt64MultiArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__msg__UInt64MultiArray__TYPE_NAME, 39, 39},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 396, 396},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__msg__UInt64MultiArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__msg__UInt64MultiArray__get_individual_type_description_source(NULL),
    sources[1] = *example_interfaces__msg__MultiArrayDimension__get_individual_type_description_source(NULL);
    sources[2] = *example_interfaces__msg__MultiArrayLayout__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
