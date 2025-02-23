// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from diagnostic_msgs:srv/SelfTest.idl
// generated code does not contain a copyright notice

#include "diagnostic_msgs/srv/detail/self_test__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__SelfTest__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdf, 0x1e, 0x4e, 0x54, 0xa8, 0x14, 0xa1, 0x6c,
      0xcc, 0x77, 0x47, 0x1e, 0x92, 0x39, 0x61, 0x8d,
      0x46, 0x81, 0x8e, 0x8d, 0x0e, 0x3b, 0x45, 0xad,
      0x6f, 0x8c, 0x34, 0xc6, 0xe2, 0xab, 0xfe, 0xf6,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__SelfTest_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3b, 0xb9, 0x4b, 0x49, 0x7e, 0x74, 0xe9, 0xbe,
      0xed, 0x2d, 0xdd, 0xc7, 0x21, 0x2d, 0x3a, 0xe5,
      0x96, 0x69, 0x6d, 0x30, 0x41, 0x3b, 0xd1, 0x55,
      0x0f, 0xe9, 0x32, 0xfc, 0x20, 0xef, 0xb8, 0xa4,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__SelfTest_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x48, 0x9d, 0xa8, 0x2e, 0x00, 0xdc, 0xe6, 0x67,
      0x1c, 0xb0, 0xff, 0x42, 0x3a, 0x0f, 0x09, 0x39,
      0xb5, 0x46, 0x33, 0x6f, 0xbf, 0xe4, 0x9d, 0x6c,
      0x7e, 0x64, 0xc9, 0xb8, 0x9f, 0x89, 0x8f, 0x57,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__SelfTest_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x94, 0x9f, 0x4b, 0xd4, 0x18, 0xda, 0xe4, 0x83,
      0x87, 0x9d, 0x64, 0x90, 0x12, 0xbe, 0xff, 0x10,
      0x52, 0x76, 0xe5, 0xb7, 0x5b, 0x3c, 0x74, 0x45,
      0x69, 0x3c, 0x70, 0xdc, 0xa1, 0x68, 0x77, 0x18,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "diagnostic_msgs/msg/detail/diagnostic_status__functions.h"
#include "diagnostic_msgs/msg/detail/key_value__functions.h"
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
static const rosidl_type_hash_t diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH = {1, {
    0xb0, 0xe3, 0xe6, 0x92, 0xea, 0x2d, 0x54, 0xa8,
    0xaf, 0x2f, 0x4e, 0xf1, 0x93, 0x0e, 0x81, 0x55,
    0x6a, 0x2d, 0xb5, 0x52, 0x16, 0xb7, 0x71, 0xf8,
    0xa7, 0xd2, 0x72, 0x4e, 0xd4, 0x7b, 0xf0, 0xe4,
  }};
