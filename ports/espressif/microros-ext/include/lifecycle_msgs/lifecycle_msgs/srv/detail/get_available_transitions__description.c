// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lifecycle_msgs:srv/GetAvailableTransitions.idl
// generated code does not contain a copyright notice

#include "lifecycle_msgs/srv/detail/get_available_transitions__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__GetAvailableTransitions__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x59, 0xb7, 0xec, 0xef, 0xce, 0x09, 0x82, 0xa8,
      0xa8, 0x44, 0xb9, 0xf2, 0xc4, 0xf1, 0x47, 0x64,
      0xc1, 0xc4, 0x54, 0x3c, 0xc5, 0x5e, 0x72, 0x92,
      0x4e, 0x2a, 0xa4, 0xad, 0xad, 0x83, 0xe9, 0xbc,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__GetAvailableTransitions_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x52, 0xa7, 0x3c, 0x0b, 0x5d, 0x74, 0x89, 0xbe,
      0xe0, 0xdf, 0x6d, 0xdb, 0x0a, 0xec, 0x45, 0xc4,
      0x12, 0x89, 0xd2, 0x23, 0xff, 0x22, 0x5b, 0x58,
      0xfe, 0x11, 0xd8, 0x59, 0xac, 0x81, 0x09, 0xa6,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__GetAvailableTransitions_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x47, 0x11, 0x33, 0x94, 0x61, 0xaa, 0xdf, 0x02,
      0xbc, 0xff, 0xf7, 0x97, 0xa1, 0x98, 0xde, 0x91,
      0x1f, 0x88, 0x50, 0x74, 0xbd, 0x3f, 0x2f, 0xe9,
      0xf7, 0xb6, 0x77, 0xef, 0x87, 0xf0, 0x74, 0x1c,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__GetAvailableTransitions_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x18, 0x8e, 0x7d, 0xe0, 0x07, 0x99, 0x3d, 0xa8,
      0x73, 0xcc, 0x24, 0x7d, 0x52, 0x90, 0x61, 0x51,
      0xb3, 0x83, 0x46, 0xfd, 0xb7, 0x2a, 0x4c, 0xc5,
      0xf7, 0x8c, 0x40, 0x7c, 0x3b, 0x5b, 0x37, 0x0f,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "lifecycle_msgs/msg/detail/transition__functions.h"
