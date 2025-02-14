// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from composition_interfaces:srv/LoadNode.idl
// generated code does not contain a copyright notice

#include "composition_interfaces/srv/detail/load_node__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__LoadNode__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x2e, 0x52, 0x1e, 0xf6, 0x06, 0x6f, 0xba, 0x6d,
      0x7f, 0xc3, 0xa3, 0xb1, 0x75, 0xd4, 0x86, 0x70,
      0x9f, 0x01, 0xb4, 0xd7, 0x38, 0xd9, 0xa5, 0x91,
      0x88, 0x85, 0x85, 0x46, 0xfc, 0x8e, 0x5a, 0xcd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__LoadNode_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x7d, 0x34, 0x96, 0x17, 0x5f, 0x54, 0xf9, 0x2d,
      0x65, 0x25, 0x47, 0xe1, 0x44, 0xc9, 0xb2, 0xa3,
      0xb2, 0x2a, 0xfd, 0x60, 0x4f, 0x84, 0xed, 0x70,
      0xad, 0xfc, 0xd1, 0xcf, 0xe3, 0x58, 0x01, 0xb5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__LoadNode_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x08, 0xcd, 0xf9, 0x42, 0xef, 0xab, 0xec, 0x24,
      0x48, 0xfd, 0x03, 0xc1, 0x31, 0xf3, 0x62, 0x7a,
      0x33, 0xed, 0x05, 0x37, 0x64, 0x00, 0x61, 0x24,
      0x25, 0xf8, 0x71, 0xfe, 0xb8, 0x7c, 0xa9, 0xfd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_composition_interfaces
const rosidl_type_hash_t *
composition_interfaces__srv__LoadNode_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xdb, 0x27, 0x6d, 0x4e, 0xb3, 0x14, 0x43, 0xe4,
      0xd3, 0x0c, 0xfb, 0x4b, 0x0c, 0x01, 0xbc, 0xd2,
      0x2a, 0xf2, 0x7f, 0xc9, 0x5b, 0xe8, 0xe1, 0xba,
      0x2c, 0xc5, 0x6d, 0x77, 0x72, 0x21, 0x9c, 0xd2,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "rcl_interfaces/msg/detail/parameter_value__functions.h"
