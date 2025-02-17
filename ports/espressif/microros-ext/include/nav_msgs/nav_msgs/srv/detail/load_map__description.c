// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from nav_msgs:srv/LoadMap.idl
// generated code does not contain a copyright notice

#include "nav_msgs/srv/detail/load_map__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__LoadMap__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1a, 0x19, 0x2a, 0xc5, 0x6c, 0x40, 0xfe, 0xd2,
      0x76, 0x7d, 0xac, 0x26, 0xf0, 0xb3, 0x71, 0x78,
      0x53, 0x72, 0x27, 0x6b, 0xd4, 0x65, 0xc9, 0x02,
      0x67, 0x6d, 0x2d, 0xca, 0x13, 0x5a, 0xae, 0x5a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__LoadMap_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x0f, 0x8f, 0xe3, 0xaa, 0xc4, 0x33, 0xc9, 0x77,
      0xd7, 0xc2, 0xf8, 0xb5, 0x7a, 0xb1, 0xc3, 0xa1,
      0x76, 0x77, 0xe2, 0x48, 0x80, 0xd7, 0x69, 0x19,
      0xb6, 0x9e, 0x42, 0x08, 0xe9, 0x9f, 0x51, 0x32,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__LoadMap_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe7, 0x97, 0x0d, 0xdf, 0x9f, 0x58, 0xe1, 0x0d,
      0x34, 0xb5, 0xea, 0xe6, 0xd2, 0xba, 0xca, 0x31,
      0xb2, 0xb3, 0xe2, 0xf2, 0xfe, 0x6d, 0x08, 0x2b,
      0x3a, 0xae, 0x2a, 0x5f, 0x92, 0x77, 0x7b, 0x2a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__LoadMap_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa7, 0x90, 0x7d, 0xcb, 0xaf, 0x7a, 0x05, 0x2c,
      0xc9, 0x6c, 0x8c, 0x71, 0x69, 0x92, 0x59, 0x1d,
      0xc0, 0x06, 0xad, 0x26, 0xee, 0x28, 0x41, 0x32,
      0x9a, 0x7e, 0x06, 0xa4, 0xa3, 0x33, 0x39, 0xc7,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/pose__functions.h"
#include "geometry_msgs/msg/detail/quaternion__functions.h"
#include "geometry_msgs/msg/detail/point__functions.h"
#include "nav_msgs/msg/detail/map_meta_data__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "nav_msgs/msg/detail/occupancy_grid__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
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
static const rosidl_type_hash_t nav_msgs__msg__MapMetaData__EXPECTED_HASH = {1, {
    0x27, 0x72, 0xd4, 0xb2, 0x00, 0x0e, 0xf2, 0xb3,
    0x5d, 0xba, 0xeb, 0x80, 0xfd, 0x39, 0x46, 0xc1,
    0x36, 0x9f, 0x81, 0x7f, 0xb4, 0xf7, 0x56, 0x77,
    0xd9, 0x16, 0xd2, 0x7c, 0x17, 0xd7, 0x63, 0xc8,
  }};
