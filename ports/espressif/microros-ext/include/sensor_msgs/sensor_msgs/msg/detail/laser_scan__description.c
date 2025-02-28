// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/LaserScan.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/laser_scan__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__LaserScan__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x64, 0xc1, 0x91, 0x39, 0x80, 0x13, 0xaf, 0x96,
      0x50, 0x9d, 0x51, 0x8d, 0xac, 0x71, 0xd5, 0x16,
      0x4f, 0x93, 0x82, 0x55, 0x3f, 0xce, 0x5c, 0x1f,
      0x8c, 0xca, 0x5b, 0xe7, 0x92, 0x4b, 0xd8, 0x28,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
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
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char sensor_msgs__msg__LaserScan__TYPE_NAME[] = "sensor_msgs/msg/LaserScan";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__LaserScan__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__angle_min[] = "angle_min";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__angle_max[] = "angle_max";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__angle_increment[] = "angle_increment";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__time_increment[] = "time_increment";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__scan_time[] = "scan_time";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__range_min[] = "range_min";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__range_max[] = "range_max";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__ranges[] = "ranges";
static char sensor_msgs__msg__LaserScan__FIELD_NAME__intensities[] = "intensities";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__LaserScan__FIELDS[] = {
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__angle_min, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__angle_max, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__angle_increment, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__time_increment, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__scan_time, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__range_min, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__range_max, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__ranges, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__LaserScan__FIELD_NAME__intensities, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__LaserScan__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__LaserScan__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__LaserScan__TYPE_NAME, 25, 25},
      {sensor_msgs__msg__LaserScan__FIELDS, 10, 10},
    },
    {sensor_msgs__msg__LaserScan__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Single scan from a planar laser range-finder\n"
  "#\n"
  "# If you have another ranging device with different behavior (e.g. a sonar\n"
  "# array), please find or create a different message, since applications\n"
  "# will make fairly laser-specific assumptions about this data\n"
  "\n"
  "std_msgs/Header header # timestamp in the header is the acquisition time of\n"
  "                             # the first ray in the scan.\n"
  "                             #\n"
  "                             # in frame frame_id, angles are measured around\n"
  "                             # the positive Z axis (counterclockwise, if Z is up)\n"
  "                             # with zero angle being forward along the x axis\n"
  "\n"
  "float32 angle_min            # start angle of the scan [rad]\n"
  "float32 angle_max            # end angle of the scan [rad]\n"
  "float32 angle_increment      # angular distance between measurements [rad]\n"
  "\n"
  "float32 time_increment       # time between measurements [seconds] - if your scanner\n"
  "                             # is moving, this will be used in interpolating position\n"
  "                             # of 3d points\n"
  "float32 scan_time            # time between scans [seconds]\n"
  "\n"
  "float32 range_min            # minimum range value [m]\n"
  "float32 range_max            # maximum range value [m]\n"
  "\n"
  "float32[] ranges             # range data [m]\n"
  "                             # (Note: values < range_min or > range_max should be discarded)\n"
  "float32[] intensities        # intensity data [device-specific units].  If your\n"
  "                             # device does not provide intensities, please leave\n"
  "                             # the array empty.";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__LaserScan__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__LaserScan__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1593, 1593},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__LaserScan__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__LaserScan__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
