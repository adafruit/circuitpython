// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from example_interfaces:srv/SetBool.idl
// generated code does not contain a copyright notice

#include "example_interfaces/srv/detail/set_bool__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__SetBool__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa6, 0x97, 0x82, 0xe5, 0x63, 0x1b, 0x12, 0xe1,
      0x5c, 0x8e, 0x21, 0x84, 0x10, 0xde, 0x16, 0x85,
      0xbb, 0xf1, 0x3e, 0x38, 0x27, 0x18, 0x29, 0x5a,
      0xda, 0xd1, 0x40, 0x37, 0xa2, 0x4a, 0xfb, 0xe8,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__SetBool_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdb, 0x31, 0xa9, 0x14, 0x6d, 0xe9, 0xf5, 0x8c,
      0x31, 0x96, 0xef, 0x92, 0xeb, 0xc4, 0x3a, 0xbc,
      0x46, 0x01, 0x99, 0xad, 0xfa, 0x57, 0xec, 0xe5,
      0x15, 0x50, 0xf3, 0x75, 0x44, 0xc4, 0xee, 0x58,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__SetBool_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xfd, 0x35, 0xd6, 0x97, 0x4b, 0x0e, 0xde, 0x7f,
      0xad, 0x12, 0x7f, 0x60, 0x06, 0x19, 0x71, 0x9e,
      0xb7, 0xca, 0xf8, 0xb0, 0xff, 0x8b, 0x02, 0xa4,
      0xa5, 0xa1, 0x03, 0x90, 0x04, 0x79, 0xa6, 0x19,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_example_interfaces
const rosidl_type_hash_t *
example_interfaces__srv__SetBool_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x02, 0x72, 0xd5, 0x15, 0x06, 0x75, 0x58, 0x2a,
      0xf7, 0x90, 0xe0, 0xfb, 0xff, 0x2b, 0xb4, 0x01,
      0x12, 0xf6, 0x1f, 0xcc, 0x62, 0x65, 0x1f, 0xff,
      0xf0, 0x20, 0x74, 0xec, 0x98, 0xb4, 0xdd, 0x2c,
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

static char example_interfaces__srv__SetBool__TYPE_NAME[] = "example_interfaces/srv/SetBool";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char example_interfaces__srv__SetBool_Event__TYPE_NAME[] = "example_interfaces/srv/SetBool_Event";
static char example_interfaces__srv__SetBool_Request__TYPE_NAME[] = "example_interfaces/srv/SetBool_Request";
static char example_interfaces__srv__SetBool_Response__TYPE_NAME[] = "example_interfaces/srv/SetBool_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char example_interfaces__srv__SetBool__FIELD_NAME__request_message[] = "request_message";
static char example_interfaces__srv__SetBool__FIELD_NAME__response_message[] = "response_message";
static char example_interfaces__srv__SetBool__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__SetBool__FIELDS[] = {
  {
    {example_interfaces__srv__SetBool__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {example_interfaces__srv__SetBool_Event__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription example_interfaces__srv__SetBool__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Event__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__srv__SetBool__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__SetBool__TYPE_NAME, 30, 30},
      {example_interfaces__srv__SetBool__FIELDS, 3, 3},
    },
    {example_interfaces__srv__SetBool__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = example_interfaces__srv__SetBool_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = example_interfaces__srv__SetBool_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = example_interfaces__srv__SetBool_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__SetBool_Request__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__SetBool_Request__FIELDS[] = {
  {
    {example_interfaces__srv__SetBool_Request__FIELD_NAME__data, 4, 4},
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
example_interfaces__srv__SetBool_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
      {example_interfaces__srv__SetBool_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__SetBool_Response__FIELD_NAME__success[] = "success";
static char example_interfaces__srv__SetBool_Response__FIELD_NAME__message[] = "message";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__SetBool_Response__FIELDS[] = {
  {
    {example_interfaces__srv__SetBool_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Response__FIELD_NAME__message, 7, 7},
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
example_interfaces__srv__SetBool_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
      {example_interfaces__srv__SetBool_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char example_interfaces__srv__SetBool_Event__FIELD_NAME__info[] = "info";
static char example_interfaces__srv__SetBool_Event__FIELD_NAME__request[] = "request";
static char example_interfaces__srv__SetBool_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field example_interfaces__srv__SetBool_Event__FIELDS[] = {
  {
    {example_interfaces__srv__SetBool_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription example_interfaces__srv__SetBool_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
example_interfaces__srv__SetBool_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {example_interfaces__srv__SetBool_Event__TYPE_NAME, 36, 36},
      {example_interfaces__srv__SetBool_Event__FIELDS, 3, 3},
    },
    {example_interfaces__srv__SetBool_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = example_interfaces__srv__SetBool_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = example_interfaces__srv__SetBool_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This is an example of a service to set a boolean value.\n"
  "# This can be used for testing but a semantically meaningful\n"
  "# one should be created to be built upon.\n"
  "\n"
  "bool data # e.g. for hardware enabling / disabling\n"
  "---\n"
  "bool success   # indicate successful run of triggered service\n"
  "string message # informational, e.g. for error messages";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__SetBool__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__SetBool__TYPE_NAME, 30, 30},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 335, 335},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__SetBool_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__SetBool_Request__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__SetBool_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__SetBool_Response__TYPE_NAME, 39, 39},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
example_interfaces__srv__SetBool_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {example_interfaces__srv__SetBool_Event__TYPE_NAME, 36, 36},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__SetBool__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__SetBool__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *example_interfaces__srv__SetBool_Event__get_individual_type_description_source(NULL);
    sources[3] = *example_interfaces__srv__SetBool_Request__get_individual_type_description_source(NULL);
    sources[4] = *example_interfaces__srv__SetBool_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__SetBool_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__SetBool_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__SetBool_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__SetBool_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
example_interfaces__srv__SetBool_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *example_interfaces__srv__SetBool_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *example_interfaces__srv__SetBool_Request__get_individual_type_description_source(NULL);
    sources[3] = *example_interfaces__srv__SetBool_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
