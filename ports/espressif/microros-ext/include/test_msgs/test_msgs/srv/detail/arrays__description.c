// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:srv/Arrays.idl
// generated code does not contain a copyright notice

#include "test_msgs/srv/detail/arrays__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Arrays__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xae, 0x66, 0xe1, 0x3b, 0x3a, 0xd1, 0x52, 0x0b,
      0xcd, 0x9f, 0xa5, 0x75, 0x1c, 0x4a, 0x45, 0x21,
      0x22, 0x2b, 0xf2, 0x7c, 0x2f, 0x84, 0x5d, 0x79,
      0xde, 0x0c, 0x76, 0xe4, 0xcb, 0xb4, 0x10, 0x80,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Arrays_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x69, 0xe1, 0xd2, 0x77, 0xb2, 0xef, 0xd1, 0xd2,
      0x76, 0x0e, 0x54, 0x5a, 0x40, 0x57, 0xf1, 0x1f,
      0x62, 0xf7, 0xc6, 0x39, 0x5c, 0x5f, 0xaa, 0x17,
      0xe5, 0x05, 0x99, 0x39, 0x9b, 0x4f, 0xb5, 0xd8,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Arrays_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe0, 0xf5, 0x9d, 0x6b, 0xd8, 0x45, 0x46, 0xeb,
      0xfb, 0x5a, 0xd7, 0x6a, 0x0a, 0xfe, 0x77, 0xf9,
      0xfe, 0xe9, 0xbd, 0x6f, 0x1f, 0xc8, 0xf8, 0xfc,
      0xa3, 0x78, 0x90, 0xfe, 0xaf, 0x27, 0xee, 0x69,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__srv__Arrays_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x41, 0xbe, 0x0b, 0xc3, 0xda, 0xa6, 0xdb, 0xbd,
      0x6d, 0x11, 0x98, 0x67, 0x8b, 0x36, 0x2b, 0xd4,
      0xb4, 0x3a, 0x08, 0xcb, 0x75, 0xfe, 0x0a, 0x4f,
      0x43, 0x4f, 0xe7, 0x9b, 0xad, 0x31, 0xf1, 0x9e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "test_msgs/msg/detail/basic_types__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "test_msgs/msg/detail/constants__functions.h"
#include "test_msgs/msg/detail/defaults__functions.h"

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
static const rosidl_type_hash_t test_msgs__msg__BasicTypes__EXPECTED_HASH = {1, {
    0x7c, 0x30, 0x0a, 0xfd, 0x4e, 0x79, 0x67, 0x98,
    0xd4, 0x9b, 0xdd, 0x6c, 0xda, 0xa0, 0xfa, 0x87,
    0xfa, 0x0e, 0xd2, 0xba, 0x32, 0x17, 0xd9, 0x77,
    0xe1, 0xfa, 0xa8, 0x70, 0x70, 0xd7, 0x97, 0xab,
  }};
static const rosidl_type_hash_t test_msgs__msg__Constants__EXPECTED_HASH = {1, {
    0x98, 0x3e, 0x7a, 0xe7, 0x09, 0x9d, 0x65, 0xb3,
    0x03, 0x0c, 0x31, 0xc2, 0x89, 0x0b, 0x0e, 0xc2,
    0xb3, 0x32, 0x48, 0x4f, 0xae, 0x9f, 0x28, 0xc6,
    0x8e, 0x01, 0x34, 0x0c, 0x8f, 0x61, 0xed, 0xcf,
  }};
static const rosidl_type_hash_t test_msgs__msg__Defaults__EXPECTED_HASH = {1, {
    0xa7, 0x25, 0xee, 0x02, 0xbf, 0x25, 0xa2, 0x24,
    0xcb, 0x7d, 0x14, 0x01, 0x5a, 0x02, 0x45, 0xb6,
    0x65, 0x4f, 0xf5, 0x91, 0xae, 0x1a, 0xc2, 0x77,
    0xe2, 0xa2, 0x67, 0x3f, 0x01, 0x3e, 0xa6, 0xd2,
  }};
#endif

