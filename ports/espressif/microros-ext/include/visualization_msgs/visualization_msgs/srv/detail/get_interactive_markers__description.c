// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:srv/GetInteractiveMarkers.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__srv__GetInteractiveMarkers__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xbf, 0xdf, 0xaa, 0x86, 0x1f, 0x4a, 0x84, 0x22,
      0xb4, 0xec, 0x3b, 0xd0, 0x2c, 0x66, 0x81, 0xe4,
      0xa4, 0x5a, 0x83, 0xbd, 0xc2, 0x91, 0x3d, 0x8f,
      0xac, 0xdf, 0x92, 0x3d, 0x8a, 0xd1, 0x37, 0x6a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x7f, 0xa7, 0xe0, 0x83, 0x0e, 0x3c, 0xbe, 0x9c,
      0x15, 0x92, 0x7a, 0x29, 0xfd, 0xc0, 0x9f, 0xed,
      0xcd, 0x9a, 0x86, 0x75, 0x7e, 0x37, 0x5f, 0x29,
      0xce, 0x6b, 0x07, 0x44, 0xcb, 0x0b, 0x69, 0x05,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x41, 0x46, 0x20, 0xcf, 0x74, 0x71, 0xb5, 0x46,
      0x9b, 0x15, 0x99, 0x89, 0xaf, 0x34, 0xed, 0xd1,
      0xed, 0x92, 0x76, 0xbe, 0x59, 0x2e, 0x43, 0xd1,
      0xb7, 0x54, 0x42, 0x04, 0xf4, 0x17, 0x73, 0x02,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4f, 0x89, 0x24, 0x3e, 0x75, 0x71, 0xb9, 0x56,
      0x34, 0x64, 0xf4, 0x34, 0x9c, 0xeb, 0x14, 0xa6,
      0x08, 0x08, 0xa9, 0xa0, 0xb6, 0xf7, 0x29, 0x44,
      0xdb, 0xdf, 0xff, 0xd6, 0x42, 0x7d, 0xa2, 0xf4,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/vector3__functions.h"
#include "visualization_msgs/msg/detail/interactive_marker_control__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "sensor_msgs/msg/detail/compressed_image__functions.h"
#include "builtin_interfaces/msg/detail/duration__functions.h"
#include "geometry_msgs/msg/detail/pose__functions.h"
#include "std_msgs/msg/detail/color_rgba__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "visualization_msgs/msg/detail/interactive_marker__functions.h"
#include "visualization_msgs/msg/detail/marker__functions.h"
#include "visualization_msgs/msg/detail/menu_entry__functions.h"
#include "geometry_msgs/msg/detail/point__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "visualization_msgs/msg/detail/mesh_file__functions.h"
#include "visualization_msgs/msg/detail/uv_coordinate__functions.h"
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
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
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
static const rosidl_type_hash_t visualization_msgs__msg__InteractiveMarker__EXPECTED_HASH = {1, {
    0x3d, 0x5b, 0x51, 0x44, 0x8b, 0x51, 0xd7, 0x3b,
    0x0f, 0x39, 0x5b, 0x94, 0xd2, 0x59, 0xed, 0xd3,
    0xa5, 0xd2, 0x69, 0xae, 0x9f, 0x7d, 0x9f, 0xd5,
    0xcc, 0xeb, 0x9a, 0xe4, 0xb7, 0x2b, 0xe3, 0x46,
  }};
static const rosidl_type_hash_t visualization_msgs__msg__InteractiveMarkerControl__EXPECTED_HASH = {1, {
    0x60, 0xe2, 0xfa, 0x36, 0x34, 0x4f, 0x5f, 0x47,
    0x91, 0xb2, 0x4a, 0x80, 0x95, 0x42, 0xa1, 0x8b,
    0xff, 0xd5, 0x55, 0xf5, 0x63, 0x55, 0x0d, 0x4b,
    0x22, 0xb3, 0xbb, 0xfc, 0x31, 0xec, 0x0e, 0xd5,
  }};
