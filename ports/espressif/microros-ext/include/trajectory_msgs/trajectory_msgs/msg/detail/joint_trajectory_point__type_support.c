// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from trajectory_msgs:msg/JointTrajectoryPoint.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "trajectory_msgs/msg/detail/joint_trajectory_point__rosidl_typesupport_introspection_c.h"
#include "trajectory_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "trajectory_msgs/msg/detail/joint_trajectory_point__functions.h"
#include "trajectory_msgs/msg/detail/joint_trajectory_point__struct.h"


// Include directives for member types
// Member `positions`
// Member `velocities`
// Member `accelerations`
// Member `effort`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `time_from_start`
#include "builtin_interfaces/msg/duration.h"
// Member `time_from_start`
#include "builtin_interfaces/msg/detail/duration__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  trajectory_msgs__msg__JointTrajectoryPoint__init(message_memory);
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_fini_function(void * message_memory)
{
  trajectory_msgs__msg__JointTrajectoryPoint__fini(message_memory);
}

size_t trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__positions(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__positions(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__positions(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__positions(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__positions(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__positions(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__positions(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__positions(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__velocities(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__velocities(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__velocities(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__velocities(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__velocities(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__velocities(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__velocities(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__velocities(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__accelerations(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__accelerations(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__accelerations(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__accelerations(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__accelerations(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__accelerations(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__accelerations(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__accelerations(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__effort(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__effort(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__effort(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__effort(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__effort(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__effort(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__effort(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__effort(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_member_array[5] = {
  {
    "positions",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectoryPoint, positions),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__positions,  // size() function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__positions,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__positions,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__positions,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__positions,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__positions  // resize(index) function pointer
  },
  {
    "velocities",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectoryPoint, velocities),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__velocities,  // size() function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__velocities,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__velocities,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__velocities,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__velocities,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__velocities  // resize(index) function pointer
  },
  {
    "accelerations",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectoryPoint, accelerations),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__accelerations,  // size() function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__accelerations,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__accelerations,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__accelerations,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__accelerations,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__accelerations  // resize(index) function pointer
  },
  {
    "effort",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectoryPoint, effort),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__size_function__JointTrajectoryPoint__effort,  // size() function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_const_function__JointTrajectoryPoint__effort,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__get_function__JointTrajectoryPoint__effort,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__fetch_function__JointTrajectoryPoint__effort,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__assign_function__JointTrajectoryPoint__effort,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__resize_function__JointTrajectoryPoint__effort  // resize(index) function pointer
  },
  {
    "time_from_start",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectoryPoint, time_from_start),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_members = {
  "trajectory_msgs__msg",  // message namespace
  "JointTrajectoryPoint",  // message name
  5,  // number of fields
  sizeof(trajectory_msgs__msg__JointTrajectoryPoint),
  false,  // has_any_key_member_
  trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_member_array,  // message members
  trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_init_function,  // function to initialize message memory (memory has to be allocated)
  trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_type_support_handle = {
  0,
  &trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_members,
  get_message_typesupport_handle_function,
  &trajectory_msgs__msg__JointTrajectoryPoint__get_type_hash,
  &trajectory_msgs__msg__JointTrajectoryPoint__get_type_description,
  &trajectory_msgs__msg__JointTrajectoryPoint__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_trajectory_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, trajectory_msgs, msg, JointTrajectoryPoint)() {
  trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  if (!trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_type_support_handle.typesupport_identifier) {
    trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &trajectory_msgs__msg__JointTrajectoryPoint__rosidl_typesupport_introspection_c__JointTrajectoryPoint_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
