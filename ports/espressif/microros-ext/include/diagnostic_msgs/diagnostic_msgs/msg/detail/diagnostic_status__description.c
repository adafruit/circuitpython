// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from diagnostic_msgs:msg/DiagnosticStatus.idl
// generated code does not contain a copyright notice

#include "diagnostic_msgs/msg/detail/diagnostic_status__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__msg__DiagnosticStatus__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xb0, 0xe3, 0xe6, 0x92, 0xea, 0x2d, 0x54, 0xa8,
      0xaf, 0x2f, 0x4e, 0xf1, 0x93, 0x0e, 0x81, 0x55,
      0x6a, 0x2d, 0xb5, 0x52, 0x16, 0xb7, 0x71, 0xf8,
      0xa7, 0xd2, 0x72, 0x4e, 0xd4, 0x7b, 0xf0, 0xe4,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "diagnostic_msgs/msg/detail/key_value__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t diagnostic_msgs__msg__KeyValue__EXPECTED_HASH = {1, {
    0xd6, 0x80, 0x81, 0xea, 0xa5, 0x40, 0x28, 0x8c,
    0x54, 0x40, 0x75, 0x3b, 0xae, 0xce, 0xf0, 0xc4,
    0xe1, 0x6e, 0x81, 0xa5, 0xf7, 0x8a, 0xd6, 0x89,
    0x02, 0xde, 0xd5, 0x10, 0x04, 0x13, 0xbb, 0x42,
  }};
#endif

static char diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME[] = "diagnostic_msgs/msg/DiagnosticStatus";
static char diagnostic_msgs__msg__KeyValue__TYPE_NAME[] = "diagnostic_msgs/msg/KeyValue";

// Define type names, field names, and default values
static char diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__level[] = "level";
static char diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__name[] = "name";
static char diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__message[] = "message";
static char diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__hardware_id[] = "hardware_id";
static char diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__values[] = "values";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__msg__DiagnosticStatus__FIELDS[] = {
  {
    {diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__level, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BYTE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__name, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__message, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__hardware_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__FIELD_NAME__values, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__msg__DiagnosticStatus__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__msg__DiagnosticStatus__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
      {diagnostic_msgs__msg__DiagnosticStatus__FIELDS, 5, 5},
    },
    {diagnostic_msgs__msg__DiagnosticStatus__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&diagnostic_msgs__msg__KeyValue__EXPECTED_HASH, diagnostic_msgs__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = diagnostic_msgs__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message holds the status of an individual component of the robot.\n"
  "\n"
  "# Possible levels of operations.\n"
  "byte OK=0\n"
  "byte WARN=1\n"
  "byte ERROR=2\n"
  "byte STALE=3\n"
  "\n"
  "# Level of operation enumerated above.\n"
  "byte level\n"
  "# A description of the test/component reporting.\n"
  "string name\n"
  "# A description of the status.\n"
  "string message\n"
  "# A hardware unique string.\n"
  "string hardware_id\n"
  "# An array of values associated with the status.\n"
  "KeyValue[] values\n"
  "";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 428, 428},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__msg__DiagnosticStatus__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(NULL),
    sources[1] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
