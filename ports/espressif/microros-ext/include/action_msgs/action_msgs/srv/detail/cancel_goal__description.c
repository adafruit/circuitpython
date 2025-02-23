// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from action_msgs:srv/CancelGoal.idl
// generated code does not contain a copyright notice

#include "action_msgs/srv/detail/cancel_goal__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__srv__CancelGoal__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x57, 0x3d, 0x8b, 0x0a, 0x53, 0x44, 0x51, 0xd7,
      0xbc, 0x2a, 0xc8, 0xc5, 0xff, 0xde, 0x8a, 0xc1,
      0x4b, 0x85, 0x93, 0xb7, 0x00, 0x11, 0x75, 0xd0,
      0xcd, 0x65, 0x16, 0xdc, 0xbe, 0xb8, 0x68, 0x9a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__srv__CancelGoal_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3d, 0x3c, 0x84, 0x65, 0x3c, 0x1f, 0x96, 0x91,
      0x80, 0x86, 0x88, 0x7e, 0x1d, 0xcb, 0x23, 0x6f,
      0xae, 0xc8, 0x8b, 0x81, 0xa5, 0xb1, 0x4f, 0xd4,
      0xcf, 0x48, 0x40, 0x06, 0x5b, 0xcd, 0xf8, 0xaf,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__srv__CancelGoal_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x35, 0xe6, 0x82, 0xcf, 0x3f, 0x51, 0x0e, 0x83,
      0xc7, 0x0a, 0x82, 0xa4, 0xaa, 0xc8, 0x88, 0x49,
      0x6d, 0xed, 0xee, 0x56, 0x77, 0x3b, 0xf9, 0xd8,
      0xe5, 0xe0, 0xaa, 0x81, 0xf9, 0xe1, 0xc9, 0x60,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__srv__CancelGoal_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x17, 0x8f, 0x81, 0x61, 0x5c, 0xe6, 0xbe, 0x41,
      0xad, 0x32, 0x82, 0x11, 0xd7, 0xa5, 0x6b, 0x08,
      0xb0, 0x0e, 0x73, 0xf4, 0x1f, 0x0b, 0x8a, 0xec,
      0x25, 0x6b, 0x2f, 0x3f, 0xf9, 0x7a, 0x5e, 0x7e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "action_msgs/msg/detail/goal_info__functions.h"
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t action_msgs__msg__GoalInfo__EXPECTED_HASH = {1, {
    0x63, 0x98, 0xfe, 0x76, 0x31, 0x54, 0x55, 0x43,
    0x53, 0x93, 0x07, 0x16, 0xb2, 0x25, 0x94, 0x7f,
    0x93, 0xb6, 0x72, 0xf0, 0xfb, 0x2e, 0x49, 0xfd,
    0xd0, 0x1b, 0xb7, 0xa7, 0xe3, 0x79, 0x33, 0xe9,
  }};
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
static const rosidl_type_hash_t unique_identifier_msgs__msg__UUID__EXPECTED_HASH = {1, {
    0x1b, 0x8e, 0x8a, 0xca, 0x95, 0x8c, 0xbe, 0xa2,
    0x8f, 0xe6, 0xef, 0x60, 0xbf, 0x6c, 0x19, 0xb6,
    0x83, 0xc9, 0x7a, 0x9e, 0xf6, 0x0b, 0xb3, 0x47,
    0x52, 0x06, 0x7d, 0x0f, 0x2f, 0x7a, 0xb4, 0x37,
  }};
#endif

static char action_msgs__srv__CancelGoal__TYPE_NAME[] = "action_msgs/srv/CancelGoal";
static char action_msgs__msg__GoalInfo__TYPE_NAME[] = "action_msgs/msg/GoalInfo";
static char action_msgs__srv__CancelGoal_Event__TYPE_NAME[] = "action_msgs/srv/CancelGoal_Event";
static char action_msgs__srv__CancelGoal_Request__TYPE_NAME[] = "action_msgs/srv/CancelGoal_Request";
static char action_msgs__srv__CancelGoal_Response__TYPE_NAME[] = "action_msgs/srv/CancelGoal_Response";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char unique_identifier_msgs__msg__UUID__TYPE_NAME[] = "unique_identifier_msgs/msg/UUID";

