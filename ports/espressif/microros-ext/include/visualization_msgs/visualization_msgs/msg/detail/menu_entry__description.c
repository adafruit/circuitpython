// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from visualization_msgs:msg/MenuEntry.idl
// generated code does not contain a copyright notice

#include "visualization_msgs/msg/detail/menu_entry__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__MenuEntry__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x22, 0x17, 0x0c, 0x38, 0x7c, 0x70, 0xfd, 0x42,
      0x36, 0x23, 0x2e, 0xc9, 0x02, 0xde, 0x86, 0x04,
      0xe7, 0x2f, 0xf0, 0x27, 0x34, 0x2c, 0x7c, 0x0f,
      0x28, 0xad, 0x9f, 0x68, 0xc6, 0x4c, 0x51, 0xd6,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char visualization_msgs__msg__MenuEntry__TYPE_NAME[] = "visualization_msgs/msg/MenuEntry";

// Define type names, field names, and default values
static char visualization_msgs__msg__MenuEntry__FIELD_NAME__id[] = "id";
static char visualization_msgs__msg__MenuEntry__FIELD_NAME__parent_id[] = "parent_id";
static char visualization_msgs__msg__MenuEntry__FIELD_NAME__title[] = "title";
static char visualization_msgs__msg__MenuEntry__FIELD_NAME__command[] = "command";
static char visualization_msgs__msg__MenuEntry__FIELD_NAME__command_type[] = "command_type";

static rosidl_runtime_c__type_description__Field visualization_msgs__msg__MenuEntry__FIELDS[] = {
  {
    {visualization_msgs__msg__MenuEntry__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__FIELD_NAME__parent_id, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__FIELD_NAME__title, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__FIELD_NAME__command, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {visualization_msgs__msg__MenuEntry__FIELD_NAME__command_type, 12, 12},
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
visualization_msgs__msg__MenuEntry__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {visualization_msgs__msg__MenuEntry__TYPE_NAME, 32, 32},
      {visualization_msgs__msg__MenuEntry__FIELDS, 5, 5},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# MenuEntry message.\n"
  "#\n"
  "# Each InteractiveMarker message has an array of MenuEntry messages.\n"
  "# A collection of MenuEntries together describe a\n"
  "# menu/submenu/subsubmenu/etc tree, though they are stored in a flat\n"
  "# array.  The tree structure is represented by giving each menu entry\n"
  "# an ID number and a \"parent_id\" field.  Top-level entries are the\n"
  "# ones with parent_id = 0.  Menu entries are ordered within their\n"
  "# level the same way they are ordered in the containing array.  Parent\n"
  "# entries must appear before their children.\n"
  "#\n"
  "# Example:\n"
  "# - id = 3\n"
  "#   parent_id = 0\n"
  "#   title = \"fun\"\n"
  "# - id = 2\n"
  "#   parent_id = 0\n"
  "#   title = \"robot\"\n"
  "# - id = 4\n"
  "#   parent_id = 2\n"
  "#   title = \"pr2\"\n"
  "# - id = 5\n"
  "#   parent_id = 2\n"
  "#   title = \"turtle\"\n"
  "#\n"
  "# Gives a menu tree like this:\n"
  "#  - fun\n"
  "#  - robot\n"
  "#    - pr2\n"
  "#    - turtle\n"
  "\n"
  "# ID is a number for each menu entry.  Must be unique within the\n"
  "# control, and should never be 0.\n"
  "uint32 id\n"
  "\n"
  "# ID of the parent of this menu entry, if it is a submenu.  If this\n"
  "# menu entry is a top-level entry, set parent_id to 0.\n"
  "uint32 parent_id\n"
  "\n"
  "# menu / entry title\n"
  "string title\n"
  "\n"
  "# Arguments to command indicated by command_type (below)\n"
  "string command\n"
  "\n"
  "# Command_type stores the type of response desired when this menu\n"
  "# entry is clicked.\n"
  "# FEEDBACK: send an InteractiveMarkerFeedback message with menu_entry_id set to this entry's id.\n"
  "# ROSRUN: execute \"rosrun\" with arguments given in the command field (above).\n"
  "# ROSLAUNCH: execute \"roslaunch\" with arguments given in the command field (above).\n"
  "uint8 FEEDBACK=0\n"
  "uint8 ROSRUN=1\n"
  "uint8 ROSLAUNCH=2\n"
  "uint8 command_type";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__MenuEntry__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {visualization_msgs__msg__MenuEntry__TYPE_NAME, 32, 32},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1588, 1588},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__MenuEntry__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *visualization_msgs__msg__MenuEntry__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