#include "rcl_interfaces/msg/detail/parameter__functions.h"
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
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char composition_interfaces__srv__LoadNode__TYPE_NAME[] = "composition_interfaces/srv/LoadNode";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char composition_interfaces__srv__LoadNode_Event__TYPE_NAME[] = "composition_interfaces/srv/LoadNode_Event";
static char composition_interfaces__srv__LoadNode_Request__TYPE_NAME[] = "composition_interfaces/srv/LoadNode_Request";
static char composition_interfaces__srv__LoadNode_Response__TYPE_NAME[] = "composition_interfaces/srv/LoadNode_Response";
static char rcl_interfaces__msg__Parameter__TYPE_NAME[] = "rcl_interfaces/msg/Parameter";
static char rcl_interfaces__msg__ParameterValue__TYPE_NAME[] = "rcl_interfaces/msg/ParameterValue";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char composition_interfaces__srv__LoadNode__FIELD_NAME__request_message[] = "request_message";
static char composition_interfaces__srv__LoadNode__FIELD_NAME__response_message[] = "response_message";
static char composition_interfaces__srv__LoadNode__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__LoadNode__FIELDS[] = {
  {
    {composition_interfaces__srv__LoadNode__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {composition_interfaces__srv__LoadNode_Event__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__LoadNode__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Event__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
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
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__LoadNode__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__LoadNode__TYPE_NAME, 35, 35},
      {composition_interfaces__srv__LoadNode__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__LoadNode__REFERENCED_TYPE_DESCRIPTIONS, 7, 7},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__LoadNode_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__LoadNode_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = composition_interfaces__srv__LoadNode_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__Parameter__EXPECTED_HASH, rcl_interfaces__msg__Parameter__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__msg__Parameter__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterValue__EXPECTED_HASH, rcl_interfaces__msg__ParameterValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = rcl_interfaces__msg__ParameterValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[6].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__package_name[] = "package_name";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__plugin_name[] = "plugin_name";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__node_name[] = "node_name";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__node_namespace[] = "node_namespace";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__log_level[] = "log_level";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__remap_rules[] = "remap_rules";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__parameters[] = "parameters";
static char composition_interfaces__srv__LoadNode_Request__FIELD_NAME__extra_arguments[] = "extra_arguments";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__LoadNode_Request__FIELDS[] = {
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__package_name, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__plugin_name, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__node_name, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__node_namespace, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__log_level, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__remap_rules, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__parameters, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__FIELD_NAME__extra_arguments, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__Parameter__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__LoadNode_Request__REFERENCED_TYPE_DESCRIPTIONS[] = {
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
composition_interfaces__srv__LoadNode_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
      {composition_interfaces__srv__LoadNode_Request__FIELDS, 8, 8},
    },
    {composition_interfaces__srv__LoadNode_Request__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
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
static char composition_interfaces__srv__LoadNode_Response__FIELD_NAME__success[] = "success";
static char composition_interfaces__srv__LoadNode_Response__FIELD_NAME__error_message[] = "error_message";
static char composition_interfaces__srv__LoadNode_Response__FIELD_NAME__full_node_name[] = "full_node_name";
static char composition_interfaces__srv__LoadNode_Response__FIELD_NAME__unique_id[] = "unique_id";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__LoadNode_Response__FIELDS[] = {
  {
    {composition_interfaces__srv__LoadNode_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Response__FIELD_NAME__error_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Response__FIELD_NAME__full_node_name, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Response__FIELD_NAME__unique_id, 9, 9},
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
composition_interfaces__srv__LoadNode_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
      {composition_interfaces__srv__LoadNode_Response__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char composition_interfaces__srv__LoadNode_Event__FIELD_NAME__info[] = "info";
static char composition_interfaces__srv__LoadNode_Event__FIELD_NAME__request[] = "request";
static char composition_interfaces__srv__LoadNode_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field composition_interfaces__srv__LoadNode_Event__FIELDS[] = {
  {
    {composition_interfaces__srv__LoadNode_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription composition_interfaces__srv__LoadNode_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
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
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
composition_interfaces__srv__LoadNode_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {composition_interfaces__srv__LoadNode_Event__TYPE_NAME, 41, 41},
      {composition_interfaces__srv__LoadNode_Event__FIELDS, 3, 3},
    },
    {composition_interfaces__srv__LoadNode_Event__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = composition_interfaces__srv__LoadNode_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = composition_interfaces__srv__LoadNode_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__Parameter__EXPECTED_HASH, rcl_interfaces__msg__Parameter__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = rcl_interfaces__msg__Parameter__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterValue__EXPECTED_HASH, rcl_interfaces__msg__ParameterValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = rcl_interfaces__msg__ParameterValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# The ROS package in which the composable node can be found.\n"
  "string package_name\n"
  "\n"
  "# A plugin within the ROS package \"package_name\".\n"
  "string plugin_name\n"
  "\n"
  "# The assigned name of the composable node. Leave empty to use the node's\n"
  "# default name.\n"
  "string node_name\n"
  "\n"
  "# The assigned namespace of the composable node. Leave empty to use the node's\n"
  "# default namespace.\n"
  "string node_namespace\n"
  "\n"
  "# The assigned log level of the composable node. Enum values are found in\n"
  "# message rcl_interfaces/Log.\n"
  "uint8 log_level\n"
  "\n"
  "# Remapping rules for this composable node.\n"
  "#\n"
  "# For more info about static_remapping rules and their syntax, see\n"
  "# https://design.ros2.org/articles/static_remapping.html\n"
  "# TODO(sloretz) rcl_interfaces message for remap rules?\n"
  "string[] remap_rules\n"
  "\n"
  "# The Parameters of this composable node to set.\n"
  "rcl_interfaces/Parameter[] parameters\n"
  "\n"
  "# key/value arguments that are specific to a type of container process.\n"
  "rcl_interfaces/Parameter[] extra_arguments\n"
  "---\n"
  "# True if the node was successfully loaded.\n"
  "bool success\n"
  "\n"
  "# Human readable error message if success is false, else empty string.\n"
  "string error_message\n"
  "\n"
  "# Name of the loaded composable node (including namespace).\n"
  "string full_node_name\n"
  "\n"
  "# A unique identifier for the loaded node.\n"
  "uint64 unique_id";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__LoadNode__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__LoadNode__TYPE_NAME, 35, 35},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 1253, 1253},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__LoadNode_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__LoadNode_Request__TYPE_NAME, 43, 43},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__LoadNode_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__LoadNode_Response__TYPE_NAME, 44, 44},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
composition_interfaces__srv__LoadNode_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {composition_interfaces__srv__LoadNode_Event__TYPE_NAME, 41, 41},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__LoadNode__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[8];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 8, 8};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__LoadNode__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__LoadNode_Event__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__LoadNode_Request__get_individual_type_description_source(NULL);
    sources[4] = *composition_interfaces__srv__LoadNode_Response__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[6] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    sources[7] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__LoadNode_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__LoadNode_Request__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__LoadNode_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__LoadNode_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
composition_interfaces__srv__LoadNode_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *composition_interfaces__srv__LoadNode_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *composition_interfaces__srv__LoadNode_Request__get_individual_type_description_source(NULL);
    sources[3] = *composition_interfaces__srv__LoadNode_Response__get_individual_type_description_source(NULL);
    sources[4] = *rcl_interfaces__msg__Parameter__get_individual_type_description_source(NULL);
    sources[5] = *rcl_interfaces__msg__ParameterValue__get_individual_type_description_source(NULL);
    sources[6] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
