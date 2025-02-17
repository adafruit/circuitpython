// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from nav_msgs:srv/GetPlan.idl
// generated code does not contain a copyright notice

#include "nav_msgs/srv/detail/get_plan__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__GetPlan__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x23, 0x4f, 0x7a, 0xff, 0x10, 0x0f, 0x5e, 0xdb,
      0x81, 0x50, 0x36, 0x66, 0x01, 0x68, 0x7b, 0x02,
      0x7b, 0xcd, 0xc2, 0x53, 0xdb, 0x47, 0xde, 0xcb,
      0x88, 0xff, 0xf8, 0x46, 0x19, 0x3f, 0xe5, 0xe8,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__GetPlan_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf1, 0xbb, 0x9f, 0xab, 0x28, 0x79, 0x48, 0xb4,
      0x43, 0x2a, 0x4a, 0x7e, 0x4f, 0x52, 0x9e, 0x7e,
      0xa6, 0x26, 0x9c, 0xd0, 0x62, 0x69, 0xee, 0x8f,
      0xcb, 0x91, 0x1f, 0xdb, 0x4a, 0xcd, 0x23, 0xc3,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__GetPlan_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x30, 0xa9, 0x47, 0xcd, 0x36, 0x07, 0x77, 0x92,
      0x90, 0x73, 0x29, 0x6c, 0x36, 0x8c, 0x59, 0x1d,
      0xc2, 0x11, 0x7a, 0x89, 0xc1, 0x5b, 0x7a, 0x97,
      0x5a, 0x6f, 0xaa, 0x0a, 0x90, 0x18, 0x36, 0xfd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_nav_msgs
const rosidl_type_hash_t *
nav_msgs__srv__GetPlan_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x25, 0x50, 0xc2, 0xe3, 0x2e, 0x7e, 0xe2, 0x4d,
      0x48, 0x06, 0xe1, 0x08, 0x47, 0x95, 0x8e, 0x1f,
      0xc3, 0x7e, 0x26, 0x31, 0xa6, 0x3d, 0xd8, 0x3f,
      0x55, 0x12, 0xfa, 0x02, 0xac, 0x0c, 0xdd, 0xa5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "geometry_msgs/msg/detail/pose__functions.h"
