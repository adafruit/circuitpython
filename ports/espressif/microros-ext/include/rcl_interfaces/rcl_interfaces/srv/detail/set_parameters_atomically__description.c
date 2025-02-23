// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:srv/SetParametersAtomically.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/srv/detail/set_parameters_atomically__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__SetParametersAtomically__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x0e, 0x19, 0x2e, 0xf2, 0x59, 0xc0, 0x7f, 0xc3,
      0xc0, 0x7a, 0x13, 0x19, 0x1d, 0x27, 0x00, 0x22,
      0x22, 0xe6, 0x5e, 0x00, 0xcc, 0xec, 0x65, 0x3c,
      0xa0, 0x5e, 0x85, 0x6f, 0x79, 0x28, 0x5f, 0xcd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__SetParametersAtomically_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdb, 0x0b, 0xd9, 0x50, 0xed, 0x5e, 0xd8, 0x67,
      0xa0, 0xe9, 0x0b, 0x22, 0x5e, 0x44, 0xde, 0xa3,
      0x82, 0xd7, 0x62, 0xc0, 0x76, 0x2f, 0xf3, 0xe0,
      0xee, 0x84, 0x21, 0x90, 0x04, 0x2a, 0x59, 0xe5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__SetParametersAtomically_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x31, 0xb1, 0xa3, 0x8c, 0x5b, 0x0b, 0x9d, 0x89,
      0x3e, 0xe2, 0x4d, 0xa4, 0x5f, 0x69, 0x63, 0x8f,
      0x45, 0x61, 0xc7, 0x84, 0x61, 0xd8, 0x2d, 0x52,
      0x75, 0x0f, 0x42, 0x9b, 0x53, 0x45, 0x02, 0xb8,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__srv__SetParametersAtomically_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x42, 0xfc, 0x08, 0x21, 0xe6, 0xe8, 0xfc, 0xff,
      0x2b, 0x15, 0x0c, 0xef, 0xc7, 0xd8, 0x26, 0x9f,
      0x0c, 0xe6, 0x51, 0x39, 0xe1, 0xdc, 0x71, 0x5b,
      0x15, 0x26, 0x7a, 0x4b, 0xcf, 0x63, 0xa1, 0x89,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "rcl_interfaces/msg/detail/set_parameters_result__functions.h"
