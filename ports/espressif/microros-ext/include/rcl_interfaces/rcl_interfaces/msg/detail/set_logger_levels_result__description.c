// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/SetLoggerLevelsResult.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/set_logger_levels_result__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__SetLoggerLevelsResult__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x93, 0x16, 0xe5, 0xe6, 0x79, 0xa5, 0xb7, 0x2d,
      0x2d, 0xd7, 0xfd, 0x80, 0xc5, 0x39, 0xba, 0xe9,
      0xe1, 0x06, 0xfa, 0x08, 0x90, 0xa0, 0x6d, 0xc5,
      0xda, 0x3a, 0x81, 0x77, 0xa3, 0xff, 0x69, 0x09,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char rcl_interfaces__msg__SetLoggerLevelsResult__TYPE_NAME[] = "rcl_interfaces/msg/SetLoggerLevelsResult";

// Define type names, field names, and default values
static char rcl_interfaces__msg__SetLoggerLevelsResult__FIELD_NAME__successful[] = "successful";
static char rcl_interfaces__msg__SetLoggerLevelsResult__FIELD_NAME__reason[] = "reason";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__SetLoggerLevelsResult__FIELDS[] = {
  {
    {rcl_interfaces__msg__SetLoggerLevelsResult__FIELD_NAME__successful, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__SetLoggerLevelsResult__FIELD_NAME__reason, 6, 6},
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
rcl_interfaces__msg__SetLoggerLevelsResult__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__SetLoggerLevelsResult__TYPE_NAME, 40, 40},
      {rcl_interfaces__msg__SetLoggerLevelsResult__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# True when succeed, false when failed.\n"
  "bool successful\n"
  "\n"
  "# Reason why the setting was either successful or a failure.\n"
  "string reason";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__SetLoggerLevelsResult__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__SetLoggerLevelsResult__TYPE_NAME, 40, 40},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 132, 132},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__SetLoggerLevelsResult__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__SetLoggerLevelsResult__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
