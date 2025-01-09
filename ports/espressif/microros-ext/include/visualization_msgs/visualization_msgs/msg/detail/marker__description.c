// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:msg/Marker.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/msg/detail/marker__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__Marker__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x45, 0xb1, 0x3c, 0xcf, 0x79, 0x1f, 0x22, 0x59,
      0x62, 0xbf, 0x74, 0xe7, 0x46, 0xf9, 0x64, 0x45,
      0x18, 0x85, 0x5d, 0x78, 0x3a, 0x6f, 0x42, 0xba,
      0x0c, 0xc1, 0x4f, 0xde, 0x2b, 0x4f, 0x3c, 0xe0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/vector3__functions.h"
#include "sensor_msgs/msg/detail/compressed_image__functions.h"
#include "builtin_interfaces/msg/detail/duration__functions.h"
#include "geometry_msgs/msg/detail/pose__functions.h"
#include "std_msgs/msg/detail/color_rgba__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "visualization_msgs/msg/detail/uv_coordinate__functions.h"
#include "visualization_msgs/msg/detail/mesh_file__functions.h"
#include "geometry_msgs/msg/detail/point__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "geometry_msgs/msg/detail/quaternion__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Duration__EXPECTED_HASH = {1, {
    0xe8, 0xd0, 0x09, 0xf6, 0x59, 0x81, 0x6f, 0x75,
    0x8b, 0x75, 0x33, 0x4e, 0xe1, 0xa9, 0xca, 0x5b,
    0x5c, 0x0b, 0x85, 0x98, 0x43, 0x26, 0x1f, 0x14,
    0xc7, 0xf9, 0x37, 0x34, 0x95, 0x99, 0xd9, 0x3b,
  }};
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Point__EXPECTED_HASH = {1, {
    0x69, 0x63, 0x08, 0x48, 0x42, 0xa9, 0xb0, 0x44,
    0x94, 0xd6, 0xb2, 0x94, 0x1d, 0x11, 0x44, 0x47,
    0x08, 0xd8, 0x92, 0xda, 0x2f, 0x4b, 0x09, 0x84,
    0x3b, 0x9c, 0x43, 0xf4, 0x2a, 0x7f, 0x68, 0x81,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Pose__EXPECTED_HASH = {1, {
    0xd5, 0x01, 0x95, 0x4e, 0x94, 0x76, 0xce, 0xa2,
    0x99, 0x69, 0x84, 0xe8, 0x12, 0x05, 0x4b, 0x68,
    0x02, 0x6a, 0xe0, 0xbf, 0xae, 0x78, 0x9d, 0x9a,
    0x10, 0xb2, 0x3d, 0xaf, 0x35, 0xcc, 0x90, 0xfa,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Quaternion__EXPECTED_HASH = {1, {
    0x8a, 0x76, 0x5f, 0x66, 0x77, 0x8c, 0x8f, 0xf7,
    0xc8, 0xab, 0x94, 0xaf, 0xcc, 0x59, 0x0a, 0x2e,
    0xd5, 0x32, 0x5a, 0x1d, 0x9a, 0x07, 0x6f, 0xff,
    0xf3, 0x8f, 0xbc, 0xe3, 0x6f, 0x45, 0x86, 0x84,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Vector3__EXPECTED_HASH = {1, {
    0xcc, 0x12, 0xfe, 0x83, 0xe4, 0xc0, 0x27, 0x19,
    0xf1, 0xce, 0x80, 0x70, 0xbf, 0xd1, 0x4a, 0xec,
    0xd4, 0x0f, 0x75, 0xa9, 0x66, 0x96, 0xa6, 0x7a,
    0x2a, 0x1f, 0x37, 0xf7, 0xdb, 0xb0, 0x76, 0x5d,
  }};
static const rosidl_type_hash_t sensor_msgs__msg__CompressedImage__EXPECTED_HASH = {1, {
    0x15, 0x64, 0x07, 0x71, 0x53, 0x15, 0x71, 0x18,
    0x5e, 0x2e, 0xfc, 0x8a, 0x10, 0x0b, 0xaf, 0x92,
    0x39, 0x61, 0xa4, 0xd1, 0x5d, 0x55, 0x69, 0x65,
    0x2e, 0x6c, 0xb6, 0x69, 0x1e, 0x8e, 0x37, 0x1a,
  }};
static const rosidl_type_hash_t std_msgs__msg__ColorRGBA__EXPECTED_HASH = {1, {
    0x77, 0xa7, 0xa5, 0xb9, 0xae, 0x47, 0x73, 0x06,
    0x09, 0x76, 0x65, 0x10, 0x6e, 0x04, 0x13, 0xba,
    0x74, 0x44, 0x02, 0x45, 0xb1, 0xf3, 0xd0, 0xc6,
    0xd6, 0x40, 0x5f, 0xe5, 0xc7, 0x81, 0x3f, 0xe8,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
static const rosidl_type_hash_t visualization_msgs__msg__MeshFile__EXPECTED_HASH = {1, {
    0x77, 0x10, 0xec, 0xe1, 0x5a, 0x14, 0x8f, 0xb7,
    0xc9, 0xb5, 0x46, 0x36, 0x4c, 0xfb, 0x21, 0x5b,
    0xb0, 0x60, 0x98, 0x08, 0x7b, 0xd6, 0x39, 0x4f,
    0xe5, 0xb7, 0x3a, 0x49, 0x35, 0x08, 0xf8, 0xc4,
  }};
static const rosidl_type_hash_t visualization_msgs__msg__UVCoordinate__EXPECTED_HASH = {1, {
    0xf2, 0x7f, 0x7e, 0xd2, 0x1f, 0xe3, 0x60, 0xc6,
    0x06, 0x69, 0x44, 0xf8, 0x56, 0xb8, 0x01, 0xa0,
    0xc0, 0xd1, 0xe9, 0x4e, 0x81, 0x5b, 0x68, 0x86,
    0x44, 0x4b, 0x42, 0xd9, 0x0b, 0x19, 0x6a, 0x26,
  }};
#endif

static char visualization_msgs__msg__Marker__TYPE_NAME[] = "visualization_msgs/msg/Marker";
static char builtin_interfaces__msg__Duration__TYPE_NAME[] = "builtin_interfaces/msg/Duration";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Pose__TYPE_NAME[] = "geometry_msgs/msg/Pose";
static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";
static char sensor_msgs__msg__CompressedImage__TYPE_NAME[] = "sensor_msgs/msg/CompressedImage";
static char std_msgs__msg__ColorRGBA__TYPE_NAME[] = "std_msgs/msg/ColorRGBA";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";
static char visualization_msgs__msg__MeshFile__TYPE_NAME[] = "visualization_msgs/msg/MeshFile";
static char visualization_msgs__msg__UVCoordinate__TYPE_NAME[] = "visualization_msgs/msg/UVCoordinate";

// Define type names, field names, and default values
static char visualization_msgs__msg__Marker__FIELD_NAME__header[] = "header";
static char visualization_msgs__msg__Marker__FIELD_NAME__ns[] = "ns";
static char visualization_msgs__msg__Marker__FIELD_NAME__id[] = "id";
static char visualization_msgs__msg__Marker__FIELD_NAME__type[] = "type";
static char visualization_msgs__msg__Marker__FIELD_NAME__action[] = "action";
static char visualization_msgs__msg__Marker__FIELD_NAME__pose[] = "pose";
static char visualization_msgs__msg__Marker__FIELD_NAME__scale[] = "scale";
static char visualization_msgs__msg__Marker__FIELD_NAME__color[] = "color";
static char visualization_msgs__msg__Marker__FIELD_NAME__lifetime[] = "lifetime";
static char visualization_msgs__msg__Marker__FIELD_NAME__frame_locked[] = "frame_locked";
static char visualization_msgs__msg__Marker__FIELD_NAME__points[] = "points";
static char visualization_msgs__msg__Marker__FIELD_NAME__colors[] = "colors";
static char visualization_msgs__msg__Marker__FIELD_NAME__texture_resource[] = "texture_resource";
static char visualization_msgs__msg__Marker__FIELD_NAME__texture[] = "texture";
static char visualization_msgs__msg__Marker__FIELD_NAME__uv_coordinates[] = "uv_coordinates";
static char visualization_msgs__msg__Marker__FIELD_NAME__text[] = "text";
static char visualization_msgs__msg__Marker__FIELD_NAME__mesh_resource[] = "mesh_resource";
static char visualization_msgs__msg__Marker__FIELD_NAME__mesh_file[] = "mesh_file";
static char visualization_msgs__msg__Marker__FIELD_NAME__mesh_use_embedded_materials[] = "mesh_use_embedded_materials";

static rosidl_runtime_c__type_description__Field visualization_msgs__msg__Marker__FIELDS[] = {
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__ns, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__action, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__pose, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Pose__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__scale, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__color, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__ColorRGBA__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__lifetime, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__frame_locked, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__points, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__colors, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {std_msgs__msg__ColorRGBA__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__texture_resource, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__texture, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {sensor_msgs__msg__CompressedImage__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__uv_coordinates, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {visualization_msgs__msg__UVCoordinate__TYPE_NAME, 35, 35},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__text, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__mesh_resource, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__mesh_file, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {visualization_msgs__msg__MeshFile__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__FIELD_NAME__mesh_use_embedded_materials, 27, 27},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription visualization_msgs__msg__Marker__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Pose__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Vector3__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__CompressedImage__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__ColorRGBA__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MeshFile__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__UVCoordinate__TYPE_NAME, 35, 35},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__Marker__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__msg__Marker__TYPE_NAME, 29, 29},
      {visualization_msgs__msg__Marker__FIELDS, 19, 19},
    },
    {visualization_msgs__msg__Marker__REFERENCED_TYPE_DESCRIPTIONS, 11, 11},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Duration__EXPECTED_HASH, builtin_interfaces__msg__Duration__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Duration__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Vector3__EXPECTED_HASH, geometry_msgs__msg__Vector3__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = geometry_msgs__msg__Vector3__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&sensor_msgs__msg__CompressedImage__EXPECTED_HASH, sensor_msgs__msg__CompressedImage__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = sensor_msgs__msg__CompressedImage__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__ColorRGBA__EXPECTED_HASH, std_msgs__msg__ColorRGBA__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = std_msgs__msg__ColorRGBA__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[8].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MeshFile__EXPECTED_HASH, visualization_msgs__msg__MeshFile__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = visualization_msgs__msg__MeshFile__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__UVCoordinate__EXPECTED_HASH, visualization_msgs__msg__UVCoordinate__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = visualization_msgs__msg__UVCoordinate__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# See:\n"
  "#  - http://www.ros.org/wiki/rviz/DisplayTypes/Marker\n"
  "#  - http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes\n"
  "#\n"
  "# for more information on using this message with rviz.\n"
  "\n"
  "int32 ARROW=0\n"
  "int32 CUBE=1\n"
  "int32 SPHERE=2\n"
  "int32 CYLINDER=3\n"
  "int32 LINE_STRIP=4\n"
  "int32 LINE_LIST=5\n"
  "int32 CUBE_LIST=6\n"
  "int32 SPHERE_LIST=7\n"
  "int32 POINTS=8\n"
  "int32 TEXT_VIEW_FACING=9\n"
  "int32 MESH_RESOURCE=10\n"
  "int32 TRIANGLE_LIST=11\n"
  "int32 ARROW_STRIP=12\n"
  "\n"
  "int32 ADD=0\n"
  "int32 MODIFY=0\n"
  "int32 DELETE=2\n"
  "int32 DELETEALL=3\n"
  "\n"
  "# Header for timestamp and frame id.\n"
  "std_msgs/Header header\n"
  "# Namespace in which to place the object.\n"
  "# Used in conjunction with id to create a unique name for the object.\n"
  "string ns\n"
  "# Object ID used in conjunction with the namespace for manipulating and deleting the object later.\n"
  "int32 id\n"
  "# Type of object.\n"
  "int32 type\n"
  "# Action to take; one of:\n"
  "#  - 0 add/modify an object\n"
  "#  - 1 (deprecated)\n"
  "#  - 2 deletes an object (with the given ns and id)\n"
  "#  - 3 deletes all objects (or those with the given ns if any)\n"
  "int32 action\n"
  "# Pose of the object with respect the frame_id specified in the header.\n"
  "geometry_msgs/Pose pose\n"
  "# Scale of the object; 1,1,1 means default (usually 1 meter square).\n"
  "geometry_msgs/Vector3 scale\n"
  "# Color of the object; in the range: [0.0-1.0]\n"
  "std_msgs/ColorRGBA color\n"
  "# How long the object should last before being automatically deleted.\n"
  "# 0 indicates forever.\n"
  "builtin_interfaces/Duration lifetime\n"
  "# If this marker should be frame-locked, i.e. retransformed into its frame every timestep.\n"
  "bool frame_locked\n"
  "\n"
  "# Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ARROW_STRIP, etc.)\n"
  "geometry_msgs/Point[] points\n"
  "# Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, etc.)\n"
  "# The number of colors provided must either be 0 or equal to the number of points provided.\n"
  "# NOTE: alpha is not yet used\n"
  "std_msgs/ColorRGBA[] colors\n"
  "\n"
  "# Texture resource is a special URI that can either reference a texture file in\n"
  "# a format acceptable to (resource retriever)[https://docs.ros.org/en/rolling/p/resource_retriever/]\n"
  "# or an embedded texture via a string matching the format:\n"
  "#   \"embedded://texture_name\"\n"
  "string texture_resource\n"
  "# An image to be loaded into the rendering engine as the texture for this marker.\n"
  "# This will be used iff texture_resource is set to embedded.\n"
  "sensor_msgs/CompressedImage texture\n"
  "# Location of each vertex within the texture; in the range: [0.0-1.0]\n"
  "UVCoordinate[] uv_coordinates\n"
  "\n"
  "# Only used for text markers\n"
  "string text\n"
  "\n"
  "# Only used for MESH_RESOURCE markers.\n"
  "# Similar to texture_resource, mesh_resource uses resource retriever to load a mesh.\n"
  "# Optionally, a mesh file can be sent in-message via the mesh_file field. If doing so,\n"
  "# use the following format for mesh_resource:\n"
  "#   \"embedded://mesh_name\"\n"
  "string mesh_resource\n"
  "MeshFile mesh_file\n"
  "bool mesh_use_embedded_materials";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__Marker__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__msg__Marker__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 2861, 2861},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__Marker__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[12];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 12, 12};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__msg__Marker__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[7] = *sensor_msgs__msg__CompressedImage__get_individual_type_description_source(NULL);
    sources[8] = *std_msgs__msg__ColorRGBA__get_individual_type_description_source(NULL);
    sources[9] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    sources[10] = *visualization_msgs__msg__MeshFile__get_individual_type_description_source(NULL);
    sources[11] = *visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
