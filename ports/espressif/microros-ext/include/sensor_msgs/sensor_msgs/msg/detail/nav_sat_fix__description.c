// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/NavSatFix.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/nav_sat_fix__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__NavSatFix__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x62, 0x22, 0x3a, 0xb3, 0xfe, 0x21, 0x0a, 0x15,
      0x97, 0x60, 0x21, 0xda, 0x7a, 0xfd, 0xdc, 0x9e,
      0x20, 0x0d, 0xc9, 0xec, 0x75, 0x23, 0x1c, 0x1b,
      0x6a, 0x55, 0x7f, 0xc5, 0x98, 0xa6, 0x54, 0x04,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "sensor_msgs/msg/detail/nav_sat_status__functions.h"
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
static const rosidl_type_hash_t sensor_msgs__msg__NavSatStatus__EXPECTED_HASH = {1, {
    0xd1, 0xed, 0x3b, 0xef, 0xa6, 0x28, 0xe0, 0x95,
    0x71, 0xbd, 0x27, 0x3b, 0x88, 0x8b, 0xa1, 0xc1,
    0xfd, 0x18, 0x7c, 0x9a, 0x5e, 0x00, 0x06, 0xb3,
    0x85, 0xd7, 0xe5, 0xe9, 0x09, 0x5a, 0x32, 0x04,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char sensor_msgs__msg__NavSatFix__TYPE_NAME[] = "sensor_msgs/msg/NavSatFix";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char sensor_msgs__msg__NavSatStatus__TYPE_NAME[] = "sensor_msgs/msg/NavSatStatus";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__status[] = "status";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__latitude[] = "latitude";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__longitude[] = "longitude";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__altitude[] = "altitude";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__position_covariance[] = "position_covariance";
static char sensor_msgs__msg__NavSatFix__FIELD_NAME__position_covariance_type[] = "position_covariance_type";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__NavSatFix__FIELDS[] = {
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__NavSatStatus__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__latitude, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__longitude, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__altitude, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__position_covariance, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      9,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatFix__FIELD_NAME__position_covariance_type, 24, 24},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__NavSatFix__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__NavSatStatus__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__NavSatFix__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__NavSatFix__TYPE_NAME, 25, 25},
      {sensor_msgs__msg__NavSatFix__FIELDS, 7, 7},
    },
    {sensor_msgs__msg__NavSatFix__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__NavSatStatus__EXPECTED_HASH, sensor_msgs__msg__NavSatStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = sensor_msgs__msg__NavSatStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Navigation Satellite fix for any Global Navigation Satellite System\n"
  "#\n"
  "# Specified using the WGS 84 reference ellipsoid\n"
  "\n"
  "# header.stamp specifies the ROS time for this measurement (the\n"
  "#        corresponding satellite time may be reported using the\n"
  "#        sensor_msgs/TimeReference message).\n"
  "#\n"
  "# header.frame_id is the frame of reference reported by the satellite\n"
  "#        receiver, usually the location of the antenna.  This is a\n"
  "#        Euclidean frame relative to the vehicle, not a reference\n"
  "#        ellipsoid.\n"
  "std_msgs/Header header\n"
  "\n"
  "# Satellite fix status information.\n"
  "NavSatStatus status\n"
  "\n"
  "# Latitude [degrees]. Positive is north of equator; negative is south.\n"
  "float64 latitude\n"
  "\n"
  "# Longitude [degrees]. Positive is east of prime meridian; negative is west.\n"
  "float64 longitude\n"
  "\n"
  "# Altitude [m]. Positive is above the WGS 84 ellipsoid\n"
  "# (quiet NaN if no altitude is available).\n"
  "float64 altitude\n"
  "\n"
  "# Position covariance [m^2] defined relative to a tangential plane\n"
  "# through the reported position. The components are East, North, and\n"
  "# Up (ENU), in row-major order.\n"
  "#\n"
  "# Beware: this coordinate system exhibits singularities at the poles.\n"
  "float64[9] position_covariance\n"
  "\n"
  "# If the covariance of the fix is known, fill it in completely. If the\n"
  "# GPS receiver provides the variance of each measurement, put them\n"
  "# along the diagonal. If only Dilution of Precision is available,\n"
  "# estimate an approximate covariance from that.\n"
  "\n"
  "uint8 COVARIANCE_TYPE_UNKNOWN = 0\n"
  "uint8 COVARIANCE_TYPE_APPROXIMATED = 1\n"
  "uint8 COVARIANCE_TYPE_DIAGONAL_KNOWN = 2\n"
  "uint8 COVARIANCE_TYPE_KNOWN = 3\n"
  "\n"
  "uint8 position_covariance_type";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__NavSatFix__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__NavSatFix__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1605, 1605},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__NavSatFix__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__NavSatFix__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *sensor_msgs__msg__NavSatStatus__get_individual_type_description_source(NULL);
    sources[3] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
