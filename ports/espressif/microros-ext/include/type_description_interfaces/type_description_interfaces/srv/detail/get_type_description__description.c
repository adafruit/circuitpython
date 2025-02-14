// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from type_description_interfaces:srv/GetTypeDescription.idl
// generated code does not contain a copyright notice

#include "type_description_interfaces/srv/detail/get_type_description__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__srv__GetTypeDescription__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x69, 0xb9, 0xc1, 0x9c, 0x10, 0x21, 0x40, 0x59,
      0x84, 0xcc, 0x60, 0xdb, 0xbb, 0x1e, 0xdc, 0xeb,
      0x14, 0x7a, 0x65, 0x38, 0xb4, 0x11, 0xd8, 0x12,
      0xba, 0x6a, 0xfa, 0xbe, 0xed, 0x96, 0x2c, 0xd5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__srv__GetTypeDescription_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x47, 0x0f, 0x56, 0xcc, 0xbc, 0xf1, 0x3d, 0x55,
      0xc7, 0x8c, 0x8b, 0x73, 0x5f, 0xb1, 0x2d, 0xa0,
      0x3e, 0x63, 0x9e, 0x5a, 0xcd, 0xed, 0x99, 0x44,
      0x26, 0xc8, 0x9d, 0x94, 0x77, 0x8d, 0x9e, 0x28,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__srv__GetTypeDescription_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x64, 0x8c, 0x49, 0xd9, 0x76, 0x24, 0x08, 0x61,
      0xa4, 0x82, 0xf9, 0x61, 0xc5, 0x62, 0x1d, 0xdd,
      0x0d, 0x21, 0x35, 0xd8, 0xcc, 0xe6, 0xf2, 0x97,
      0xa7, 0x6d, 0x4f, 0x63, 0x76, 0xd9, 0x3b, 0x75,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_type_description_interfaces
const rosidl_type_hash_t *
type_description_interfaces__srv__GetTypeDescription_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x49, 0x1e, 0x33, 0x50, 0xe7, 0xa0, 0xb1, 0x19,
      0x18, 0xfb, 0x16, 0xa5, 0x23, 0xcf, 0x01, 0xa2,
      0x2d, 0x76, 0xfd, 0xaa, 0x53, 0xc4, 0x3b, 0x52,
      0x16, 0x4f, 0xea, 0x14, 0xaa, 0xbb, 0x56, 0x12,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "type_description_interfaces/msg/detail/field__functions.h"
#include "type_description_interfaces/msg/detail/key_value__functions.h"
#include "type_description_interfaces/msg/detail/type_source__functions.h"
#include "type_description_interfaces/msg/detail/field_type__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "type_description_interfaces/msg/detail/individual_type_description__functions.h"
#include "type_description_interfaces/msg/detail/type_description__functions.h"
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
static const rosidl_type_hash_t type_description_interfaces__msg__Field__EXPECTED_HASH = {1, {
    0xc0, 0xb0, 0x13, 0x79, 0xcd, 0x42, 0x26, 0x28,
    0x12, 0x85, 0xcc, 0xaf, 0x6b, 0xe4, 0x66, 0x53,
    0x96, 0x8f, 0x85, 0x5f, 0x7c, 0x5e, 0x41, 0x61,
    0x4f, 0xf5, 0xd7, 0xa8, 0x54, 0xef, 0xef, 0x7c,
  }};
static const rosidl_type_hash_t type_description_interfaces__msg__FieldType__EXPECTED_HASH = {1, {
    0xa7, 0x0b, 0x6d, 0xd9, 0x19, 0x64, 0x5a, 0x03,
    0xa3, 0x58, 0x6f, 0x7f, 0x82, 0x1d, 0xef, 0xbc,
    0x88, 0x6e, 0xa3, 0xe5, 0x31, 0xa1, 0xd9, 0x5c,
    0xc0, 0xf3, 0x80, 0xa3, 0x97, 0x3c, 0xca, 0xa6,
  }};
static const rosidl_type_hash_t type_description_interfaces__msg__IndividualTypeDescription__EXPECTED_HASH = {1, {
    0x55, 0xc8, 0x27, 0xd8, 0x6c, 0x3c, 0x14, 0x1b,
    0xdd, 0x31, 0x8f, 0xe6, 0xc2, 0x2e, 0x11, 0x19,
    0x0e, 0x4d, 0x3b, 0x37, 0xc8, 0xf4, 0xf9, 0x75,
    0x1a, 0x08, 0x4a, 0xa0, 0x5c, 0xe9, 0x65, 0x60,
  }};