static const rosidl_type_hash_t diagnostic_msgs__msg__KeyValue__EXPECTED_HASH = {1, {
    0xd6, 0x80, 0x81, 0xea, 0xa5, 0x40, 0x28, 0x8c,
    0x54, 0x40, 0x75, 0x3b, 0xae, 0xce, 0xf0, 0xc4,
    0xe1, 0x6e, 0x81, 0xa5, 0xf7, 0x8a, 0xd6, 0x89,
    0x02, 0xde, 0xd5, 0x10, 0x04, 0x13, 0xbb, 0x42,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char diagnostic_msgs__srv__SelfTest__TYPE_NAME[] = "diagnostic_msgs/srv/SelfTest";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME[] = "diagnostic_msgs/msg/DiagnosticStatus";
static char diagnostic_msgs__msg__KeyValue__TYPE_NAME[] = "diagnostic_msgs/msg/KeyValue";
static char diagnostic_msgs__srv__SelfTest_Event__TYPE_NAME[] = "diagnostic_msgs/srv/SelfTest_Event";
static char diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME[] = "diagnostic_msgs/srv/SelfTest_Request";
static char diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME[] = "diagnostic_msgs/srv/SelfTest_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char diagnostic_msgs__srv__SelfTest__FIELD_NAME__request_message[] = "request_message";
static char diagnostic_msgs__srv__SelfTest__FIELD_NAME__response_message[] = "response_message";
static char diagnostic_msgs__srv__SelfTest__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__SelfTest__FIELDS[] = {
  {
    {diagnostic_msgs__srv__SelfTest__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__SelfTest_Event__TYPE_NAME, 34, 34},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__srv__SelfTest__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Event__TYPE_NAME, 34, 34},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__srv__SelfTest__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__SelfTest__TYPE_NAME, 28, 28},
      {diagnostic_msgs__srv__SelfTest__FIELDS, 3, 3},
    },
    {diagnostic_msgs__srv__SelfTest__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH, diagnostic_msgs__msg__DiagnosticStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__msg__DiagnosticStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__KeyValue__EXPECTED_HASH, diagnostic_msgs__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = diagnostic_msgs__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = diagnostic_msgs__srv__SelfTest_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = diagnostic_msgs__srv__SelfTest_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = diagnostic_msgs__srv__SelfTest_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__SelfTest_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__SelfTest_Request__FIELDS[] = {
  {
    {diagnostic_msgs__srv__SelfTest_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
diagnostic_msgs__srv__SelfTest_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
      {diagnostic_msgs__srv__SelfTest_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__id[] = "id";
static char diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__passed[] = "passed";
static char diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__status[] = "status";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__SelfTest_Response__FIELDS[] = {
  {
    {diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__passed, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Response__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__srv__SelfTest_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__srv__SelfTest_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
      {diagnostic_msgs__srv__SelfTest_Response__FIELDS, 3, 3},
    },
    {diagnostic_msgs__srv__SelfTest_Response__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH, diagnostic_msgs__msg__DiagnosticStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = diagnostic_msgs__msg__DiagnosticStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__KeyValue__EXPECTED_HASH, diagnostic_msgs__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__info[] = "info";
static char diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__request[] = "request";
static char diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__SelfTest_Event__FIELDS[] = {
  {
    {diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__srv__SelfTest_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__srv__SelfTest_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__SelfTest_Event__TYPE_NAME, 34, 34},
      {diagnostic_msgs__srv__SelfTest_Event__FIELDS, 3, 3},
    },
    {diagnostic_msgs__srv__SelfTest_Event__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH, diagnostic_msgs__msg__DiagnosticStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__msg__DiagnosticStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__KeyValue__EXPECTED_HASH, diagnostic_msgs__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = diagnostic_msgs__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = diagnostic_msgs__srv__SelfTest_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = diagnostic_msgs__srv__SelfTest_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "---\n"
  "string id\n"
  "byte passed\n"
  "DiagnosticStatus[] status";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__SelfTest__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__SelfTest__TYPE_NAME, 28, 28},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 52, 52},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__SelfTest_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__SelfTest_Request__TYPE_NAME, 36, 36},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__SelfTest_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__SelfTest_Response__TYPE_NAME, 37, 37},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__SelfTest_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__SelfTest_Event__TYPE_NAME, 34, 34},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__SelfTest__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__SelfTest__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(NULL);
    sources[3] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[4] = *diagnostic_msgs__srv__SelfTest_Event__get_individual_type_description_source(NULL);
    sources[5] = *diagnostic_msgs__srv__SelfTest_Request__get_individual_type_description_source(NULL);
    sources[6] = *diagnostic_msgs__srv__SelfTest_Response__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__SelfTest_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__SelfTest_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__SelfTest_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__SelfTest_Response__get_individual_type_description_source(NULL),
    sources[1] = *diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__SelfTest_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__SelfTest_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(NULL);
    sources[3] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[4] = *diagnostic_msgs__srv__SelfTest_Request__get_individual_type_description_source(NULL);
    sources[5] = *diagnostic_msgs__srv__SelfTest_Response__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
