// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/MultiDOFJointState.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/multi_dof_joint_state__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/multi_dof_joint_state__functions.h"
#include "sensor_msgs/msg/detail/multi_dof_joint_state__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `joint_names`
#include "rosidl_runtime_c/string_functions.h"
// Member `transforms`
#include "geometry_msgs/msg/transform.h"
// Member `transforms`
#include "geometry_msgs/msg/detail/transform__rosidl_typesupport_introspection_c.h"
// Member `twist`
#include "geometry_msgs/msg/twist.h"
// Member `twist`
#include "geometry_msgs/msg/detail/twist__rosidl_typesupport_introspection_c.h"
// Member `wrench`
#include "geometry_msgs/msg/wrench.h"
// Member `wrench`
#include "geometry_msgs/msg/detail/wrench__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__MultiDOFJointState__init(message_memory);
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_fini_function(void * message_memory)
{
  sensor_msgs__msg__MultiDOFJointState__fini(message_memory);
}

size_t sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__joint_names(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__joint_names(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__joint_names(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__joint_names(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__joint_names(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__joint_names(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__joint_names(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__joint_names(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__transforms(
  const void * untyped_member)
{
  const geometry_msgs__msg__Transform__Sequence * member =
    (const geometry_msgs__msg__Transform__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__transforms(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Transform__Sequence * member =
    (const geometry_msgs__msg__Transform__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__transforms(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Transform__Sequence * member =
    (geometry_msgs__msg__Transform__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__transforms(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Transform * item =
    ((const geometry_msgs__msg__Transform *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__transforms(untyped_member, index));
  geometry_msgs__msg__Transform * value =
    (geometry_msgs__msg__Transform *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__transforms(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Transform * item =
    ((geometry_msgs__msg__Transform *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__transforms(untyped_member, index));
  const geometry_msgs__msg__Transform * value =
    (const geometry_msgs__msg__Transform *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__transforms(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Transform__Sequence * member =
    (geometry_msgs__msg__Transform__Sequence *)(untyped_member);
  geometry_msgs__msg__Transform__Sequence__fini(member);
  return geometry_msgs__msg__Transform__Sequence__init(member, size);
}

size_t sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__twist(
  const void * untyped_member)
{
  const geometry_msgs__msg__Twist__Sequence * member =
    (const geometry_msgs__msg__Twist__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__twist(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Twist__Sequence * member =
    (const geometry_msgs__msg__Twist__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__twist(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Twist__Sequence * member =
    (geometry_msgs__msg__Twist__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__twist(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Twist * item =
    ((const geometry_msgs__msg__Twist *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__twist(untyped_member, index));
  geometry_msgs__msg__Twist * value =
    (geometry_msgs__msg__Twist *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__twist(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Twist * item =
    ((geometry_msgs__msg__Twist *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__twist(untyped_member, index));
  const geometry_msgs__msg__Twist * value =
    (const geometry_msgs__msg__Twist *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__twist(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Twist__Sequence * member =
    (geometry_msgs__msg__Twist__Sequence *)(untyped_member);
  geometry_msgs__msg__Twist__Sequence__fini(member);
  return geometry_msgs__msg__Twist__Sequence__init(member, size);
}

size_t sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__wrench(
  const void * untyped_member)
{
  const geometry_msgs__msg__Wrench__Sequence * member =
    (const geometry_msgs__msg__Wrench__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__wrench(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Wrench__Sequence * member =
    (const geometry_msgs__msg__Wrench__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__wrench(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Wrench__Sequence * member =
    (geometry_msgs__msg__Wrench__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__wrench(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Wrench * item =
    ((const geometry_msgs__msg__Wrench *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__wrench(untyped_member, index));
  geometry_msgs__msg__Wrench * value =
    (geometry_msgs__msg__Wrench *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__wrench(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Wrench * item =
    ((geometry_msgs__msg__Wrench *)
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__wrench(untyped_member, index));
  const geometry_msgs__msg__Wrench * value =
    (const geometry_msgs__msg__Wrench *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__wrench(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Wrench__Sequence * member =
    (geometry_msgs__msg__Wrench__Sequence *)(untyped_member);
  geometry_msgs__msg__Wrench__Sequence__fini(member);
  return geometry_msgs__msg__Wrench__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array[5] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiDOFJointState, header),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiDOFJointState, joint_names),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__joint_names,  // size() function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__joint_names,  // get_const(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__joint_names,  // get(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__joint_names,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__joint_names,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__joint_names  // resize(index) function pointer
  },
  {
    "transforms",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiDOFJointState, transforms),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__transforms,  // size() function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__transforms,  // get_const(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__transforms,  // get(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__transforms,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__transforms,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__transforms  // resize(index) function pointer
  },
  {
    "twist",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiDOFJointState, twist),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__twist,  // size() function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__twist,  // get_const(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__twist,  // get(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__twist,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__twist,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__twist  // resize(index) function pointer
  },
  {
    "wrench",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiDOFJointState, wrench),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__size_function__MultiDOFJointState__wrench,  // size() function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_const_function__MultiDOFJointState__wrench,  // get_const(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__get_function__MultiDOFJointState__wrench,  // get(index) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__fetch_function__MultiDOFJointState__wrench,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__assign_function__MultiDOFJointState__wrench,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__resize_function__MultiDOFJointState__wrench  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_members = {
  "sensor_msgs__msg",  // message namespace
  "MultiDOFJointState",  // message name
  5,  // number of fields
  sizeof(sensor_msgs__msg__MultiDOFJointState),
  false,  // has_any_key_member_
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array,  // message members
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_type_support_handle = {
  0,
  &sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__MultiDOFJointState__get_type_hash,
  &sensor_msgs__msg__MultiDOFJointState__get_type_description,
  &sensor_msgs__msg__MultiDOFJointState__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, MultiDOFJointState)() {
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Transform)();
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Twist)();
  sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Wrench)();
  if (!sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__MultiDOFJointState__rosidl_typesupport_introspection_c__MultiDOFJointState_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
