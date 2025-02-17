// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from diagnostic_msgs:srv/AddDiagnostics.idl
// generated code does not contain a copyright notice

#include "diagnostic_msgs/srv/detail/add_diagnostics__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__AddDiagnostics__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x6b, 0x91, 0x08, 0x4c, 0x9f, 0x8b, 0xff, 0xd9,
      0xb0, 0xc1, 0xf6, 0x88, 0x3e, 0x0e, 0x8e, 0x49,
      0x1a, 0x83, 0x1f, 0x4f, 0xc6, 0xc6, 0xae, 0xc7,
      0x7a, 0x4e, 0xfa, 0xf3, 0xf1, 0xea, 0x90, 0xb5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__AddDiagnostics_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe0, 0xb6, 0x57, 0x2a, 0x07, 0xc2, 0xb3, 0xca,
      0x2c, 0x4f, 0x2a, 0xcf, 0x74, 0x22, 0x97, 0x66,
      0x1f, 0x6c, 0xeb, 0x50, 0xf5, 0xba, 0xa4, 0xf9,
      0x30, 0x5b, 0x60, 0x94, 0xcf, 0x95, 0x01, 0x46,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__AddDiagnostics_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe7, 0x07, 0x0a, 0x23, 0x73, 0xc8, 0x92, 0xc4,
      0xcf, 0x07, 0x1d, 0xcd, 0x90, 0x40, 0x5d, 0x76,
      0x96, 0x18, 0x7e, 0xb2, 0xff, 0x0e, 0x40, 0xd8,
      0xb6, 0x47, 0x2a, 0x04, 0xa4, 0xb7, 0x3d, 0x22,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__srv__AddDiagnostics_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x21, 0xac, 0x80, 0x18, 0x10, 0x29, 0x7a, 0xe8,
      0x6b, 0x5e, 0x5d, 0xcc, 0x66, 0x5b, 0xbd, 0x19,
      0x18, 0xbf, 0x71, 0xb1, 0x96, 0x15, 0xc0, 0x31,
      0xc4, 0xc6, 0xe7, 0x55, 0xad, 0xa8, 0x0c, 0x7c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
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
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char diagnostic_msgs__srv__AddDiagnostics__TYPE_NAME[] = "diagnostic_msgs/srv/AddDiagnostics";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char diagnostic_msgs__srv__AddDiagnostics_Event__TYPE_NAME[] = "diagnostic_msgs/srv/AddDiagnostics_Event";
static char diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME[] = "diagnostic_msgs/srv/AddDiagnostics_Request";
static char diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME[] = "diagnostic_msgs/srv/AddDiagnostics_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__request_message[] = "request_message";
static char diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__response_message[] = "response_message";
static char diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__AddDiagnostics__FIELDS[] = {
  {
    {diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {diagnostic_msgs__srv__AddDiagnostics_Event__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__srv__AddDiagnostics__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Event__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__srv__AddDiagnostics__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__AddDiagnostics__TYPE_NAME, 34, 34},
      {diagnostic_msgs__srv__AddDiagnostics__FIELDS, 3, 3},
    },
    {diagnostic_msgs__srv__AddDiagnostics__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__srv__AddDiagnostics_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__AddDiagnostics_Request__FIELD_NAME__load_namespace[] = "load_namespace";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__AddDiagnostics_Request__FIELDS[] = {
  {
    {diagnostic_msgs__srv__AddDiagnostics_Request__FIELD_NAME__load_namespace, 14, 14},
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
diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
      {diagnostic_msgs__srv__AddDiagnostics_Request__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__AddDiagnostics_Response__FIELD_NAME__success[] = "success";
static char diagnostic_msgs__srv__AddDiagnostics_Response__FIELD_NAME__message[] = "message";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__AddDiagnostics_Response__FIELDS[] = {
  {
    {diagnostic_msgs__srv__AddDiagnostics_Response__FIELD_NAME__success, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Response__FIELD_NAME__message, 7, 7},
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
diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
      {diagnostic_msgs__srv__AddDiagnostics_Response__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__info[] = "info";
static char diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__request[] = "request";
static char diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__srv__AddDiagnostics_Event__FIELDS[] = {
  {
    {diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__srv__AddDiagnostics_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__srv__AddDiagnostics_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__srv__AddDiagnostics_Event__TYPE_NAME, 40, 40},
      {diagnostic_msgs__srv__AddDiagnostics_Event__FIELDS, 3, 3},
    },
    {diagnostic_msgs__srv__AddDiagnostics_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This service is used as part of the process for loading analyzers at runtime,\n"
  "# and should be used by a loader script or program, not as a standalone service.\n"
  "# Information about dynamic addition of analyzers can be found at\n"
  "# http://wiki.ros.org/diagnostics/Tutorials/Adding%20Analyzers%20at%20Runtime\n"
  "\n"
  "# The load_namespace parameter defines the namespace where parameters for the\n"
  "# initialization of analyzers in the diagnostic aggregator have been loaded. The\n"
  "# value should be a global name (i.e. /my/name/space), not a relative\n"
  "# (my/name/space) or private (~my/name/space) name. Analyzers will not be added\n"
  "# if a non-global name is used. The call will also fail if the namespace\n"
  "# contains parameters that follow a namespace structure that does not conform to\n"
  "# that expected by the analyzer definitions. See\n"
  "# http://wiki.ros.org/diagnostics/Tutorials/Configuring%20Diagnostic%20Aggregators\n"
  "# and http://wiki.ros.org/diagnostics/Tutorials/Using%20the%20GenericAnalyzer\n"
  "# for examples of the structure of yaml files which are expected to have been\n"
  "# loaded into the namespace.\n"
  "string load_namespace\n"
  "---\n"
  "\n"
  "# True if diagnostic aggregator was updated with new diagnostics, False\n"
  "# otherwise. A false return value means that either there is a bond in the\n"
  "# aggregator which already used the requested namespace, or the initialization\n"
  "# of analyzers failed.\n"
  "bool success\n"
  "\n"
  "# Message with additional information about the success or failure\n"
  "string message";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__AddDiagnostics__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__AddDiagnostics__TYPE_NAME, 34, 34},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 1458, 1458},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__AddDiagnostics_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__AddDiagnostics_Request__TYPE_NAME, 42, 42},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__AddDiagnostics_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__AddDiagnostics_Response__TYPE_NAME, 43, 43},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__srv__AddDiagnostics_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__srv__AddDiagnostics_Event__TYPE_NAME, 40, 40},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__AddDiagnostics__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__AddDiagnostics__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__srv__AddDiagnostics_Event__get_individual_type_description_source(NULL);
    sources[3] = *diagnostic_msgs__srv__AddDiagnostics_Request__get_individual_type_description_source(NULL);
    sources[4] = *diagnostic_msgs__srv__AddDiagnostics_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__AddDiagnostics_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__AddDiagnostics_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__srv__AddDiagnostics_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__srv__AddDiagnostics_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__srv__AddDiagnostics_Request__get_individual_type_description_source(NULL);
    sources[3] = *diagnostic_msgs__srv__AddDiagnostics_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
