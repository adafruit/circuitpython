// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/Image.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/image__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__Image__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xd3, 0x1d, 0x41, 0xa9, 0xa4, 0xc4, 0xbc, 0x8e,
      0xae, 0x9b, 0xe7, 0x57, 0xb0, 0xbe, 0xed, 0x30,
      0x65, 0x64, 0xf7, 0x52, 0x6c, 0x88, 0xea, 0x6a,
      0x45, 0x88, 0xfb, 0x95, 0x82, 0x52, 0x7d, 0x47,
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

static char sensor_msgs__msg__Image__TYPE_NAME[] = "sensor_msgs/msg/Image";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__Image__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__Image__FIELD_NAME__height[] = "height";
static char sensor_msgs__msg__Image__FIELD_NAME__width[] = "width";
static char sensor_msgs__msg__Image__FIELD_NAME__encoding[] = "encoding";
static char sensor_msgs__msg__Image__FIELD_NAME__is_bigendian[] = "is_bigendian";
static char sensor_msgs__msg__Image__FIELD_NAME__step[] = "step";
static char sensor_msgs__msg__Image__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__Image__FIELDS[] = {
  {
    {sensor_msgs__msg__Image__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__height, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__width, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__encoding, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__is_bigendian, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__step, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__Image__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__Image__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
sensor_msgs__msg__Image__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__Image__TYPE_NAME, 21, 21},
      {sensor_msgs__msg__Image__FIELDS, 7, 7},
    },
    {sensor_msgs__msg__Image__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
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
  "# This message contains an uncompressed image\n"
  "# (0, 0) is at top-left corner of image\n"
  "\n"
  "std_msgs/Header header # Header timestamp should be acquisition time of image\n"
  "                             # Header frame_id should be optical frame of camera\n"
  "                             # origin of frame should be optical center of cameara\n"
  "                             # +x should point to the right in the image\n"
  "                             # +y should point down in the image\n"
  "                             # +z should point into to plane of the image\n"
  "                             # If the frame_id here and the frame_id of the CameraInfo\n"
  "                             # message associated with the image conflict\n"
  "                             # the behavior is undefined\n"
  "\n"
  "uint32 height                # image height, that is, number of rows\n"
  "uint32 width                 # image width, that is, number of columns\n"
  "\n"
  "# The legal values for encoding are in file include/sensor_msgs/image_encodings.hpp\n"
  "# If you want to standardize a new string format, join\n"
  "# ros-users@lists.ros.org and send an email proposing a new encoding.\n"
  "\n"
  "string encoding       # Encoding of pixels -- channel meaning, ordering, size\n"
  "                      # taken from the list of strings in include/sensor_msgs/image_encodings.hpp\n"
  "\n"
  "uint8 is_bigendian    # is this data bigendian?\n"
  "uint32 step           # Full row length in bytes\n"
  "uint8[] data          # actual matrix data, size is (step * rows)";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__Image__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__Image__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1451, 1451},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__Image__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__Image__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
