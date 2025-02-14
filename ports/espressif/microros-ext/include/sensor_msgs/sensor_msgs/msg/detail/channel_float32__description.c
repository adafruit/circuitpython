// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/ChannelFloat32.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/channel_float32__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__ChannelFloat32__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x92, 0x66, 0x54, 0x37, 0xdd, 0xf3, 0x93, 0x46,
      0xf4, 0xba, 0x39, 0xee, 0x32, 0xe6, 0x48, 0x39,
      0x06, 0x05, 0xb6, 0x33, 0xcc, 0x07, 0x7d, 0x40,
      0xf4, 0xbd, 0x4d, 0x7b, 0x58, 0xaf, 0x6c, 0xd4,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__ChannelFloat32__TYPE_NAME[] = "sensor_msgs/msg/ChannelFloat32";

// Define type names, field names, and default values
static char sensor_msgs__msg__ChannelFloat32__FIELD_NAME__name[] = "name";
static char sensor_msgs__msg__ChannelFloat32__FIELD_NAME__values[] = "values";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__ChannelFloat32__FIELDS[] = {
  {
    {sensor_msgs__msg__ChannelFloat32__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__ChannelFloat32__FIELD_NAME__values, 6, 6},
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
sensor_msgs__msg__ChannelFloat32__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__ChannelFloat32__TYPE_NAME, 30, 30},
      {sensor_msgs__msg__ChannelFloat32__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message is used by the PointCloud message to hold optional data\n"
  "# associated with each point in the cloud. The length of the values\n"
  "# array should be the same as the length of the points array in the\n"
  "# PointCloud, and each value should be associated with the corresponding\n"
  "# point.\n"
  "#\n"
  "# Channel names in existing practice include:\n"
  "#   \"u\", \"v\" - row and column (respectively) in the left stereo image.\n"
  "#              This is opposite to usual conventions but remains for\n"
  "#              historical reasons. The newer PointCloud2 message has no\n"
  "#              such problem.\n"
  "#   \"rgb\" - For point clouds produced by color stereo cameras. uint8\n"
  "#           (R,G,B) values packed into the least significant 24 bits,\n"
  "#           in order.\n"
  "#   \"intensity\" - laser or pixel intensity.\n"
  "#   \"distance\"\n"
  "\n"
  "# The channel name should give semantics of the channel (e.g.\n"
  "# \"intensity\" instead of \"value\").\n"
  "string name\n"
  "\n"
  "# The values array should be 1-1 with the elements of the associated\n"
  "# PointCloud.\n"
  "float32[] values";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__ChannelFloat32__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__ChannelFloat32__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1009, 1009},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__ChannelFloat32__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__ChannelFloat32__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
