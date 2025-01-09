// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/JoyFeedback.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/joy_feedback__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__JoyFeedback__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x23, 0x1d, 0xd3, 0x62, 0xf7, 0x1d, 0x6f, 0xc0,
      0x82, 0x72, 0x77, 0x0d, 0x07, 0x12, 0x0a, 0xd5,
      0xfe, 0x58, 0x74, 0xce, 0x2d, 0xba, 0xc7, 0x01,
      0x09, 0xb2, 0x89, 0x86, 0x83, 0x42, 0x90, 0xcd,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__JoyFeedback__TYPE_NAME[] = "sensor_msgs/msg/JoyFeedback";

// Define type names, field names, and default values
static char sensor_msgs__msg__JoyFeedback__FIELD_NAME__type[] = "type";
static char sensor_msgs__msg__JoyFeedback__FIELD_NAME__id[] = "id";
static char sensor_msgs__msg__JoyFeedback__FIELD_NAME__intensity[] = "intensity";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__JoyFeedback__FIELDS[] = {
  {
    {sensor_msgs__msg__JoyFeedback__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__JoyFeedback__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__JoyFeedback__FIELD_NAME__intensity, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__JoyFeedback__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__JoyFeedback__TYPE_NAME, 27, 27},
      {sensor_msgs__msg__JoyFeedback__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Declare of the type of feedback\n"
  "uint8 TYPE_LED    = 0\n"
  "uint8 TYPE_RUMBLE = 1\n"
  "uint8 TYPE_BUZZER = 2\n"
  "\n"
  "uint8 type\n"
  "\n"
  "# This will hold an id number for each type of each feedback.\n"
  "# Example, the first led would be id=0, the second would be id=1\n"
  "uint8 id\n"
  "\n"
  "# Intensity of the feedback, from 0.0 to 1.0, inclusive.  If device is\n"
  "# actually binary, driver should treat 0<=x<0.5 as off, 0.5<=x<=1 as on.\n"
  "float32 intensity";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__JoyFeedback__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__JoyFeedback__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 412, 412},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__JoyFeedback__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__JoyFeedback__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
