// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/Range.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/range__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__Range__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb4, 0x2b, 0x62, 0x56, 0x2e, 0x93, 0xcb, 0xfe,
      0x9d, 0x42, 0xb8, 0x2f, 0xe5, 0x99, 0x4d, 0xfa,
      0x3d, 0x63, 0xd7, 0xd5, 0xc9, 0x0a, 0x31, 0x79,
      0x81, 0x70, 0x3f, 0x73, 0x88, 0xad, 0xff, 0x3a,
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

static char sensor_msgs__msg__Range__TYPE_NAME[] = "sensor_msgs/msg/Range";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__Range__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__Range__FIELD_NAME__radiation_type[] = "radiation_type";
static char sensor_msgs__msg__Range__FIELD_NAME__field_of_view[] = "field_of_view";
static char sensor_msgs__msg__Range__FIELD_NAME__min_range[] = "min_range";
static char sensor_msgs__msg__Range__FIELD_NAME__max_range[] = "max_range";
static char sensor_msgs__msg__Range__FIELD_NAME__range[] = "range";
static char sensor_msgs__msg__Range__FIELD_NAME__variance[] = "variance";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__Range__FIELDS[] = {
  {
    {sensor_msgs__msg__Range__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__radiation_type, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__field_of_view, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__min_range, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__max_range, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__range, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Range__FIELD_NAME__variance, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__Range__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
sensor_msgs__msg__Range__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__Range__TYPE_NAME, 21, 21},
      {sensor_msgs__msg__Range__FIELDS, 7, 7},
    },
    {sensor_msgs__msg__Range__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
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
  "# Single range reading from an active ranger that emits energy and reports\n"
  "# one range reading that is valid along an arc at the distance measured.\n"
  "# This message is  not appropriate for laser scanners. See the LaserScan\n"
  "# message if you are working with a laser scanner.\n"
  "#\n"
  "# This message also can represent a fixed-distance (binary) ranger.  This\n"
  "# sensor will have min_range===max_range===distance of detection.\n"
  "# These sensors follow REP 117 and will output -Inf if the object is detected\n"
  "# and +Inf if the object is outside of the detection range.\n"
  "\n"
  "std_msgs/Header header # timestamp in the header is the time the ranger\n"
  "                             # returned the distance reading\n"
  "\n"
  "# Radiation type enums\n"
  "# If you want a value added to this list, send an email to the ros-users list\n"
  "uint8 ULTRASOUND=0\n"
  "uint8 INFRARED=1\n"
  "\n"
  "uint8 radiation_type    # the type of radiation used by the sensor\n"
  "                        # (sound, IR, etc) [enum]\n"
  "\n"
  "float32 field_of_view   # the size of the arc that the distance reading is\n"
  "                        # valid for [rad]\n"
  "                        # the object causing the range reading may have\n"
  "                        # been anywhere within -field_of_view/2 and\n"
  "                        # field_of_view/2 at the measured range.\n"
  "                        # 0 angle corresponds to the x-axis of the sensor.\n"
  "\n"
  "float32 min_range       # minimum range value [m]\n"
  "float32 max_range       # maximum range value [m]\n"
  "                        # Fixed distance rangers require min_range==max_range\n"
  "\n"
  "float32 range           # range data [m]\n"
  "                        # (Note: values < range_min or > range_max should be discarded)\n"
  "                        # Fixed distance rangers only output -Inf or +Inf.\n"
  "                        # -Inf represents a detection within fixed distance.\n"
  "                        # (Detection too close to the sensor to quantify)\n"
  "                        # +Inf represents no detection within the fixed distance.\n"
  "                        # (Object out of range)\n"
  "\n"
  "float32 variance        # variance of the range sensor\n"
  "                        # 0 is interpreted as variance unknown";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__Range__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__Range__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 2124, 2124},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__Range__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__Range__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