static const rosidl_type_hash_t visualization_msgs__msg__Marker__EXPECTED_HASH = {1, {
    0x45, 0xb1, 0x3c, 0xcf, 0x79, 0x1f, 0x22, 0x59,
    0x62, 0xbf, 0x74, 0xe7, 0x46, 0xf9, 0x64, 0x45,
    0x18, 0x85, 0x5d, 0x78, 0x3a, 0x6f, 0x42, 0xba,
    0x0c, 0xc1, 0x4f, 0xde, 0x2b, 0x4f, 0x3c, 0xe0,
  }};
static const rosidl_type_hash_t visualization_msgs__msg__MenuEntry__EXPECTED_HASH = {1, {
    0x22, 0x17, 0x0c, 0x38, 0x7c, 0x70, 0xfd, 0x42,
    0x36, 0x23, 0x2e, 0xc9, 0x02, 0xde, 0x86, 0x04,
    0xe7, 0x2f, 0xf0, 0x27, 0x34, 0x2c, 0x7c, 0x0f,
    0x28, 0xad, 0x9f, 0x68, 0xc6, 0x4c, 0x51, 0xd6,
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

static char visualization_msgs__srv__GetInteractiveMarkers__TYPE_NAME[] = "visualization_msgs/srv/GetInteractiveMarkers";
static char builtin_interfaces__msg__Duration__TYPE_NAME[] = "builtin_interfaces/msg/Duration";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Pose__TYPE_NAME[] = "geometry_msgs/msg/Pose";
static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";
static char geometry_msgs__msg__Vector3__TYPE_NAME[] = "geometry_msgs/msg/Vector3";
static char sensor_msgs__msg__CompressedImage__TYPE_NAME[] = "sensor_msgs/msg/CompressedImage";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char std_msgs__msg__ColorRGBA__TYPE_NAME[] = "std_msgs/msg/ColorRGBA";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";
static char visualization_msgs__msg__InteractiveMarker__TYPE_NAME[] = "visualization_msgs/msg/InteractiveMarker";
static char visualization_msgs__msg__InteractiveMarkerControl__TYPE_NAME[] = "visualization_msgs/msg/InteractiveMarkerControl";
static char visualization_msgs__msg__Marker__TYPE_NAME[] = "visualization_msgs/msg/Marker";
static char visualization_msgs__msg__MenuEntry__TYPE_NAME[] = "visualization_msgs/msg/MenuEntry";
static char visualization_msgs__msg__MeshFile__TYPE_NAME[] = "visualization_msgs/msg/MeshFile";
static char visualization_msgs__msg__UVCoordinate__TYPE_NAME[] = "visualization_msgs/msg/UVCoordinate";
static char visualization_msgs__srv__GetInteractiveMarkers_Event__TYPE_NAME[] = "visualization_msgs/srv/GetInteractiveMarkers_Event";
static char visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME[] = "visualization_msgs/srv/GetInteractiveMarkers_Request";
static char visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME[] = "visualization_msgs/srv/GetInteractiveMarkers_Response";

// Define type names, field names, and default values
static char visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__request_message[] = "request_message";
static char visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__response_message[] = "response_message";
static char visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field visualization_msgs__srv__GetInteractiveMarkers__FIELDS[] = {
  {
    {visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {visualization_msgs__srv__GetInteractiveMarkers_Event__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription visualization_msgs__srv__GetInteractiveMarkers__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
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
    {visualization_msgs__msg__InteractiveMarker__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerControl__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__TYPE_NAME, 32, 32},
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
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Event__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__srv__GetInteractiveMarkers__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__srv__GetInteractiveMarkers__TYPE_NAME, 44, 44},
      {visualization_msgs__srv__GetInteractiveMarkers__FIELDS, 3, 3},
    },
    {visualization_msgs__srv__GetInteractiveMarkers__REFERENCED_TYPE_DESCRIPTIONS, 19, 19},
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
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__ColorRGBA__EXPECTED_HASH, std_msgs__msg__ColorRGBA__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[8].fields = std_msgs__msg__ColorRGBA__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarker__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = visualization_msgs__msg__InteractiveMarker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarkerControl__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarkerControl__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[11].fields = visualization_msgs__msg__InteractiveMarkerControl__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__Marker__EXPECTED_HASH, visualization_msgs__msg__Marker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[12].fields = visualization_msgs__msg__Marker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MenuEntry__EXPECTED_HASH, visualization_msgs__msg__MenuEntry__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[13].fields = visualization_msgs__msg__MenuEntry__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MeshFile__EXPECTED_HASH, visualization_msgs__msg__MeshFile__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[14].fields = visualization_msgs__msg__MeshFile__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__UVCoordinate__EXPECTED_HASH, visualization_msgs__msg__UVCoordinate__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[15].fields = visualization_msgs__msg__UVCoordinate__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[16].fields = visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[17].fields = visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[18].fields = visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char visualization_msgs__srv__GetInteractiveMarkers_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field visualization_msgs__srv__GetInteractiveMarkers_Request__FIELDS[] = {
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
      {visualization_msgs__srv__GetInteractiveMarkers_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char visualization_msgs__srv__GetInteractiveMarkers_Response__FIELD_NAME__sequence_number[] = "sequence_number";
static char visualization_msgs__srv__GetInteractiveMarkers_Response__FIELD_NAME__markers[] = "markers";

static rosidl_runtime_c__type_description__Field visualization_msgs__srv__GetInteractiveMarkers_Response__FIELDS[] = {
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Response__FIELD_NAME__sequence_number, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Response__FIELD_NAME__markers, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {visualization_msgs__msg__InteractiveMarker__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription visualization_msgs__srv__GetInteractiveMarkers_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {visualization_msgs__msg__InteractiveMarker__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerControl__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__TYPE_NAME, 32, 32},
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
visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
      {visualization_msgs__srv__GetInteractiveMarkers_Response__FIELDS, 2, 2},
    },
    {visualization_msgs__srv__GetInteractiveMarkers_Response__REFERENCED_TYPE_DESCRIPTIONS, 15, 15},
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
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarker__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = visualization_msgs__msg__InteractiveMarker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarkerControl__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarkerControl__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = visualization_msgs__msg__InteractiveMarkerControl__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__Marker__EXPECTED_HASH, visualization_msgs__msg__Marker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[11].fields = visualization_msgs__msg__Marker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MenuEntry__EXPECTED_HASH, visualization_msgs__msg__MenuEntry__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[12].fields = visualization_msgs__msg__MenuEntry__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MeshFile__EXPECTED_HASH, visualization_msgs__msg__MeshFile__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[13].fields = visualization_msgs__msg__MeshFile__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__UVCoordinate__EXPECTED_HASH, visualization_msgs__msg__UVCoordinate__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[14].fields = visualization_msgs__msg__UVCoordinate__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__info[] = "info";
static char visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__request[] = "request";
static char visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field visualization_msgs__srv__GetInteractiveMarkers_Event__FIELDS[] = {
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription visualization_msgs__srv__GetInteractiveMarkers_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
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
    {visualization_msgs__msg__InteractiveMarker__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerControl__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__Marker__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__TYPE_NAME, 32, 32},
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
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__srv__GetInteractiveMarkers_Event__TYPE_NAME, 50, 50},
      {visualization_msgs__srv__GetInteractiveMarkers_Event__FIELDS, 3, 3},
    },
    {visualization_msgs__srv__GetInteractiveMarkers_Event__REFERENCED_TYPE_DESCRIPTIONS, 18, 18},
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
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__ColorRGBA__EXPECTED_HASH, std_msgs__msg__ColorRGBA__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[8].fields = std_msgs__msg__ColorRGBA__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarker__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = visualization_msgs__msg__InteractiveMarker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__InteractiveMarkerControl__EXPECTED_HASH, visualization_msgs__msg__InteractiveMarkerControl__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[11].fields = visualization_msgs__msg__InteractiveMarkerControl__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__Marker__EXPECTED_HASH, visualization_msgs__msg__Marker__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[12].fields = visualization_msgs__msg__Marker__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MenuEntry__EXPECTED_HASH, visualization_msgs__msg__MenuEntry__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[13].fields = visualization_msgs__msg__MenuEntry__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__MeshFile__EXPECTED_HASH, visualization_msgs__msg__MeshFile__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[14].fields = visualization_msgs__msg__MeshFile__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&visualization_msgs__msg__UVCoordinate__EXPECTED_HASH, visualization_msgs__msg__UVCoordinate__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[15].fields = visualization_msgs__msg__UVCoordinate__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[16].fields = visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[17].fields = visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "---\n"
  "# Sequence number.\n"
  "# Set to the sequence number of the latest update message\n"
  "# at the time the server received the request.\n"
  "# Clients use this to detect if any updates were missed.\n"
  "uint64 sequence_number\n"
  "\n"
  "# All interactive markers provided by the server.\n"
  "InteractiveMarker[] markers";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__srv__GetInteractiveMarkers__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__srv__GetInteractiveMarkers__TYPE_NAME, 44, 44},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 287, 287},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__srv__GetInteractiveMarkers_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__srv__GetInteractiveMarkers_Request__TYPE_NAME, 52, 52},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__srv__GetInteractiveMarkers_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__srv__GetInteractiveMarkers_Response__TYPE_NAME, 53, 53},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__srv__GetInteractiveMarkers_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__srv__GetInteractiveMarkers_Event__TYPE_NAME, 50, 50},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__srv__GetInteractiveMarkers__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[20];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 20, 20};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__srv__GetInteractiveMarkers__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[7] = *sensor_msgs__msg__CompressedImage__get_individual_type_description_source(NULL);
    sources[8] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[9] = *std_msgs__msg__ColorRGBA__get_individual_type_description_source(NULL);
    sources[10] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    sources[11] = *visualization_msgs__msg__InteractiveMarker__get_individual_type_description_source(NULL);
    sources[12] = *visualization_msgs__msg__InteractiveMarkerControl__get_individual_type_description_source(NULL);
    sources[13] = *visualization_msgs__msg__Marker__get_individual_type_description_source(NULL);
    sources[14] = *visualization_msgs__msg__MenuEntry__get_individual_type_description_source(NULL);
    sources[15] = *visualization_msgs__msg__MeshFile__get_individual_type_description_source(NULL);
    sources[16] = *visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(NULL);
    sources[17] = *visualization_msgs__srv__GetInteractiveMarkers_Event__get_individual_type_description_source(NULL);
    sources[18] = *visualization_msgs__srv__GetInteractiveMarkers_Request__get_individual_type_description_source(NULL);
    sources[19] = *visualization_msgs__srv__GetInteractiveMarkers_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__srv__GetInteractiveMarkers_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[16];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 16, 16};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__srv__GetInteractiveMarkers_Response__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[7] = *sensor_msgs__msg__CompressedImage__get_individual_type_description_source(NULL);
    sources[8] = *std_msgs__msg__ColorRGBA__get_individual_type_description_source(NULL);
    sources[9] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    sources[10] = *visualization_msgs__msg__InteractiveMarker__get_individual_type_description_source(NULL);
    sources[11] = *visualization_msgs__msg__InteractiveMarkerControl__get_individual_type_description_source(NULL);
    sources[12] = *visualization_msgs__msg__Marker__get_individual_type_description_source(NULL);
    sources[13] = *visualization_msgs__msg__MenuEntry__get_individual_type_description_source(NULL);
    sources[14] = *visualization_msgs__msg__MeshFile__get_individual_type_description_source(NULL);
    sources[15] = *visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[19];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 19, 19};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__srv__GetInteractiveMarkers_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *geometry_msgs__msg__Vector3__get_individual_type_description_source(NULL);
    sources[7] = *sensor_msgs__msg__CompressedImage__get_individual_type_description_source(NULL);
    sources[8] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[9] = *std_msgs__msg__ColorRGBA__get_individual_type_description_source(NULL);
    sources[10] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    sources[11] = *visualization_msgs__msg__InteractiveMarker__get_individual_type_description_source(NULL);
    sources[12] = *visualization_msgs__msg__InteractiveMarkerControl__get_individual_type_description_source(NULL);
    sources[13] = *visualization_msgs__msg__Marker__get_individual_type_description_source(NULL);
    sources[14] = *visualization_msgs__msg__MenuEntry__get_individual_type_description_source(NULL);
    sources[15] = *visualization_msgs__msg__MeshFile__get_individual_type_description_source(NULL);
    sources[16] = *visualization_msgs__msg__UVCoordinate__get_individual_type_description_source(NULL);
    sources[17] = *visualization_msgs__srv__GetInteractiveMarkers_Request__get_individual_type_description_source(NULL);
    sources[18] = *visualization_msgs__srv__GetInteractiveMarkers_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
