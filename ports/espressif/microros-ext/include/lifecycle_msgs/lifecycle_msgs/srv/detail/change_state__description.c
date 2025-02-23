// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lifecycle_msgs:srv/ChangeState.idl
// generated code does not contain a copyright notice

#include "lifecycle_msgs/srv/detail/change_state__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__ChangeState__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x35, 0x6f, 0xe3, 0x4f, 0x04, 0x75, 0xa4, 0x3a,
      0xcf, 0x54, 0x54, 0x20, 0x13, 0xaf, 0x41, 0x67,
      0xb0, 0xe7, 0x29, 0xf7, 0x7e, 0xa2, 0x2f, 0xfb,
      0x04, 0x5c, 0x6a, 0xd8, 0xe2, 0x06, 0x68, 0xe5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__ChangeState_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xd7, 0x4c, 0x6f, 0x52, 0x4e, 0x14, 0xca, 0xde,
      0x7b, 0x67, 0xe7, 0x29, 0x71, 0x90, 0x36, 0x0e,
      0xd7, 0x49, 0x26, 0x4a, 0xf3, 0x00, 0x52, 0x4f,
      0xc1, 0x45, 0x6f, 0x27, 0xcc, 0x2a, 0x55, 0x3a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__ChangeState_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe1, 0xdd, 0xfe, 0xd0, 0x27, 0x1f, 0xef, 0xe6,
      0x37, 0x67, 0xaa, 0x71, 0x56, 0x19, 0x26, 0xe4,
      0xd9, 0xea, 0xc0, 0xf1, 0xdd, 0xf9, 0xe7, 0x1a,
      0x88, 0x73, 0xeb, 0x46, 0xb3, 0x6d, 0xea, 0xc2,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_lifecycle_msgs
const rosidl_type_hash_t *
lifecycle_msgs__srv__ChangeState_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xbf, 0x7f, 0x35, 0x5a, 0xca, 0x87, 0x62, 0xa7,
      0xa5, 0x1d, 0xf0, 0x2e, 0xaf, 0x4a, 0x51, 0x02,
      0x42, 0x55, 0xd1, 0x5b, 0x31, 0xb1, 0xb6, 0x8b,
      0xf0, 0xa0, 0x99, 0x67, 0x3c, 0x27, 0x09, 0xbc,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "lifecycle_msgs/msg/detail/transition__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t lifecycle_msgs__msg__Transition__EXPECTED_HASH = {1, {
    0xc6, 0x5d, 0x7b, 0x31, 0xea, 0x13, 0x4c, 0xba,
    0x4f, 0x54, 0xfc, 0x86, 0x7b, 0x81, 0x79, 0x79,
    0xbe, 0x79, 0x9f, 0x74, 0x52, 0x03, 0x5c, 0xd3,
    0x5f, 0xac, 0x9b, 0x74, 0x21, 0xfb, 0x34, 0x24,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char lifecycle_msgs__srv__ChangeState__TYPE_NAME[] = "lifecycle_msgs/srv/ChangeState";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char lifecycle_msgs__msg__Transition__TYPE_NAME[] = "lifecycle_msgs/msg/Transition";
static char lifecycle_msgs__srv__ChangeState_Event__TYPE_NAME[] = "lifecycle_msgs/srv/ChangeState_Event";
static char lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME[] = "lifecycle_msgs/srv/ChangeState_Request";
static char lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME[] = "lifecycle_msgs/srv/ChangeState_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char lifecycle_msgs__srv__ChangeState__FIELD_NAME__request_message[] = "request_message";
static char lifecycle_msgs__srv__ChangeState__FIELD_NAME__response_message[] = "response_message";
static char lifecycle_msgs__srv__ChangeState__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__ChangeState__FIELDS[] = {
  {
    {lifecycle_msgs__srv__ChangeState__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__srv__ChangeState_Event__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__ChangeState__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Event__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__ChangeState__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__ChangeState__TYPE_NAME, 30, 30},
      {lifecycle_msgs__srv__ChangeState__FIELDS, 3, 3},
    },
    {lifecycle_msgs__srv__ChangeState__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lifecycle_msgs__srv__ChangeState_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lifecycle_msgs__srv__ChangeState_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = lifecycle_msgs__srv__ChangeState_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__ChangeState_Request__FIELD_NAME__transition[] = "transition";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__ChangeState_Request__FIELDS[] = {
  {
    {lifecycle_msgs__srv__ChangeState_Request__FIELD_NAME__transition, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__ChangeState_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__ChangeState_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
      {lifecycle_msgs__srv__ChangeState_Request__FIELDS, 1, 1},
    },
    {lifecycle_msgs__srv__ChangeState_Request__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__ChangeState_Response__FIELD_NAME__success[] = "success";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__ChangeState_Response__FIELDS[] = {
  {
    {lifecycle_msgs__srv__ChangeState_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__ChangeState_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
      {lifecycle_msgs__srv__ChangeState_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__info[] = "info";
static char lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__request[] = "request";
static char lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field lifecycle_msgs__srv__ChangeState_Event__FIELDS[] = {
  {
    {lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription lifecycle_msgs__srv__ChangeState_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__msg__Transition__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lifecycle_msgs__srv__ChangeState_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lifecycle_msgs__srv__ChangeState_Event__TYPE_NAME, 36, 36},
      {lifecycle_msgs__srv__ChangeState_Event__FIELDS, 3, 3},
    },
    {lifecycle_msgs__srv__ChangeState_Event__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&lifecycle_msgs__msg__Transition__EXPECTED_HASH, lifecycle_msgs__msg__Transition__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = lifecycle_msgs__msg__Transition__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = lifecycle_msgs__srv__ChangeState_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = lifecycle_msgs__srv__ChangeState_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# The requested transition.\n"
  "#\n"
  "# This change state service will fail if the transition is not possible.\n"
  "Transition transition\n"
  "---\n"
  "\n"
  "# Indicates whether the service was able to initiate the state transition\n"
  "bool success";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__ChangeState__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__ChangeState__TYPE_NAME, 30, 30},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 217, 217},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__ChangeState_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__ChangeState_Request__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__ChangeState_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__ChangeState_Response__TYPE_NAME, 39, 39},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
lifecycle_msgs__srv__ChangeState_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lifecycle_msgs__srv__ChangeState_Event__TYPE_NAME, 36, 36},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__ChangeState__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__ChangeState__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    sources[3] = *lifecycle_msgs__srv__ChangeState_Event__get_individual_type_description_source(NULL);
    sources[4] = *lifecycle_msgs__srv__ChangeState_Request__get_individual_type_description_source(NULL);
    sources[5] = *lifecycle_msgs__srv__ChangeState_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__ChangeState_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__ChangeState_Request__get_individual_type_description_source(NULL),
    sources[1] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__ChangeState_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__ChangeState_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lifecycle_msgs__srv__ChangeState_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lifecycle_msgs__srv__ChangeState_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *lifecycle_msgs__msg__Transition__get_individual_type_description_source(NULL);
    sources[3] = *lifecycle_msgs__srv__ChangeState_Request__get_individual_type_description_source(NULL);
    sources[4] = *lifecycle_msgs__srv__ChangeState_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
