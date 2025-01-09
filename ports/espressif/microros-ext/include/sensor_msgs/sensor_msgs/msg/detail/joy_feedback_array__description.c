// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/JoyFeedbackArray.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/joy_feedback_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__JoyFeedbackArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x32, 0x87, 0xc3, 0x2e, 0x1b, 0x68, 0x8c, 0xae,
      0x04, 0x55, 0x5e, 0x46, 0x54, 0x43, 0xdf, 0x3c,
      0xca, 0x7d, 0xae, 0x76, 0xee, 0x4e, 0xbf, 0x85,
      0xc4, 0x65, 0x8d, 0x58, 0x50, 0x37, 0xbc, 0xaa,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "sensor_msgs/msg/detail/joy_feedback__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t sensor_msgs__msg__JoyFeedback__EXPECTED_HASH = {1, {
    0x23, 0x1d, 0xd3, 0x62, 0xf7, 0x1d, 0x6f, 0xc0,
    0x82, 0x72, 0x77, 0x0d, 0x07, 0x12, 0x0a, 0xd5,
    0xfe, 0x58, 0x74, 0xce, 0x2d, 0xba, 0xc7, 0x01,
    0x09, 0xb2, 0x89, 0x86, 0x83, 0x42, 0x90, 0xcd,
  }};
#endif

static char sensor_msgs__msg__JoyFeedbackArray__TYPE_NAME[] = "sensor_msgs/msg/JoyFeedbackArray";
static char sensor_msgs__msg__JoyFeedback__TYPE_NAME[] = "sensor_msgs/msg/JoyFeedback";

// Define type names, field names, and default values
static char sensor_msgs__msg__JoyFeedbackArray__FIELD_NAME__array[] = "array";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__JoyFeedbackArray__FIELDS[] = {
  {
    {sensor_msgs__msg__JoyFeedbackArray__FIELD_NAME__array, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {sensor_msgs__msg__JoyFeedback__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__JoyFeedbackArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {sensor_msgs__msg__JoyFeedback__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__JoyFeedbackArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__JoyFeedbackArray__TYPE_NAME, 32, 32},
      {sensor_msgs__msg__JoyFeedbackArray__FIELDS, 1, 1},
    },
    {sensor_msgs__msg__JoyFeedbackArray__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&sensor_msgs__msg__JoyFeedback__EXPECTED_HASH, sensor_msgs__msg__JoyFeedback__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = sensor_msgs__msg__JoyFeedback__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message publishes values for multiple feedback at once.\n"
  "JoyFeedback[] array";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__JoyFeedbackArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__JoyFeedbackArray__TYPE_NAME, 32, 32},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 83, 83},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__JoyFeedbackArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__JoyFeedbackArray__get_individual_type_description_source(NULL),
    sources[1] = *sensor_msgs__msg__JoyFeedback__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
