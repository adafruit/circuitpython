// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/RegionOfInterest.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/region_of_interest__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__RegionOfInterest__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xad, 0x16, 0xbc, 0xba, 0x5f, 0x91, 0x31, 0xdc,
      0xdb, 0xa6, 0xfb, 0xde, 0xd1, 0x9f, 0x72, 0x6f,
      0x54, 0x40, 0xe3, 0xc5, 0x13, 0xb4, 0xfb, 0x58,
      0x6d, 0xd3, 0x02, 0x7e, 0xee, 0xd8, 0xab, 0xb1,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__RegionOfInterest__TYPE_NAME[] = "sensor_msgs/msg/RegionOfInterest";

// Define type names, field names, and default values
static char sensor_msgs__msg__RegionOfInterest__FIELD_NAME__x_offset[] = "x_offset";
static char sensor_msgs__msg__RegionOfInterest__FIELD_NAME__y_offset[] = "y_offset";
static char sensor_msgs__msg__RegionOfInterest__FIELD_NAME__height[] = "height";
static char sensor_msgs__msg__RegionOfInterest__FIELD_NAME__width[] = "width";
static char sensor_msgs__msg__RegionOfInterest__FIELD_NAME__do_rectify[] = "do_rectify";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__RegionOfInterest__FIELDS[] = {
  {
    {sensor_msgs__msg__RegionOfInterest__FIELD_NAME__x_offset, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__RegionOfInterest__FIELD_NAME__y_offset, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__RegionOfInterest__FIELD_NAME__height, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__RegionOfInterest__FIELD_NAME__width, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__RegionOfInterest__FIELD_NAME__do_rectify, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__RegionOfInterest__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__RegionOfInterest__TYPE_NAME, 32, 32},
      {sensor_msgs__msg__RegionOfInterest__FIELDS, 5, 5},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message is used to specify a region of interest within an image.\n"
  "#\n"
  "# When used to specify the ROI setting of the camera when the image was\n"
  "# taken, the height and width fields should either match the height and\n"
  "# width fields for the associated image; or height = width = 0\n"
  "# indicates that the full resolution image was captured.\n"
  "\n"
  "uint32 x_offset  # Leftmost pixel of the ROI\n"
  "                 # (0 if the ROI includes the left edge of the image)\n"
  "uint32 y_offset  # Topmost pixel of the ROI\n"
  "                 # (0 if the ROI includes the top edge of the image)\n"
  "uint32 height    # Height of ROI\n"
  "uint32 width     # Width of ROI\n"
  "\n"
  "# True if a distinct rectified ROI should be calculated from the \"raw\"\n"
  "# ROI in this message. Typically this should be False if the full image\n"
  "# is captured (ROI not used), and True if a subwindow is captured (ROI\n"
  "# used).\n"
  "bool do_rectify";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__RegionOfInterest__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__RegionOfInterest__TYPE_NAME, 32, 32},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 872, 872},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__RegionOfInterest__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__RegionOfInterest__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
