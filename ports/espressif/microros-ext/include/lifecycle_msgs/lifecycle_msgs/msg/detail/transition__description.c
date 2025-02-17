// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lifecycle_msgs:msg/Transition.idl
// generated code does not contain a copyright notice

#include "lifecycle_msgs/msg/detail/transition__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__msg__Transition__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc6, 0x5d, 0x7b, 0x31, 0xea, 0x13, 0x4c, 0xba,
      0x4f, 0x54, 0xfc, 0x86, 0x7b, 0x81, 0x79, 0x79,
      0xbe, 0x79, 0x9f, 0x74, 0x52, 0x03, 0x5c, 0xd3,
      0x5f, 0xac, 0x9b, 0x74, 0x21, 0xfb, 0x34, 0x24,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char lifecycle_msgs__msg__Transition__TYPE_NAME[] = "lifecycle_msgs/msg/Transition";

// Define type names, field names, and default values
static char lifecycle_msgs__msg__Transition__FIELD_NAME__id[] = "id";
static char lifecycle_msgs__msg__Transition__FIELD_NAME__label[] = "label";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__msg__Transition__FIELDS[] = {
  {
    {lifecycle_msgs__msg__Transition__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__FIELD_NAME__label, 5, 5},
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
lifecycle_msgs__msg__Transition__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
      {lifecycle_msgs__msg__Transition__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Default values for transitions as described in:\n"
  "# http://design.ros2.org/articles/node_lifecycle.html\n"
  "\n"
  "# Reserved [0-9], publicly available transitions.\n"
  "# When a node is in one of these primary states, these transitions can be\n"
  "# invoked.\n"
  "\n"
  "# This transition will instantiate the node, but will not run any code beyond\n"
  "# the constructor.\n"
  "uint8 TRANSITION_CREATE = 0\n"
  "\n"
  "# The node's onConfigure callback will be called to allow the node to load its\n"
  "# configuration and conduct any required setup.\n"
  "uint8 TRANSITION_CONFIGURE = 1\n"
  "\n"
  "# The node's callback onCleanup will be called in this transition to allow the\n"
  "# node to load its configuration and conduct any required setup.\n"
  "uint8 TRANSITION_CLEANUP = 2\n"
  "\n"
  "# The node's callback onActivate will be executed to do any final preparations\n"
  "# to start executing.\n"
  "uint8 TRANSITION_ACTIVATE = 3\n"
  "\n"
  "# The node's callback onDeactivate will be executed to do any cleanup to start\n"
  "# executing, and reverse the onActivate changes.\n"
  "uint8 TRANSITION_DEACTIVATE = 4\n"
  "\n"
  "# This signals shutdown during an unconfigured state, the node's callback\n"
  "# onShutdown will be executed to do any cleanup necessary before destruction.\n"
  "uint8 TRANSITION_UNCONFIGURED_SHUTDOWN  = 5\n"
  "\n"
  "# This signals shutdown during an inactive state, the node's callback onShutdown\n"
  "# will be executed to do any cleanup necessary before destruction.\n"
  "uint8 TRANSITION_INACTIVE_SHUTDOWN = 6\n"
  "\n"
  "# This signals shutdown during an active state, the node's callback onShutdown\n"
  "# will be executed to do any cleanup necessary before destruction.\n"
  "uint8 TRANSITION_ACTIVE_SHUTDOWN = 7\n"
  "\n"
  "# This transition will simply cause the deallocation of the node.\n"
  "uint8 TRANSITION_DESTROY = 8\n"
  "\n"
  "# Reserved [10-69], private transitions\n"
  "# These transitions are not publicly available and cannot be invoked by a user.\n"
  "# The following transitions are implicitly invoked based on the callback\n"
  "# feedback of the intermediate transition states.\n"
  "uint8 TRANSITION_ON_CONFIGURE_SUCCESS = 10\n"
  "uint8 TRANSITION_ON_CONFIGURE_FAILURE = 11\n"
  "uint8 TRANSITION_ON_CONFIGURE_ERROR = 12\n"
  "\n"
  "uint8 TRANSITION_ON_CLEANUP_SUCCESS = 20\n"
  "uint8 TRANSITION_ON_CLEANUP_FAILURE = 21\n"
  "uint8 TRANSITION_ON_CLEANUP_ERROR = 22\n"
  "\n"
  "uint8 TRANSITION_ON_ACTIVATE_SUCCESS = 30\n"
  "uint8 TRANSITION_ON_ACTIVATE_FAILURE = 31\n"
  "uint8 TRANSITION_ON_ACTIVATE_ERROR = 32\n"
  "\n"
  "uint8 TRANSITION_ON_DEACTIVATE_SUCCESS = 40\n"
  "uint8 TRANSITION_ON_DEACTIVATE_FAILURE = 41\n"
  "uint8 TRANSITION_ON_DEACTIVATE_ERROR = 42\n"
  "\n"
  "uint8 TRANSITION_ON_SHUTDOWN_SUCCESS = 50\n"
  "uint8 TRANSITION_ON_SHUTDOWN_FAILURE = 51\n"
  "uint8 TRANSITION_ON_SHUTDOWN_ERROR = 52\n"
  "\n"
  "uint8 TRANSITION_ON_ERROR_SUCCESS = 60\n"
  "uint8 TRANSITION_ON_ERROR_FAILURE = 61\n"
  "uint8 TRANSITION_ON_ERROR_ERROR = 62\n"
  "\n"
  "# Reserved [90-99]. Transition callback success values.\n"
  "# These return values ought to be set as a return value for each callback.\n"
  "# Depending on which return value, the transition will be executed correctly or\n"
  "# fallback/error callbacks will be triggered.\n"
  "\n"
  "# The transition callback successfully performed its required functionality.\n"
  "uint8 TRANSITION_CALLBACK_SUCCESS = 97\n"
  "\n"
  "# The transition callback failed to perform its required functionality.\n"
  "uint8 TRANSITION_CALLBACK_FAILURE = 98\n"
  "\n"
  "# The transition callback encountered an error that requires special cleanup, if\n"
  "# possible.\n"
  "uint8 TRANSITION_CALLBACK_ERROR = 99\n"
  "\n"
  "##\n"
  "## Fields\n"
  "##\n"
  "\n"
  "# The transition id from above definitions.\n"
  "uint8 id\n"
  "\n"
  "# A text label of the transition.\n"
  "string label";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__msg__Transition__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 3384, 3384},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__msg__Transition__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