static const rosidl_type_hash_t type_description_interfaces__msg__KeyValue__EXPECTED_HASH = {1, {
    0x27, 0x4f, 0xe5, 0x6b, 0xf1, 0x4f, 0x33, 0xc7,
    0x51, 0x2e, 0x34, 0xc6, 0x46, 0xa3, 0x75, 0x79,
    0xee, 0x36, 0x77, 0x9f, 0x74, 0x5f, 0x04, 0x9a,
    0x97, 0x60, 0x76, 0x3e, 0x81, 0x7f, 0x0c, 0x42,
  }};
static const rosidl_type_hash_t type_description_interfaces__msg__TypeDescription__EXPECTED_HASH = {1, {
    0x73, 0x9f, 0x25, 0x08, 0xc9, 0xfa, 0x3a, 0x6f,
    0x33, 0x09, 0x13, 0xff, 0x5b, 0x9d, 0x25, 0xfb,
    0x74, 0x15, 0x9a, 0x07, 0x7d, 0xa7, 0x1e, 0x10,
    0x87, 0xf5, 0x1a, 0x60, 0xc1, 0x2a, 0x08, 0x0b,
  }};
static const rosidl_type_hash_t type_description_interfaces__msg__TypeSource__EXPECTED_HASH = {1, {
    0xfa, 0xea, 0xec, 0x75, 0x96, 0xc0, 0x4e, 0xcf,
    0x5b, 0x6e, 0x99, 0xad, 0x22, 0x5e, 0x4c, 0x7c,
    0xbb, 0x99, 0x7a, 0xd5, 0x43, 0x5f, 0x79, 0x35,
    0x26, 0xfb, 0x39, 0x84, 0xd0, 0x11, 0xaa, 0xe5,
  }};
#endif

static char type_description_interfaces__srv__GetTypeDescription__TYPE_NAME[] = "type_description_interfaces/srv/GetTypeDescription";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char type_description_interfaces__msg__Field__TYPE_NAME[] = "type_description_interfaces/msg/Field";
static char type_description_interfaces__msg__FieldType__TYPE_NAME[] = "type_description_interfaces/msg/FieldType";
static char type_description_interfaces__msg__IndividualTypeDescription__TYPE_NAME[] = "type_description_interfaces/msg/IndividualTypeDescription";
static char type_description_interfaces__msg__KeyValue__TYPE_NAME[] = "type_description_interfaces/msg/KeyValue";
static char type_description_interfaces__msg__TypeDescription__TYPE_NAME[] = "type_description_interfaces/msg/TypeDescription";
static char type_description_interfaces__msg__TypeSource__TYPE_NAME[] = "type_description_interfaces/msg/TypeSource";
static char type_description_interfaces__srv__GetTypeDescription_Event__TYPE_NAME[] = "type_description_interfaces/srv/GetTypeDescription_Event";
static char type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME[] = "type_description_interfaces/srv/GetTypeDescription_Request";
static char type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME[] = "type_description_interfaces/srv/GetTypeDescription_Response";

// Define type names, field names, and default values
static char type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__request_message[] = "request_message";
static char type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__response_message[] = "response_message";
static char type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field type_description_interfaces__srv__GetTypeDescription__FIELDS[] = {
  {
    {type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {type_description_interfaces__srv__GetTypeDescription_Event__TYPE_NAME, 56, 56},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription type_description_interfaces__srv__GetTypeDescription__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__Field__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__IndividualTypeDescription__TYPE_NAME, 57, 57},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__KeyValue__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeDescription__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Event__TYPE_NAME, 56, 56},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__srv__GetTypeDescription__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__srv__GetTypeDescription__TYPE_NAME, 50, 50},
      {type_description_interfaces__srv__GetTypeDescription__FIELDS, 3, 3},
    },
    {type_description_interfaces__srv__GetTypeDescription__REFERENCED_TYPE_DESCRIPTIONS, 11, 11},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__Field__EXPECTED_HASH, type_description_interfaces__msg__Field__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = type_description_interfaces__msg__Field__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__FieldType__EXPECTED_HASH, type_description_interfaces__msg__FieldType__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = type_description_interfaces__msg__FieldType__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__IndividualTypeDescription__EXPECTED_HASH, type_description_interfaces__msg__IndividualTypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = type_description_interfaces__msg__IndividualTypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__KeyValue__EXPECTED_HASH, type_description_interfaces__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = type_description_interfaces__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeDescription__EXPECTED_HASH, type_description_interfaces__msg__TypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = type_description_interfaces__msg__TypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeSource__EXPECTED_HASH, type_description_interfaces__msg__TypeSource__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = type_description_interfaces__msg__TypeSource__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[8].fields = type_description_interfaces__srv__GetTypeDescription_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[9].fields = type_description_interfaces__srv__GetTypeDescription_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[10].fields = type_description_interfaces__srv__GetTypeDescription_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__type_name[] = "type_name";