// Define type names, field names, and default values
static char action_msgs__srv__CancelGoal__FIELD_NAME__request_message[] = "request_message";
static char action_msgs__srv__CancelGoal__FIELD_NAME__response_message[] = "response_message";
static char action_msgs__srv__CancelGoal__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field action_msgs__srv__CancelGoal__FIELDS[] = {
  {
    {action_msgs__srv__CancelGoal__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {action_msgs__srv__CancelGoal_Event__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__srv__CancelGoal__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Event__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
action_msgs__srv__CancelGoal__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__srv__CancelGoal__TYPE_NAME, 26, 26},
      {action_msgs__srv__CancelGoal__FIELDS, 3, 3},
    },
    {action_msgs__srv__CancelGoal__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&action_msgs__msg__GoalInfo__EXPECTED_HASH, action_msgs__msg__GoalInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = action_msgs__msg__GoalInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = action_msgs__srv__CancelGoal_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = action_msgs__srv__CancelGoal_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = action_msgs__srv__CancelGoal_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&unique_identifier_msgs__msg__UUID__EXPECTED_HASH, unique_identifier_msgs__msg__UUID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = unique_identifier_msgs__msg__UUID__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char action_msgs__srv__CancelGoal_Request__FIELD_NAME__goal_info[] = "goal_info";

static rosidl_runtime_c__type_description__Field action_msgs__srv__CancelGoal_Request__FIELDS[] = {
  {
    {action_msgs__srv__CancelGoal_Request__FIELD_NAME__goal_info, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__srv__CancelGoal_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
action_msgs__srv__CancelGoal_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
      {action_msgs__srv__CancelGoal_Request__FIELDS, 1, 1},
    },
    {action_msgs__srv__CancelGoal_Request__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&action_msgs__msg__GoalInfo__EXPECTED_HASH, action_msgs__msg__GoalInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = action_msgs__msg__GoalInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&unique_identifier_msgs__msg__UUID__EXPECTED_HASH, unique_identifier_msgs__msg__UUID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = unique_identifier_msgs__msg__UUID__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char action_msgs__srv__CancelGoal_Response__FIELD_NAME__return_code[] = "return_code";
static char action_msgs__srv__CancelGoal_Response__FIELD_NAME__goals_canceling[] = "goals_canceling";

static rosidl_runtime_c__type_description__Field action_msgs__srv__CancelGoal_Response__FIELDS[] = {
  {
    {action_msgs__srv__CancelGoal_Response__FIELD_NAME__return_code, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Response__FIELD_NAME__goals_canceling, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__srv__CancelGoal_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
action_msgs__srv__CancelGoal_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
      {action_msgs__srv__CancelGoal_Response__FIELDS, 2, 2},
    },
    {action_msgs__srv__CancelGoal_Response__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&action_msgs__msg__GoalInfo__EXPECTED_HASH, action_msgs__msg__GoalInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = action_msgs__msg__GoalInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&unique_identifier_msgs__msg__UUID__EXPECTED_HASH, unique_identifier_msgs__msg__UUID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = unique_identifier_msgs__msg__UUID__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char action_msgs__srv__CancelGoal_Event__FIELD_NAME__info[] = "info";
static char action_msgs__srv__CancelGoal_Event__FIELD_NAME__request[] = "request";
static char action_msgs__srv__CancelGoal_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field action_msgs__srv__CancelGoal_Event__FIELDS[] = {
  {
    {action_msgs__srv__CancelGoal_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__srv__CancelGoal_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
    {NULL, 0, 0},
  },
  {
    {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {unique_identifier_msgs__msg__UUID__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
action_msgs__srv__CancelGoal_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__srv__CancelGoal_Event__TYPE_NAME, 32, 32},
      {action_msgs__srv__CancelGoal_Event__FIELDS, 3, 3},
    },
    {action_msgs__srv__CancelGoal_Event__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&action_msgs__msg__GoalInfo__EXPECTED_HASH, action_msgs__msg__GoalInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = action_msgs__msg__GoalInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = action_msgs__srv__CancelGoal_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = action_msgs__srv__CancelGoal_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&unique_identifier_msgs__msg__UUID__EXPECTED_HASH, unique_identifier_msgs__msg__UUID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = unique_identifier_msgs__msg__UUID__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Cancel one or more goals with the following policy:\n"
  "#\n"
  "# - If the goal ID is zero and timestamp is zero, cancel all goals.\n"
  "# - If the goal ID is zero and timestamp is not zero, cancel all goals accepted\n"
  "#   at or before the timestamp.\n"
  "# - If the goal ID is not zero and timestamp is zero, cancel the goal with the\n"
  "#   given ID regardless of the time it was accepted.\n"
  "# - If the goal ID is not zero and timestamp is not zero, cancel the goal with\n"
  "#   the given ID and all goals accepted at or before the timestamp.\n"
  "\n"
  "# Goal info describing the goals to cancel, see above.\n"
  "GoalInfo goal_info\n"
  "---\n"
  "##\n"
  "## Return codes.\n"
  "##\n"
  "\n"
  "# Indicates the request was accepted without any errors.\n"
  "#\n"
  "# One or more goals have transitioned to the CANCELING state. The\n"
  "# goals_canceling list is not empty.\n"
  "int8 ERROR_NONE=0\n"
  "\n"
  "# Indicates the request was rejected.\n"
  "#\n"
  "# No goals have transitioned to the CANCELING state. The goals_canceling list is\n"
  "# empty.\n"
  "int8 ERROR_REJECTED=1\n"
  "\n"
  "# Indicates the requested goal ID does not exist.\n"
  "#\n"
  "# No goals have transitioned to the CANCELING state. The goals_canceling list is\n"
  "# empty.\n"
  "int8 ERROR_UNKNOWN_GOAL_ID=2\n"
  "\n"
  "# Indicates the goal is not cancelable because it is already in a terminal state.\n"
  "#\n"
  "# No goals have transitioned to the CANCELING state. The goals_canceling list is\n"
  "# empty.\n"
  "int8 ERROR_GOAL_TERMINATED=3\n"
  "\n"
  "# Return code, see above definitions.\n"
  "int8 return_code\n"
  "\n"
  "# Goals that accepted the cancel request.\n"
  "GoalInfo[] goals_canceling";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__srv__CancelGoal__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__srv__CancelGoal__TYPE_NAME, 26, 26},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 1453, 1453},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__srv__CancelGoal_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__srv__CancelGoal_Request__TYPE_NAME, 34, 34},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__srv__CancelGoal_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__srv__CancelGoal_Response__TYPE_NAME, 35, 35},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__srv__CancelGoal_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__srv__CancelGoal_Event__TYPE_NAME, 32, 32},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__srv__CancelGoal__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__srv__CancelGoal__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *action_msgs__srv__CancelGoal_Event__get_individual_type_description_source(NULL);
    sources[3] = *action_msgs__srv__CancelGoal_Request__get_individual_type_description_source(NULL);
    sources[4] = *action_msgs__srv__CancelGoal_Response__get_individual_type_description_source(NULL);
    sources[5] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[7] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__srv__CancelGoal_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__srv__CancelGoal_Request__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__srv__CancelGoal_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__srv__CancelGoal_Response__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__srv__CancelGoal_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__srv__CancelGoal_Event__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *action_msgs__srv__CancelGoal_Request__get_individual_type_description_source(NULL);
    sources[3] = *action_msgs__srv__CancelGoal_Response__get_individual_type_description_source(NULL);
    sources[4] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[6] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
