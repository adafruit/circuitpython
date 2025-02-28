// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/MultiNested.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/multi_nested__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__MultiNested__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x9e, 0xe9, 0xcb, 0xfb, 0x17, 0xc2, 0xdf, 0x99,
      0x1a, 0x75, 0x0b, 0x76, 0x1a, 0x56, 0xde, 0xf7,
      0x86, 0x74, 0x03, 0x49, 0x77, 0xc8, 0x29, 0x44,
      0x98, 0x20, 0xca, 0x07, 0xef, 0xd9, 0x75, 0x8b,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "test_msgs/msg/detail/bounded_sequences__functions.h"
#include "test_msgs/msg/detail/arrays__functions.h"
#include "test_msgs/msg/detail/basic_types__functions.h"
#include "test_msgs/msg/detail/unbounded_sequences__functions.h"
#include "test_msgs/msg/detail/constants__functions.h"
#include "test_msgs/msg/detail/defaults__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t test_msgs__msg__Arrays__EXPECTED_HASH = {1, {
    0x8a, 0x32, 0x1b, 0x80, 0xda, 0xe8, 0xc4, 0x4d,
    0xfd, 0x1c, 0xe5, 0x58, 0xea, 0xf1, 0xb1, 0xcd,
    0x5e, 0xbb, 0xe5, 0x95, 0x08, 0x63, 0xed, 0x67,
    0x58, 0x36, 0x53, 0x55, 0x11, 0xa7, 0xf9, 0x1a,
  }};
static const rosidl_type_hash_t test_msgs__msg__BasicTypes__EXPECTED_HASH = {1, {
    0x7c, 0x30, 0x0a, 0xfd, 0x4e, 0x79, 0x67, 0x98,
    0xd4, 0x9b, 0xdd, 0x6c, 0xda, 0xa0, 0xfa, 0x87,
    0xfa, 0x0e, 0xd2, 0xba, 0x32, 0x17, 0xd9, 0x77,
    0xe1, 0xfa, 0xa8, 0x70, 0x70, 0xd7, 0x97, 0xab,
  }};
static const rosidl_type_hash_t test_msgs__msg__BoundedSequences__EXPECTED_HASH = {1, {
    0xfb, 0x36, 0x1a, 0x3b, 0x38, 0x27, 0xc7, 0x6a,
    0x14, 0x2c, 0x21, 0x90, 0x12, 0x6a, 0x20, 0xa9,
    0x2e, 0x7e, 0x51, 0x0e, 0x04, 0x7f, 0x88, 0x01,
    0x41, 0xa7, 0xe9, 0xef, 0x86, 0x79, 0x61, 0xed,
  }};
static const rosidl_type_hash_t test_msgs__msg__Constants__EXPECTED_HASH = {1, {
    0x98, 0x3e, 0x7a, 0xe7, 0x09, 0x9d, 0x65, 0xb3,
    0x03, 0x0c, 0x31, 0xc2, 0x89, 0x0b, 0x0e, 0xc2,
    0xb3, 0x32, 0x48, 0x4f, 0xae, 0x9f, 0x28, 0xc6,
    0x8e, 0x01, 0x34, 0x0c, 0x8f, 0x61, 0xed, 0xcf,
  }};
static const rosidl_type_hash_t test_msgs__msg__Defaults__EXPECTED_HASH = {1, {
    0xa7, 0x25, 0xee, 0x02, 0xbf, 0x25, 0xa2, 0x24,
    0xcb, 0x7d, 0x14, 0x01, 0x5a, 0x02, 0x45, 0xb6,
    0x65, 0x4f, 0xf5, 0x91, 0xae, 0x1a, 0xc2, 0x77,
    0xe2, 0xa2, 0x67, 0x3f, 0x01, 0x3e, 0xa6, 0xd2,
  }};
static const rosidl_type_hash_t test_msgs__msg__UnboundedSequences__EXPECTED_HASH = {1, {
    0xa6, 0x39, 0x75, 0x51, 0x42, 0x93, 0xa2, 0xc6,
    0xb2, 0x4a, 0x80, 0x76, 0x75, 0xbc, 0xbd, 0x91,
    0x80, 0xbd, 0xa1, 0x99, 0x5d, 0xd4, 0xee, 0x10,
    0xf2, 0x69, 0x8d, 0x45, 0xff, 0x77, 0xdb, 0x05,
  }};
#endif

static char test_msgs__msg__MultiNested__TYPE_NAME[] = "test_msgs/msg/MultiNested";
static char test_msgs__msg__Arrays__TYPE_NAME[] = "test_msgs/msg/Arrays";
static char test_msgs__msg__BasicTypes__TYPE_NAME[] = "test_msgs/msg/BasicTypes";
static char test_msgs__msg__BoundedSequences__TYPE_NAME[] = "test_msgs/msg/BoundedSequences";
static char test_msgs__msg__Constants__TYPE_NAME[] = "test_msgs/msg/Constants";
static char test_msgs__msg__Defaults__TYPE_NAME[] = "test_msgs/msg/Defaults";
static char test_msgs__msg__UnboundedSequences__TYPE_NAME[] = "test_msgs/msg/UnboundedSequences";

