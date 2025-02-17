// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/Log.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/log__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__Log__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe2, 0x8c, 0xe2, 0x54, 0xca, 0x8a, 0xbc, 0x06,
      0xab, 0xf9, 0x27, 0x73, 0xb7, 0x46, 0x02, 0xcd,
      0xbf, 0x11, 0x6e, 0xd3, 0x4f, 0xba, 0xf2, 0x94,
      0xfb, 0x9f, 0x81, 0xda, 0x9f, 0x31, 0x8e, 0xac,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
#endif

static char rcl_interfaces__msg__Log__TYPE_NAME[] = "rcl_interfaces/msg/Log";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char rcl_interfaces__msg__Log__FIELD_NAME__stamp[] = "stamp";
static char rcl_interfaces__msg__Log__FIELD_NAME__level[] = "level";
static char rcl_interfaces__msg__Log__FIELD_NAME__name[] = "name";
static char rcl_interfaces__msg__Log__FIELD_NAME__msg[] = "msg";
static char rcl_interfaces__msg__Log__FIELD_NAME__file[] = "file";
static char rcl_interfaces__msg__Log__FIELD_NAME__function[] = "function";
static char rcl_interfaces__msg__Log__FIELD_NAME__line[] = "line";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__Log__FIELDS[] = {
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__stamp, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__level, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__msg, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__file, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__function, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__Log__FIELD_NAME__line, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__msg__Log__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__Log__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__Log__TYPE_NAME, 22, 22},
      {rcl_interfaces__msg__Log__FIELDS, 7, 7},
    },
    {rcl_interfaces__msg__Log__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "##\n"
  "## Severity level constants\n"
  "## \n"
  "## These logging levels follow the Python Standard\n"
  "## https://docs.python.org/3/library/logging.html#logging-levels\n"
  "## And are implemented in rcutils as well\n"
  "## https://github.com/ros2/rcutils/blob/35f29850064e0c33a4063cbc947ebbfeada11dba/include/rcutils/logging.h#L164-L172\n"
  "## This leaves space for other standard logging levels to be inserted in the middle in the future,\n"
  "## as well as custom user defined levels.\n"
  "## Since there are several other logging enumeration standard for different implementations,\n"
  "## other logging implementations may need to provide level mappings to match their internal implementations.\n"
  "##\n"
  "\n"
  "# Debug is for pedantic information, which is useful when debugging issues.\n"
  "uint8 DEBUG=10\n"
  "\n"
  "# Info is the standard informational level and is used to report expected\n"
  "# information.\n"
  "uint8 INFO=20\n"
  "\n"
  "# Warning is for information that may potentially cause issues or possibly unexpected\n"
  "# behavior.\n"
  "uint8 WARN=30\n"
  "\n"
  "# Error is for information that this node cannot resolve.\n"
  "uint8 ERROR=40\n"
  "\n"
  "# Information about a impending node shutdown.\n"
  "uint8 FATAL=50\n"
  "\n"
  "##\n"
  "## Fields\n"
  "##\n"
  "\n"
  "# Timestamp when this message was generated by the node.\n"
  "builtin_interfaces/Time stamp\n"
  "\n"
  "# Corresponding log level, see above definitions.\n"
  "uint8 level\n"
  "\n"
  "# The name representing the logger this message came from.\n"
  "string name\n"
  "\n"
  "# The full log message.\n"
  "string msg\n"
  "\n"
  "# The file the message came from.\n"
  "string file\n"
  "\n"
  "# The function the message came from.\n"
  "string function\n"
  "\n"
  "# The line in the file the message came from.\n"
  "uint32 line";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__Log__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__Log__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1539, 1539},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__Log__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__Log__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
