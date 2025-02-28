// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from composition_interfaces:srv/ListNodes.idl
// generated code does not contain a copyright notice

#include "composition_interfaces/srv/detail/list_nodes__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__ListNodes__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x13, 0xf3, 0x0d, 0x73, 0xa7, 0x4c, 0x84, 0x4e,
      0xe4, 0xba, 0xcf, 0x78, 0xf3, 0x68, 0x0f, 0x08,
      0x77, 0x7c, 0xad, 0x13, 0x7f, 0x10, 0xfb, 0xd6,
      0xa6, 0x48, 0x54, 0x4c, 0xc7, 0xb5, 0x9d, 0x2a,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__ListNodes_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x57, 0xc8, 0x79, 0xa1, 0x82, 0x5b, 0x3f, 0xa1,
      0x5f, 0xd5, 0x8e, 0x70, 0xd2, 0xe7, 0x32, 0x23,
      0xaf, 0x6b, 0x9c, 0x45, 0xc7, 0xae, 0x10, 0x4c,
      0xc1, 0x07, 0xe7, 0x34, 0x64, 0xd3, 0xa7, 0x24,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__ListNodes_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb7, 0xcd, 0x84, 0x01, 0xae, 0xdc, 0xd8, 0x5c,
      0x4c, 0xed, 0xce, 0x38, 0x49, 0x44, 0x00, 0x1d,
      0x5c, 0x1a, 0xb6, 0x11, 0xe0, 0x16, 0x4a, 0x82,
      0x07, 0x04, 0x5c, 0xfc, 0xaf, 0x4a, 0x25, 0x84,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__ListNodes_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x84, 0x3a, 0xd4, 0xc8, 0x00, 0x92, 0x06, 0x0d,
      0x68, 0xdb, 0xf8, 0x00, 0x8c, 0xe5, 0x33, 0x54,
      0x31, 0x4d, 0x98, 0x25, 0x7c, 0xdd, 0xdd, 0xaf,
      0x55, 0xdf, 0x03, 0xab, 0xd7, 0x32, 0x1b, 0x38,
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

static char composition_interfaces__srv__ListNodes__TYPE_NAME[] = "composition_interfaces/srv/ListNodes";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char composition_interfaces__srv__ListNodes_Event__TYPE_NAME[] = "composition_interfaces/srv/ListNodes_Event";
static char composition_interfaces__srv__ListNodes_Request__TYPE_NAME[] = "composition_interfaces/srv/ListNodes_Request";
static char composition_interfaces__srv__ListNodes_Response__TYPE_NAME[] = "composition_interfaces/srv/ListNodes_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char composition_interfaces__srv__ListNodes__FIELD_NAME__request_message[] = "request_message";
static char composition_interfaces__srv__ListNodes__FIELD_NAME__response_message[] = "response_message";
static char composition_interfaces__srv__ListNodes__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__ListNodes__FIELDS[] = {
  {
    {composition_interfaces__srv__ListNodes__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__ListNodes_Event__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__ListNodes__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Event__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__ListNodes__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__ListNodes__TYPE_NAME, 36, 36},
      {composition_interfaces__srv__ListNodes__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__ListNodes__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__ListNodes_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__ListNodes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = composition_interfaces__srv__ListNodes_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__ListNodes_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__ListNodes_Request__FIELDS[] = {
  {
    {composition_interfaces__srv__ListNodes_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
composition_interfaces__srv__ListNodes_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
      {composition_interfaces__srv__ListNodes_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__ListNodes_Response__FIELD_NAME__full_node_names[] = "full_node_names";
static char composition_interfaces__srv__ListNodes_Response__FIELD_NAME__unique_ids[] = "unique_ids";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__ListNodes_Response__FIELDS[] = {
  {
    {composition_interfaces__srv__ListNodes_Response__FIELD_NAME__full_node_names, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Response__FIELD_NAME__unique_ids, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__ListNodes_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
      {composition_interfaces__srv__ListNodes_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__ListNodes_Event__FIELD_NAME__info[] = "info";
static char composition_interfaces__srv__ListNodes_Event__FIELD_NAME__request[] = "request";
static char composition_interfaces__srv__ListNodes_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__ListNodes_Event__FIELDS[] = {
  {
    {composition_interfaces__srv__ListNodes_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__ListNodes_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__ListNodes_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__ListNodes_Event__TYPE_NAME, 42, 42},
      {composition_interfaces__srv__ListNodes_Event__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__ListNodes_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__ListNodes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__ListNodes_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "---\n"
  "# List of full node names including namespace.\n"
  "string[] full_node_names\n"
  "# corresponding unique ids (must have same length as full_node_names).\n"
  "uint64[] unique_ids";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__ListNodes__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__ListNodes__TYPE_NAME, 36, 36},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 167, 167},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__ListNodes_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__ListNodes_Request__TYPE_NAME, 44, 44},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__ListNodes_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__ListNodes_Response__TYPE_NAME, 45, 45},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__ListNodes_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__ListNodes_Event__TYPE_NAME, 42, 42},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__ListNodes__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__ListNodes__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__ListNodes_Event__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__ListNodes_Request__get_individual_type_description_source(NULL);
    sources[4] = *composition_interfaces__srv__ListNodes_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__ListNodes_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__ListNodes_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__ListNodes_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__ListNodes_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__ListNodes_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__ListNodes_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__ListNodes_Request__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__ListNodes_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
