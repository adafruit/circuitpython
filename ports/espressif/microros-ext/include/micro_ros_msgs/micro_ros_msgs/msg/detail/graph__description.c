// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from micro_ros_msgs:msg/Graph.idl
// generated code does not contain a copyright notice

#include "micro_ros_msgs/msg/detail/graph__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_type_hash_t *
micro_ros_msgs__msg__Graph__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x43, 0xaa, 0xc5, 0x88, 0x23, 0x93, 0x27, 0x50,
      0x60, 0x7f, 0xe3, 0xb8, 0xdd, 0x34, 0x2c, 0xa1,
      0x68, 0xbe, 0xf4, 0x81, 0xb4, 0x08, 0x89, 0xf8,
      0x93, 0xcb, 0xea, 0xa3, 0xd5, 0xc4, 0xc7, 0x37,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "micro_ros_msgs/msg/detail/node__functions.h"
#include "micro_ros_msgs/msg/detail/entity__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t micro_ros_msgs__msg__Entity__EXPECTED_HASH = {1, {
    0xf6, 0x6f, 0x49, 0xcb, 0xcc, 0x49, 0xd8, 0x60,
    0x2b, 0xed, 0xa0, 0xff, 0xc3, 0xb6, 0x00, 0x6e,
    0x5a, 0xdd, 0xf5, 0xa7, 0x8e, 0x9d, 0x7d, 0x62,
    0xe8, 0x31, 0xb2, 0x7c, 0x91, 0x2a, 0x3e, 0x08,
  }};
static const rosidl_type_hash_t micro_ros_msgs__msg__Node__EXPECTED_HASH = {1, {
    0xb1, 0x4c, 0xdb, 0xce, 0x7f, 0x1a, 0x1f, 0xe7,
    0xfa, 0x56, 0x1a, 0x53, 0x16, 0xdd, 0x45, 0x44,
    0x5d, 0x7a, 0x76, 0xc3, 0x99, 0x8b, 0x29, 0x44,
    0x06, 0x86, 0xa3, 0x33, 0xbf, 0x10, 0x22, 0x1d,
  }};
#endif

static char micro_ros_msgs__msg__Graph__TYPE_NAME[] = "micro_ros_msgs/msg/Graph";
static char micro_ros_msgs__msg__Entity__TYPE_NAME[] = "micro_ros_msgs/msg/Entity";
static char micro_ros_msgs__msg__Node__TYPE_NAME[] = "micro_ros_msgs/msg/Node";

// Define type names, field names, and default values
static char micro_ros_msgs__msg__Graph__FIELD_NAME__nodes[] = "nodes";

static rosidl_runtime_c__type_description__Field micro_ros_msgs__msg__Graph__FIELDS[] = {
  {
    {micro_ros_msgs__msg__Graph__FIELD_NAME__nodes, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {micro_ros_msgs__msg__Node__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription micro_ros_msgs__msg__Graph__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {micro_ros_msgs__msg__Entity__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {micro_ros_msgs__msg__Node__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
micro_ros_msgs__msg__Graph__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {micro_ros_msgs__msg__Graph__TYPE_NAME, 24, 24},
      {micro_ros_msgs__msg__Graph__FIELDS, 1, 1},
    },
    {micro_ros_msgs__msg__Graph__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&micro_ros_msgs__msg__Entity__EXPECTED_HASH, micro_ros_msgs__msg__Entity__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = micro_ros_msgs__msg__Entity__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&micro_ros_msgs__msg__Node__EXPECTED_HASH, micro_ros_msgs__msg__Node__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = micro_ros_msgs__msg__Node__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "Node[] nodes";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
micro_ros_msgs__msg__Graph__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {micro_ros_msgs__msg__Graph__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 13, 13},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
micro_ros_msgs__msg__Graph__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *micro_ros_msgs__msg__Graph__get_individual_type_description_source(NULL),
    sources[1] = *micro_ros_msgs__msg__Entity__get_individual_type_description_source(NULL);
    sources[2] = *micro_ros_msgs__msg__Node__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
