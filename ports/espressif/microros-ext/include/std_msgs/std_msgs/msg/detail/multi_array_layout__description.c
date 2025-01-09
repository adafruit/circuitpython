// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from std_msgs:msg/MultiArrayLayout.idl
// generated code does not contain a copyright notice

#include "std_msgs/msg/detail/multi_array_layout__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_std_msgs
const rosidl_type_hash_t *
std_msgs__msg__MultiArrayLayout__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4c, 0x66, 0xe6, 0xf7, 0x8e, 0x74, 0x0a, 0xc1,
      0x03, 0xa9, 0x4c, 0xf6, 0x32, 0x59, 0xf9, 0x68,
      0xe4, 0x8c, 0x61, 0x7e, 0x76, 0x99, 0xe8, 0x29,
      0xb6, 0x3c, 0x21, 0xa5, 0xcb, 0x50, 0xda, 0xc6,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "std_msgs/msg/detail/multi_array_dimension__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t std_msgs__msg__MultiArrayDimension__EXPECTED_HASH = {1, {
    0x5e, 0x77, 0x3a, 0x60, 0xa4, 0xc7, 0xfc, 0x8a,
    0x54, 0x98, 0x5f, 0x30, 0x7c, 0x78, 0x37, 0xaa,
    0x29, 0x94, 0x25, 0x2a, 0x12, 0x6c, 0x30, 0x19,
    0x57, 0xa2, 0x4e, 0x31, 0x28, 0x2c, 0x9c, 0xbe,
  }};
#endif

static char std_msgs__msg__MultiArrayLayout__TYPE_NAME[] = "std_msgs/msg/MultiArrayLayout";
static char std_msgs__msg__MultiArrayDimension__TYPE_NAME[] = "std_msgs/msg/MultiArrayDimension";

// Define type names, field names, and default values
static char std_msgs__msg__MultiArrayLayout__FIELD_NAME__dim[] = "dim";
static char std_msgs__msg__MultiArrayLayout__FIELD_NAME__data_offset[] = "data_offset";

static rosidl_runtime_c__type_description__Field std_msgs__msg__MultiArrayLayout__FIELDS[] = {
  {
    {std_msgs__msg__MultiArrayLayout__FIELD_NAME__dim, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {std_msgs__msg__MultiArrayDimension__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__MultiArrayLayout__FIELD_NAME__data_offset, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription std_msgs__msg__MultiArrayLayout__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {std_msgs__msg__MultiArrayDimension__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
std_msgs__msg__MultiArrayLayout__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {std_msgs__msg__MultiArrayLayout__TYPE_NAME, 29, 29},
      {std_msgs__msg__MultiArrayLayout__FIELDS, 2, 2},
    },
    {std_msgs__msg__MultiArrayLayout__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&std_msgs__msg__MultiArrayDimension__EXPECTED_HASH, std_msgs__msg__MultiArrayDimension__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = std_msgs__msg__MultiArrayDimension__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This was originally provided as an example message.\n"
  "# It is deprecated as of Foxy\n"
  "# It is recommended to create your own semantically meaningful message.\n"
  "# However if you would like to continue using this please use the equivalent in example_msgs.\n"
  "\n"
  "# The multiarray declares a generic multi-dimensional array of a\n"
  "# particular data type.  Dimensions are ordered from outer most\n"
  "# to inner most.\n"
  "#\n"
  "# Accessors should ALWAYS be written in terms of dimension stride\n"
  "# and specified outer-most dimension first.\n"
  "#\n"
  "# multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]\n"
  "#\n"
  "# A standard, 3-channel 640x480 image with interleaved color channels\n"
  "# would be specified as:\n"
  "#\n"
  "# dim[0].label  = \"height\"\n"
  "# dim[0].size   = 480\n"
  "# dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)\n"
  "# dim[1].label  = \"width\"\n"
  "# dim[1].size   = 640\n"
  "# dim[1].stride = 3*640 = 1920\n"
  "# dim[2].label  = \"channel\"\n"
  "# dim[2].size   = 3\n"
  "# dim[2].stride = 3\n"
  "#\n"
  "# multiarray(i,j,k) refers to the ith row, jth column, and kth channel.\n"
  "\n"
  "MultiArrayDimension[] dim # Array of dimension properties\n"
  "uint32 data_offset        # padding bytes at front of data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
std_msgs__msg__MultiArrayLayout__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {std_msgs__msg__MultiArrayLayout__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1159, 1159},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
std_msgs__msg__MultiArrayLayout__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *std_msgs__msg__MultiArrayLayout__get_individual_type_description_source(NULL),
    sources[1] = *std_msgs__msg__MultiArrayDimension__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
