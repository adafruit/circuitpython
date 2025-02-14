// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:srv/BasicTypes.idl
// generated code does not contain a copyright notice

#include "test_msgs/srv/detail/basic_types__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__BasicTypes__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xda, 0x34, 0x86, 0x97, 0x43, 0x7c, 0x10, 0x3c,
      0xc9, 0xa9, 0x32, 0x2a, 0xc1, 0xbd, 0xbd, 0xcf,
      0x8e, 0xb1, 0x1a, 0x85, 0xde, 0x22, 0xf7, 0xf2,
      0x49, 0x2c, 0xa1, 0xdf, 0x41, 0x6e, 0x1a, 0x5f,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__BasicTypes_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1d, 0xaf, 0xcf, 0x38, 0x9e, 0x22, 0x46, 0x92,
      0x78, 0x3e, 0x9c, 0x4f, 0x60, 0x66, 0xba, 0x0d,
      0xb3, 0xfa, 0x75, 0xfd, 0xbd, 0x8f, 0x08, 0x1f,
      0xfb, 0xe7, 0xed, 0x03, 0x14, 0x4d, 0x73, 0xd4,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__BasicTypes_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x84, 0x79, 0xb3, 0xde, 0xfe, 0x55, 0x4d, 0x96,
      0x9e, 0x81, 0xc6, 0xcf, 0x8c, 0xda, 0xf0, 0xb9,
      0x7b, 0x4f, 0xcd, 0x76, 0x92, 0x8b, 0x2d, 0x64,
      0x4c, 0x68, 0x13, 0xb0, 0x32, 0x67, 0x3c, 0x0d,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__BasicTypes_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5b, 0x99, 0x88, 0xa4, 0x80, 0x5a, 0x06, 0xae,
      0x22, 0xdf, 0xf1, 0x29, 0x03, 0xa7, 0x74, 0xcc,
      0xda, 0x71, 0x0e, 0x95, 0x14, 0x92, 0x6b, 0x82,
      0x6f, 0xfc, 0x88, 0x4e, 0x3a, 0x9a, 0x98, 0x2e,
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

static char test_msgs__srv__BasicTypes__TYPE_NAME[] = "test_msgs/srv/BasicTypes";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char test_msgs__srv__BasicTypes_Event__TYPE_NAME[] = "test_msgs/srv/BasicTypes_Event";
static char test_msgs__srv__BasicTypes_Request__TYPE_NAME[] = "test_msgs/srv/BasicTypes_Request";
static char test_msgs__srv__BasicTypes_Response__TYPE_NAME[] = "test_msgs/srv/BasicTypes_Response";

