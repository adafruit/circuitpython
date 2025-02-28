// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/LoggerLevel.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/logger_level__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__LoggerLevel__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x95, 0x78, 0x5c, 0xc4, 0x2f, 0x04, 0x8a, 0xb4,
      0xf3, 0x95, 0xaf, 0x65, 0x03, 0x5a, 0xea, 0xf2,
      0x18, 0x1d, 0x8e, 0x1c, 0x7a, 0x44, 0xed, 0xb8,
      0xad, 0x45, 0x58, 0x44, 0x5f, 0xdb, 0x43, 0xc0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__LoggerLevel__TYPE_NAME[] = "rcl_interfaces/msg/LoggerLevel";

// Define type names, field names, and default values
static char rcl_interfaces__msg__LoggerLevel__FIELD_NAME__name[] = "name";
static char rcl_interfaces__msg__LoggerLevel__FIELD_NAME__level[] = "level";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__LoggerLevel__FIELDS[] = {
  {
    {rcl_interfaces__msg__LoggerLevel__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__LoggerLevel__FIELD_NAME__level, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__LoggerLevel__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
      {rcl_interfaces__msg__LoggerLevel__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# All available logger levels; these correspond to the enum in rcutils/logger.h\n"
  "\n"
  "uint8 LOG_LEVEL_UNKNOWN = 0\n"
  "uint8 LOG_LEVEL_DEBUG = 10\n"
  "uint8 LOG_LEVEL_INFO = 20\n"
  "uint8 LOG_LEVEL_WARN = 30\n"
  "uint8 LOG_LEVEL_ERROR = 40\n"
  "uint8 LOG_LEVEL_FATAL = 50\n"
  "\n"
  "# The logger name.\n"
  "string name\n"
  "\n"
  "# The logger level\n"
  "uint32 level";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__LoggerLevel__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__LoggerLevel__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 307, 307},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__LoggerLevel__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__LoggerLevel__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
