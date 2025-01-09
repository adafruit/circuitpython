// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lifecycle_msgs:msg/State.idl
// generated code does not contain a copyright notice

#include "lifecycle_msgs/msg/detail/state__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__msg__State__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdd, 0x2d, 0x02, 0xb8, 0x2f, 0x3e, 0xbc, 0x85,
      0x8e, 0x53, 0xc4, 0x31, 0xb1, 0xe6, 0xe9, 0x1f,
      0x3f, 0xfc, 0x71, 0x43, 0x6f, 0xc8, 0x1d, 0x07,
      0x15, 0x21, 0x4a, 0xc6, 0xee, 0x21, 0x07, 0xa0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char lifecycle_msgs__msg__State__TYPE_NAME[] = "lifecycle_msgs/msg/State";

// Define type names, field names, and default values
static char lifecycle_msgs__msg__State__FIELD_NAME__id[] = "id";
static char lifecycle_msgs__msg__State__FIELD_NAME__label[] = "label";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__msg__State__FIELDS[] = {
  {
    {lifecycle_msgs__msg__State__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__State__FIELD_NAME__label, 5, 5},
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
lifecycle_msgs__msg__State__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
      {lifecycle_msgs__msg__State__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Primary state definitions as depicted in:\n"
  "# http://design.ros2.org/articles/node_lifecycle.html\n"
  "\n"
  "# These are the primary states. State changes can only be requested when the\n"
  "# node is in one of these states.\n"
  "\n"
  "# Indicates state has not yet been set.\n"
  "uint8 PRIMARY_STATE_UNKNOWN = 0\n"
  "\n"
  "# This is the life cycle state the node is in immediately after being\n"
  "# instantiated.\n"
  "uint8 PRIMARY_STATE_UNCONFIGURED = 1\n"
  "\n"
  "# This state represents a node that is not currently performing any processing.\n"
  "uint8 PRIMARY_STATE_INACTIVE = 2\n"
  "\n"
  "# This is the main state of the node's life cycle. While in this state, the node\n"
  "# performs any processing, responds to service requests, reads and processes\n"
  "# data, produces output, etc.\n"
  "uint8 PRIMARY_STATE_ACTIVE = 3\n"
  "\n"
  "# The finalized state is the state in which the node ends immediately before\n"
  "# being destroyed.\n"
  "uint8 PRIMARY_STATE_FINALIZED = 4\n"
  "\n"
  "# Temporary intermediate states. When a transition is requested, the node\n"
  "# changes its state into one of these states.\n"
  "\n"
  "# In this transition state the node's onConfigure callback will be called to\n"
  "# allow the node to load its configuration and conduct any required setup.\n"
  "uint8 TRANSITION_STATE_CONFIGURING = 10\n"
  "\n"
  "# In this transition state the node's callback onCleanup will be called to clear\n"
  "# all state and return the node to a functionally equivalent state as when\n"
  "# first created.\n"
  "uint8 TRANSITION_STATE_CLEANINGUP = 11\n"
  "\n"
  "# In this transition state the callback onShutdown will be executed to do any\n"
  "# cleanup necessary before destruction.\n"
  "uint8 TRANSITION_STATE_SHUTTINGDOWN = 12\n"
  "\n"
  "# In this transition state the callback onActivate will be executed to do any\n"
  "# final preparations to start executing.\n"
  "uint8 TRANSITION_STATE_ACTIVATING = 13\n"
  "\n"
  "# In this transition state the callback onDeactivate will be executed to do any\n"
  "# cleanup to start executing, and reverse the onActivate changes.\n"
  "uint8 TRANSITION_STATE_DEACTIVATING = 14\n"
  "\n"
  "# This transition state is where any error may be cleaned up.\n"
  "uint8 TRANSITION_STATE_ERRORPROCESSING = 15\n"
  "\n"
  "# The state id value from the above definitions.\n"
  "uint8 id\n"
  "\n"
  "# A text label of the state.\n"
  "string label";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__msg__State__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 2115, 2115},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__msg__State__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__msg__State__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
