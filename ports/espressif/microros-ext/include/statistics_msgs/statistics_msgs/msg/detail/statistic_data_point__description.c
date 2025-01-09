// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from statistics_msgs:msg/StatisticDataPoint.idl
// generated code does not contain a copyright notice

#include "statistics_msgs/msg/detail/statistic_data_point__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_statistics_msgs
const rosidl_type_hash_t *
statistics_msgs__msg__StatisticDataPoint__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb7, 0xe6, 0x1a, 0x40, 0x73, 0x46, 0xea, 0x91,
      0x2e, 0xff, 0xb6, 0x95, 0x40, 0x23, 0xd2, 0xd2,
      0xc9, 0xad, 0xf8, 0xf7, 0xbc, 0x10, 0xf6, 0xca,
      0xe5, 0x76, 0xd3, 0x50, 0xc4, 0x45, 0xf6, 0xa5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char statistics_msgs__msg__StatisticDataPoint__TYPE_NAME[] = "statistics_msgs/msg/StatisticDataPoint";

// Define type names, field names, and default values
static char statistics_msgs__msg__StatisticDataPoint__FIELD_NAME__data_type[] = "data_type";
static char statistics_msgs__msg__StatisticDataPoint__FIELD_NAME__data[] = "data";

static rosidl_runtime_c__type_description__Field statistics_msgs__msg__StatisticDataPoint__FIELDS[] = {
  {
    {statistics_msgs__msg__StatisticDataPoint__FIELD_NAME__data_type, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__StatisticDataPoint__FIELD_NAME__data, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
statistics_msgs__msg__StatisticDataPoint__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {statistics_msgs__msg__StatisticDataPoint__TYPE_NAME, 38, 38},
      {statistics_msgs__msg__StatisticDataPoint__FIELDS, 2, 2},
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
  "# This holds the structure of a single data point of a StatisticDataType.\n"
  "#\n"
  "# This message is used in MetricsStatisticsMessage, defined in MetricsStatisticsMessage.msg.\n"
  "#\n"
  "# Examples of the value of data point are\n"
  "# - average size of messages received\n"
  "# - standard deviation of the period of messages published\n"
  "# - maximum age of messages published\n"
  "#\n"
  "# A value of nan represents no data is available.\n"
  "# One example is that standard deviation is only available when there are two or more data points but there is only one,\n"
  "# and in this case the value would be nan.\n"
  "# +inf and -inf are not allowed.\n"
  "#\n"
  "#############################################\n"
  "\n"
  "# The statistic type of this data point, defined in StatisticDataType.msg\n"
  "# Default value should be StatisticDataType.STATISTICS_DATA_TYPE_UNINITIALIZED (0).\n"
  "uint8 data_type\n"
  "\n"
  "# The value of the data point\n"
  "float64 data";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
statistics_msgs__msg__StatisticDataPoint__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {statistics_msgs__msg__StatisticDataPoint__TYPE_NAME, 38, 38},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 910, 910},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
statistics_msgs__msg__StatisticDataPoint__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *statistics_msgs__msg__StatisticDataPoint__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