static char type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__type_hash[] = "type_hash";
static char type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__include_type_sources[] = "include_type_sources";
static char type_description_interfaces__srv__GetTypeDescription_Request__DEFAULT_VALUE__include_type_sources[] = "True";

static rosidl_runtime_c__type_description__Field type_description_interfaces__srv__GetTypeDescription_Request__FIELDS[] = {
  {
    {type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__type_name, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__type_hash, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Request__FIELD_NAME__include_type_sources, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {type_description_interfaces__srv__GetTypeDescription_Request__DEFAULT_VALUE__include_type_sources, 4, 4},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__srv__GetTypeDescription_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
      {type_description_interfaces__srv__GetTypeDescription_Request__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__successful[] = "successful";
static char type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__failure_reason[] = "failure_reason";
static char type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__type_description[] = "type_description";
static char type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__type_sources[] = "type_sources";
static char type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__extra_information[] = "extra_information";

static rosidl_runtime_c__type_description__Field type_description_interfaces__srv__GetTypeDescription_Response__FIELDS[] = {
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__successful, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__failure_reason, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__type_description, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {type_description_interfaces__msg__TypeDescription__TYPE_NAME, 47, 47},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__type_sources, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__FIELD_NAME__extra_information, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {type_description_interfaces__msg__KeyValue__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription type_description_interfaces__srv__GetTypeDescription_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {type_description_interfaces__msg__Field__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__IndividualTypeDescription__TYPE_NAME, 57, 57},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__KeyValue__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeDescription__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__srv__GetTypeDescription_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
      {type_description_interfaces__srv__GetTypeDescription_Response__FIELDS, 5, 5},
    },
    {type_description_interfaces__srv__GetTypeDescription_Response__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&type_description_interfaces__msg__Field__EXPECTED_HASH, type_description_interfaces__msg__Field__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = type_description_interfaces__msg__Field__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__FieldType__EXPECTED_HASH, type_description_interfaces__msg__FieldType__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = type_description_interfaces__msg__FieldType__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__IndividualTypeDescription__EXPECTED_HASH, type_description_interfaces__msg__IndividualTypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = type_description_interfaces__msg__IndividualTypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__KeyValue__EXPECTED_HASH, type_description_interfaces__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = type_description_interfaces__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeDescription__EXPECTED_HASH, type_description_interfaces__msg__TypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = type_description_interfaces__msg__TypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeSource__EXPECTED_HASH, type_description_interfaces__msg__TypeSource__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = type_description_interfaces__msg__TypeSource__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__info[] = "info";
static char type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__request[] = "request";
static char type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field type_description_interfaces__srv__GetTypeDescription_Event__FIELDS[] = {
  {
    {type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
    },
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription type_description_interfaces__srv__GetTypeDescription_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__Field__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__FieldType__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__IndividualTypeDescription__TYPE_NAME, 57, 57},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__KeyValue__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeDescription__TYPE_NAME, 47, 47},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__msg__TypeSource__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
    {NULL, 0, 0},
  },
  {
    {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
type_description_interfaces__srv__GetTypeDescription_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {type_description_interfaces__srv__GetTypeDescription_Event__TYPE_NAME, 56, 56},
      {type_description_interfaces__srv__GetTypeDescription_Event__FIELDS, 3, 3},
    },
    {type_description_interfaces__srv__GetTypeDescription_Event__REFERENCED_TYPE_DESCRIPTIONS, 10, 10},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__Field__EXPECTED_HASH, type_description_interfaces__msg__Field__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = type_description_interfaces__msg__Field__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__FieldType__EXPECTED_HASH, type_description_interfaces__msg__FieldType__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = type_description_interfaces__msg__FieldType__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__IndividualTypeDescription__EXPECTED_HASH, type_description_interfaces__msg__IndividualTypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = type_description_interfaces__msg__IndividualTypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__KeyValue__EXPECTED_HASH, type_description_interfaces__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = type_description_interfaces__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeDescription__EXPECTED_HASH, type_description_interfaces__msg__TypeDescription__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = type_description_interfaces__msg__TypeDescription__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&type_description_interfaces__msg__TypeSource__EXPECTED_HASH, type_description_interfaces__msg__TypeSource__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = type_description_interfaces__msg__TypeSource__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[8].fields = type_description_interfaces__srv__GetTypeDescription_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[9].fields = type_description_interfaces__srv__GetTypeDescription_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# ROS interface type name, in PACKAGE/NAMESPACE/TYPENAME format.\n"
  "string type_name\n"
  "\n"
  "# REP-2011 RIHS hash string.\n"
  "string type_hash\n"
  "\n"
  "# Whether to return the original idl/msg/etc. source file(s) in the response.\n"
  "bool include_type_sources true\n"
  "---\n"
  "# True if the type description information is available and populated in the response.\n"
  "# If false, all other fields except `failure_reason` are considered undefined.\n"
  "bool successful\n"
  "# If `successful` is false, contains a reason for failure.\n"
  "# If `successful` is true, this is left empty.\n"
  "string failure_reason\n"
  "\n"
  "# The parsed type description which can be used programmatically.\n"
  "TypeDescription type_description\n"
  "\n"
  "# A list containing the interface definition source text of the requested type,\n"
  "# plus all types it recursively depends on.\n"
  "# Each source text is a copy of the original contents of the\n"
  "# .msg, .srv, .action, .idl, or other file if it exists, including comments and whitespace.\n"
  "# Sources can be matched with IndividualTypeDescriptions by their `type_name`.\n"
  "# The `encoding` field of each entry informs how to interpret its contents.\n"
  "TypeSource[] type_sources\n"
  "\n"
  "# Key-value pairs of extra information.\n"
  "KeyValue[] extra_information";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__srv__GetTypeDescription__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__srv__GetTypeDescription__TYPE_NAME, 50, 50},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 1182, 1182},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__srv__GetTypeDescription_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__srv__GetTypeDescription_Request__TYPE_NAME, 58, 58},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__srv__GetTypeDescription_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__srv__GetTypeDescription_Response__TYPE_NAME, 59, 59},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
type_description_interfaces__srv__GetTypeDescription_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {type_description_interfaces__srv__GetTypeDescription_Event__TYPE_NAME, 56, 56},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__srv__GetTypeDescription__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[12];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 12, 12};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__srv__GetTypeDescription__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *type_description_interfaces__msg__Field__get_individual_type_description_source(NULL);
    sources[4] = *type_description_interfaces__msg__FieldType__get_individual_type_description_source(NULL);
    sources[5] = *type_description_interfaces__msg__IndividualTypeDescription__get_individual_type_description_source(NULL);
    sources[6] = *type_description_interfaces__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[7] = *type_description_interfaces__msg__TypeDescription__get_individual_type_description_source(NULL);
    sources[8] = *type_description_interfaces__msg__TypeSource__get_individual_type_description_source(NULL);
    sources[9] = *type_description_interfaces__srv__GetTypeDescription_Event__get_individual_type_description_source(NULL);
    sources[10] = *type_description_interfaces__srv__GetTypeDescription_Request__get_individual_type_description_source(NULL);
    sources[11] = *type_description_interfaces__srv__GetTypeDescription_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__srv__GetTypeDescription_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__srv__GetTypeDescription_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__srv__GetTypeDescription_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__srv__GetTypeDescription_Response__get_individual_type_description_source(NULL),
    sources[1] = *type_description_interfaces__msg__Field__get_individual_type_description_source(NULL);
    sources[2] = *type_description_interfaces__msg__FieldType__get_individual_type_description_source(NULL);
    sources[3] = *type_description_interfaces__msg__IndividualTypeDescription__get_individual_type_description_source(NULL);
    sources[4] = *type_description_interfaces__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[5] = *type_description_interfaces__msg__TypeDescription__get_individual_type_description_source(NULL);
    sources[6] = *type_description_interfaces__msg__TypeSource__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
type_description_interfaces__srv__GetTypeDescription_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[11];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 11, 11};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *type_description_interfaces__srv__GetTypeDescription_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *type_description_interfaces__msg__Field__get_individual_type_description_source(NULL);
    sources[4] = *type_description_interfaces__msg__FieldType__get_individual_type_description_source(NULL);
    sources[5] = *type_description_interfaces__msg__IndividualTypeDescription__get_individual_type_description_source(NULL);
    sources[6] = *type_description_interfaces__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[7] = *type_description_interfaces__msg__TypeDescription__get_individual_type_description_source(NULL);
    sources[8] = *type_description_interfaces__msg__TypeSource__get_individual_type_description_source(NULL);
    sources[9] = *type_description_interfaces__srv__GetTypeDescription_Request__get_individual_type_description_source(NULL);
    sources[10] = *type_description_interfaces__srv__GetTypeDescription_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
