// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from trajectory_msgs:msg/JointTrajectoryPoint.idl
// generated code does not contain a copyright notice

#include "trajectory_msgs/msg/detail/joint_trajectory_point__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_type_hash_t *
trajectory_msgs__msg__JointTrajectoryPoint__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xde, 0x89, 0x07, 0x03, 0x6d, 0x8b, 0xd4, 0x5a,
      0xac, 0x6f, 0x30, 0xcc, 0x90, 0x44, 0xa3, 0xd4,
      0xa3, 0x29, 0xc4, 0x2c, 0xbf, 0x71, 0x9a, 0xff,
      0x7d, 0x95, 0xa5, 0x84, 0xcf, 0xa5, 0x32, 0xd7,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/duration__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Duration__EXPECTED_HASH = {1, {
    0xe8, 0xd0, 0x09, 0xf6, 0x59, 0x81, 0x6f, 0x75,
    0x8b, 0x75, 0x33, 0x4e, 0xe1, 0xa9, 0xca, 0x5b,
    0x5c, 0x0b, 0x85, 0x98, 0x43, 0x26, 0x1f, 0x14,
    0xc7, 0xf9, 0x37, 0x34, 0x95, 0x99, 0xd9, 0x3b,
  }};
#endif

static char trajectory_msgs__msg__JointTrajectoryPoint__TYPE_NAME[] = "trajectory_msgs/msg/JointTrajectoryPoint";
static char builtin_interfaces__msg__Duration__TYPE_NAME[] = "builtin_interfaces/msg/Duration";

// Define type names, field names, and default values
static char trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__positions[] = "positions";
static char trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__velocities[] = "velocities";
static char trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__accelerations[] = "accelerations";
static char trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__effort[] = "effort";
static char trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__time_from_start[] = "time_from_start";

static rosidl_runtime_c__type_description__Field trajectory_msgs__msg__JointTrajectoryPoint__FIELDS[] = {
  {
    {trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__positions, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__velocities, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__accelerations, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__effort, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {trajectory_msgs__msg__JointTrajectoryPoint__FIELD_NAME__time_from_start, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription trajectory_msgs__msg__JointTrajectoryPoint__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Duration__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
trajectory_msgs__msg__JointTrajectoryPoint__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {trajectory_msgs__msg__JointTrajectoryPoint__TYPE_NAME, 40, 40},
      {trajectory_msgs__msg__JointTrajectoryPoint__FIELDS, 5, 5},
    },
    {trajectory_msgs__msg__JointTrajectoryPoint__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Duration__EXPECTED_HASH, builtin_interfaces__msg__Duration__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Duration__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Each trajectory point specifies either positions[, velocities[, accelerations]]\n"
  "# or positions[, effort] for the trajectory to be executed.\n"
  "# All specified values are in the same order as the joint names in JointTrajectory.msg.\n"
  "\n"
  "# Single DOF joint positions for each joint relative to their \"0\" position.\n"
  "# The units depend on the specific joint type: radians for revolute or\n"
  "# continuous joints, and meters for prismatic joints.\n"
  "float64[] positions\n"
  "\n"
  "# The rate of change in position of each joint. Units are joint type dependent.\n"
  "# Radians/second for revolute or continuous joints, and meters/second for\n"
  "# prismatic joints.\n"
  "float64[] velocities\n"
  "\n"
  "# Rate of change in velocity of each joint. Units are joint type dependent.\n"
  "# Radians/second^2 for revolute or continuous joints, and meters/second^2 for\n"
  "# prismatic joints.\n"
  "float64[] accelerations\n"
  "\n"
  "# The torque or the force to be applied at each joint. For revolute/continuous\n"
  "# joints effort denotes a torque in newton-meters. For prismatic joints, effort\n"
  "# denotes a force in newtons.\n"
  "float64[] effort\n"
  "\n"
  "# Desired time from the trajectory start to arrive at this trajectory point.\n"
  "builtin_interfaces/Duration time_from_start";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
trajectory_msgs__msg__JointTrajectoryPoint__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {trajectory_msgs__msg__JointTrajectoryPoint__TYPE_NAME, 40, 40},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1176, 1176},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
trajectory_msgs__msg__JointTrajectoryPoint__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *trajectory_msgs__msg__JointTrajectoryPoint__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Duration__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
