// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from actionlib_msgs:msg/GoalStatusArray.idl
// generated code does not contain a copyright notice

#include "actionlib_msgs/msg/detail/goal_status_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_type_hash_t *
actionlib_msgs__msg__GoalStatusArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf3, 0x23, 0x02, 0x08, 0xe8, 0xda, 0xec, 0x53,
      0xeb, 0x7b, 0x8d, 0xa6, 0xdf, 0x1c, 0x1d, 0x1d,
      0x4e, 0xb0, 0x95, 0xf3, 0x08, 0x2b, 0xe9, 0x22,
      0x66, 0x1c, 0x29, 0xa1, 0xf8, 0x55, 0xa1, 0xbc,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "actionlib_msgs/msg/detail/goal_status__functions.h"
#include "actionlib_msgs/msg/detail/goal_id__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t actionlib_msgs__msg__GoalID__EXPECTED_HASH = {1, {
    0x14, 0x0e, 0xa4, 0xa1, 0xba, 0xd4, 0x78, 0x1e,
    0xdc, 0x32, 0x03, 0x0c, 0x79, 0x83, 0xab, 0x3a,
    0x62, 0xc6, 0x75, 0xf8, 0x61, 0xc6, 0x1c, 0x9b,
    0x22, 0x00, 0xae, 0x5e, 0x53, 0x98, 0xbe, 0x24,
  }};
static const rosidl_type_hash_t actionlib_msgs__msg__GoalStatus__EXPECTED_HASH = {1, {
    0xc9, 0x0c, 0x64, 0x23, 0x09, 0x54, 0xd9, 0x0e,
    0xe0, 0x79, 0xcb, 0xdd, 0x22, 0xa1, 0x4b, 0xca,
    0x36, 0x50, 0xb3, 0xd4, 0x03, 0xf5, 0xcc, 0x3f,
    0xaf, 0x0e, 0xee, 0x99, 0x2c, 0x6a, 0xe9, 0xff,
  }};
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char actionlib_msgs__msg__GoalStatusArray__TYPE_NAME[] = "actionlib_msgs/msg/GoalStatusArray";
static char actionlib_msgs__msg__GoalID__TYPE_NAME[] = "actionlib_msgs/msg/GoalID";
static char actionlib_msgs__msg__GoalStatus__TYPE_NAME[] = "actionlib_msgs/msg/GoalStatus";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char actionlib_msgs__msg__GoalStatusArray__FIELD_NAME__header[] = "header";
static char actionlib_msgs__msg__GoalStatusArray__FIELD_NAME__status_list[] = "status_list";

static rosidl_runtime_c__type_description__Field actionlib_msgs__msg__GoalStatusArray__FIELDS[] = {
  {
    {actionlib_msgs__msg__GoalStatusArray__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {actionlib_msgs__msg__GoalStatusArray__FIELD_NAME__status_list, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {actionlib_msgs__msg__GoalStatus__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription actionlib_msgs__msg__GoalStatusArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {actionlib_msgs__msg__GoalID__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {actionlib_msgs__msg__GoalStatus__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
actionlib_msgs__msg__GoalStatusArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {actionlib_msgs__msg__GoalStatusArray__TYPE_NAME, 34, 34},
      {actionlib_msgs__msg__GoalStatusArray__FIELDS, 2, 2},
    },
    {actionlib_msgs__msg__GoalStatusArray__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&actionlib_msgs__msg__GoalID__EXPECTED_HASH, actionlib_msgs__msg__GoalID__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = actionlib_msgs__msg__GoalID__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&actionlib_msgs__msg__GoalStatus__EXPECTED_HASH, actionlib_msgs__msg__GoalStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = actionlib_msgs__msg__GoalStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Stores the statuses for goals that are currently being tracked\n"
  "# by an action server\n"
  "std_msgs/Header header\n"
  "GoalStatus[] status_list";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
actionlib_msgs__msg__GoalStatusArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {actionlib_msgs__msg__GoalStatusArray__TYPE_NAME, 34, 34},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 135, 135},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
actionlib_msgs__msg__GoalStatusArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *actionlib_msgs__msg__GoalStatusArray__get_individual_type_description_source(NULL),
    sources[1] = *actionlib_msgs__msg__GoalID__get_individual_type_description_source(NULL);
    sources[2] = *actionlib_msgs__msg__GoalStatus__get_individual_type_description_source(NULL);
    sources[3] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[4] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