#include "lifecycle_msgs/msg/detail/state__functions.h"
#include "lifecycle_msgs/msg/detail/transition_description__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
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
static const rosidl_type_hash_t lifecycle_msgs__msg__TransitionDescription__EXPECTED_HASH = {1, {
    0xc5, 0xf1, 0xcd, 0x4b, 0xb1, 0xad, 0x2b, 0xa0,
    0xe3, 0x32, 0x9d, 0x4a, 0xc7, 0x01, 0x5c, 0x52,
    0xa6, 0x74, 0xa7, 0x2c, 0x1f, 0xaf, 0x79, 0x74,
    0xc3, 0x7a, 0x33, 0xf4, 0xf6, 0x04, 0x8b, 0x28,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char lifecycle_msgs__srv__GetAvailableTransitions__TYPE_NAME[] = "lifecycle_msgs/srv/GetAvailableTransitions";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char lifecycle_msgs__msg__State__TYPE_NAME[] = "lifecycle_msgs/msg/State";
static char lifecycle_msgs__msg__Transition__TYPE_NAME[] = "lifecycle_msgs/msg/Transition";
static char lifecycle_msgs__msg__TransitionDescription__TYPE_NAME[] = "lifecycle_msgs/msg/TransitionDescription";
static char lifecycle_msgs__srv__GetAvailableTransitions_Event__TYPE_NAME[] = "lifecycle_msgs/srv/GetAvailableTransitions_Event";
static char lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME[] = "lifecycle_msgs/srv/GetAvailableTransitions_Request";
static char lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME[] = "lifecycle_msgs/srv/GetAvailableTransitions_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__request_message[] = "request_message";
static char lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__response_message[] = "response_message";
static char lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__GetAvailableTransitions__FIELDS[] = {
  {
    {lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__GetAvailableTransitions_Event__TYPE_NAME, 48, 48},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__GetAvailableTransitions__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__TYPE_NAME, 48, 48},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__GetAvailableTransitions__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__GetAvailableTransitions__TYPE_NAME, 42, 42},
      {lifecycle_msgs__srv__GetAvailableTransitions__FIELDS, 3, 3},
    },
    {lifecycle_msgs__srv__GetAvailableTransitions__REFERENCED_TYPE_DESCRIPTIONS, 8, 8},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__State__EXPECTED_HASH, lifecycle_msgs__msg__State__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__State__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__TransitionDescription__EXPECTED_HASH, lifecycle_msgs__msg__TransitionDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = lifecycle_msgs__msg__TransitionDescription__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lifecycle_msgs__srv__GetAvailableTransitions_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = lifecycle_msgs__srv__GetAvailableTransitions_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = lifecycle_msgs__srv__GetAvailableTransitions_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__GetAvailableTransitions_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__GetAvailableTransitions_Request__FIELDS[] = {
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__GetAvailableTransitions_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
      {lifecycle_msgs__srv__GetAvailableTransitions_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__GetAvailableTransitions_Response__FIELD_NAME__available_transitions[] = "available_transitions";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__GetAvailableTransitions_Response__FIELDS[] = {
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Response__FIELD_NAME__available_transitions, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__GetAvailableTransitions_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__GetAvailableTransitions_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
      {lifecycle_msgs__srv__GetAvailableTransitions_Response__FIELDS, 1, 1},
    },
    {lifecycle_msgs__srv__GetAvailableTransitions_Response__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&lifecycle_msgs__msg__State__EXPECTED_HASH, lifecycle_msgs__msg__State__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = lifecycle_msgs__msg__State__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__TransitionDescription__EXPECTED_HASH, lifecycle_msgs__msg__TransitionDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = lifecycle_msgs__msg__TransitionDescription__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__info[] = "info";
static char lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__request[] = "request";
static char lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELDS[] = {
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__GetAvailableTransitions_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__State__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__TransitionDescription__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__GetAvailableTransitions_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__GetAvailableTransitions_Event__TYPE_NAME, 48, 48},
      {lifecycle_msgs__srv__GetAvailableTransitions_Event__FIELDS, 3, 3},
    },
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__State__EXPECTED_HASH, lifecycle_msgs__msg__State__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__State__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__TransitionDescription__EXPECTED_HASH, lifecycle_msgs__msg__TransitionDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = lifecycle_msgs__msg__TransitionDescription__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lifecycle_msgs__srv__GetAvailableTransitions_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = lifecycle_msgs__srv__GetAvailableTransitions_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "---\n"
  "# An array of the possible start_state-goal_state transitions\n"
  "TransitionDescription[] available_transitions";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__GetAvailableTransitions__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__GetAvailableTransitions__TYPE_NAME, 42, 42},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 112, 112},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__GetAvailableTransitions_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__GetAvailableTransitions_Request__TYPE_NAME, 50, 50},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__GetAvailableTransitions_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__GetAvailableTransitions_Response__TYPE_NAME, 51, 51},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__GetAvailableTransitions_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__GetAvailableTransitions_Event__TYPE_NAME, 48, 48},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__GetAvailableTransitions__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[9];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 9, 9};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__GetAvailableTransitions__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__State__get_individual_type_description_source(NULL);
    sources[3] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    sources[4] = *lifecycle_msgs__msg__TransitionDescription__get_individual_type_description_source(NULL);
    sources[5] = *lifecycle_msgs__srv__GetAvailableTransitions_Event__get_individual_type_description_source(NULL);
    sources[6] = *lifecycle_msgs__srv__GetAvailableTransitions_Request__get_individual_type_description_source(NULL);
    sources[7] = *lifecycle_msgs__srv__GetAvailableTransitions_Response__get_individual_type_description_source(NULL);
    sources[8] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__GetAvailableTransitions_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__GetAvailableTransitions_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__GetAvailableTransitions_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__GetAvailableTransitions_Response__get_individual_type_description_source(NULL),
    sources[1] = *lifecycle_msgs__msg__State__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    sources[3] = *lifecycle_msgs__msg__TransitionDescription__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__GetAvailableTransitions_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__GetAvailableTransitions_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__State__get_individual_type_description_source(NULL);
    sources[3] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    sources[4] = *lifecycle_msgs__msg__TransitionDescription__get_individual_type_description_source(NULL);
    sources[5] = *lifecycle_msgs__srv__GetAvailableTransitions_Request__get_individual_type_description_source(NULL);
    sources[6] = *lifecycle_msgs__srv__GetAvailableTransitions_Response__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
