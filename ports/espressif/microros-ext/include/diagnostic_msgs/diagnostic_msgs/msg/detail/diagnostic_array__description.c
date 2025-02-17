// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from diagnostic_msgs:msg/DiagnosticArray.idl
// generated code does not contain a copyright notice

#include "diagnostic_msgs/msg/detail/diagnostic_array__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_diagnostic_msgs
const rosidl_type_hash_t *
diagnostic_msgs__msg__DiagnosticArray__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5a, 0x8a, 0x36, 0xef, 0xb0, 0x5f, 0xb2, 0x50,
      0x70, 0xfa, 0x0f, 0xb3, 0x81, 0x02, 0x90, 0xc0,
      0xe6, 0xcd, 0x48, 0x62, 0xb5, 0x4a, 0x8f, 0xb9,
      0x75, 0xa1, 0xee, 0x8d, 0xc5, 0x5a, 0x33, 0x3e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "diagnostic_msgs/msg/detail/diagnostic_status__functions.h"
#include "std_msgs/msg/detail/header__functions.h"
#include "diagnostic_msgs/msg/detail/key_value__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH = {1, {
    0xb0, 0xe3, 0xe6, 0x92, 0xea, 0x2d, 0x54, 0xa8,
    0xaf, 0x2f, 0x4e, 0xf1, 0x93, 0x0e, 0x81, 0x55,
    0x6a, 0x2d, 0xb5, 0x52, 0x16, 0xb7, 0x71, 0xf8,
    0xa7, 0xd2, 0x72, 0x4e, 0xd4, 0x7b, 0xf0, 0xe4,
  }};
static const rosidl_type_hash_t diagnostic_msgs__msg__KeyValue__EXPECTED_HASH = {1, {
    0xd6, 0x80, 0x81, 0xea, 0xa5, 0x40, 0x28, 0x8c,
    0x54, 0x40, 0x75, 0x3b, 0xae, 0xce, 0xf0, 0xc4,
    0xe1, 0x6e, 0x81, 0xa5, 0xf7, 0x8a, 0xd6, 0x89,
    0x02, 0xde, 0xd5, 0x10, 0x04, 0x13, 0xbb, 0x42,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char diagnostic_msgs__msg__DiagnosticArray__TYPE_NAME[] = "diagnostic_msgs/msg/DiagnosticArray";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME[] = "diagnostic_msgs/msg/DiagnosticStatus";
static char diagnostic_msgs__msg__KeyValue__TYPE_NAME[] = "diagnostic_msgs/msg/KeyValue";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char diagnostic_msgs__msg__DiagnosticArray__FIELD_NAME__header[] = "header";
static char diagnostic_msgs__msg__DiagnosticArray__FIELD_NAME__status[] = "status";

static rosidl_runtime_c__type_description__Field diagnostic_msgs__msg__DiagnosticArray__FIELDS[] = {
  {
    {diagnostic_msgs__msg__DiagnosticArray__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticArray__FIELD_NAME__status, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription diagnostic_msgs__msg__DiagnosticArray__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__DiagnosticStatus__TYPE_NAME, 36, 36},
    {NULL, 0, 0},
  },
  {
    {diagnostic_msgs__msg__KeyValue__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
diagnostic_msgs__msg__DiagnosticArray__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {diagnostic_msgs__msg__DiagnosticArray__TYPE_NAME, 35, 35},
      {diagnostic_msgs__msg__DiagnosticArray__FIELDS, 2, 2},
    },
    {diagnostic_msgs__msg__DiagnosticArray__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__DiagnosticStatus__EXPECTED_HASH, diagnostic_msgs__msg__DiagnosticStatus__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = diagnostic_msgs__msg__DiagnosticStatus__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&diagnostic_msgs__msg__KeyValue__EXPECTED_HASH, diagnostic_msgs__msg__KeyValue__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = diagnostic_msgs__msg__KeyValue__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message is used to send diagnostic information about the state of the robot.\n"
  "std_msgs/Header header # for timestamp\n"
  "DiagnosticStatus[] status # an array of components being reported on";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
diagnostic_msgs__msg__DiagnosticArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {diagnostic_msgs__msg__DiagnosticArray__TYPE_NAME, 35, 35},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 192, 192},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
diagnostic_msgs__msg__DiagnosticArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *diagnostic_msgs__msg__DiagnosticArray__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *diagnostic_msgs__msg__DiagnosticStatus__get_individual_type_description_source(NULL);
    sources[3] = *diagnostic_msgs__msg__KeyValue__get_individual_type_description_source(NULL);
    sources[4] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