static char test_msgs__srv__Arrays__TYPE_NAME[] = "test_msgs/srv/Arrays";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char test_msgs__msg__BasicTypes__TYPE_NAME[] = "test_msgs/msg/BasicTypes";
static char test_msgs__msg__Constants__TYPE_NAME[] = "test_msgs/msg/Constants";
static char test_msgs__msg__Defaults__TYPE_NAME[] = "test_msgs/msg/Defaults";
static char test_msgs__srv__Arrays_Event__TYPE_NAME[] = "test_msgs/srv/Arrays_Event";
static char test_msgs__srv__Arrays_Request__TYPE_NAME[] = "test_msgs/srv/Arrays_Request";
static char test_msgs__srv__Arrays_Response__TYPE_NAME[] = "test_msgs/srv/Arrays_Response";

// Define type names, field names, and default values
static char test_msgs__srv__Arrays__FIELD_NAME__request_message[] = "request_message";
static char test_msgs__srv__Arrays__FIELD_NAME__response_message[] = "response_message";
static char test_msgs__srv__Arrays__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Arrays__FIELDS[] = {
  {
    {test_msgs__srv__Arrays__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__srv__Arrays_Event__TYPE_NAME, 26, 26},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Arrays__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Event__TYPE_NAME, 26, 26},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Arrays__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Arrays__TYPE_NAME, 20, 20},
      {test_msgs__srv__Arrays__FIELDS, 3, 3},
    },
    {test_msgs__srv__Arrays__REFERENCED_TYPE_DESCRIPTIONS, 8, 8},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = test_msgs__srv__Arrays_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = test_msgs__srv__Arrays_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[7].fields = test_msgs__srv__Arrays_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Arrays_Request__FIELD_NAME__bool_values[] = "bool_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__byte_values[] = "byte_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__char_values[] = "char_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__float32_values[] = "float32_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__float64_values[] = "float64_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int8_values[] = "int8_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint8_values[] = "uint8_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int16_values[] = "int16_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint16_values[] = "uint16_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int32_values[] = "int32_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint32_values[] = "uint32_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int64_values[] = "int64_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint64_values[] = "uint64_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__string_values[] = "string_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__basic_types_values[] = "basic_types_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__constants_values[] = "constants_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__defaults_values[] = "defaults_values";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__bool_values_default[] = "bool_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__bool_values_default[] = "(False, True, False)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__byte_values_default[] = "byte_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__byte_values_default[] = "(0, 1, 255)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__char_values_default[] = "char_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__char_values_default[] = "(0, 1, 127)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__float32_values_default[] = "float32_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__float32_values_default[] = "(1.125, 0.0, -1.125)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__float64_values_default[] = "float64_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__float64_values_default[] = "(3.1415, 0.0, -3.1415)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int8_values_default[] = "int8_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int8_values_default[] = "(0, 127, -128)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint8_values_default[] = "uint8_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint8_values_default[] = "(0, 1, 255)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int16_values_default[] = "int16_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int16_values_default[] = "(0, 32767, -32768)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint16_values_default[] = "uint16_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint16_values_default[] = "(0, 1, 65535)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int32_values_default[] = "int32_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int32_values_default[] = "(0, 2147483647, -2147483648)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint32_values_default[] = "uint32_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint32_values_default[] = "(0, 1, 4294967295)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__int64_values_default[] = "int64_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int64_values_default[] = "(0, 9223372036854775807, -9223372036854775808)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__uint64_values_default[] = "uint64_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint64_values_default[] = "(0, 1, 18446744073709551615)";
