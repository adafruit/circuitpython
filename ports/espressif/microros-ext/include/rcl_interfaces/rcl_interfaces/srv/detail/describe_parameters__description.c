// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:srv/DescribeParameters.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/srv/detail/describe_parameters__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__DescribeParameters__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x84, 0x5b, 0x48, 0x4d, 0x71, 0xeb, 0x06, 0x73,
      0xda, 0xe6, 0x82, 0xf2, 0xe3, 0xba, 0x3c, 0x48,
      0x51, 0xa6, 0x5a, 0x3d, 0xcf, 0xb9, 0x7b, 0xdd,
      0xd8, 0x2c, 0x5b, 0x57, 0xe9, 0x1e, 0x4c, 0xff,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__DescribeParameters_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x05, 0xdd, 0x1c, 0x2b, 0xc8, 0x7d, 0xec, 0x26,
      0x27, 0x10, 0x5c, 0x80, 0x70, 0xd6, 0xf9, 0x72,
      0x5c, 0x78, 0x1a, 0x4f, 0x2a, 0xb8, 0x52, 0x97,
      0x5b, 0xf1, 0x05, 0x46, 0x2b, 0x82, 0xb6, 0x44,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__DescribeParameters_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xad, 0x67, 0x47, 0x31, 0x96, 0x13, 0xc6, 0x13,
      0x15, 0x89, 0xe0, 0x62, 0xe0, 0x95, 0xb4, 0x69,
      0xa4, 0x6c, 0xc7, 0x64, 0xf7, 0xd0, 0xf1, 0x2a,
      0x00, 0x1a, 0x49, 0xe6, 0xcd, 0x89, 0x39, 0x95,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__DescribeParameters_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3d, 0xeb, 0xa6, 0x4c, 0x03, 0x50, 0x93, 0x03,
      0x3c, 0x69, 0x01, 0x84, 0x83, 0x44, 0x41, 0x55,
      0x1a, 0x53, 0x12, 0x8e, 0xa2, 0x53, 0x9f, 0x71,
      0xf2, 0x59, 0x6f, 0xbc, 0x4e, 0xd9, 0xde, 0x43,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "rcl_interfaces/msg/detail/integer_range__functions.h"
