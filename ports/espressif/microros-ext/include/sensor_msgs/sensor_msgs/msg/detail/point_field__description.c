// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/PointField.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/point_field__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__PointField__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5c, 0x6a, 0x47, 0x50, 0x72, 0x8c, 0x2b, 0xcf,
      0xbb, 0xf7, 0x03, 0x72, 0x25, 0xb2, 0x0b, 0x02,
      0xd4, 0x42, 0x96, 0x34, 0x73, 0x21, 0x46, 0xb7,
      0x42, 0xde, 0xe1, 0x72, 0x66, 0x37, 0xef, 0x01,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__PointField__TYPE_NAME[] = "sensor_msgs/msg/PointField";

// Define type names, field names, and default values
static char sensor_msgs__msg__PointField__FIELD_NAME__name[] = "name";
static char sensor_msgs__msg__PointField__FIELD_NAME__offset[] = "offset";
static char sensor_msgs__msg__PointField__FIELD_NAME__datatype[] = "datatype";
static char sensor_msgs__msg__PointField__FIELD_NAME__count[] = "count";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__PointField__FIELDS[] = {
  {
    {sensor_msgs__msg__PointField__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointField__FIELD_NAME__offset, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointField__FIELD_NAME__datatype, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__PointField__FIELD_NAME__count, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__PointField__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__PointField__TYPE_NAME, 26, 26},
      {sensor_msgs__msg__PointField__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message holds the description of one point entry in the\n"
  "# PointCloud2 message format.\n"
  "uint8 INT8    = 1\n"
  "uint8 UINT8   = 2\n"
  "uint8 INT16   = 3\n"
  "uint8 UINT16  = 4\n"
  "uint8 INT32   = 5\n"
  "uint8 UINT32  = 6\n"
  "uint8 FLOAT32 = 7\n"
  "uint8 FLOAT64 = 8\n"
  "\n"
  "# Common PointField names are x, y, z, intensity, rgb, rgba\n"
  "string name      # Name of field\n"
  "uint32 offset    # Offset from start of point struct\n"
  "uint8  datatype  # Datatype enumeration, see above\n"
  "uint32 count     # How many elements in the field";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__PointField__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__PointField__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 485, 485},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__PointField__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__PointField__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
