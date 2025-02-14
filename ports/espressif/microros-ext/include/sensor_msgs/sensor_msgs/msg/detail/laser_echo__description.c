// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/LaserEcho.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/laser_echo__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__LaserEcho__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x0f, 0xbc, 0x05, 0xa0, 0xdb, 0x7d, 0x37, 0xfe,
      0x52, 0xc0, 0xf0, 0x37, 0x53, 0x56, 0xdb, 0x55,
      0xda, 0x00, 0x46, 0xf7, 0xef, 0x5b, 0xd2, 0x7c,
      0xa6, 0xb3, 0x4b, 0xd0, 0x58, 0x2b, 0xc9, 0x52,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__LaserEcho__TYPE_NAME[] = "sensor_msgs/msg/LaserEcho";

// Define type names, field names, and default values
static char sensor_msgs__msg__LaserEcho__FIELD_NAME__echoes[] = "echoes";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__LaserEcho__FIELDS[] = {
  {
    {sensor_msgs__msg__LaserEcho__FIELD_NAME__echoes, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__LaserEcho__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__LaserEcho__TYPE_NAME, 25, 25},
      {sensor_msgs__msg__LaserEcho__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message is a submessage of MultiEchoLaserScan and is not intended\n"
  "# to be used separately.\n"
  "\n"
  "float32[] echoes  # Multiple values of ranges or intensities.\n"
  "                  # Each array represents data from the same angle increment.";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__LaserEcho__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__LaserEcho__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 239, 239},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__LaserEcho__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__LaserEcho__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
