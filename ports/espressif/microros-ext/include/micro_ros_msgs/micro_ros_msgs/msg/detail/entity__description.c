// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from micro_ros_msgs:msg/Entity.idl
// generated code does not contain a copyright notice

#include "micro_ros_msgs/msg/detail/entity__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_type_hash_t *
micro_ros_msgs__msg__Entity__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf6, 0x6f, 0x49, 0xcb, 0xcc, 0x49, 0xd8, 0x60,
      0x2b, 0xed, 0xa0, 0xff, 0xc3, 0xb6, 0x00, 0x6e,
      0x5a, 0xdd, 0xf5, 0xa7, 0x8e, 0x9d, 0x7d, 0x62,
      0xe8, 0x31, 0xb2, 0x7c, 0x91, 0x2a, 0x3e, 0x08,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char micro_ros_msgs__msg__Entity__TYPE_NAME[] = "micro_ros_msgs/msg/Entity";

// Define type names, field names, and default values
static char micro_ros_msgs__msg__Entity__FIELD_NAME__entity_type[] = "entity_type";
static char micro_ros_msgs__msg__Entity__FIELD_NAME__name[] = "name";
static char micro_ros_msgs__msg__Entity__FIELD_NAME__types[] = "types";

static rosidl_runtime_c__type_description__Field micro_ros_msgs__msg__Entity__FIELDS[] = {
  {
    {micro_ros_msgs__msg__Entity__FIELD_NAME__entity_type, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {micro_ros_msgs__msg__Entity__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING,
      0,
      256,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {micro_ros_msgs__msg__Entity__FIELD_NAME__types, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOUNDED_STRING_UNBOUNDED_SEQUENCE,
      0,
      256,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
micro_ros_msgs__msg__Entity__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {micro_ros_msgs__msg__Entity__TYPE_NAME, 25, 25},
      {micro_ros_msgs__msg__Entity__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "byte PUBLISHER=0\n"
  "byte SUBSCRIBER=1\n"
  "byte SERVICE_SERVER=2\n"
  "byte SERVICE_CLIENT=3\n"
  "\n"
  "byte entity_type\n"
  "string<=256 name\n"
  "string<=256[] types";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
micro_ros_msgs__msg__Entity__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {micro_ros_msgs__msg__Entity__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 134, 134},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
micro_ros_msgs__msg__Entity__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *micro_ros_msgs__msg__Entity__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
