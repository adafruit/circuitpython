// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from statistics_msgs:msg/StatisticDataType.idl
// generated code does not contain a copyright notice

#include "statistics_msgs/msg/detail/statistic_data_type__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_statistics_msgs
const rosidl_type_hash_t *
statistics_msgs__msg__StatisticDataType__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x84, 0x0d, 0x83, 0xbd, 0x5d, 0xc6, 0x60, 0xe0,
      0xc6, 0xe9, 0x96, 0xf9, 0xe1, 0xab, 0x80, 0xbe,
      0xc3, 0xc5, 0xa9, 0x19, 0x76, 0x36, 0x0a, 0x40,
      0xd6, 0xcc, 0x57, 0x9a, 0x37, 0xa8, 0xc9, 0x59,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char statistics_msgs__msg__StatisticDataType__TYPE_NAME[] = "statistics_msgs/msg/StatisticDataType";

// Define type names, field names, and default values
static char statistics_msgs__msg__StatisticDataType__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field statistics_msgs__msg__StatisticDataType__FIELDS[] = {
  {
    {statistics_msgs__msg__StatisticDataType__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
statistics_msgs__msg__StatisticDataType__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {statistics_msgs__msg__StatisticDataType__TYPE_NAME, 37, 37},
      {statistics_msgs__msg__StatisticDataType__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "#############################################\n"
  "# This file contains the commonly used constants for the statistics data type.\n"
  "#\n"
  "# The value 0 is reserved for unitialized statistic message data type.\n"
  "# Range of values: [0, 255].\n"
  "# Unallowed values: any value that is not specified in this file.\n"
  "#\n"
  "#############################################\n"
  "\n"
  "# Constant for uninitialized\n"
  "uint8 STATISTICS_DATA_TYPE_UNINITIALIZED = 0\n"
  "\n"
  "# Allowed values\n"
  "uint8 STATISTICS_DATA_TYPE_AVERAGE = 1\n"
  "uint8 STATISTICS_DATA_TYPE_MINIMUM = 2\n"
  "uint8 STATISTICS_DATA_TYPE_MAXIMUM = 3\n"
  "uint8 STATISTICS_DATA_TYPE_STDDEV = 4\n"
  "uint8 STATISTICS_DATA_TYPE_SAMPLE_COUNT = 5";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
statistics_msgs__msg__StatisticDataType__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {statistics_msgs__msg__StatisticDataType__TYPE_NAME, 37, 37},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 633, 633},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
statistics_msgs__msg__StatisticDataType__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *statistics_msgs__msg__StatisticDataType__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
