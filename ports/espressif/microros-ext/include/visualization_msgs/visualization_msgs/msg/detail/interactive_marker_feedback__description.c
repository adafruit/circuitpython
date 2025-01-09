// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:msg/InteractiveMarkerFeedback.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/msg/detail/interactive_marker_feedback__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__InteractiveMarkerFeedback__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x6c, 0xc4, 0x87, 0x41, 0xdf, 0x9f, 0x05, 0xd1,
      0x9b, 0xa7, 0xd9, 0xea, 0x31, 0x01, 0xe9, 0xfc,
      0xd1, 0x30, 0x9c, 0x9d, 0x6b, 0xda, 0x3c, 0x55,
      0x66, 0x8b, 0xa6, 0x07, 0x49, 0x2f, 0x72, 0x5e,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "std_msgs/msg/detail/header__functions.h"
#include "geometry_msgs/msg/detail/pose__functions.h"
#include "geometry_msgs/msg/detail/point__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "geometry_msgs/msg/detail/quaternion__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Point__EXPECTED_HASH = {1, {
    0x69, 0x63, 0x08, 0x48, 0x42, 0xa9, 0xb0, 0x44,
    0x94, 0xd6, 0xb2, 0x94, 0x1d, 0x11, 0x44, 0x47,
    0x08, 0xd8, 0x92, 0xda, 0x2f, 0x4b, 0x09, 0x84,
    0x3b, 0x9c, 0x43, 0xf4, 0x2a, 0x7f, 0x68, 0x81,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Pose__EXPECTED_HASH = {1, {
    0xd5, 0x01, 0x95, 0x4e, 0x94, 0x76, 0xce, 0xa2,
    0x99, 0x69, 0x84, 0xe8, 0x12, 0x05, 0x4b, 0x68,
    0x02, 0x6a, 0xe0, 0xbf, 0xae, 0x78, 0x9d, 0x9a,
    0x10, 0xb2, 0x3d, 0xaf, 0x35, 0xcc, 0x90, 0xfa,
  }};
static const rosidl_type_hash_t geometry_msgs__msg__Quaternion__EXPECTED_HASH = {1, {
    0x8a, 0x76, 0x5f, 0x66, 0x77, 0x8c, 0x8f, 0xf7,
    0xc8, 0xab, 0x94, 0xaf, 0xcc, 0x59, 0x0a, 0x2e,
    0xd5, 0x32, 0x5a, 0x1d, 0x9a, 0x07, 0x6f, 0xff,
    0xf3, 0x8f, 0xbc, 0xe3, 0x6f, 0x45, 0x86, 0x84,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char visualization_msgs__msg__InteractiveMarkerFeedback__TYPE_NAME[] = "visualization_msgs/msg/InteractiveMarkerFeedback";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char geometry_msgs__msg__Point__TYPE_NAME[] = "geometry_msgs/msg/Point";
static char geometry_msgs__msg__Pose__TYPE_NAME[] = "geometry_msgs/msg/Pose";
static char geometry_msgs__msg__Quaternion__TYPE_NAME[] = "geometry_msgs/msg/Quaternion";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__header[] = "header";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__client_id[] = "client_id";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__marker_name[] = "marker_name";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__control_name[] = "control_name";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__event_type[] = "event_type";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__pose[] = "pose";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__menu_entry_id[] = "menu_entry_id";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__mouse_point[] = "mouse_point";
static char visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__mouse_point_valid[] = "mouse_point_valid";

static rosidl_runtime_c__type_description__Field visualization_msgs__msg__InteractiveMarkerFeedback__FIELDS[] = {
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__client_id, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__marker_name, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__control_name, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__event_type, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__pose, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Pose__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__menu_entry_id, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__mouse_point, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__InteractiveMarkerFeedback__FIELD_NAME__mouse_point_valid, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription visualization_msgs__msg__InteractiveMarkerFeedback__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Point__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Pose__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {geometry_msgs__msg__Quaternion__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__InteractiveMarkerFeedback__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__msg__InteractiveMarkerFeedback__TYPE_NAME, 48, 48},
      {visualization_msgs__msg__InteractiveMarkerFeedback__FIELDS, 9, 9},
    },
    {visualization_msgs__msg__InteractiveMarkerFeedback__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Point__EXPECTED_HASH, geometry_msgs__msg__Point__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = geometry_msgs__msg__Point__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Pose__EXPECTED_HASH, geometry_msgs__msg__Pose__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = geometry_msgs__msg__Pose__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&geometry_msgs__msg__Quaternion__EXPECTED_HASH, geometry_msgs__msg__Quaternion__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = geometry_msgs__msg__Quaternion__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Time/frame info.\n"
  "std_msgs/Header header\n"
  "\n"
  "# Identifying string. Must be unique in the topic namespace.\n"
  "string client_id\n"
  "\n"
  "# Feedback message sent back from the GUI, e.g.\n"
  "# when the status of an interactive marker was modified by the user.\n"
  "\n"
  "# Specifies which interactive marker and control this message refers to\n"
  "string marker_name\n"
  "string control_name\n"
  "\n"
  "# Type of the event\n"
  "# KEEP_ALIVE: sent while dragging to keep up control of the marker\n"
  "# MENU_SELECT: a menu entry has been selected\n"
  "# BUTTON_CLICK: a button control has been clicked\n"
  "# POSE_UPDATE: the pose has been changed using one of the controls\n"
  "uint8 KEEP_ALIVE = 0\n"
  "uint8 POSE_UPDATE = 1\n"
  "uint8 MENU_SELECT = 2\n"
  "uint8 BUTTON_CLICK = 3\n"
  "\n"
  "uint8 MOUSE_DOWN = 4\n"
  "uint8 MOUSE_UP = 5\n"
  "\n"
  "uint8 event_type\n"
  "\n"
  "# Current pose of the marker\n"
  "# Note: Has to be valid for all feedback types.\n"
  "geometry_msgs/Pose pose\n"
  "\n"
  "# Contains the ID of the selected menu entry\n"
  "# Only valid for MENU_SELECT events.\n"
  "uint32 menu_entry_id\n"
  "\n"
  "# If event_type is BUTTON_CLICK, MOUSE_DOWN, or MOUSE_UP, mouse_point\n"
  "# may contain the 3 dimensional position of the event on the\n"
  "# control.  If it does, mouse_point_valid will be true.  mouse_point\n"
  "# will be relative to the frame listed in the header.\n"
  "geometry_msgs/Point mouse_point\n"
  "bool mouse_point_valid";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__InteractiveMarkerFeedback__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__msg__InteractiveMarkerFeedback__TYPE_NAME, 48, 48},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1265, 1265},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__InteractiveMarkerFeedback__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__msg__InteractiveMarkerFeedback__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *geometry_msgs__msg__Point__get_individual_type_description_source(NULL);
    sources[3] = *geometry_msgs__msg__Pose__get_individual_type_description_source(NULL);
    sources[4] = *geometry_msgs__msg__Quaternion__get_individual_type_description_source(NULL);
    sources[5] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
