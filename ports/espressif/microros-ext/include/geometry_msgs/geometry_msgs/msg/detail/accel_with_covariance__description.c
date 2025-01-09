// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/AccelWithCovariance.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/accel_with_covariance__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__AccelWithCovariance__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x23, 0x0d, 0x51, 0xbd, 0x53, 0xbc, 0x36, 0xf2,
      0x60, 0x57, 0x4e, 0x73, 0xb4, 0x29, 0x41, 0xce,
      0xfe, 0x44, 0x68, 0x47, 0x53, 0x48, 0x0b, 0x6f,
      0xc3, 0x30, 0xc0, 0x32, 0xc5, 0xdb, 0x59, 0x97,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/accel__functions.h"
#include "geometry_msgs/msg/detail/vector3__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Accel__EXPECTED_HASH = {1, {
    0xdc, 0x44, 0x82, 0x43, 0xde, 0xd9, 0xb1, 0xfc,
    0xbc, 0xca, 0x24, 0xab, 0xa0, 0xc2, 0x2f, 0x01,
    0x3d, 0xae, 0x06, 0xc3, 0x54, 0xba, 0x2d, 0x84,
    0x95, 0x71, 0xc0, 0xa2, 0xa3, 0xf5, 0x7c, 0xa0,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
#endif

static char geometry_msgs__msg__AccelWithCovariance__TYPE_NAME[] = "geometry_msgs/msg/AccelWithCovariance";
static char geometry_msgs__msg__Accel__TYPE_NAME[] = "geometry_msgs/msg/Accel";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";

// Define type names, field names, and default values
static char geometry_msgs__msg__AccelWithCovariance__FIELD_NAME__accel[] = "accel";
static char geometry_msgs__msg__AccelWithCovariance__FIELD_NAME__covariance[] = "covariance";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__AccelWithCovariance__FIELDS[] = {
  {
    {geometry_msgs__msg__AccelWithCovariance__FIELD_NAME__accel, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Accel__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__AccelWithCovariance__FIELD_NAME__covariance, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      36,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription geometry_msgs__msg__AccelWithCovariance__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Accel__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
geometry_msgs__msg__AccelWithCovariance__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__AccelWithCovariance__TYPE_NAME, 37, 37},
      {geometry_msgs__msg__AccelWithCovariance__FIELDS, 2, 2},
    },
    {geometry_msgs__msg__AccelWithCovariance__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Accel__EXPECTED_HASH, geometry_msgs__msg__Accel__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Accel__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This expresses acceleration in free space with uncertainty.\n"
  "\n"
  "Accel accel\n"
  "\n"
  "# Row-major representation of the 6x6 covariance matrix\n"
  "# The orientation parameters use a fixed-axis representation.\n"
  "# In order, the parameters are:\n"
  "# (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)\n"
  "float64[36] covariance";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__AccelWithCovariance__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__AccelWithCovariance__TYPE_NAME, 37, 37},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 330, 330},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__AccelWithCovariance__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__AccelWithCovariance__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Accel__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
