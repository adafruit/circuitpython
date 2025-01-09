// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/LaserScan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/laser_scan__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/laser_scan__functions.h"
#include "sensor_msgs/msg/detail/laser_scan__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `ranges`
// Member `intensities`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__LaserScan__init(message_memory);
}

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_fini_function(void * message_memory)
{
  sensor_msgs__msg__LaserScan__fini(message_memory);
}

size_t sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__size_function__LaserScan__ranges(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__ranges(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__ranges(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__fetch_function__LaserScan__ranges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__ranges(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__assign_function__LaserScan__ranges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__ranges(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__resize_function__LaserScan__ranges(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

size_t sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__size_function__LaserScan__intensities(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__intensities(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__intensities(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__fetch_function__LaserScan__intensities(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__intensities(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__assign_function__LaserScan__intensities(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__intensities(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__resize_function__LaserScan__intensities(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_member_array[10] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "angle_min",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, angle_min),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "angle_max",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, angle_max),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "angle_increment",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, angle_increment),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "time_increment",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, time_increment),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "scan_time",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, scan_time),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "range_min",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, range_min),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "range_max",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, range_max),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "ranges",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, ranges),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__size_function__LaserScan__ranges,  // size() function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__ranges,  // get_const(index) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__ranges,  // get(index) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__fetch_function__LaserScan__ranges,  // fetch(index, &value) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__assign_function__LaserScan__ranges,  // assign(index, value) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__resize_function__LaserScan__ranges  // resize(index) function pointer
  },
  {
    "intensities",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__LaserScan, intensities),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__size_function__LaserScan__intensities,  // size() function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_const_function__LaserScan__intensities,  // get_const(index) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__get_function__LaserScan__intensities,  // get(index) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__fetch_function__LaserScan__intensities,  // fetch(index, &value) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__assign_function__LaserScan__intensities,  // assign(index, value) function pointer
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__resize_function__LaserScan__intensities  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_members = {
  "sensor_msgs__msg",  // message namespace
  "LaserScan",  // message name
  10,  // number of fields
  sizeof(sensor_msgs__msg__LaserScan),
  false,  // has_any_key_member_
  sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_member_array,  // message members
  sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_type_support_handle = {
  0,
  &sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__LaserScan__get_type_hash,
  &sensor_msgs__msg__LaserScan__get_type_description,
  &sensor_msgs__msg__LaserScan__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, LaserScan)() {
  sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__LaserScan__rosidl_typesupport_introspection_c__LaserScan_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
