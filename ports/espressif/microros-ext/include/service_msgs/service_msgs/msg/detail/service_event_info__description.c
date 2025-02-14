// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from service_msgs:msg/ServiceEventInfo.idl
// generated code does not contain a copyright notice

#include "service_msgs/msg/detail/service_event_info__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_service_msgs
const rosidl_type_hash_t *
service_msgs__msg__ServiceEventInfo__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
      0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
      0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
      0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
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

static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char service_msgs__msg__ServiceEventInfo__FIELD_NAME__event_type[] = "event_type";
static char service_msgs__msg__ServiceEventInfo__FIELD_NAME__stamp[] = "stamp";
static char service_msgs__msg__ServiceEventInfo__FIELD_NAME__client_gid[] = "client_gid";
static char service_msgs__msg__ServiceEventInfo__FIELD_NAME__sequence_number[] = "sequence_number";

static rosidl_runtime_c__type_description__Field service_msgs__msg__ServiceEventInfo__FIELDS[] = {
  {
    {service_msgs__msg__ServiceEventInfo__FIELD_NAME__event_type, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__FIELD_NAME__stamp, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__FIELD_NAME__client_gid, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_ARRAY,
      16,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__FIELD_NAME__sequence_number, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription service_msgs__msg__ServiceEventInfo__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
service_msgs__msg__ServiceEventInfo__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
      {service_msgs__msg__ServiceEventInfo__FIELDS, 4, 4},
    },
    {service_msgs__msg__ServiceEventInfo__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "uint8 REQUEST_SENT = 0\n"
  "uint8 REQUEST_RECEIVED = 1\n"
  "uint8 RESPONSE_SENT = 2\n"
  "uint8 RESPONSE_RECEIVED = 3\n"
  "\n"
  "# The type of event this message represents\n"
  "uint8 event_type\n"
  "\n"
  "# Timestamp for when the event occurred (sent or received time)\n"
  "builtin_interfaces/Time stamp\n"
  "\n"
  "# Unique identifier for the client that sent the service request\n"
  "# Note, this is only unique for the current session.\n"
  "# The size here has to match the size of rmw_dds_common/msg/Gid,\n"
  "# but unfortunately we cannot use that message directly due to a\n"
  "# circular dependency.\n"
  "char[16] client_gid\n"
  "\n"
  "# Sequence number for the request\n"
  "# Combined with the client ID, this creates a unique ID for the service transaction\n"
  "int64 sequence_number";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 692, 692},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
service_msgs__msg__ServiceEventInfo__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
