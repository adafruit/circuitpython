// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:srv/Empty.idl
// generated code does not contain a copyright notice

#include "test_msgs/srv/detail/empty__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Empty__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x97, 0x37, 0x67, 0x9d, 0x04, 0x95, 0x0f, 0xec,
      0xab, 0x59, 0xb9, 0xe8, 0x2e, 0xc2, 0xbf, 0x28,
      0x5f, 0x58, 0x44, 0x6c, 0xe8, 0xe4, 0x8d, 0x28,
      0x7e, 0x13, 0xb8, 0xa9, 0xf1, 0x9f, 0x08, 0x23,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Empty_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xab, 0x04, 0x6f, 0x24, 0xc1, 0xca, 0x55, 0xcd,
      0xb5, 0xe7, 0x7f, 0x81, 0xa5, 0xbf, 0x1b, 0xa9,
      0x2c, 0x4b, 0xd9, 0xa5, 0xf6, 0x75, 0x60, 0x1d,
      0x4f, 0x9a, 0x48, 0xfe, 0xf7, 0x0a, 0x4e, 0x89,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Empty_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x72, 0x58, 0xf2, 0x80, 0x34, 0xbb, 0x37, 0x47,
      0xd3, 0x74, 0x3b, 0x73, 0xf2, 0xe0, 0x99, 0xd9,
      0xeb, 0x73, 0x9f, 0x92, 0xb4, 0xbf, 0x6d, 0x69,
      0x57, 0xd1, 0xa8, 0xe9, 0xdf, 0x04, 0x3a, 0xd8,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Empty_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3d, 0x58, 0xc3, 0x87, 0x25, 0x2d, 0xad, 0xcd,
      0x64, 0x22, 0xbd, 0xd1, 0x3b, 0xf9, 0x20, 0xd3,
      0xbd, 0xb2, 0xfe, 0x27, 0x89, 0xaa, 0xe6, 0x3b,
      0xf9, 0xdf, 0x38, 0x26, 0x7c, 0xa1, 0x50, 0x05,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
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
#endif

static char test_msgs__srv__Empty__TYPE_NAME[] = "test_msgs/srv/Empty";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char test_msgs__srv__Empty_Event__TYPE_NAME[] = "test_msgs/srv/Empty_Event";
static char test_msgs__srv__Empty_Request__TYPE_NAME[] = "test_msgs/srv/Empty_Request";
static char test_msgs__srv__Empty_Response__TYPE_NAME[] = "test_msgs/srv/Empty_Response";

// Define type names, field names, and default values
static char test_msgs__srv__Empty__FIELD_NAME__request_message[] = "request_message";
static char test_msgs__srv__Empty__FIELD_NAME__response_message[] = "response_message";
static char test_msgs__srv__Empty__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Empty__FIELDS[] = {
  {
    {test_msgs__srv__Empty__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Empty_Event__TYPE_NAME, 25, 25},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Empty__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Event__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Empty__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Empty__TYPE_NAME, 19, 19},
      {test_msgs__srv__Empty__FIELDS, 3, 3},
    },
    {test_msgs__srv__Empty__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = test_msgs__srv__Empty_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = test_msgs__srv__Empty_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = test_msgs__srv__Empty_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Empty_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Empty_Request__FIELDS[] = {
  {
    {test_msgs__srv__Empty_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
test_msgs__srv__Empty_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
      {test_msgs__srv__Empty_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Empty_Response__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Empty_Response__FIELDS[] = {
  {
    {test_msgs__srv__Empty_Response__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
test_msgs__srv__Empty_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
      {test_msgs__srv__Empty_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Empty_Event__FIELD_NAME__info[] = "info";
static char test_msgs__srv__Empty_Event__FIELD_NAME__request[] = "request";
static char test_msgs__srv__Empty_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Empty_Event__FIELDS[] = {
  {
    {test_msgs__srv__Empty_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Empty_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Empty_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Empty_Event__TYPE_NAME, 25, 25},
      {test_msgs__srv__Empty_Event__FIELDS, 3, 3},
    },
    {test_msgs__srv__Empty_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = test_msgs__srv__Empty_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = test_msgs__srv__Empty_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "---";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Empty__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Empty__TYPE_NAME, 19, 19},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 4, 4},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Empty_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Empty_Request__TYPE_NAME, 27, 27},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Empty_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Empty_Response__TYPE_NAME, 28, 28},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Empty_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Empty_Event__TYPE_NAME, 25, 25},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Empty__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Empty__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__srv__Empty_Event__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__srv__Empty_Request__get_individual_type_description_source(NULL);
    sources[5] = *test_msgs__srv__Empty_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Empty_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Empty_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Empty_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Empty_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Empty_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Empty_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__srv__Empty_Request__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__srv__Empty_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
