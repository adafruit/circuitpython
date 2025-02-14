// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/NavSatStatus.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/nav_sat_status__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__NavSatStatus__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xd1, 0xed, 0x3b, 0xef, 0xa6, 0x28, 0xe0, 0x95,
      0x71, 0xbd, 0x27, 0x3b, 0x88, 0x8b, 0xa1, 0xc1,
      0xfd, 0x18, 0x7c, 0x9a, 0x5e, 0x00, 0x06, 0xb3,
      0x85, 0xd7, 0xe5, 0xe9, 0x09, 0x5a, 0x32, 0x04,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char sensor_msgs__msg__NavSatStatus__TYPE_NAME[] = "sensor_msgs/msg/NavSatStatus";

// Define type names, field names, and default values
static char sensor_msgs__msg__NavSatStatus__FIELD_NAME__status[] = "status";
static char sensor_msgs__msg__NavSatStatus__DEFAULT_VALUE__status[] = "-2";
static char sensor_msgs__msg__NavSatStatus__FIELD_NAME__service[] = "service";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__NavSatStatus__FIELDS[] = {
  {
    {sensor_msgs__msg__NavSatStatus__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {sensor_msgs__msg__NavSatStatus__DEFAULT_VALUE__status, 2, 2},
  },
  {
    {sensor_msgs__msg__NavSatStatus__FIELD_NAME__service, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__NavSatStatus__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__NavSatStatus__TYPE_NAME, 28, 28},
      {sensor_msgs__msg__NavSatStatus__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Navigation Satellite fix status for any Global Navigation Satellite System.\n"
  "#\n"
  "# Whether to output an augmented fix is determined by both the fix\n"
  "# type and the last time differential corrections were received.  A\n"
  "# fix is valid when status >= STATUS_FIX.\n"
  "\n"
  "int8 STATUS_UNKNOWN = -2        # status is not yet set\n"
  "int8 STATUS_NO_FIX =  -1        # unable to fix position\n"
  "int8 STATUS_FIX =      0        # unaugmented fix\n"
  "int8 STATUS_SBAS_FIX = 1        # with satellite-based augmentation\n"
  "int8 STATUS_GBAS_FIX = 2        # with ground-based augmentation\n"
  "\n"
  "int8 status -2 # STATUS_UNKNOWN\n"
  "\n"
  "# Bits defining which Global Navigation Satellite System signals were\n"
  "# used by the receiver.\n"
  "\n"
  "uint16 SERVICE_UNKNOWN = 0  # Remember service is a bitfield, so checking (service & SERVICE_UNKNOWN) will not work. Use == instead.\n"
  "uint16 SERVICE_GPS =     1\n"
  "uint16 SERVICE_GLONASS = 2\n"
  "uint16 SERVICE_COMPASS = 4      # includes BeiDou.\n"
  "uint16 SERVICE_GALILEO = 8\n"
  "\n"
  "uint16 service";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__NavSatStatus__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__NavSatStatus__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 964, 964},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__NavSatStatus__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__NavSatStatus__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