#include "geometry_msgs/msg/detail/pose_stamped__functions.h"
#include "geometry_msgs/msg/detail/quaternion__functions.h"
#include "geometry_msgs/msg/detail/point__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "nav_msgs/msg/detail/path__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
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
static const rosidl_type_hash_t geometry_msgs__msg__PoseStamped__EXPECTED_HASH = {1, {
    0x10, 0xf3, 0x78, 0x6d, 0x7d, 0x40, 0xfd, 0x2b,
    0x54, 0x36, 0x78, 0x35, 0x61, 0x4b, 0xff, 0x85,
    0xd4, 0xad, 0x3b, 0x5d, 0xab, 0x62, 0xbf, 0x8b,
    0xca, 0x0c, 0xc2, 0x32, 0xd7, 0x3b, 0x4c, 0xd8,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Quaternion__EXPECTED_HASH = {1, {
    0x8a, 0x76, 0x5f, 0x66, 0x77, 0x8c, 0x8f, 0xf7,
    0xc8, 0xab, 0x94, 0xaf, 0xcc, 0x59, 0x0a, 0x2e,
    0xd5, 0x32, 0x5a, 0x1d, 0x9a, 0x07, 0x6f, 0xff,
    0xf3, 0x8f, 0xbc, 0xe3, 0x6f, 0x45, 0x86, 0x84,
  }};
static const rosidl_type_hash_t nav_msgs__msg__Path__EXPECTED_HASH = {1, {
    0x19, 0x57, 0xa5, 0xbb, 0x3c, 0xee, 0x5d, 0xa6,
    0x5c, 0x4e, 0x52, 0xe5, 0x2b, 0x65, 0xa9, 0x3d,
    0xf2, 0x27, 0xef, 0xce, 0x4c, 0x20, 0xf8, 0x45,
    0x8b, 0x36, 0xe7, 0x30, 0x66, 0xca, 0x33, 0x4b,
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

static char nav_msgs__srv__GetPlan__TYPE_NAME[] = "nav_msgs/srv/GetPlan";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Pose__TYPE_NAME[] = "geometry_msgs/msg/Pose";
static char geometry_msgs__msg__PoseStamped__TYPE_NAME[] = "geometry_msgs/msg/PoseStamped";
static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";
static char nav_msgs__msg__Path__TYPE_NAME[] = "nav_msgs/msg/Path";
static char nav_msgs__srv__GetPlan_Event__TYPE_NAME[] = "nav_msgs/srv/GetPlan_Event";
static char nav_msgs__srv__GetPlan_Request__TYPE_NAME[] = "nav_msgs/srv/GetPlan_Request";
static char nav_msgs__srv__GetPlan_Response__TYPE_NAME[] = "nav_msgs/srv/GetPlan_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char nav_msgs__srv__GetPlan__FIELD_NAME__request_message[] = "request_message";
static char nav_msgs__srv__GetPlan__FIELD_NAME__response_message[] = "response_message";
static char nav_msgs__srv__GetPlan__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__GetPlan__FIELDS[] = {
  {
    {nav_msgs__srv__GetPlan__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__srv__GetPlan_Event__TYPE_NAME, 26, 26},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__GetPlan__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__Path__TYPE_NAME, 17, 17},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Event__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
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
nav_msgs__srv__GetPlan__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__GetPlan__TYPE_NAME, 20, 20},
      {nav_msgs__srv__GetPlan__FIELDS, 3, 3},
    },
    {nav_msgs__srv__GetPlan__REFERENCED_TYPE_DESCRIPTIONS, 11, 11},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__PoseStamped__EXPECTED_HASH, geometry_msgs__msg__PoseStamped__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__PoseStamped__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__Path__EXPECTED_HASH, nav_msgs__msg__Path__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__Path__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = nav_msgs__srv__GetPlan_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[7].fields = nav_msgs__srv__GetPlan_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[8].fields = nav_msgs__srv__GetPlan_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[10].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__GetPlan_Request__FIELD_NAME__start[] = "start";
static char nav_msgs__srv__GetPlan_Request__FIELD_NAME__goal[] = "goal";
static char nav_msgs__srv__GetPlan_Request__FIELD_NAME__tolerance[] = "tolerance";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__GetPlan_Request__FIELDS[] = {
  {
    {nav_msgs__srv__GetPlan_Request__FIELD_NAME__start, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Request__FIELD_NAME__goal, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Request__FIELD_NAME__tolerance, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__GetPlan_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__GetPlan_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
      {nav_msgs__srv__GetPlan_Request__FIELDS, 3, 3},
    },
    {nav_msgs__srv__GetPlan_Request__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__PoseStamped__EXPECTED_HASH, geometry_msgs__msg__PoseStamped__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__PoseStamped__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__GetPlan_Response__FIELD_NAME__plan[] = "plan";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__GetPlan_Response__FIELDS[] = {
  {
    {nav_msgs__srv__GetPlan_Response__FIELD_NAME__plan, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {nav_msgs__msg__Path__TYPE_NAME, 17, 17},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__GetPlan_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__Path__TYPE_NAME, 17, 17},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
nav_msgs__srv__GetPlan_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
      {nav_msgs__srv__GetPlan_Response__FIELDS, 1, 1},
    },
    {nav_msgs__srv__GetPlan_Response__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__PoseStamped__EXPECTED_HASH, geometry_msgs__msg__PoseStamped__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__PoseStamped__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__Path__EXPECTED_HASH, nav_msgs__msg__Path__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__Path__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char nav_msgs__srv__GetPlan_Event__FIELD_NAME__info[] = "info";
static char nav_msgs__srv__GetPlan_Event__FIELD_NAME__request[] = "request";
static char nav_msgs__srv__GetPlan_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field nav_msgs__srv__GetPlan_Event__FIELDS[] = {
  {
    {nav_msgs__srv__GetPlan_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription nav_msgs__srv__GetPlan_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
    {geometry_msgs__msg__PoseStamped__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__msg__Path__TYPE_NAME, 17, 17},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
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
nav_msgs__srv__GetPlan_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {nav_msgs__srv__GetPlan_Event__TYPE_NAME, 26, 26},
      {nav_msgs__srv__GetPlan_Event__FIELDS, 3, 3},
    },
    {nav_msgs__srv__GetPlan_Event__REFERENCED_TYPE_DESCRIPTIONS, 10, 10},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__PoseStamped__EXPECTED_HASH, geometry_msgs__msg__PoseStamped__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__PoseStamped__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&nav_msgs__msg__Path__EXPECTED_HASH, nav_msgs__msg__Path__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = nav_msgs__msg__Path__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = nav_msgs__srv__GetPlan_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[7].fields = nav_msgs__srv__GetPlan_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[8].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[9].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Get a plan from the current position to the goal Pose\n"
  "\n"
  "# The start pose for the plan\n"
  "geometry_msgs/PoseStamped start\n"
  "\n"
  "# The final pose of the goal position\n"
  "geometry_msgs/PoseStamped goal\n"
  "\n"
  "# If the goal is obstructed, how many meters the planner can\n"
  "# relax the constraint in x and y before failing.\n"
  "float32 tolerance\n"
  "---\n"
  "# Array of poses from start to goal if one was successfully found.\n"
  "Path plan";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__GetPlan__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__GetPlan__TYPE_NAME, 20, 20},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 400, 400},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__GetPlan_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__GetPlan_Request__TYPE_NAME, 28, 28},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__GetPlan_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__GetPlan_Response__TYPE_NAME, 29, 29},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
nav_msgs__srv__GetPlan_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {nav_msgs__srv__GetPlan_Event__TYPE_NAME, 26, 26},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__GetPlan__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[12];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 12, 12};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__GetPlan__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__PoseStamped__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__Path__get_individual_type_description_source(NULL);
    sources[7] = *nav_msgs__srv__GetPlan_Event__get_individual_type_description_source(NULL);
    sources[8] = *nav_msgs__srv__GetPlan_Request__get_individual_type_description_source(NULL);
    sources[9] = *nav_msgs__srv__GetPlan_Response__get_individual_type_description_source(NULL);
    sources[10] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[11] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__GetPlan_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__GetPlan_Request__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__PoseStamped__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__GetPlan_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__GetPlan_Response__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__PoseStamped__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__Path__get_individual_type_description_source(NULL);
    sources[7] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
nav_msgs__srv__GetPlan_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[11];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 11, 11};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *nav_msgs__srv__GetPlan_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__PoseStamped__get_individual_type_description_source(NULL);
    sources[5] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[6] = *nav_msgs__msg__Path__get_individual_type_description_source(NULL);
    sources[7] = *nav_msgs__srv__GetPlan_Request__get_individual_type_description_source(NULL);
    sources[8] = *nav_msgs__srv__GetPlan_Response__get_individual_type_description_source(NULL);
    sources[9] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[10] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
