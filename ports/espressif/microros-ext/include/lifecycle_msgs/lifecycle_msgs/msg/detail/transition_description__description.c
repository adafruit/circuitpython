// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lifecycle_msgs:msg/TransitionDescription.idl
// generated code does not contain a copyright notice

#include "lifecycle_msgs/msg/detail/transition_description__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__msg__TransitionDescription__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc5, 0xf1, 0xcd, 0x4b, 0xb1, 0xad, 0x2b, 0xa0,
      0xe3, 0x32, 0x9d, 0x4a, 0xc7, 0x01, 0x5c, 0x52,
      0xa6, 0x74, 0xa7, 0x2c, 0x1f, 0xaf, 0x79, 0x74,
      0xc3, 0x7a, 0x33, 0xf4, 0xf6, 0x04, 0x8b, 0x28,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "lifecycle_msgs/msg/detail/transition__functions.h"
#include "lifecycle_msgs/msg/detail/state__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t lifecycle_msgs__msg__State__EXPECTED_HASH = {1, {
    0xdd, 0x2d, 0x02, 0xb8, 0x2f, 0x3e, 0xbc, 0x85,
    0x8e, 0x53, 0xc4, 0x31, 0xb1, 0xe6, 0xe9, 0x1f,
    0x3f, 0xfc, 0x71, 0x43, 0x6f, 0xc8, 0x1d, 0x07,
    0x15, 0x21, 0x4a, 0xc6, 0xee, 0x21, 0x07, 0xa0,
  }};
static const rosidl_type_hash_t lifecycle_msgs__msg__Transition__EXPECTED_HASH = {1, {
    0xc6, 0x5d, 0x7b, 0x31, 0xea, 0x13, 0x4c, 0xba,
    0x4f, 0x54, 0xfc, 0x86, 0x7b, 0x81, 0x79, 0x79,
    0xbe, 0x79, 0x9f, 0x74, 0x52, 0x03, 0x5c, 0xd3,
    0x5f, 0xac, 0x9b, 0x74, 0x21, 0xfb, 0x34, 0x24,
  }};
#endif

static char lifecycle_msgs__msg__TransitionDescription__TYPE_NAME[] = "lifecycle_msgs/msg/TransitionDescription";
static char lifecycle_msgs__msg__State__TYPE_NAME[] = "lifecycle_msgs/msg/State";
static char lifecycle_msgs__msg__Transition__TYPE_NAME[] = "lifecycle_msgs/msg/Transition";

// Define type names, field names, and default values
static char lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__transition[] = "transition";
static char lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__start_state[] = "start_state";
static char lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__goal_state[] = "goal_state";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__msg__TransitionDescription__FIELDS[] = {
  {
    {lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__transition, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__start_state, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__TransitionDescription__FIELD_NAME__goal_state, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__msg__TransitionDescription__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__msg__TransitionDescription__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
      {lifecycle_msgs__msg__TransitionDescription__FIELDS, 3, 3},
    },
    {lifecycle_msgs__msg__TransitionDescription__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&lifecycle_msgs__msg__State__EXPECTED_HASH, lifecycle_msgs__msg__State__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = lifecycle_msgs__msg__State__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# The transition id and label of this description.\n"
  "Transition transition\n"
  "\n"
  "# The current state from which this transition transitions.\n"
  "State start_state\n"
  "\n"
  "# The desired target state of this transition.\n"
  "State goal_state";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__msg__TransitionDescription__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 217, 217},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__msg__TransitionDescription__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__msg__TransitionDescription__get_individual_type_description_source(NULL),
    sources[1] = *lifecycle_msgs__msg__State__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
