// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from action_msgs:msg/GoalStatus.idl
// generated code does not contain a copyright notice

#include "action_msgs/msg/detail/goal_status__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_action_msgs
const rosidl_type_hash_t *
action_msgs__msg__GoalStatus__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x32, 0xf4, 0xcf, 0xd7, 0x17, 0x73, 0x5d, 0x17,
      0x65, 0x7e, 0x11, 0x78, 0xf2, 0x44, 0x31, 0xc1,
      0xce, 0x99, 0x6c, 0x87, 0x8c, 0x51, 0x52, 0x30,
      0xf6, 0xc5, 0xb3, 0x47, 0x68, 0x19, 0xdb, 0xb9,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "action_msgs/msg/detail/goal_info__functions.h"
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

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
static const rosidl_type_hash_t unique_identifier_msgs__msg__UUID__EXPECTED_HASH = {1, {
    0x1b, 0x8e, 0x8a, 0xca, 0x95, 0x8c, 0xbe, 0xa2,
    0x8f, 0xe6, 0xef, 0x60, 0xbf, 0x6c, 0x19, 0xb6,
    0x83, 0xc9, 0x7a, 0x9e, 0xf6, 0x0b, 0xb3, 0x47,
    0x52, 0x06, 0x7d, 0x0f, 0x2f, 0x7a, 0xb4, 0x37,
  }};
#endif

static char action_msgs__msg__GoalStatus__TYPE_NAME[] = "action_msgs/msg/GoalStatus";
static char action_msgs__msg__GoalInfo__TYPE_NAME[] = "action_msgs/msg/GoalInfo";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char unique_identifier_msgs__msg__UUID__TYPE_NAME[] = "unique_identifier_msgs/msg/UUID";

// Define type names, field names, and default values
static char action_msgs__msg__GoalStatus__FIELD_NAME__goal_info[] = "goal_info";
static char action_msgs__msg__GoalStatus__FIELD_NAME__status[] = "status";

static rosidl_runtime_c__type_description__Field action_msgs__msg__GoalStatus__FIELDS[] = {
  {
    {action_msgs__msg__GoalStatus__FIELD_NAME__goal_info, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {action_msgs__msg__GoalInfo__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {action_msgs__msg__GoalStatus__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription action_msgs__msg__GoalStatus__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
action_msgs__msg__GoalStatus__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {action_msgs__msg__GoalStatus__TYPE_NAME, 26, 26},
      {action_msgs__msg__GoalStatus__FIELDS, 2, 2},
    },
    {action_msgs__msg__GoalStatus__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
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

static char toplevel_type_raw_source[] =
  "# An action goal can be in one of these states after it is accepted by an action\n"
  "# server.\n"
  "#\n"
  "# For more information, see http://design.ros2.org/articles/actions.html\n"
  "\n"
  "# Indicates status has not been properly set.\n"
  "int8 STATUS_UNKNOWN   = 0\n"
  "\n"
  "# The goal has been accepted and is awaiting execution.\n"
  "int8 STATUS_ACCEPTED  = 1\n"
  "\n"
  "# The goal is currently being executed by the action server.\n"
  "int8 STATUS_EXECUTING = 2\n"
  "\n"
  "# The client has requested that the goal be canceled and the action server has\n"
  "# accepted the cancel request.\n"
  "int8 STATUS_CANCELING = 3\n"
  "\n"
  "# The goal was achieved successfully by the action server.\n"
  "int8 STATUS_SUCCEEDED = 4\n"
  "\n"
  "# The goal was canceled after an external request from an action client.\n"
  "int8 STATUS_CANCELED  = 5\n"
  "\n"
  "# The goal was terminated by the action server without an external request.\n"
  "int8 STATUS_ABORTED   = 6\n"
  "\n"
  "# Goal info (contains ID and timestamp).\n"
  "GoalInfo goal_info\n"
  "\n"
  "# Action goal state-machine status.\n"
  "int8 status";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
action_msgs__msg__GoalStatus__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {action_msgs__msg__GoalStatus__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 946, 946},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
action_msgs__msg__GoalStatus__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *action_msgs__msg__GoalStatus__get_individual_type_description_source(NULL),
    sources[1] = *action_msgs__msg__GoalInfo__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[3] = *unique_identifier_msgs__msg__UUID__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
