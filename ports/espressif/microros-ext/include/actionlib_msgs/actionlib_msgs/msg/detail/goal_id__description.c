// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from actionlib_msgs:msg/GoalID.idl
// generated code does not contain a copyright notice

#include "actionlib_msgs/msg/detail/goal_id__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_type_hash_t *
actionlib_msgs__msg__GoalID__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x14, 0x0e, 0xa4, 0xa1, 0xba, 0xd4, 0x78, 0x1e,
      0xdc, 0x32, 0x03, 0x0c, 0x79, 0x83, 0xab, 0x3a,
      0x62, 0xc6, 0x75, 0xf8, 0x61, 0xc6, 0x1c, 0x9b,
      0x22, 0x00, 0xae, 0x5e, 0x53, 0x98, 0xbe, 0x24,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
#endif

static char actionlib_msgs__msg__GoalID__TYPE_NAME[] = "actionlib_msgs/msg/GoalID";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char actionlib_msgs__msg__GoalID__FIELD_NAME__stamp[] = "stamp";
static char actionlib_msgs__msg__GoalID__FIELD_NAME__id[] = "id";

static rosidl_runtime_c__type_description__Field actionlib_msgs__msg__GoalID__FIELDS[] = {
  {
    {actionlib_msgs__msg__GoalID__FIELD_NAME__stamp, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {actionlib_msgs__msg__GoalID__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription actionlib_msgs__msg__GoalID__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
actionlib_msgs__msg__GoalID__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {actionlib_msgs__msg__GoalID__TYPE_NAME, 25, 25},
      {actionlib_msgs__msg__GoalID__FIELDS, 2, 2},
    },
    {actionlib_msgs__msg__GoalID__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "\n"
  "# The stamp should store the time at which this goal was requested.\n"
  "# It is used by an action server when it tries to preempt all\n"
  "# goals that were requested before a certain time\n"
  "builtin_interfaces/Time stamp\n"
  "\n"
  "# The id provides a way to associate feedback and\n"
  "# result message with specific goal requests. The id\n"
  "# specified must be unique.\n"
  "string id";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
actionlib_msgs__msg__GoalID__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {actionlib_msgs__msg__GoalID__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 353, 353},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
actionlib_msgs__msg__GoalID__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *actionlib_msgs__msg__GoalID__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