// Define type names, field names, and default values
static char test_msgs__srv__BasicTypes__FIELD_NAME__request_message[] = "request_message";
static char test_msgs__srv__BasicTypes__FIELD_NAME__response_message[] = "response_message";
static char test_msgs__srv__BasicTypes__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field test_msgs__srv__BasicTypes__FIELDS[] = {
  {
    {test_msgs__srv__BasicTypes__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__BasicTypes_Event__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__BasicTypes__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Event__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__BasicTypes__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__BasicTypes__TYPE_NAME, 24, 24},
      {test_msgs__srv__BasicTypes__FIELDS, 3, 3},
    },
    {test_msgs__srv__BasicTypes__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = test_msgs__srv__BasicTypes_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = test_msgs__srv__BasicTypes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = test_msgs__srv__BasicTypes_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__bool_value[] = "bool_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__byte_value[] = "byte_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__char_value[] = "char_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__float32_value[] = "float32_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__float64_value[] = "float64_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__int8_value[] = "int8_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint8_value[] = "uint8_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__int16_value[] = "int16_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint16_value[] = "uint16_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__int32_value[] = "int32_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint32_value[] = "uint32_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__int64_value[] = "int64_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint64_value[] = "uint64_value";
static char test_msgs__srv__BasicTypes_Request__FIELD_NAME__string_value[] = "string_value";

static rosidl_runtime_c__type_description__Field test_msgs__srv__BasicTypes_Request__FIELDS[] = {
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__bool_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__byte_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__char_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__float32_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__float64_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__int8_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint8_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__int16_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint16_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__int32_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint32_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__int64_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__uint64_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__FIELD_NAME__string_value, 12, 12},
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
test_msgs__srv__BasicTypes_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
      {test_msgs__srv__BasicTypes_Request__FIELDS, 14, 14},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__bool_value[] = "bool_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__byte_value[] = "byte_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__char_value[] = "char_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__float32_value[] = "float32_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__float64_value[] = "float64_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__int8_value[] = "int8_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint8_value[] = "uint8_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__int16_value[] = "int16_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint16_value[] = "uint16_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__int32_value[] = "int32_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint32_value[] = "uint32_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__int64_value[] = "int64_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint64_value[] = "uint64_value";
static char test_msgs__srv__BasicTypes_Response__FIELD_NAME__string_value[] = "string_value";

static rosidl_runtime_c__type_description__Field test_msgs__srv__BasicTypes_Response__FIELDS[] = {
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__bool_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__byte_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__char_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__float32_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__float64_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__int8_value, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint8_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__int16_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint16_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__int32_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint32_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__int64_value, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__uint64_value, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__FIELD_NAME__string_value, 12, 12},
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
test_msgs__srv__BasicTypes_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
      {test_msgs__srv__BasicTypes_Response__FIELDS, 14, 14},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__BasicTypes_Event__FIELD_NAME__info[] = "info";
static char test_msgs__srv__BasicTypes_Event__FIELD_NAME__request[] = "request";
static char test_msgs__srv__BasicTypes_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field test_msgs__srv__BasicTypes_Event__FIELDS[] = {
  {
    {test_msgs__srv__BasicTypes_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__BasicTypes_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__BasicTypes_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__BasicTypes_Event__TYPE_NAME, 30, 30},
      {test_msgs__srv__BasicTypes_Event__FIELDS, 3, 3},
    },
    {test_msgs__srv__BasicTypes_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = test_msgs__srv__BasicTypes_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = test_msgs__srv__BasicTypes_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "bool bool_value\n"
  "byte byte_value\n"
  "char char_value\n"
  "float32 float32_value\n"
  "float64 float64_value\n"
  "int8 int8_value\n"
  "uint8 uint8_value\n"
  "int16 int16_value\n"
  "uint16 uint16_value\n"
  "int32 int32_value\n"
  "uint32 uint32_value\n"
  "int64 int64_value\n"
  "uint64 uint64_value\n"
  "string string_value\n"
  "---\n"
  "bool bool_value\n"
  "byte byte_value\n"
  "char char_value\n"
  "float32 float32_value\n"
  "float64 float64_value\n"
  "int8 int8_value\n"
  "uint8 uint8_value\n"
  "int16 int16_value\n"
  "uint16 uint16_value\n"
  "int32 int32_value\n"
  "uint32 uint32_value\n"
  "int64 int64_value\n"
  "uint64 uint64_value\n"
  "string string_value";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__BasicTypes__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__BasicTypes__TYPE_NAME, 24, 24},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 524, 524},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__BasicTypes_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__BasicTypes_Request__TYPE_NAME, 32, 32},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__BasicTypes_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__BasicTypes_Response__TYPE_NAME, 33, 33},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__BasicTypes_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__BasicTypes_Event__TYPE_NAME, 30, 30},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__BasicTypes__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__BasicTypes__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__srv__BasicTypes_Event__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__srv__BasicTypes_Request__get_individual_type_description_source(NULL);
    sources[5] = *test_msgs__srv__BasicTypes_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__BasicTypes_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__BasicTypes_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__BasicTypes_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__BasicTypes_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__BasicTypes_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__BasicTypes_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__srv__BasicTypes_Request__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__srv__BasicTypes_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
