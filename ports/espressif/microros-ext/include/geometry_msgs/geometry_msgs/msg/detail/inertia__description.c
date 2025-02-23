// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from geometry_msgs:msg/Inertia.idl
// generated code does not contain a copyright notice

#include "geometry_msgs/msg/detail/inertia__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_geometry_msgs
const rosidl_type_hash_t *
geometry_msgs__msg__Inertia__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x2d, 0xdd, 0x5d, 0xab, 0x5c, 0x34, 0x78, 0x25,
      0xba, 0x2e, 0x56, 0xc8, 0x95, 0xdd, 0xcc, 0xfd,
      0x0b, 0x8e, 0xfe, 0x53, 0xae, 0x93, 0x1b, 0xf6,
      0x7f, 0x90, 0x55, 0x29, 0x93, 0x0b, 0x4b, 0xd7,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/vector3__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
#endif

static char geometry_msgs__msg__Inertia__TYPE_NAME[] = "geometry_msgs/msg/Inertia";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";

// Define type names, field names, and default values
static char geometry_msgs__msg__Inertia__FIELD_NAME__m[] = "m";
static char geometry_msgs__msg__Inertia__FIELD_NAME__com[] = "com";
static char geometry_msgs__msg__Inertia__FIELD_NAME__ixx[] = "ixx";
static char geometry_msgs__msg__Inertia__FIELD_NAME__ixy[] = "ixy";
static char geometry_msgs__msg__Inertia__FIELD_NAME__ixz[] = "ixz";
static char geometry_msgs__msg__Inertia__FIELD_NAME__iyy[] = "iyy";
static char geometry_msgs__msg__Inertia__FIELD_NAME__iyz[] = "iyz";
static char geometry_msgs__msg__Inertia__FIELD_NAME__izz[] = "izz";

static rosidl_runtime_c__type_description__Field geometry_msgs__msg__Inertia__FIELDS[] = {
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__m, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__com, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__ixx, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__ixy, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__ixz, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__iyy, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__iyz, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Inertia__FIELD_NAME__izz, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription geometry_msgs__msg__Inertia__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
geometry_msgs__msg__Inertia__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {geometry_msgs__msg__Inertia__TYPE_NAME, 25, 25},
      {geometry_msgs__msg__Inertia__FIELDS, 8, 8},
    },
    {geometry_msgs__msg__Inertia__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Mass [kg]\n"
  "float64 m\n"
  "\n"
  "# Center of mass [m]\n"
  "geometry_msgs/Vector3 com\n"
  "\n"
  "# Inertia Tensor [kg-m^2]\n"
  "#     | ixx ixy ixz |\n"
  "# I = | ixy iyy iyz |\n"
  "#     | ixz iyz izz |\n"
  "float64 ixx\n"
  "float64 ixy\n"
  "float64 ixz\n"
  "float64 iyy\n"
  "float64 iyz\n"
  "float64 izz";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
geometry_msgs__msg__Inertia__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {geometry_msgs__msg__Inertia__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 235, 235},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
geometry_msgs__msg__Inertia__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *geometry_msgs__msg__Inertia__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