// Define type names, field names, and default values
static char test_msgs__msg__MultiNested__FIELD_NAME__array_of_arrays[] = "array_of_arrays";
static char test_msgs__msg__MultiNested__FIELD_NAME__array_of_bounded_sequences[] = "array_of_bounded_sequences";
static char test_msgs__msg__MultiNested__FIELD_NAME__array_of_unbounded_sequences[] = "array_of_unbounded_sequences";
static char test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_arrays[] = "bounded_sequence_of_arrays";
static char test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_bounded_sequences[] = "bounded_sequence_of_bounded_sequences";
static char test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_unbounded_sequences[] = "bounded_sequence_of_unbounded_sequences";
static char test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_arrays[] = "unbounded_sequence_of_arrays";
static char test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_bounded_sequences[] = "unbounded_sequence_of_bounded_sequences";
static char test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_unbounded_sequences[] = "unbounded_sequence_of_unbounded_sequences";

static rosidl_runtime_c__type_description__Field test_msgs__msg__MultiNested__FIELDS[] = {
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__array_of_arrays, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__array_of_bounded_sequences, 26, 26},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__BoundedSequences__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__array_of_unbounded_sequences, 28, 28},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_ARRAY,
      3,
      0,
      {test_msgs__msg__UnboundedSequences__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_arrays, 26, 26},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      3,
      0,
      {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_bounded_sequences, 37, 37},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      3,
      0,
      {test_msgs__msg__BoundedSequences__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__bounded_sequence_of_unbounded_sequences, 39, 39},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      3,
      0,
      {test_msgs__msg__UnboundedSequences__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_arrays, 28, 28},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_bounded_sequences, 39, 39},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {test_msgs__msg__BoundedSequences__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__MultiNested__FIELD_NAME__unbounded_sequence_of_unbounded_sequences, 41, 41},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {test_msgs__msg__UnboundedSequences__TYPE_NAME, 32, 32},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__msg__MultiNested__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__Arrays__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__BasicTypes__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__BoundedSequences__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Constants__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__Defaults__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__UnboundedSequences__TYPE_NAME, 32, 32},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__MultiNested__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__MultiNested__TYPE_NAME, 25, 25},
      {test_msgs__msg__MultiNested__FIELDS, 9, 9},
    },
    {test_msgs__msg__MultiNested__REFERENCED_TYPE_DESCRIPTIONS, 6, 6},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__Arrays__EXPECTED_HASH, test_msgs__msg__Arrays__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__Arrays__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__BasicTypes__EXPECTED_HASH, test_msgs__msg__BasicTypes__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = test_msgs__msg__BasicTypes__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__BoundedSequences__EXPECTED_HASH, test_msgs__msg__BoundedSequences__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = test_msgs__msg__BoundedSequences__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Constants__EXPECTED_HASH, test_msgs__msg__Constants__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = test_msgs__msg__Constants__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__Defaults__EXPECTED_HASH, test_msgs__msg__Defaults__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = test_msgs__msg__Defaults__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__UnboundedSequences__EXPECTED_HASH, test_msgs__msg__UnboundedSequences__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[5].fields = test_msgs__msg__UnboundedSequences__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Mulitple levels of nested messages\n"
  "Arrays[3] array_of_arrays\n"
  "BoundedSequences[3] array_of_bounded_sequences\n"
  "UnboundedSequences[3] array_of_unbounded_sequences\n"
  "Arrays[<=3] bounded_sequence_of_arrays\n"
  "BoundedSequences[<=3] bounded_sequence_of_bounded_sequences\n"
  "UnboundedSequences[<=3] bounded_sequence_of_unbounded_sequences\n"
  "Arrays[] unbounded_sequence_of_arrays\n"
  "BoundedSequences[] unbounded_sequence_of_bounded_sequences\n"
  "UnboundedSequences[] unbounded_sequence_of_unbounded_sequences";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__MultiNested__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__MultiNested__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 484, 484},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__MultiNested__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[7];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 7, 7};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__MultiNested__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__Arrays__get_individual_type_description_source(NULL);
    sources[2] = *test_msgs__msg__BasicTypes__get_individual_type_description_source(NULL);
    sources[3] = *test_msgs__msg__BoundedSequences__get_individual_type_description_source(NULL);
    sources[4] = *test_msgs__msg__Constants__get_individual_type_description_source(NULL);
    sources[5] = *test_msgs__msg__Defaults__get_individual_type_description_source(NULL);
    sources[6] = *test_msgs__msg__UnboundedSequences__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
