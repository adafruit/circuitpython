// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from trajectory_msgs:msg/JointTrajectory.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "trajectory_msgs/msg/detail/joint_trajectory__rosidl_typesupport_introspection_c.h"
#include "trajectory_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "trajectory_msgs/msg/detail/joint_trajectory__functions.h"
#include "trajectory_msgs/msg/detail/joint_trajectory__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `joint_names`
#include "rosidl_runtime_c/string_functions.h"
// Member `points`
#include "trajectory_msgs/msg/joint_trajectory_point.h"
// Member `points`
#include "trajectory_msgs/msg/detail/joint_trajectory_point__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  trajectory_msgs__msg__JointTrajectory__init(message_memory);
}

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_fini_function(void * message_memory)
{
  trajectory_msgs__msg__JointTrajectory__fini(message_memory);
}

size_t trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__size_function__JointTrajectory__joint_names(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__joint_names(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__joint_names(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__fetch_function__JointTrajectory__joint_names(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__joint_names(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__assign_function__JointTrajectory__joint_names(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__joint_names(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__resize_function__JointTrajectory__joint_names(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__size_function__JointTrajectory__points(
  const void * untyped_member)
{
  const trajectory_msgs__msg__JointTrajectoryPoint__Sequence * member =
    (const trajectory_msgs__msg__JointTrajectoryPoint__Sequence *)(untyped_member);
  return member->size;
}

const void * trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__points(
  const void * untyped_member, size_t index)
{
  const trajectory_msgs__msg__JointTrajectoryPoint__Sequence * member =
    (const trajectory_msgs__msg__JointTrajectoryPoint__Sequence *)(untyped_member);
  return &member->data[index];
}

void * trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__points(
  void * untyped_member, size_t index)
{
  trajectory_msgs__msg__JointTrajectoryPoint__Sequence * member =
    (trajectory_msgs__msg__JointTrajectoryPoint__Sequence *)(untyped_member);
  return &member->data[index];
}

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__fetch_function__JointTrajectory__points(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const trajectory_msgs__msg__JointTrajectoryPoint * item =
    ((const trajectory_msgs__msg__JointTrajectoryPoint *)
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__points(untyped_member, index));
  trajectory_msgs__msg__JointTrajectoryPoint * value =
    (trajectory_msgs__msg__JointTrajectoryPoint *)(untyped_value);
  *value = *item;
}

void trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__assign_function__JointTrajectory__points(
  void * untyped_member, size_t index, const void * untyped_value)
{
  trajectory_msgs__msg__JointTrajectoryPoint * item =
    ((trajectory_msgs__msg__JointTrajectoryPoint *)
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__points(untyped_member, index));
  const trajectory_msgs__msg__JointTrajectoryPoint * value =
    (const trajectory_msgs__msg__JointTrajectoryPoint *)(untyped_value);
  *item = *value;
}

bool trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__resize_function__JointTrajectory__points(
  void * untyped_member, size_t size)
{
  trajectory_msgs__msg__JointTrajectoryPoint__Sequence * member =
    (trajectory_msgs__msg__JointTrajectoryPoint__Sequence *)(untyped_member);
  trajectory_msgs__msg__JointTrajectoryPoint__Sequence__fini(member);
  return trajectory_msgs__msg__JointTrajectoryPoint__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectory, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "joint_names",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectory, joint_names),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__size_function__JointTrajectory__joint_names,  // size() function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__joint_names,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__joint_names,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__fetch_function__JointTrajectory__joint_names,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__assign_function__JointTrajectory__joint_names,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__resize_function__JointTrajectory__joint_names  // resize(index) function pointer
  },
  {
    "points",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(trajectory_msgs__msg__JointTrajectory, points),  // bytes offset in struct
    NULL,  // default value
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__size_function__JointTrajectory__points,  // size() function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_const_function__JointTrajectory__points,  // get_const(index) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__get_function__JointTrajectory__points,  // get(index) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__fetch_function__JointTrajectory__points,  // fetch(index, &value) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__assign_function__JointTrajectory__points,  // assign(index, value) function pointer
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__resize_function__JointTrajectory__points  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_members = {
  "trajectory_msgs__msg",  // message namespace
  "JointTrajectory",  // message name
  3,  // number of fields
  sizeof(trajectory_msgs__msg__JointTrajectory),
  false,  // has_any_key_member_
  trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_member_array,  // message members
  trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_init_function,  // function to initialize message memory (memory has to be allocated)
  trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_type_support_handle = {
  0,
  &trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_members,
  get_message_typesupport_handle_function,
  &trajectory_msgs__msg__JointTrajectory__get_type_hash,
  &trajectory_msgs__msg__JointTrajectory__get_type_description,
  &trajectory_msgs__msg__JointTrajectory__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_trajectory_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, trajectory_msgs, msg, JointTrajectory)() {
  trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, trajectory_msgs, msg, JointTrajectoryPoint)();
  if (!trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_type_support_handle.typesupport_identifier) {
    trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &trajectory_msgs__msg__JointTrajectory__rosidl_typesupport_introspection_c__JointTrajectory_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
