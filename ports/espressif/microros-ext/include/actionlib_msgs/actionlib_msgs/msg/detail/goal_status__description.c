// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from actionlib_msgs:msg/GoalStatus.idl
// generated code does not contain a copyright notice

#include "actionlib_msgs/msg/detail/goal_status__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_type_hash_t *
actionlib_msgs__msg__GoalStatus__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc9, 0x0c, 0x64, 0x23, 0x09, 0x54, 0xd9, 0x0e,
      0xe0, 0x79, 0xcb, 0xdd, 0x22, 0xa1, 0x4b, 0xca,
      0x36, 0x50, 0xb3, 0xd4, 0x03, 0xf5, 0xcc, 0x3f,
      0xaf, 0x0e, 0xee, 0x99, 0x2c, 0x6a, 0xe9, 0xff,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "actionlib_msgs/msg/detail/goal_id__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t actionlib_msgs__msg__GoalID__EXPECTED_HASH = {1, {
    0x14, 0x0e, 0xa4, 0xa1, 0xba, 0xd4, 0x78, 0x1e,
    0xdc, 0x32, 0x03, 0x0c, 0x79, 0x83, 0xab, 0x3a,
    0x62, 0xc6, 0x75, 0xf8, 0x61, 0xc6, 0x1c, 0x9b,
    0x22, 0x00, 0xae, 0x5e, 0x53, 0x98, 0xbe, 0x24,
  }};
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
#endif

static char actionlib_msgs__msg__GoalStatus__TYPE_NAME[] = "actionlib_msgs/msg/GoalStatus";
static char actionlib_msgs__msg__GoalID__TYPE_NAME[] = "actionlib_msgs/msg/GoalID";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char actionlib_msgs__msg__GoalStatus__FIELD_NAME__goal_id[] = "goal_id";
static char actionlib_msgs__msg__GoalStatus__FIELD_NAME__status[] = "status";
static char actionlib_msgs__msg__GoalStatus__FIELD_NAME__text[] = "text";

static rosidl_runtime_c__type_description__Field actionlib_msgs__msg__GoalStatus__FIELDS[] = {
  {
    {actionlib_msgs__msg__GoalStatus__FIELD_NAME__goal_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {actionlib_msgs__msg__GoalID__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
  {
    {actionlib_msgs__msg__GoalStatus__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {actionlib_msgs__msg__GoalStatus__FIELD_NAME__text, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription actionlib_msgs__msg__GoalStatus__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {actionlib_msgs__msg__GoalID__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
actionlib_msgs__msg__GoalStatus__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {actionlib_msgs__msg__GoalStatus__TYPE_NAME, 29, 29},
      {actionlib_msgs__msg__GoalStatus__FIELDS, 3, 3},
    },
    {actionlib_msgs__msg__GoalStatus__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&actionlib_msgs__msg__GoalID__EXPECTED_HASH, actionlib_msgs__msg__GoalID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = actionlib_msgs__msg__GoalID__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "GoalID goal_id\n"
  "uint8 status\n"
  "uint8 PENDING         = 0   # The goal has yet to be processed by the action server.\n"
  "uint8 ACTIVE          = 1   # The goal is currently being processed by the action server.\n"
  "uint8 PREEMPTED       = 2   # The goal received a cancel request after it started executing\n"
  "                            #   and has since completed its execution (Terminal State).\n"
  "uint8 SUCCEEDED       = 3   # The goal was achieved successfully by the action server\n"
  "                            #   (Terminal State).\n"
  "uint8 ABORTED         = 4   # The goal was aborted during execution by the action server due\n"
  "                            #    to some failure (Terminal State).\n"
  "uint8 REJECTED        = 5   # The goal was rejected by the action server without being processed,\n"
  "                            #    because the goal was unattainable or invalid (Terminal State).\n"
  "uint8 PREEMPTING      = 6   # The goal received a cancel request after it started executing\n"
  "                            #    and has not yet completed execution.\n"
  "uint8 RECALLING       = 7   # The goal received a cancel request before it started executing, but\n"
  "                            #    the action server has not yet confirmed that the goal is canceled.\n"
  "uint8 RECALLED        = 8   # The goal received a cancel request before it started executing\n"
  "                            #    and was successfully cancelled (Terminal State).\n"
  "uint8 LOST            = 9   # An action client can determine that a goal is LOST. This should not\n"
  "                            #    be sent over the wire by an action server.\n"
  "\n"
  "# Allow for the user to associate a string with GoalStatus for debugging.\n"
  "string text";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
actionlib_msgs__msg__GoalStatus__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {actionlib_msgs__msg__GoalStatus__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1669, 1669},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
actionlib_msgs__msg__GoalStatus__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *actionlib_msgs__msg__GoalStatus__get_individual_type_description_source(NULL),
    sources[1] = *actionlib_msgs__msg__GoalID__get_individual_type_description_source(NULL);
    sources[2] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
