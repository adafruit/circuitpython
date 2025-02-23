// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/PointCloud.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/point_cloud__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__PointCloud__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x61, 0x45, 0x93, 0xdf, 0x71, 0xd3, 0xc2, 0xb9,
      0xbd, 0x46, 0x04, 0xa7, 0x1b, 0x75, 0x0f, 0xd2,
      0x18, 0xf0, 0xd6, 0x5c, 0x04, 0x5e, 0xa9, 0x88,
      0xb7, 0x13, 0x71, 0x94, 0x55, 0xa6, 0x5b, 0x3b,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "sensor_msgs/msg/detail/channel_float32__functions.h"
#include "geometry_msgs/msg/detail/point32__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Point32__EXPECTED_HASH = {1, {
    0x2f, 0xc4, 0xdb, 0x7c, 0xae, 0x16, 0xa4, 0x58,
    0x2c, 0x79, 0xa5, 0x6b, 0x66, 0x17, 0x3a, 0x8d,
    0x48, 0xd5, 0x2c, 0x7d, 0xc5, 0x20, 0xdd, 0xc5,
    0x5a, 0x0d, 0x4b, 0xcf, 0x2a, 0x4b, 0xfd, 0xbc,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__ChannelFloat32__EXPECTED_HASH = {1, {
    0x92, 0x66, 0x54, 0x37, 0xdd, 0xf3, 0x93, 0x46,
    0xf4, 0xba, 0x39, 0xee, 0x32, 0xe6, 0x48, 0x39,
    0x06, 0x05, 0xb6, 0x33, 0xcc, 0x07, 0x7d, 0x40,
    0xf4, 0xbd, 0x4d, 0x7b, 0x58, 0xaf, 0x6c, 0xd4,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char sensor_msgs__msg__PointCloud__TYPE_NAME[] = "sensor_msgs/msg/PointCloud";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point32__TYPE_NAME[] = "geometry_msgs/msg/Point32";
static char sensor_msgs__msg__ChannelFloat32__TYPE_NAME[] = "sensor_msgs/msg/ChannelFloat32";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__PointCloud__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__PointCloud__FIELD_NAME__points[] = "points";
static char sensor_msgs__msg__PointCloud__FIELD_NAME__channels[] = "channels";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__PointCloud__FIELDS[] = {
  {
    {sensor_msgs__msg__PointCloud__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointCloud__FIELD_NAME__points, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {geometry_msgs__msg__Point32__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointCloud__FIELD_NAME__channels, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {sensor_msgs__msg__ChannelFloat32__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__PointCloud__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Point32__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__ChannelFloat32__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__PointCloud__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__PointCloud__TYPE_NAME, 26, 26},
      {sensor_msgs__msg__PointCloud__FIELDS, 3, 3},
    },
    {sensor_msgs__msg__PointCloud__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point32__EXPECTED_HASH, geometry_msgs__msg__Point32__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point32__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__ChannelFloat32__EXPECTED_HASH, sensor_msgs__msg__ChannelFloat32__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = sensor_msgs__msg__ChannelFloat32__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "## THIS MESSAGE IS DEPRECATED AS OF FOXY\n"
  "## Please use sensor_msgs/PointCloud2\n"
  "\n"
  "# This message holds a collection of 3d points, plus optional additional\n"
  "# information about each point.\n"
  "\n"
  "# Time of sensor data acquisition, coordinate frame ID.\n"
  "std_msgs/Header header\n"
  "\n"
  "# Array of 3d points. Each Point32 should be interpreted as a 3d point\n"
  "# in the frame given in the header.\n"
  "geometry_msgs/Point32[] points\n"
  "\n"
  "# Each channel should have the same number of elements as points array,\n"
  "# and the data in each channel should correspond 1:1 with each point.\n"
  "# Channel names in common practice are listed in ChannelFloat32.msg.\n"
  "ChannelFloat32[] channels";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__PointCloud__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__PointCloud__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 642, 642},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__PointCloud__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__PointCloud__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point32__get_individual_type_description_source(NULL);
    sources[3] = *sensor_msgs__msg__ChannelFloat32__get_individual_type_description_source(NULL);
    sources[4] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