#include "rcl_interfaces/msg/detail/parameter__functions.h"
#include "rcl_interfaces/msg/detail/parameter_value__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__Parameter__EXPECTED_HASH = {1, {
    0xdd, 0xfe, 0x64, 0x42, 0xcf, 0xfc, 0x46, 0x23,
    0x17, 0xad, 0xb5, 0xc9, 0x25, 0x36, 0xa7, 0xb6,
    0xdd, 0x55, 0x85, 0x8c, 0x5c, 0x3e, 0x1e, 0x32,
    0x81, 0x65, 0xa6, 0xb7, 0x3c, 0x28, 0x31, 0xaf,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__ParameterValue__EXPECTED_HASH = {1, {
    0x11, 0x5f, 0xc0, 0x89, 0xa3, 0x87, 0xe2, 0x3c,
    0x7e, 0xcd, 0x35, 0x25, 0xc9, 0x18, 0x9c, 0x37,
    0x91, 0x09, 0x11, 0x9d, 0x6a, 0xb8, 0x2e, 0x8d,
    0xfb, 0xde, 0x0f, 0xdf, 0x6a, 0x7f, 0x9b, 0x68,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__SetParametersResult__EXPECTED_HASH = {1, {
    0xcf, 0xcc, 0x0f, 0xb0, 0x37, 0x1e, 0xe5, 0x15,
    0x9b, 0x40, 0x39, 0x60, 0xef, 0x43, 0x00, 0xf8,
    0xf9, 0xd2, 0xf1, 0xfd, 0x61, 0x17, 0xc8, 0x66,
    0x6b, 0x7f, 0x96, 0x54, 0xd5, 0x28, 0xa9, 0xb1,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char rcl_interfaces__srv__SetParametersAtomically__TYPE_NAME[] = "rcl_interfaces/srv/SetParametersAtomically";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rcl_interfaces__msg__Parameter__TYPE_NAME[] = "rcl_interfaces/msg/Parameter";
static char rcl_interfaces__msg__ParameterValue__TYPE_NAME[] = "rcl_interfaces/msg/ParameterValue";
static char rcl_interfaces__msg__SetParametersResult__TYPE_NAME[] = "rcl_interfaces/msg/SetParametersResult";
static char rcl_interfaces__srv__SetParametersAtomically_Event__TYPE_NAME[] = "rcl_interfaces/srv/SetParametersAtomically_Event";
static char rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME[] = "rcl_interfaces/srv/SetParametersAtomically_Request";
static char rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME[] = "rcl_interfaces/srv/SetParametersAtomically_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__request_message[] = "request_message";
static char rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__response_message[] = "response_message";
static char rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__SetParametersAtomically__FIELDS[] = {
  {
    {rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__srv__SetParametersAtomically_Event__TYPE_NAME, 48, 48},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__SetParametersAtomically__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Event__TYPE_NAME, 48, 48},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__SetParametersAtomically__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__SetParametersAtomically__TYPE_NAME, 42, 42},
      {rcl_interfaces__srv__SetParametersAtomically__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__SetParametersAtomically__REFERENCED_TYPE_DESCRIPTIONS, 8, 8},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__Parameter__EXPECTED_HASH, rcl_interfaces__msg__Parameter__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__Parameter__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterValue__EXPECTED_HASH, rcl_interfaces__msg__ParameterValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__ParameterValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__SetParametersResult__EXPECTED_HASH, rcl_interfaces__msg__SetParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__msg__SetParametersResult__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__SetParametersAtomically_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = rcl_interfaces__srv__SetParametersAtomically_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[6].fields = rcl_interfaces__srv__SetParametersAtomically_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[7].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__SetParametersAtomically_Request__FIELD_NAME__parameters[] = "parameters";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__SetParametersAtomically_Request__FIELDS[] = {
  {
    {rcl_interfaces__srv__SetParametersAtomically_Request__FIELD_NAME__parameters, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__SetParametersAtomically_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__SetParametersAtomically_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
      {rcl_interfaces__srv__SetParametersAtomically_Request__FIELDS, 1, 1},
    },
    {rcl_interfaces__srv__SetParametersAtomically_Request__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__Parameter__EXPECTED_HASH, rcl_interfaces__msg__Parameter__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__Parameter__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterValue__EXPECTED_HASH, rcl_interfaces__msg__ParameterValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__ParameterValue__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__SetParametersAtomically_Response__FIELD_NAME__result[] = "result";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__SetParametersAtomically_Response__FIELDS[] = {
  {
    {rcl_interfaces__srv__SetParametersAtomically_Response__FIELD_NAME__result, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__SetParametersAtomically_Response__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__SetParametersAtomically_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
      {rcl_interfaces__srv__SetParametersAtomically_Response__FIELDS, 1, 1},
    },
    {rcl_interfaces__srv__SetParametersAtomically_Response__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__SetParametersResult__EXPECTED_HASH, rcl_interfaces__msg__SetParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__SetParametersResult__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__info[] = "info";
static char rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__request[] = "request";
static char rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rcl_interfaces__srv__SetParametersAtomically_Event__FIELDS[] = {
  {
    {rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__srv__SetParametersAtomically_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterValue__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__SetParametersResult__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__srv__SetParametersAtomically_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__srv__SetParametersAtomically_Event__TYPE_NAME, 48, 48},
      {rcl_interfaces__srv__SetParametersAtomically_Event__FIELDS, 3, 3},
    },
    {rcl_interfaces__srv__SetParametersAtomically_Event__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__Parameter__EXPECTED_HASH, rcl_interfaces__msg__Parameter__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__Parameter__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterValue__EXPECTED_HASH, rcl_interfaces__msg__ParameterValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__ParameterValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__SetParametersResult__EXPECTED_HASH, rcl_interfaces__msg__SetParametersResult__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__msg__SetParametersResult__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__srv__SetParametersAtomically_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[5].fields = rcl_interfaces__srv__SetParametersAtomically_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# A list of parameters to set atomically.\n"
  "#\n"
  "# This call will either set all values, or none of the values.\n"
  "Parameter[] parameters\n"
  "\n"
  "---\n"
  "# Indicates whether setting all of the parameters succeeded or not and why.\n"
  "SetParametersResult result";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__SetParametersAtomically__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__SetParametersAtomically__TYPE_NAME, 42, 42},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 238, 238},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__SetParametersAtomically_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__SetParametersAtomically_Request__TYPE_NAME, 50, 50},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__SetParametersAtomically_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__SetParametersAtomically_Response__TYPE_NAME, 51, 51},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__srv__SetParametersAtomically_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__srv__SetParametersAtomically_Event__TYPE_NAME, 48, 48},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__SetParametersAtomically__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[9];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 9, 9};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__SetParametersAtomically__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__msg__SetParametersResult__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__SetParametersAtomically_Event__get_individual_type_description_source(NULL);
    sources[6] = *rcl_interfaces__srv__SetParametersAtomically_Request__get_individual_type_description_source(NULL);
    sources[7] = *rcl_interfaces__srv__SetParametersAtomically_Response__get_individual_type_description_source(NULL);
    sources[8] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__SetParametersAtomically_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__SetParametersAtomically_Request__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__SetParametersAtomically_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__SetParametersAtomically_Response__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__SetParametersResult__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__srv__SetParametersAtomically_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__srv__SetParametersAtomically_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__msg__SetParametersResult__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__srv__SetParametersAtomically_Request__get_individual_type_description_source(NULL);
    sources[6] = *rcl_interfaces__srv__SetParametersAtomically_Response__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