#include "rcl_interfaces/msg/detail/parameter_descriptor__functions.h"
#include "rcl_interfaces/msg/detail/floating_point_range__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH = {1, {
    0xe6, 0xaf, 0x23, 0xa2, 0x3c, 0x17, 0x7f, 0xee,
    0x5f, 0x30, 0x75, 0xc8, 0xb1, 0xe4, 0x35, 0x16,
    0x2a, 0x9b, 0x63, 0xc8, 0x63, 0xd7, 0x8c, 0x06,
    0x01, 0x74, 0x60, 0xb4, 0x96, 0x84, 0x26, 0x2d,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__IntegerRange__EXPECTED_HASH = {1, {
    0xf7, 0xb7, 0xfd, 0xc0, 0xf6, 0x5f, 0x07, 0x70,
    0x2e, 0x09, 0x92, 0x18, 0xe1, 0x32, 0x88, 0xc3,
    0x96, 0x3b, 0xcb, 0x93, 0x45, 0xbd, 0xe7, 0x8b,
    0x56, 0x0e, 0x6c, 0xd1, 0x98, 0x00, 0xfc, 0x5a,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH = {1, {
    0x52, 0x17, 0x5d, 0xbf, 0xda, 0x6c, 0x51, 0x15,
    0x31, 0x01, 0xd3, 0x3d, 0x2a, 0x9d, 0xa0, 0x57,
    0x43, 0xf6, 0x6f, 0x02, 0xd5, 0xab, 0x2c, 0xa9,
    0xec, 0x47, 0x09, 0xb4, 0x6b, 0x73, 0xd7, 0x04,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char rcl_interfaces__srv__DescribeParameters__TYPE_NAME[] = "rcl_interfaces/srv/DescribeParameters";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rcl_interfaces__msg__FloatingPointRange__TYPE_NAME[] = "rcl_interfaces/msg/FloatingPointRange";
static char rcl_interfaces__msg__IntegerRange__TYPE_NAME[] = "rcl_interfaces/msg/IntegerRange";
static char rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME[] = "rcl_interfaces/msg/ParameterDescriptor";
static char rcl_interfaces__srv__DescribeParameters_Event__TYPE_NAME[] = "rcl_interfaces/srv/DescribeParameters_Event";
static char rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME[] = "rcl_interfaces/srv/DescribeParameters_Request";
static char rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME[] = "rcl_interfaces/srv/DescribeParameters_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rcl_interfaces__srv__DescribeParameters__FIELD_NAME__request_message[] = "request_message";
static char rcl_interfaces__srv__DescribeParameters__FIELD_NAME__response_message[] = "response_message";
static char rcl_interfaces__srv__DescribeParameters__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__DescribeParameters__FIELDS[] = {
  {
    {rcl_interfaces__srv__DescribeParameters__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__DescribeParameters_Event__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__DescribeParameters__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Event__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__DescribeParameters__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__DescribeParameters__TYPE_NAME, 37, 37},
      {rcl_interfaces__srv__DescribeParameters__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__DescribeParameters__REFERENCED_TYPE_DESCRIPTIONS, 8, 8},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH, rcl_interfaces__msg__FloatingPointRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__FloatingPointRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__IntegerRange__EXPECTED_HASH, rcl_interfaces__msg__IntegerRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__IntegerRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH, rcl_interfaces__msg__ParameterDescriptor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__msg__ParameterDescriptor__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__DescribeParameters_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = rcl_interfaces__srv__DescribeParameters_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = rcl_interfaces__srv__DescribeParameters_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__DescribeParameters_Request__FIELD_NAME__names[] = "names";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__DescribeParameters_Request__FIELDS[] = {
  {
    {rcl_interfaces__srv__DescribeParameters_Request__FIELD_NAME__names, 5, 5},
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
rcl_interfaces__srv__DescribeParameters_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
      {rcl_interfaces__srv__DescribeParameters_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__DescribeParameters_Response__FIELD_NAME__descriptors[] = "descriptors";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__DescribeParameters_Response__FIELDS[] = {
  {
    {rcl_interfaces__srv__DescribeParameters_Response__FIELD_NAME__descriptors, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__DescribeParameters_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__DescribeParameters_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
      {rcl_interfaces__srv__DescribeParameters_Response__FIELDS, 1, 1},
    },
    {rcl_interfaces__srv__DescribeParameters_Response__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH, rcl_interfaces__msg__FloatingPointRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__FloatingPointRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__IntegerRange__EXPECTED_HASH, rcl_interfaces__msg__IntegerRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__IntegerRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH, rcl_interfaces__msg__ParameterDescriptor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__ParameterDescriptor__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__info[] = "info";
static char rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__request[] = "request";
static char rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__DescribeParameters_Event__FIELDS[] = {
  {
    {rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__DescribeParameters_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__DescribeParameters_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__DescribeParameters_Event__TYPE_NAME, 43, 43},
      {rcl_interfaces__srv__DescribeParameters_Event__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__DescribeParameters_Event__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH, rcl_interfaces__msg__FloatingPointRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__FloatingPointRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__IntegerRange__EXPECTED_HASH, rcl_interfaces__msg__IntegerRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__IntegerRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH, rcl_interfaces__msg__ParameterDescriptor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__msg__ParameterDescriptor__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__DescribeParameters_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = rcl_interfaces__srv__DescribeParameters_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A list of parameters of which to get the descriptor.\n"
  "string[] names\n"
  "\n"
  "---\n"
  "# A list of the descriptors of all parameters requested in the same order\n"
  "# as they were requested. This list has the same length as the list of\n"
  "# parameters requested.\n"
  "ParameterDescriptor[] descriptors";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__DescribeParameters__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__DescribeParameters__TYPE_NAME, 37, 37},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 278, 278},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__DescribeParameters_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__DescribeParameters_Request__TYPE_NAME, 45, 45},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__DescribeParameters_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__DescribeParameters_Response__TYPE_NAME, 46, 46},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__DescribeParameters_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__DescribeParameters_Event__TYPE_NAME, 43, 43},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__DescribeParameters__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[9];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 9, 9};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__DescribeParameters__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__DescribeParameters_Event__get_individual_type_description_source(NULL);
    sources[6] = *rcl_interfaces__srv__DescribeParameters_Request__get_individual_type_description_source(NULL);
    sources[7] = *rcl_interfaces__srv__DescribeParameters_Response__get_individual_type_description_source(NULL);
    sources[8] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__DescribeParameters_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__DescribeParameters_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__DescribeParameters_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__DescribeParameters_Response__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__DescribeParameters_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__DescribeParameters_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__DescribeParameters_Request__get_individual_type_description_source(NULL);
    sources[6] = *rcl_interfaces__srv__DescribeParameters_Response__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