static const rosidl_type_hash_t nav_msgs__msg__OccupancyGrid__EXPECTED_HASH = {1, {
    0x8d, 0x34, 0x81, 0x50, 0xc1, 0x29, 0x13, 0xa3,
    0x1e, 0xe0, 0xec, 0x17, 0x0f, 0xbf, 0x25, 0x08,
    0x9e, 0x47, 0x45, 0xd1, 0x70, 0x35, 0x79, 0x2a,
    0x1b, 0xa9, 0x4d, 0x6f, 0x0b, 0xc0, 0xcf, 0xc7,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char nav_msgs__srv__LoadMap__TYPE_NAME[] = "nav_msgs/srv/LoadMap";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Pose__TYPE_NAME[] = "geometry_msgs/msg/Pose";
static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";
static char nav_msgs__msg__MapMetaData__TYPE_NAME[] = "nav_msgs/msg/MapMetaData";
static char nav_msgs__msg__OccupancyGrid__TYPE_NAME[] = "nav_msgs/msg/OccupancyGrid";
static char nav_msgs__srv__LoadMap_Event__TYPE_NAME[] = "nav_msgs/srv/LoadMap_Event";
static char nav_msgs__srv__LoadMap_Request__TYPE_NAME[] = "nav_msgs/srv/LoadMap_Request";
static char nav_msgs__srv__LoadMap_Response__TYPE_NAME[] = "nav_msgs/srv/LoadMap_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char nav_msgs__srv__LoadMap__FIELD_NAME__request_message[] = "request_message";
static char nav_msgs__srv__LoadMap__FIELD_NAME__response_message[] = "response_message";
static char nav_msgs__srv__LoadMap__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__LoadMap__FIELDS[] = {
  {
    {nav_msgs__srv__LoadMap__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__LoadMap_Event__TYPE_NAME, 26, 26},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__LoadMap__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {nav_msgs__msg__MapMetaData__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__OccupancyGrid__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Event__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__LoadMap__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__LoadMap__TYPE_NAME, 20, 20},
      {nav_msgs__srv__LoadMap__FIELDS, 3, 3},
    },
    {nav_msgs__srv__LoadMap__REFERENCED_TYPE_DESCRIPTIONS, 11, 11},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__MapMetaData__EXPECTED_HASH, nav_msgs__msg__MapMetaData__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = nav_msgs__msg__MapMetaData__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__OccupancyGrid__EXPECTED_HASH, nav_msgs__msg__OccupancyGrid__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__OccupancyGrid__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = nav_msgs__srv__LoadMap_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[7].fields = nav_msgs__srv__LoadMap_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[8].fields = nav_msgs__srv__LoadMap_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__LoadMap_Request__FIELD_NAME__map_url[] = "map_url";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__LoadMap_Request__FIELDS[] = {
  {
    {nav_msgs__srv__LoadMap_Request__FIELD_NAME__map_url, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__LoadMap_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
      {nav_msgs__srv__LoadMap_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__LoadMap_Response__FIELD_NAME__map[] = "map";
static char nav_msgs__srv__LoadMap_Response__FIELD_NAME__result[] = "result";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__LoadMap_Response__FIELDS[] = {
  {
    {nav_msgs__srv__LoadMap_Response__FIELD_NAME__map, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__msg__OccupancyGrid__TYPE_NAME, 26, 26},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Response__FIELD_NAME__result, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__LoadMap_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {nav_msgs__msg__MapMetaData__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__OccupancyGrid__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__LoadMap_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
      {nav_msgs__srv__LoadMap_Response__FIELDS, 2, 2},
    },
    {nav_msgs__srv__LoadMap_Response__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__MapMetaData__EXPECTED_HASH, nav_msgs__msg__MapMetaData__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = nav_msgs__msg__MapMetaData__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__OccupancyGrid__EXPECTED_HASH, nav_msgs__msg__OccupancyGrid__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__OccupancyGrid__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__LoadMap_Event__FIELD_NAME__info[] = "info";
static char nav_msgs__srv__LoadMap_Event__FIELD_NAME__request[] = "request";
static char nav_msgs__srv__LoadMap_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__LoadMap_Event__FIELDS[] = {
  {
    {nav_msgs__srv__LoadMap_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__LoadMap_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {nav_msgs__msg__MapMetaData__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__OccupancyGrid__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__LoadMap_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__LoadMap_Event__TYPE_NAME, 26, 26},
      {nav_msgs__srv__LoadMap_Event__FIELDS, 3, 3},
    },
    {nav_msgs__srv__LoadMap_Event__REFERENCED_TYPE_DESCRIPTIONS, 10, 10},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__MapMetaData__EXPECTED_HASH, nav_msgs__msg__MapMetaData__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = nav_msgs__msg__MapMetaData__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__OccupancyGrid__EXPECTED_HASH, nav_msgs__msg__OccupancyGrid__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__OccupancyGrid__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = nav_msgs__srv__LoadMap_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[7].fields = nav_msgs__srv__LoadMap_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[8].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# URL of map resource\n"
  "# Can be an absolute path to a file: file:///path/to/maps/floor1.yaml\n"
  "# Or, relative to a ROS package: package://my_ros_package/maps/floor2.yaml\n"
  "string map_url\n"
  "---\n"
  "# Result code defintions\n"
  "uint8 RESULT_SUCCESS=0\n"
  "uint8 RESULT_MAP_DOES_NOT_EXIST=1\n"
  "uint8 RESULT_INVALID_MAP_DATA=2\n"
  "uint8 RESULT_INVALID_MAP_METADATA=3\n"
  "uint8 RESULT_UNDEFINED_FAILURE=255\n"
  "\n"
  "# Returned map is only valid if result equals RESULT_SUCCESS\n"
  "nav_msgs/OccupancyGrid map\n"
  "uint8 result";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__LoadMap__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__LoadMap__TYPE_NAME, 20, 20},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 473, 473},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__LoadMap_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__LoadMap_Request__TYPE_NAME, 28, 28},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__LoadMap_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__LoadMap_Response__TYPE_NAME, 29, 29},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__LoadMap_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__LoadMap_Event__TYPE_NAME, 26, 26},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__LoadMap__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[12];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 12, 12};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__LoadMap__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[5] = *nav_msgs__msg__MapMetaData__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__OccupancyGrid__get_individual_type_description_source(NULL);
    sources[7] = *nav_msgs__srv__LoadMap_Event__get_individual_type_description_source(NULL);
    sources[8] = *nav_msgs__srv__LoadMap_Request__get_individual_type_description_source(NULL);
    sources[9] = *nav_msgs__srv__LoadMap_Response__get_individual_type_description_source(NULL);
    sources[10] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[11] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__LoadMap_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__LoadMap_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__LoadMap_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__LoadMap_Response__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[5] = *nav_msgs__msg__MapMetaData__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__OccupancyGrid__get_individual_type_description_source(NULL);
    sources[7] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__LoadMap_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[11];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 11, 11};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__LoadMap_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[5] = *nav_msgs__msg__MapMetaData__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__OccupancyGrid__get_individual_type_description_source(NULL);
    sources[7] = *nav_msgs__srv__LoadMap_Request__get_individual_type_description_source(NULL);
    sources[8] = *nav_msgs__srv__LoadMap_Response__get_individual_type_description_source(NULL);
    sources[9] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[10] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
