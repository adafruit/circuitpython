// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:srv/Trigger.idl
// generated code does not contain a copyright notice

#include "example_interfaces/srv/detail/trigger__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__Trigger__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xcf, 0xee, 0xee, 0x47, 0xf8, 0x10, 0x5d, 0xd7,
      0x68, 0x5e, 0x4c, 0x92, 0xd4, 0x6d, 0x40, 0x74,
      0x66, 0x9c, 0xb1, 0xc4, 0x77, 0x40, 0x2b, 0xe1,
      0xde, 0xa3, 0x74, 0x86, 0x54, 0x2a, 0x69, 0xe0,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__Trigger_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x25, 0x21, 0x57, 0x1b, 0xb7, 0xfb, 0x4d, 0xe9,
      0x4d, 0xa0, 0x45, 0x72, 0x0b, 0x06, 0x9b, 0x28,
      0x59, 0x08, 0x4c, 0x68, 0x9e, 0xe2, 0x73, 0x82,
      0xf4, 0x84, 0x14, 0xeb, 0xf4, 0xdd, 0x3a, 0x8d,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__Trigger_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3f, 0xaa, 0x1e, 0x36, 0xb8, 0x34, 0xf5, 0x70,
      0x5a, 0x7e, 0x9a, 0xa9, 0x90, 0xc4, 0x72, 0x0c,
      0x08, 0x2f, 0x06, 0x30, 0xbf, 0x28, 0xab, 0xed,
      0xf3, 0x15, 0xcc, 0x69, 0xf0, 0x1d, 0xc8, 0xfc,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__Trigger_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x37, 0x21, 0x6e, 0x82, 0x64, 0x6e, 0x85, 0xe9,
      0xda, 0xcb, 0x76, 0x81, 0x10, 0x00, 0xfc, 0x4c,
      0x5e, 0x30, 0x3d, 0xd4, 0xef, 0x29, 0xb4, 0x15,
      0x1c, 0x18, 0x9a, 0x3f, 0xa7, 0x34, 0x29, 0x6f,
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

static char example_interfaces__srv__Trigger__TYPE_NAME[] = "example_interfaces/srv/Trigger";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char example_interfaces__srv__Trigger_Event__TYPE_NAME[] = "example_interfaces/srv/Trigger_Event";
static char example_interfaces__srv__Trigger_Request__TYPE_NAME[] = "example_interfaces/srv/Trigger_Request";
static char example_interfaces__srv__Trigger_Response__TYPE_NAME[] = "example_interfaces/srv/Trigger_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char example_interfaces__srv__Trigger__FIELD_NAME__request_message[] = "request_message";
static char example_interfaces__srv__Trigger__FIELD_NAME__response_message[] = "response_message";
static char example_interfaces__srv__Trigger__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__Trigger__FIELDS[] = {
  {
    {example_interfaces__srv__Trigger__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__Trigger_Event__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription example_interfaces__srv__Trigger__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Event__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__srv__Trigger__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__Trigger__TYPE_NAME, 30, 30},
      {example_interfaces__srv__Trigger__FIELDS, 3, 3},
    },
    {example_interfaces__srv__Trigger__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = example_interfaces__srv__Trigger_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = example_interfaces__srv__Trigger_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = example_interfaces__srv__Trigger_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__Trigger_Request__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__Trigger_Request__FIELDS[] = {
  {
    {example_interfaces__srv__Trigger_Request__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
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
example_interfaces__srv__Trigger_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
      {example_interfaces__srv__Trigger_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__Trigger_Response__FIELD_NAME__success[] = "success";
static char example_interfaces__srv__Trigger_Response__FIELD_NAME__message[] = "message";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__Trigger_Response__FIELDS[] = {
  {
    {example_interfaces__srv__Trigger_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Response__FIELD_NAME__message, 7, 7},
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
example_interfaces__srv__Trigger_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
      {example_interfaces__srv__Trigger_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__Trigger_Event__FIELD_NAME__info[] = "info";
static char example_interfaces__srv__Trigger_Event__FIELD_NAME__request[] = "request";
static char example_interfaces__srv__Trigger_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__Trigger_Event__FIELDS[] = {
  {
    {example_interfaces__srv__Trigger_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription example_interfaces__srv__Trigger_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__srv__Trigger_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__Trigger_Event__TYPE_NAME, 36, 36},
      {example_interfaces__srv__Trigger_Event__FIELDS, 3, 3},
    },
    {example_interfaces__srv__Trigger_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = example_interfaces__srv__Trigger_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = example_interfaces__srv__Trigger_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# An example of a trigger service.\n"
  "# This can be used for testing but a semantically meaningful\n"
  "# one should be created to be built upon.\n"
  "\n"
  "---\n"
  "bool success   # indicate successful run of triggered service\n"
  "string message # informational, e.g. for error messages.";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__Trigger__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__Trigger__TYPE_NAME, 30, 30},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 262, 262},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__Trigger_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__Trigger_Request__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__Trigger_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__Trigger_Response__TYPE_NAME, 39, 39},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__Trigger_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__Trigger_Event__TYPE_NAME, 36, 36},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__Trigger__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__Trigger__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *example_interfaces__srv__Trigger_Event__get_individual_type_description_source(NULL);
    sources[3] = *example_interfaces__srv__Trigger_Request__get_individual_type_description_source(NULL);
    sources[4] = *example_interfaces__srv__Trigger_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__Trigger_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__Trigger_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__Trigger_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__Trigger_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__Trigger_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__Trigger_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *example_interfaces__srv__Trigger_Request__get_individual_type_description_source(NULL);
    sources[3] = *example_interfaces__srv__Trigger_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
