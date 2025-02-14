// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from micro_ros_msgs:msg/Node.idl
// generated code does not contain a copyright notice

#include "micro_ros_msgs/msg/detail/node__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_type_hash_t *
micro_ros_msgs__msg__Node__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb1, 0x4c, 0xdb, 0xce, 0x7f, 0x1a, 0x1f, 0xe7,
      0xfa, 0x56, 0x1a, 0x53, 0x16, 0xdd, 0x45, 0x44,
      0x5d, 0x7a, 0x76, 0xc3, 0x99, 0x8b, 0x29, 0x44,
      0x06, 0x86, 0xa3, 0x33, 0xbf, 0x10, 0x22, 0x1d,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "micro_ros_msgs/msg/detail/entity__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t micro_ros_msgs__msg__Entity__EXPECTED_HASH = {1, {
    0xf6, 0x6f, 0x49, 0xcb, 0xcc, 0x49, 0xd8, 0x60,
    0x2b, 0xed, 0xa0, 0xff, 0xc3, 0xb6, 0x00, 0x6e,
    0x5a, 0xdd, 0xf5, 0xa7, 0x8e, 0x9d, 0x7d, 0x62,
    0xe8, 0x31, 0xb2, 0x7c, 0x91, 0x2a, 0x3e, 0x08,
  }};
#endif

static char micro_ros_msgs__msg__Node__TYPE_NAME[] = "micro_ros_msgs/msg/Node";
static char micro_ros_msgs__msg__Entity__TYPE_NAME[] = "micro_ros_msgs/msg/Entity";

// Define type names, field names, and default values
static char micro_ros_msgs__msg__Node__FIELD_NAME__node_namespace[] = "node_namespace";
static char micro_ros_msgs__msg__Node__FIELD_NAME__node_name[] = "node_name";
static char micro_ros_msgs__msg__Node__FIELD_NAME__entities[] = "entities";

static rosidl_runtime_c__type_description__Field micro_ros_msgs__msg__Node__FIELDS[] = {
  {
    {micro_ros_msgs__msg__Node__FIELD_NAME__node_namespace, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      256,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {micro_ros_msgs__msg__Node__FIELD_NAME__node_name, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      256,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {micro_ros_msgs__msg__Node__FIELD_NAME__entities, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {micro_ros_msgs__msg__Entity__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription micro_ros_msgs__msg__Node__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {micro_ros_msgs__msg__Entity__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
micro_ros_msgs__msg__Node__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {micro_ros_msgs__msg__Node__TYPE_NAME, 23, 23},
      {micro_ros_msgs__msg__Node__FIELDS, 3, 3},
    },
    {micro_ros_msgs__msg__Node__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&micro_ros_msgs__msg__Entity__EXPECTED_HASH, micro_ros_msgs__msg__Entity__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = micro_ros_msgs__msg__Entity__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string<=256 node_namespace\n"
  "string<=256 node_name\n"
  "Entity[] entities";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
micro_ros_msgs__msg__Node__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {micro_ros_msgs__msg__Node__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 67, 67},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
micro_ros_msgs__msg__Node__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *micro_ros_msgs__msg__Node__get_individual_type_description_source(NULL),
    sources[1] = *micro_ros_msgs__msg__Entity__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
