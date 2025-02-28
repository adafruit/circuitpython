// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rcl_interfaces:msg/ParameterEventDescriptors.idl
// generated code does not contain a copyright notice

#include "rcl_interfaces/msg/detail/parameter_event_descriptors__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rcl_interfaces
const rosidl_type_hash_t *
rcl_interfaces__msg__ParameterEventDescriptors__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x45, 0x6f, 0x90, 0x91, 0x5d, 0x72, 0xef, 0x69,
      0x37, 0x9e, 0x70, 0x2e, 0xf3, 0xba, 0x2a, 0x11,
      0x50, 0x98, 0xff, 0x67, 0x7e, 0xab, 0x9c, 0x00,
      0x80, 0xfa, 0x11, 0x23, 0x9c, 0x6b, 0x14, 0x7e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "rcl_interfaces/msg/detail/parameter_descriptor__functions.h"
#include "rcl_interfaces/msg/detail/integer_range__functions.h"
#include "rcl_interfaces/msg/detail/floating_point_range__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH = {1, {
    0xe6, 0xaf, 0x23, 0xa2, 0x3c, 0x17, 0x7f, 0xee,
    0x5f, 0x30, 0x75, 0xc8, 0xb1, 0xe4, 0x35, 0x16,
    0x2a, 0x9b, 0x63, 0xc8, 0x63, 0xd7, 0x8c, 0x06,
    0x01, 0x74, 0x60, 0xb4, 0x96, 0x84, 0x26, 0x2d,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__IntegerRange__EXPECTED_HASH = {1, {
    0xf7, 0xb7, 0xfd, 0xc0, 0xf6, 0x5f, 0x07, 0x70,
    0x2e, 0x09, 0x92, 0x18, 0xe1, 0x32, 0x88, 0xc3,
    0x96, 0x3b, 0xcb, 0x93, 0x45, 0xbd, 0xe7, 0x8b,
    0x56, 0x0e, 0x6c, 0xd1, 0x98, 0x00, 0xfc, 0x5a,
  }};
static const rosidl_type_hash_t rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH = {1, {
    0x52, 0x17, 0x5d, 0xbf, 0xda, 0x6c, 0x51, 0x15,
    0x31, 0x01, 0xd3, 0x3d, 0x2a, 0x9d, 0xa0, 0x57,
    0x43, 0xf6, 0x6f, 0x02, 0xd5, 0xab, 0x2c, 0xa9,
    0xec, 0x47, 0x09, 0xb4, 0x6b, 0x73, 0xd7, 0x04,
  }};
#endif

static char rcl_interfaces__msg__ParameterEventDescriptors__TYPE_NAME[] = "rcl_interfaces/msg/ParameterEventDescriptors";
static char rcl_interfaces__msg__FloatingPointRange__TYPE_NAME[] = "rcl_interfaces/msg/FloatingPointRange";
static char rcl_interfaces__msg__IntegerRange__TYPE_NAME[] = "rcl_interfaces/msg/IntegerRange";
static char rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME[] = "rcl_interfaces/msg/ParameterDescriptor";

// Define type names, field names, and default values
static char rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__new_parameters[] = "new_parameters";
static char rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__changed_parameters[] = "changed_parameters";
static char rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__deleted_parameters[] = "deleted_parameters";

static rosidl_runtime_c__type_description__Field rcl_interfaces__msg__ParameterEventDescriptors__FIELDS[] = {
  {
    {rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__new_parameters, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__changed_parameters, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterEventDescriptors__FIELD_NAME__deleted_parameters, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rcl_interfaces__msg__ParameterEventDescriptors__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {rcl_interfaces__msg__FloatingPointRange__TYPE_NAME, 37, 37},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__IntegerRange__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
  {
    {rcl_interfaces__msg__ParameterDescriptor__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rcl_interfaces__msg__ParameterEventDescriptors__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rcl_interfaces__msg__ParameterEventDescriptors__TYPE_NAME, 44, 44},
      {rcl_interfaces__msg__ParameterEventDescriptors__FIELDS, 3, 3},
    },
    {rcl_interfaces__msg__ParameterEventDescriptors__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&rcl_interfaces__msg__FloatingPointRange__EXPECTED_HASH, rcl_interfaces__msg__FloatingPointRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = rcl_interfaces__msg__FloatingPointRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__IntegerRange__EXPECTED_HASH, rcl_interfaces__msg__IntegerRange__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = rcl_interfaces__msg__IntegerRange__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&rcl_interfaces__msg__ParameterDescriptor__EXPECTED_HASH, rcl_interfaces__msg__ParameterDescriptor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = rcl_interfaces__msg__ParameterDescriptor__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# This message contains descriptors of a parameter event.\n"
  "# It was an atomic update.\n"
  "# A specific parameter name can only be in one of the three sets.\n"
  "\n"
  "ParameterDescriptor[] new_parameters\n"
  "ParameterDescriptor[] changed_parameters\n"
  "ParameterDescriptor[] deleted_parameters";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rcl_interfaces__msg__ParameterEventDescriptors__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rcl_interfaces__msg__ParameterEventDescriptors__TYPE_NAME, 44, 44},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 271, 271},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rcl_interfaces__msg__ParameterEventDescriptors__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rcl_interfaces__msg__ParameterEventDescriptors__get_individual_type_description_source(NULL),
    sources[1] = *rcl_interfaces__msg__FloatingPointRange__get_individual_type_description_source(NULL);
    sources[2] = *rcl_interfaces__msg__IntegerRange__get_individual_type_description_source(NULL);
    sources[3] = *rcl_interfaces__msg__ParameterDescriptor__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
