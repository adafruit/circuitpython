// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from composition_interfaces:srv/UnloadNode.idl
// generated code does not contain a copyright notice

#include "composition_interfaces/srv/detail/unload_node__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__UnloadNode__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdc, 0xe5, 0x62, 0xf8, 0x3a, 0xdc, 0x73, 0x5d,
      0xdc, 0xfe, 0x9d, 0xfb, 0x92, 0x7b, 0x89, 0x29,
      0x3d, 0x4d, 0xd0, 0xfe, 0x2b, 0x8f, 0xc9, 0x1e,
      0x77, 0x2f, 0xb8, 0x3e, 0x97, 0x0a, 0x43, 0xb0,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__UnloadNode_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x95, 0xd5, 0xcf, 0xc3, 0x57, 0x95, 0x67, 0x40,
      0x19, 0x0c, 0x76, 0x85, 0xac, 0x86, 0x8d, 0x9c,
      0x7c, 0x98, 0xdc, 0xb8, 0xc1, 0x41, 0x6a, 0xae,
      0x8e, 0xaa, 0x35, 0x14, 0x22, 0x07, 0x4e, 0x12,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__UnloadNode_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xfa, 0x7f, 0xc3, 0x8f, 0x92, 0x06, 0x22, 0x0a,
      0x6b, 0xc1, 0x55, 0x8a, 0x3c, 0x01, 0x92, 0x69,
      0xe5, 0xaa, 0x59, 0x2f, 0x7f, 0xae, 0x01, 0xdf,
      0xfb, 0xd1, 0x9f, 0x56, 0xeb, 0x86, 0xd5, 0x99,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__UnloadNode_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x44, 0x8a, 0x52, 0x5d, 0xf1, 0x40, 0x6f, 0x0c,
      0x3d, 0xc6, 0x53, 0xb0, 0x3c, 0x68, 0xcb, 0x80,
      0xd5, 0xc6, 0x7c, 0xff, 0x48, 0xa8, 0x00, 0x6a,
      0x65, 0xf7, 0x6f, 0x13, 0xe2, 0x3e, 0x29, 0xcf,
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

static char composition_interfaces__srv__UnloadNode__TYPE_NAME[] = "composition_interfaces/srv/UnloadNode";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char composition_interfaces__srv__UnloadNode_Event__TYPE_NAME[] = "composition_interfaces/srv/UnloadNode_Event";
static char composition_interfaces__srv__UnloadNode_Request__TYPE_NAME[] = "composition_interfaces/srv/UnloadNode_Request";
static char composition_interfaces__srv__UnloadNode_Response__TYPE_NAME[] = "composition_interfaces/srv/UnloadNode_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char composition_interfaces__srv__UnloadNode__FIELD_NAME__request_message[] = "request_message";
static char composition_interfaces__srv__UnloadNode__FIELD_NAME__response_message[] = "response_message";
static char composition_interfaces__srv__UnloadNode__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__UnloadNode__FIELDS[] = {
  {
    {composition_interfaces__srv__UnloadNode__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__UnloadNode_Event__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__UnloadNode__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Event__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__UnloadNode__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__UnloadNode__TYPE_NAME, 37, 37},
      {composition_interfaces__srv__UnloadNode__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__UnloadNode__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__UnloadNode_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__UnloadNode_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = composition_interfaces__srv__UnloadNode_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__UnloadNode_Request__FIELD_NAME__unique_id[] = "unique_id";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__UnloadNode_Request__FIELDS[] = {
  {
    {composition_interfaces__srv__UnloadNode_Request__FIELD_NAME__unique_id, 9, 9},
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
composition_interfaces__srv__UnloadNode_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
      {composition_interfaces__srv__UnloadNode_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__UnloadNode_Response__FIELD_NAME__success[] = "success";
static char composition_interfaces__srv__UnloadNode_Response__FIELD_NAME__error_message[] = "error_message";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__UnloadNode_Response__FIELDS[] = {
  {
    {composition_interfaces__srv__UnloadNode_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Response__FIELD_NAME__error_message, 13, 13},
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
composition_interfaces__srv__UnloadNode_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
      {composition_interfaces__srv__UnloadNode_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__info[] = "info";
static char composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__request[] = "request";
static char composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__UnloadNode_Event__FIELDS[] = {
  {
    {composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__UnloadNode_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__UnloadNode_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__UnloadNode_Event__TYPE_NAME, 43, 43},
      {composition_interfaces__srv__UnloadNode_Event__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__UnloadNode_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__UnloadNode_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__UnloadNode_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Container specific unique id of a loaded node.\n"
  "uint64 unique_id\n"
  "---\n"
  "# True if the node existed and was unloaded.\n"
  "bool success\n"
  "\n"
  "# Human readable error message if success is false, else empty string.\n"
  "string error_message";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__UnloadNode__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__UnloadNode__TYPE_NAME, 37, 37},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 221, 221},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__UnloadNode_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__UnloadNode_Request__TYPE_NAME, 45, 45},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__UnloadNode_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__UnloadNode_Response__TYPE_NAME, 46, 46},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__UnloadNode_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__UnloadNode_Event__TYPE_NAME, 43, 43},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__UnloadNode__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__UnloadNode__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__UnloadNode_Event__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__UnloadNode_Request__get_individual_type_description_source(NULL);
    sources[4] = *composition_interfaces__srv__UnloadNode_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__UnloadNode_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__UnloadNode_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__UnloadNode_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__UnloadNode_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__UnloadNode_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__UnloadNode_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__UnloadNode_Request__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__UnloadNode_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
