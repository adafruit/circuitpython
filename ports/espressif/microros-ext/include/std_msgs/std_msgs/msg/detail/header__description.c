// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from std_msgs:msg/Header.idl
// generated code does not contain a copyright notice

#include "std_msgs/msg/detail/header__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_std_msgs
const rosidl_type_hash_t *
std_msgs__msg__Header__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
      0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
      0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
      0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
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

static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char std_msgs__msg__Header__FIELD_NAME__stamp[] = "stamp";
static char std_msgs__msg__Header__FIELD_NAME__frame_id[] = "frame_id";

static rosidl_runtime_c__type_description__Field std_msgs__msg__Header__FIELDS[] = {
  {
    {std_msgs__msg__Header__FIELD_NAME__stamp, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__FIELD_NAME__frame_id, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription std_msgs__msg__Header__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
std_msgs__msg__Header__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
      {std_msgs__msg__Header__FIELDS, 2, 2},
    },
    {std_msgs__msg__Header__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Standard metadata for higher-level stamped data types.\n"
  "# This is generally used to communicate timestamped data\n"
  "# in a particular coordinate frame.\n"
  "\n"
  "# Two-integer timestamp that is expressed as seconds and nanoseconds.\n"
  "builtin_interfaces/Time stamp\n"
  "\n"
  "# Transform frame with which this data is associated.\n"
  "string frame_id";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
std_msgs__msg__Header__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 322, 322},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
std_msgs__msg__Header__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *std_msgs__msg__Header__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
