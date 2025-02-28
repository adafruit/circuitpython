// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from shape_msgs:msg/SolidPrimitive.idl
// generated code does not contain a copyright notice

#include "shape_msgs/msg/detail/solid_primitive__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_type_hash_t *
shape_msgs__msg__SolidPrimitive__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x28, 0x02, 0xa1, 0x51, 0x90, 0xaa, 0xdc, 0x3f,
      0x49, 0x65, 0x84, 0xdf, 0x4b, 0x0b, 0x4c, 0x58,
      0x24, 0xd8, 0xa0, 0xb3, 0x1a, 0xae, 0xf8, 0x39,
      0xfa, 0xa7, 0x5b, 0xc3, 0x4d, 0xda, 0x38, 0xac,
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

static char shape_msgs__msg__SolidPrimitive__TYPE_NAME[] = "shape_msgs/msg/SolidPrimitive";
static char geometry_msgs__msg__Point32__TYPE_NAME[] = "geometry_msgs/msg/Point32";
static char geometry_msgs__msg__Polygon__TYPE_NAME[] = "geometry_msgs/msg/Polygon";

// Define type names, field names, and default values
static char shape_msgs__msg__SolidPrimitive__FIELD_NAME__type[] = "type";
static char shape_msgs__msg__SolidPrimitive__FIELD_NAME__dimensions[] = "dimensions";
static char shape_msgs__msg__SolidPrimitive__FIELD_NAME__polygon[] = "polygon";

static rosidl_runtime_c__type_description__Field shape_msgs__msg__SolidPrimitive__FIELDS[] = {
  {
    {shape_msgs__msg__SolidPrimitive__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {shape_msgs__msg__SolidPrimitive__FIELD_NAME__dimensions, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_BOUNDED_SEQUENCE,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {shape_msgs__msg__SolidPrimitive__FIELD_NAME__polygon, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Polygon__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription shape_msgs__msg__SolidPrimitive__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
shape_msgs__msg__SolidPrimitive__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {shape_msgs__msg__SolidPrimitive__TYPE_NAME, 29, 29},
      {shape_msgs__msg__SolidPrimitive__FIELDS, 3, 3},
    },
    {shape_msgs__msg__SolidPrimitive__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
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
  "# Defines box, sphere, cylinder, cone and prism.\n"
  "# All shapes are defined to have their bounding boxes centered around 0,0,0.\n"
  "\n"
  "uint8 BOX=1\n"
  "uint8 SPHERE=2\n"
  "uint8 CYLINDER=3\n"
  "uint8 CONE=4\n"
  "uint8 PRISM=5\n"
  "\n"
  "# The type of the shape\n"
  "uint8 type\n"
  "\n"
  "# The dimensions of the shape\n"
  "float64[<=3] dimensions  # At no point will dimensions have a length > 3.\n"
  "\n"
  "# The meaning of the shape dimensions: each constant defines the index in the 'dimensions' array.\n"
  "\n"
  "# For type BOX, the X, Y, and Z dimensions are the length of the corresponding sides of the box.\n"
  "uint8 BOX_X=0\n"
  "uint8 BOX_Y=1\n"
  "uint8 BOX_Z=2\n"
  "\n"
  "# For the SPHERE type, only one component is used, and it gives the radius of the sphere.\n"
  "uint8 SPHERE_RADIUS=0\n"
  "\n"
  "# For the CYLINDER and CONE types, the center line is oriented along the Z axis.\n"
  "# Therefore the CYLINDER_HEIGHT (CONE_HEIGHT) component of dimensions gives the\n"
  "# height of the cylinder (cone).\n"
  "# The CYLINDER_RADIUS (CONE_RADIUS) component of dimensions gives the radius of\n"
  "# the base of the cylinder (cone).\n"
  "# Cone and cylinder primitives are defined to be circular. The tip of the cone\n"
  "# is pointing up, along +Z axis.\n"
  "\n"
  "uint8 CYLINDER_HEIGHT=0\n"
  "uint8 CYLINDER_RADIUS=1\n"
  "\n"
  "uint8 CONE_HEIGHT=0\n"
  "uint8 CONE_RADIUS=1\n"
  "\n"
  "# For the type PRISM, the center line is oriented along Z axis.\n"
  "# The PRISM_HEIGHT component of dimensions gives the\n"
  "# height of the prism.\n"
  "# The polygon defines the Z axis centered base of the prism.\n"
  "# The prism is constructed by extruding the base in +Z and -Z\n"
  "# directions by half of the PRISM_HEIGHT\n"
  "# Only x and y fields of the points are used in the polygon.\n"
  "# Points of the polygon are ordered counter-clockwise.\n"
  "\n"
  "uint8 PRISM_HEIGHT=0\n"
  "geometry_msgs/Polygon polygon";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
shape_msgs__msg__SolidPrimitive__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {shape_msgs__msg__SolidPrimitive__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1676, 1676},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
shape_msgs__msg__SolidPrimitive__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *shape_msgs__msg__SolidPrimitive__get_individual_type_description_source(NULL),
    sources[1] = *geometry_msgs__msg__Point32__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Polygon__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
