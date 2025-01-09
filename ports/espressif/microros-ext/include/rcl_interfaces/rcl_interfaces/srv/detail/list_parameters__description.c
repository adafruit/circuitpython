// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:srv/ListParameters.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/srv/detail/list_parameters__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__ListParameters__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3e, 0x60, 0x62, 0xbf, 0xbb, 0x27, 0xbf, 0xb8,
      0x73, 0x0d, 0x4c, 0xef, 0x25, 0x58, 0x22, 0x1f,
      0x51, 0xa1, 0x16, 0x46, 0xd7, 0x8e, 0x7b, 0xb3,
      0x0a, 0x1e, 0x83, 0xaf, 0xac, 0x3a, 0xad, 0x9d,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__ListParameters_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa1, 0xb0, 0xb5, 0xd6, 0xb9, 0x67, 0xa5, 0xb8,
      0xac, 0x5b, 0xc8, 0x56, 0x3c, 0x3e, 0xa6, 0x78,
      0xf3, 0x49, 0xe3, 0x12, 0xa6, 0xa4, 0x96, 0x72,
      0x27, 0xc5, 0xa9, 0x6b, 0xc5, 0xce, 0x38, 0xdf,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__ListParameters_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x31, 0xb9, 0x37, 0xb8, 0x2d, 0xe3, 0x61, 0x24,
      0x26, 0xbe, 0x79, 0xf2, 0x31, 0xe3, 0x86, 0x25,
      0x9f, 0x97, 0xe8, 0xdc, 0x84, 0x3e, 0x3f, 0x28,
      0x45, 0x28, 0x8b, 0x4b, 0xfa, 0x29, 0x6b, 0xdd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__ListParameters_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x89, 0xc5, 0xb5, 0xaa, 0xf6, 0x8e, 0xc9, 0x25,
      0xae, 0x9b, 0xbc, 0x55, 0x69, 0xee, 0x21, 0xde,
      0x82, 0x95, 0x9b, 0x4d, 0x14, 0x96, 0xdb, 0xd4,
      0x98, 0x79, 0x2a, 0xd8, 0x25, 0xb9, 0xb2, 0x92,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "rcl_interfaces/msg/detail/list_parameters_result__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__ListParametersResult__EXPECTED_HASH = {1, {
    0x23, 0x7a, 0xe3, 0x42, 0x84, 0x13, 0xdc, 0xbc,
    0xfb, 0x45, 0x2b, 0x51, 0x0c, 0x42, 0x35, 0x5f,
    0x3a, 0x2b, 0x02, 0x1d, 0xc0, 0x91, 0xaf, 0xa3,
    0xe1, 0x85, 0x26, 0xd5, 0x70, 0x22, 0xf1, 0xcd,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char rcl_interfaces__srv__ListParameters__TYPE_NAME[] = "rcl_interfaces/srv/ListParameters";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rcl_interfaces__msg__ListParametersResult__TYPE_NAME[] = "rcl_interfaces/msg/ListParametersResult";
static char rcl_interfaces__srv__ListParameters_Event__TYPE_NAME[] = "rcl_interfaces/srv/ListParameters_Event";
static char rcl_interfaces__srv__ListParameters_Request__TYPE_NAME[] = "rcl_interfaces/srv/ListParameters_Request";
static char rcl_interfaces__srv__ListParameters_Response__TYPE_NAME[] = "rcl_interfaces/srv/ListParameters_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rcl_interfaces__srv__ListParameters__FIELD_NAME__request_message[] = "request_message";
static char rcl_interfaces__srv__ListParameters__FIELD_NAME__response_message[] = "response_message";
static char rcl_interfaces__srv__ListParameters__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__ListParameters__FIELDS[] = {
  {
    {rcl_interfaces__srv__ListParameters__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__ListParameters_Event__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__ListParameters__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Event__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__ListParameters__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__ListParameters__TYPE_NAME, 33, 33},
      {rcl_interfaces__srv__ListParameters__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__ListParameters__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ListParametersResult__EXPECTED_HASH, rcl_interfaces__msg__ListParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__ListParametersResult__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__ListParameters_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__srv__ListParameters_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__ListParameters_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__ListParameters_Request__FIELD_NAME__prefixes[] = "prefixes";
static char rcl_interfaces__srv__ListParameters_Request__FIELD_NAME__depth[] = "depth";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__ListParameters_Request__FIELDS[] = {
  {
    {rcl_interfaces__srv__ListParameters_Request__FIELD_NAME__prefixes, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Request__FIELD_NAME__depth, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__ListParameters_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
      {rcl_interfaces__srv__ListParameters_Request__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__ListParameters_Response__FIELD_NAME__result[] = "result";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__ListParameters_Response__FIELDS[] = {
  {
    {rcl_interfaces__srv__ListParameters_Response__FIELD_NAME__result, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__ListParameters_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__ListParameters_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
      {rcl_interfaces__srv__ListParameters_Response__FIELDS, 1, 1},
    },
    {rcl_interfaces__srv__ListParameters_Response__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__ListParametersResult__EXPECTED_HASH, rcl_interfaces__msg__ListParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__ListParametersResult__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__info[] = "info";
static char rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__request[] = "request";
static char rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__ListParameters_Event__FIELDS[] = {
  {
    {rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__ListParameters_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ListParametersResult__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__ListParameters_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__ListParameters_Event__TYPE_NAME, 39, 39},
      {rcl_interfaces__srv__ListParameters_Event__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__ListParameters_Event__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ListParametersResult__EXPECTED_HASH, rcl_interfaces__msg__ListParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__ListParametersResult__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__ListParameters_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__srv__ListParameters_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Recursively get parameters with unlimited depth.\n"
  "uint64 DEPTH_RECURSIVE=0\n"
  "\n"
  "# The list of parameter prefixes to query.\n"
  "string[] prefixes\n"
  "\n"
  "# Relative depth from given prefixes to return.\n"
  "#\n"
  "# Use DEPTH_RECURSIVE to get the recursive parameters and prefixes for each prefix.\n"
  "uint64 depth\n"
  "\n"
  "---\n"
  "# The list of parameter names and their prefixes.\n"
  "ListParametersResult result";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__ListParameters__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__ListParameters__TYPE_NAME, 33, 33},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 369, 369},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__ListParameters_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__ListParameters_Request__TYPE_NAME, 41, 41},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__ListParameters_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__ListParameters_Response__TYPE_NAME, 42, 42},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__ListParameters_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__ListParameters_Event__TYPE_NAME, 39, 39},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__ListParameters__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__ListParameters__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__ListParametersResult__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__ListParameters_Event__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__srv__ListParameters_Request__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__ListParameters_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__ListParameters_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__ListParameters_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__ListParameters_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__ListParameters_Response__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__ListParametersResult__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__ListParameters_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__ListParameters_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__ListParametersResult__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__ListParameters_Request__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__srv__ListParameters_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
