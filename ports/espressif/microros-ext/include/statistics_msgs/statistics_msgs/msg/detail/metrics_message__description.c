// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from statistics_msgs:msg/MetricsMessage.idl
// generated code does not contain a copyright notice

#include "statistics_msgs/msg/detail/metrics_message__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_statistics_msgs
const rosidl_type_hash_t *
statistics_msgs__msg__MetricsMessage__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x36, 0xa6, 0x7f, 0xb4, 0x99, 0xfc, 0x2a, 0x11,
      0xb8, 0xb9, 0xff, 0x9a, 0xc7, 0x35, 0xed, 0x76,
      0xe8, 0xd5, 0xa1, 0xf7, 0x77, 0x5d, 0x6d, 0x5b,
      0x97, 0xb6, 0x25, 0x18, 0x8e, 0xc6, 0x8e, 0x20,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "statistics_msgs/msg/detail/statistic_data_point__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t statistics_msgs__msg__StatisticDataPoint__EXPECTED_HASH = {1, {
    0xb7, 0xe6, 0x1a, 0x40, 0x73, 0x46, 0xea, 0x91,
    0x2e, 0xff, 0xb6, 0x95, 0x40, 0x23, 0xd2, 0xd2,
    0xc9, 0xad, 0xf8, 0xf7, 0xbc, 0x10, 0xf6, 0xca,
    0xe5, 0x76, 0xd3, 0x50, 0xc4, 0x45, 0xf6, 0xa5,
  }};
#endif

static char statistics_msgs__msg__MetricsMessage__TYPE_NAME[] = "statistics_msgs/msg/MetricsMessage";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char statistics_msgs__msg__StatisticDataPoint__TYPE_NAME[] = "statistics_msgs/msg/StatisticDataPoint";

// Define type names, field names, and default values
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__measurement_source_name[] = "measurement_source_name";
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__metrics_source[] = "metrics_source";
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__unit[] = "unit";
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__window_start[] = "window_start";
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__window_stop[] = "window_stop";
static char statistics_msgs__msg__MetricsMessage__FIELD_NAME__statistics[] = "statistics";

static rosidl_runtime_c__type_description__Field statistics_msgs__msg__MetricsMessage__FIELDS[] = {
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__measurement_source_name, 23, 23},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__metrics_source, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__unit, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__window_start, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__window_stop, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__MetricsMessage__FIELD_NAME__statistics, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {statistics_msgs__msg__StatisticDataPoint__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription statistics_msgs__msg__MetricsMessage__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {statistics_msgs__msg__StatisticDataPoint__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
statistics_msgs__msg__MetricsMessage__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {statistics_msgs__msg__MetricsMessage__TYPE_NAME, 34, 34},
      {statistics_msgs__msg__MetricsMessage__FIELDS, 6, 6},
    },
    {statistics_msgs__msg__MetricsMessage__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&statistics_msgs__msg__StatisticDataPoint__EXPECTED_HASH, statistics_msgs__msg__StatisticDataPoint__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = statistics_msgs__msg__StatisticDataPoint__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "#############################################\n"
  "# A generic metrics message providing statistics for measurements from different sources. For example,\n"
  "# measure a system's CPU % for a given window yields the following data points over a window of time:\n"
  "#\n"
  "#   - average cpu %\n"
  "#   - std deviation\n"
  "#   - min\n"
  "#   - max\n"
  "#   - sample count\n"
  "#\n"
  "# These are all represented as different 'StatisticDataPoint's.\n"
  "#############################################\n"
  "\n"
  "# Name metric measurement source, e.g., node, topic, or process name\n"
  "string measurement_source_name\n"
  "\n"
  "# Name of the metric being measured, e.g. cpu_percentage, free_memory_mb, message_age, etc.\n"
  "string metrics_source\n"
  "\n"
  "# Unit of measure of the metric, e.g. percent, mb, seconds, etc.\n"
  "string unit\n"
  "\n"
  "# Measurement window start time\n"
  "builtin_interfaces/Time window_start\n"
  "\n"
  "# Measurement window end time\n"
  "builtin_interfaces/Time window_stop\n"
  "\n"
  "# A list of statistics data point, defined in StatisticDataPoint.msg\n"
  "StatisticDataPoint[] statistics";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
statistics_msgs__msg__MetricsMessage__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {statistics_msgs__msg__MetricsMessage__TYPE_NAME, 34, 34},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 977, 977},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
statistics_msgs__msg__MetricsMessage__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *statistics_msgs__msg__MetricsMessage__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *statistics_msgs__msg__StatisticDataPoint__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
