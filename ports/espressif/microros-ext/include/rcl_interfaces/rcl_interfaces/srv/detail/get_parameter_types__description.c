// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:srv/GetParameterTypes.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/srv/detail/get_parameter_types__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetParameterTypes__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xda, 0x19, 0x9c, 0x87, 0x86, 0x88, 0xb3, 0xe5,
      0x30, 0xbd, 0xfe, 0x3c, 0xa8, 0xf7, 0x4c, 0xb9,
      0xfa, 0x0c, 0x30, 0x31, 0x01, 0xe9, 0x80, 0xa9,
      0xe8, 0xf2, 0x60, 0xe2, 0x5e, 0x1c, 0x80, 0xca,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetParameterTypes_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x87, 0xd7, 0xec, 0x5e, 0xf5, 0x45, 0xd4, 0xda,
      0xa2, 0x89, 0xd5, 0x00, 0x02, 0x5d, 0x31, 0xfb,
      0x24, 0x37, 0xbc, 0x8b, 0x83, 0x65, 0xca, 0x36,
      0x71, 0x61, 0xc4, 0xda, 0xc4, 0xbe, 0x33, 0xfd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetParameterTypes_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xac, 0x13, 0x35, 0xdc, 0xe6, 0x7c, 0x47, 0x8e,
      0x66, 0x94, 0x34, 0xf6, 0xac, 0x2f, 0xbf, 0xf3,
      0x4f, 0xcd, 0x52, 0x95, 0x80, 0xf2, 0x72, 0xd3,
      0x00, 0x67, 0x85, 0xba, 0x8b, 0x54, 0xe4, 0x7f,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__GetParameterTypes_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x95, 0x16, 0xad, 0xca, 0x0c, 0xc8, 0x85, 0xd4,
      0x6a, 0x59, 0xc2, 0x36, 0x2f, 0xb8, 0xeb, 0x5f,
      0x49, 0x06, 0xb0, 0x46, 0x51, 0x82, 0x66, 0xf8,
      0xc6, 0x45, 0xb4, 0x54, 0xb0, 0xdd, 0xdc, 0x60,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

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

static char rcl_interfaces__srv__GetParameterTypes__TYPE_NAME[] = "rcl_interfaces/srv/GetParameterTypes";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rcl_interfaces__srv__GetParameterTypes_Event__TYPE_NAME[] = "rcl_interfaces/srv/GetParameterTypes_Event";
static char rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME[] = "rcl_interfaces/srv/GetParameterTypes_Request";
static char rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME[] = "rcl_interfaces/srv/GetParameterTypes_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__request_message[] = "request_message";
static char rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__response_message[] = "response_message";
static char rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetParameterTypes__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__GetParameterTypes_Event__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__GetParameterTypes__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Event__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetParameterTypes__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetParameterTypes__TYPE_NAME, 36, 36},
      {rcl_interfaces__srv__GetParameterTypes__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__GetParameterTypes__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__srv__GetParameterTypes_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__GetParameterTypes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__srv__GetParameterTypes_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetParameterTypes_Request__FIELD_NAME__names[] = "names";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetParameterTypes_Request__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetParameterTypes_Request__FIELD_NAME__names, 5, 5},
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
rcl_interfaces__srv__GetParameterTypes_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
      {rcl_interfaces__srv__GetParameterTypes_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetParameterTypes_Response__FIELD_NAME__types[] = "types";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetParameterTypes_Response__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetParameterTypes_Response__FIELD_NAME__types, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetParameterTypes_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
      {rcl_interfaces__srv__GetParameterTypes_Response__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__info[] = "info";
static char rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__request[] = "request";
static char rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__GetParameterTypes_Event__FIELDS[] = {
  {
    {rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__GetParameterTypes_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__GetParameterTypes_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__GetParameterTypes_Event__TYPE_NAME, 42, 42},
      {rcl_interfaces__srv__GetParameterTypes_Event__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__GetParameterTypes_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__srv__GetParameterTypes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__srv__GetParameterTypes_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A list of parameter names.\n"
  "# TODO(wjwwood): link to parameter naming rules.\n"
  "string[] names\n"
  "\n"
  "---\n"
  "# List of types which is the same length and order as the provided names.\n"
  "#\n"
  "# The type enum is defined in ParameterType.msg. ParameterType.PARAMETER_NOT_SET\n"
  "# indicates that the parameter is not currently set.\n"
  "uint8[] types";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetParameterTypes__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetParameterTypes__TYPE_NAME, 36, 36},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 322, 322},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetParameterTypes_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetParameterTypes_Request__TYPE_NAME, 44, 44},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetParameterTypes_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetParameterTypes_Response__TYPE_NAME, 45, 45},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__GetParameterTypes_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__GetParameterTypes_Event__TYPE_NAME, 42, 42},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetParameterTypes__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetParameterTypes__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__srv__GetParameterTypes_Event__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__GetParameterTypes_Request__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__srv__GetParameterTypes_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetParameterTypes_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetParameterTypes_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetParameterTypes_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetParameterTypes_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__GetParameterTypes_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__GetParameterTypes_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__srv__GetParameterTypes_Request__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__srv__GetParameterTypes_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