static char test_msgs__srv__Arrays_Request__FIELD_NAME__string_values_default[] = "string_values_default";
static char test_msgs__srv__Arrays_Request__DEFAULT_VALUE__string_values_default[] = "('', 'max value', 'min value')";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Arrays_Request__FIELDS[] = {
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__bool_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__byte_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__char_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__float32_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__float64_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int8_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint8_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int16_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint16_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int32_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint32_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int64_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint64_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__string_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__basic_types_values, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__constants_values, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__defaults_values, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__bool_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__bool_values_default, 20, 20},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__byte_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__byte_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__char_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__char_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__float32_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__float32_values_default, 20, 20},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__float64_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__float64_values_default, 22, 22},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int8_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int8_values_default, 14, 14},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint8_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint8_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int16_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int16_values_default, 18, 18},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint16_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint16_values_default, 13, 13},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int32_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int32_values_default, 28, 28},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint32_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint32_values_default, 18, 18},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__int64_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__int64_values_default, 46, 46},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__uint64_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__uint64_values_default, 28, 28},
  },
  {
    {test_msgs__srv__Arrays_Request__FIELD_NAME__string_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Request__DEFAULT_VALUE__string_values_default, 30, 30},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Arrays_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Arrays_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
      {test_msgs__srv__Arrays_Request__FIELDS, 31, 31},
    },
    {test_msgs__srv__Arrays_Request__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Arrays_Response__FIELD_NAME__bool_values[] = "bool_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__byte_values[] = "byte_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__char_values[] = "char_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__float32_values[] = "float32_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__float64_values[] = "float64_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int8_values[] = "int8_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint8_values[] = "uint8_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int16_values[] = "int16_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint16_values[] = "uint16_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int32_values[] = "int32_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint32_values[] = "uint32_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int64_values[] = "int64_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint64_values[] = "uint64_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__string_values[] = "string_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__basic_types_values[] = "basic_types_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__constants_values[] = "constants_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__defaults_values[] = "defaults_values";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__bool_values_default[] = "bool_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__bool_values_default[] = "(False, True, False)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__byte_values_default[] = "byte_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__byte_values_default[] = "(0, 1, 255)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__char_values_default[] = "char_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__char_values_default[] = "(0, 1, 127)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__float32_values_default[] = "float32_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__float32_values_default[] = "(1.125, 0.0, -1.125)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__float64_values_default[] = "float64_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__float64_values_default[] = "(3.1415, 0.0, -3.1415)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int8_values_default[] = "int8_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int8_values_default[] = "(0, 127, -128)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint8_values_default[] = "uint8_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint8_values_default[] = "(0, 1, 255)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int16_values_default[] = "int16_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int16_values_default[] = "(0, 32767, -32768)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint16_values_default[] = "uint16_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint16_values_default[] = "(0, 1, 65535)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int32_values_default[] = "int32_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int32_values_default[] = "(0, 2147483647, -2147483648)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint32_values_default[] = "uint32_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint32_values_default[] = "(0, 1, 4294967295)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__int64_values_default[] = "int64_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int64_values_default[] = "(0, 9223372036854775807, -9223372036854775808)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__uint64_values_default[] = "uint64_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint64_values_default[] = "(0, 1, 18446744073709551615)";
static char test_msgs__srv__Arrays_Response__FIELD_NAME__string_values_default[] = "string_values_default";
static char test_msgs__srv__Arrays_Response__DEFAULT_VALUE__string_values_default[] = "('', 'max value', 'min value')";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Arrays_Response__FIELDS[] = {
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__bool_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__byte_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__char_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__float32_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__float64_values, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int8_values, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint8_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int16_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint16_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int32_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint32_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int64_values, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint64_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__string_values, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__basic_types_values, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__constants_values, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__defaults_values, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__bool_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__bool_values_default, 20, 20},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__byte_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__byte_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__char_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__char_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__float32_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__float32_values_default, 20, 20},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__float64_values_default, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__float64_values_default, 22, 22},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int8_values_default, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int8_values_default, 14, 14},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint8_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint8_values_default, 11, 11},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int16_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int16_values_default, 18, 18},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint16_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint16_values_default, 13, 13},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int32_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int32_values_default, 28, 28},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint32_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint32_values_default, 18, 18},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__int64_values_default, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__int64_values_default, 46, 46},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__uint64_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__uint64_values_default, 28, 28},
  },
  {
    {test_msgs__srv__Arrays_Response__FIELD_NAME__string_values_default, 21, 21},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_ARRAY,
      3,
      0,
      {NULL, 0, 0},
    },
    {test_msgs__srv__Arrays_Response__DEFAULT_VALUE__string_values_default, 30, 30},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Arrays_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Arrays_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
      {test_msgs__srv__Arrays_Response__FIELDS, 31, 31},
    },
    {test_msgs__srv__Arrays_Response__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char test_msgs__srv__Arrays_Event__FIELD_NAME__info[] = "info";
static char test_msgs__srv__Arrays_Event__FIELD_NAME__request[] = "request";
static char test_msgs__srv__Arrays_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field test_msgs__srv__Arrays_Event__FIELDS[] = {
  {
    {test_msgs__srv__Arrays_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__srv__Arrays_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__srv__Arrays_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__srv__Arrays_Event__TYPE_NAME, 26, 26},
      {test_msgs__srv__Arrays_Event__FIELDS, 3, 3},
    },
    {test_msgs__srv__Arrays_Event__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = test_msgs__srv__Arrays_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = test_msgs__srv__Arrays_Response__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "bool[3] bool_values\n"
  "byte[3] byte_values\n"
  "char[3] char_values\n"
  "float32[3] float32_values\n"
  "float64[3] float64_values\n"
  "int8[3] int8_values\n"
  "uint8[3] uint8_values\n"
  "int16[3] int16_values\n"
  "uint16[3] uint16_values\n"
  "int32[3] int32_values\n"
  "uint32[3] uint32_values\n"
  "int64[3] int64_values\n"
  "uint64[3] uint64_values\n"
  "string[3] string_values\n"
  "BasicTypes[3] basic_types_values\n"
  "Constants[3] constants_values\n"
  "Defaults[3] defaults_values\n"
  "bool[3] bool_values_default [false, true, false]\n"
  "byte[3] byte_values_default [0, 1, 255]\n"
  "char[3] char_values_default [0, 1, 127]\n"
  "float32[3] float32_values_default [1.125, 0.0, -1.125]\n"
  "float64[3] float64_values_default [3.1415, 0.0, -3.1415]\n"
  "int8[3] int8_values_default [0, 127, -128]\n"
  "uint8[3] uint8_values_default [0, 1, 255]\n"
  "int16[3] int16_values_default [0, 32767, -32768]\n"
  "uint16[3] uint16_values_default [0, 1, 65535]\n"
  "int32[3] int32_values_default [0, 2147483647, -2147483648]\n"
  "uint32[3] uint32_values_default [0, 1, 4294967295]\n"
  "int64[3] int64_values_default [0, 9223372036854775807, -9223372036854775808]\n"
  "uint64[3] uint64_values_default [0, 1, 18446744073709551615]\n"
  "string[3] string_values_default [\"\", \"max value\", \"min value\"]\n"
  "---\n"
  "bool[3] bool_values\n"
  "byte[3] byte_values\n"
  "char[3] char_values\n"
  "float32[3] float32_values\n"
  "float64[3] float64_values\n"
  "int8[3] int8_values\n"
  "uint8[3] uint8_values\n"
  "int16[3] int16_values\n"
  "uint16[3] uint16_values\n"
  "int32[3] int32_values\n"
  "uint32[3] uint32_values\n"
  "int64[3] int64_values\n"
  "uint64[3] uint64_values\n"
  "string[3] string_values\n"
  "BasicTypes[3] basic_types_values\n"
  "Constants[3] constants_values\n"
  "Defaults[3] defaults_values\n"
  "bool[3] bool_values_default [false, true, false]\n"
  "byte[3] byte_values_default [0, 1, 255]\n"
  "char[3] char_values_default [0, 1, 127]\n"
  "float32[3] float32_values_default [1.125, 0.0, -1.125]\n"
  "float64[3] float64_values_default [3.1415, 0.0, -3.1415]\n"
  "int8[3] int8_values_default [0, 127, -128]\n"
  "uint8[3] uint8_values_default [0, 1, 255]\n"
  "int16[3] int16_values_default [0, 32767, -32768]\n"
  "uint16[3] uint16_values_default [0, 1, 65535]\n"
  "int32[3] int32_values_default [0, 2147483647, -2147483648]\n"
  "uint32[3] uint32_values_default [0, 1, 4294967295]\n"
  "int64[3] int64_values_default [0, 9223372036854775807, -9223372036854775808]\n"
  "uint64[3] uint64_values_default [0, 1, 18446744073709551615]\n"
  "string[3] string_values_default [\"\", \"max value\", \"min value\"]";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Arrays__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Arrays__TYPE_NAME, 20, 20},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 2282, 2282},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Arrays_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Arrays_Request__TYPE_NAME, 28, 28},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Arrays_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Arrays_Response__TYPE_NAME, 29, 29},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__srv__Arrays_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__srv__Arrays_Event__TYPE_NAME, 26, 26},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Arrays__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[9];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 9, 9};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Arrays__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[5] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    sources[6] = *test_msgs__srv__Arrays_Event__get_individual_type_description_source(NULL);
    sources[7] = *test_msgs__srv__Arrays_Request__get_individual_type_description_source(NULL);
    sources[8] = *test_msgs__srv__Arrays_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Arrays_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Arrays_Request__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[2] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Arrays_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Arrays_Response__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[2] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__srv__Arrays_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__srv__Arrays_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[5] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    sources[6] = *test_msgs__srv__Arrays_Request__get_individual_type_description_source(NULL);
    sources[7] = *test_msgs__srv__Arrays_Response__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
