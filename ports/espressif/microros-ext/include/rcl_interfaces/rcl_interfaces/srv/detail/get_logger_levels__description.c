// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:srv/GetLoggerLevels.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/srv/detail/get_logger_levels__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetLoggerLevels__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x03, 0xbf, 0x1b, 0xeb, 0xd0, 0xd6, 0x51, 0x4c,
      0x7e, 0xd0, 0xba, 0x7c, 0x5e, 0x08, 0xdc, 0x9f,
      0x2f, 0x39, 0xc7, 0x59, 0xfe, 0x99, 0xe1, 0xe3,
      0x0e, 0xa4, 0x15, 0x7d, 0x76, 0x74, 0xf7, 0x2d,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetLoggerLevels_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x49, 0x0a, 0xce, 0x42, 0x4a, 0x5f, 0x5b, 0x14,
      0xcb, 0x4e, 0x1e, 0x95, 0x03, 0x0d, 0x7f, 0xe3,
      0x66, 0xd8, 0xdf, 0x0f, 0xd9, 0xfe, 0x4d, 0x03,
      0xb4, 0x4d, 0x60, 0x09, 0x50, 0x4f, 0xd2, 0xcc,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetLoggerLevels_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x22, 0x86, 0x85, 0xd3, 0x50, 0x91, 0x1d, 0x21,
      0x30, 0x47, 0xab, 0xe7, 0x56, 0xf5, 0xbf, 0x56,
      0x6d, 0x2f, 0x76, 0xb3, 0x59, 0xc4, 0xe9, 0xfa,
      0xc1, 0x6e, 0x5e, 0xf6, 0x52, 0xaf, 0xf9, 0x10,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetLoggerLevels_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xc8, 0x35, 0xa5, 0x86, 0xd8, 0x32, 0xcb, 0x17,
      0xc0, 0x1c, 0x51, 0x2f, 0x8f, 0xfd, 0x44, 0x9f,
      0xd5, 0x38, 0x4f, 0x5b, 0xd3, 0x3f, 0xb4, 0x4d,
      0x76, 0x32, 0x9d, 0x87, 0x5d, 0x1f, 0x08, 0x75,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "rcl_interfaces/msg/detail/logger_level__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__LoggerLevel__EXPECTED_HASH = {1, {
    0x95, 0x78, 0x5c, 0xc4, 0x2f, 0x04, 0x8a, 0xb4,
    0xf3, 0x95, 0xaf, 0x65, 0x03, 0x5a, 0xea, 0xf2,
    0x18, 0x1d, 0x8e, 0x1c, 0x7a, 0x44, 0xed, 0xb8,
    0xad, 0x45, 0x58, 0x44, 0x5f, 0xdb, 0x43, 0xc0,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char rcl_interfaces__srv__GetLoggerLevels__TYPE_NAME[] = "rcl_interfaces/srv/GetLoggerLevels";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rcl_interfaces__msg__LoggerLevel__TYPE_NAME[] = "rcl_interfaces/msg/LoggerLevel";
static char rcl_interfaces__srv__GetLoggerLevels_Event__TYPE_NAME[] = "rcl_interfaces/srv/GetLoggerLevels_Event";
static char rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME[] = "rcl_interfaces/srv/GetLoggerLevels_Request";
static char rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME[] = "rcl_interfaces/srv/GetLoggerLevels_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__request_message[] = "request_message";
static char rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__response_message[] = "response_message";
static char rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetLoggerLevels__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetLoggerLevels_Event__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__GetLoggerLevels__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Event__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetLoggerLevels__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetLoggerLevels__TYPE_NAME, 34, 34},
      {rcl_interfaces__srv__GetLoggerLevels__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__GetLoggerLevels__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__LoggerLevel__EXPECTED_HASH, rcl_interfaces__msg__LoggerLevel__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__LoggerLevel__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__GetLoggerLevels_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__srv__GetLoggerLevels_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__GetLoggerLevels_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetLoggerLevels_Request__FIELD_NAME__names[] = "names";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetLoggerLevels_Request__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetLoggerLevels_Request__FIELD_NAME__names, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetLoggerLevels_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
      {rcl_interfaces__srv__GetLoggerLevels_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetLoggerLevels_Response__FIELD_NAME__levels[] = "levels";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetLoggerLevels_Response__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetLoggerLevels_Response__FIELD_NAME__levels, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__GetLoggerLevels_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetLoggerLevels_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
      {rcl_interfaces__srv__GetLoggerLevels_Response__FIELDS, 1, 1},
    },
    {rcl_interfaces__srv__GetLoggerLevels_Response__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__LoggerLevel__EXPECTED_HASH, rcl_interfaces__msg__LoggerLevel__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__LoggerLevel__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__info[] = "info";
static char rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__request[] = "request";
static char rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetLoggerLevels_Event__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__GetLoggerLevels_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetLoggerLevels_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetLoggerLevels_Event__TYPE_NAME, 40, 40},
      {rcl_interfaces__srv__GetLoggerLevels_Event__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__GetLoggerLevels_Event__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__LoggerLevel__EXPECTED_HASH, rcl_interfaces__msg__LoggerLevel__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__LoggerLevel__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__GetLoggerLevels_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__srv__GetLoggerLevels_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A list of logger names to get.\n"
  "string[] names\n"
  "\n"
  "---\n"
  "# List of logger levels which is the same order as the provided names. If a\n"
  "# logger was not yet set, the value will have 'unknown' as the value.\n"
  "LoggerLevel[] levels";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetLoggerLevels__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetLoggerLevels__TYPE_NAME, 34, 34},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 220, 220},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetLoggerLevels_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetLoggerLevels_Request__TYPE_NAME, 42, 42},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetLoggerLevels_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetLoggerLevels_Response__TYPE_NAME, 43, 43},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetLoggerLevels_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetLoggerLevels_Event__TYPE_NAME, 40, 40},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetLoggerLevels__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetLoggerLevels__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__LoggerLevel__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__GetLoggerLevels_Event__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__srv__GetLoggerLevels_Request__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__GetLoggerLevels_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetLoggerLevels_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetLoggerLevels_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetLoggerLevels_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetLoggerLevels_Response__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__LoggerLevel__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetLoggerLevels_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetLoggerLevels_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__LoggerLevel__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__GetLoggerLevels_Request__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__srv__GetLoggerLevels_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
