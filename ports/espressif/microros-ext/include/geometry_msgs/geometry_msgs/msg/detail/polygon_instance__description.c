// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/PolygonInstance.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/polygon_instance__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__PolygonInstance__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xfa, 0x1c, 0xb3, 0xdc, 0x77, 0x43, 0x29, 0x86,
      0x52, 0x58, 0xaf, 0xef, 0x74, 0xf6, 0x5b, 0x05,
      0x53, 0xd4, 0x87, 0x51, 0x0c, 0x6d, 0x0f, 0x93,
      0xba, 0x38, 0xcc, 0x32, 0xd6, 0x2a, 0xc0, 0xe5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/polygon__functions.h"
#include "geometry_msgs/msg/detail/point32__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Point32__EXPECTED_HASH = {1, {
    0x2f, 0xc4, 0xdb, 0x7c, 0xae, 0x16, 0xa4, 0x58,
    0x2c, 0x79, 0xa5, 0x6b, 0x66, 0x17, 0x3a, 0x8d,
    0x48, 0xd5, 0x2c, 0x7d, 0xc5, 0x20, 0xdd, 0xc5,
    0x5a, 0x0d, 0x4b, 0xcf, 0x2a, 0x4b, 0xfd, 0xbc,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Polygon__EXPECTED_HASH = {1, {
    0x37, 0x82, 0xf9, 0xf0, 0xbf, 0x04, 0x49, 0x64,
    0xd6, 0x92, 0xd6, 0xc0, 0x17, 0xd7, 0x05, 0xe3,
    0x76, 0x11, 0xaf, 0xb1, 0xf0, 0xbf, 0x6a, 0x9d,
    0xee, 0x24, 0x8a, 0x7d, 0xda, 0x0f, 0x78, 0x4a,
  }};
#endif

static char geometry_msgs__msg__PolygonInstance__TYPE_NAME[] = "geometry_msgs/msg/PolygonInstance";
static char geometry_msgs__msg__Point32__TYPE_NAME[] = "geometry_msgs/msg/Point32";
static char geometry_msgs__msg__Polygon__TYPE_NAME[] = "geometry_msgs/msg/Polygon";

// Define type names, field names, and default values
static char geometry_msgs__msg__PolygonInstance__FIELD_NAME__polygon[] = "polygon";
static char geometry_msgs__msg__PolygonInstance__FIELD_NAME__id[] = "id";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__PolygonInstance__FIELDS[] = {
  {
    {geometry_msgs__msg__PolygonInstance__FIELD_NAME__polygon, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Polygon__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__PolygonInstance__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription geometry_msgs__msg__PolygonInstance__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Point32__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Polygon__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
geometry_msgs__msg__PolygonInstance__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__PolygonInstance__TYPE_NAME, 33, 33},
      {geometry_msgs__msg__PolygonInstance__FIELDS, 2, 2},
    },
    {geometry_msgs__msg__PolygonInstance__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Point32__EXPECTED_HASH, geometry_msgs__msg__Point32__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Point32__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Polygon__EXPECTED_HASH, geometry_msgs__msg__Polygon__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Polygon__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A specification of a polygon where the first and last points are assumed to be connected\n"
  "# It includes a unique identification field for disambiguating multiple instances\n"
  "\n"
  "geometry_msgs/Polygon polygon\n"
  "int64 id";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__PolygonInstance__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__PolygonInstance__TYPE_NAME, 33, 33},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 213, 213},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__PolygonInstance__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__PolygonInstance__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Point32__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Polygon__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
