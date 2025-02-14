// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/MultiEchoLaserScan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/multi_echo_laser_scan__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/multi_echo_laser_scan__functions.h"
#include "sensor_msgs/msg/detail/multi_echo_laser_scan__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `ranges`
// Member `intensities`
#include "sensor_msgs/msg/laser_echo.h"
// Member `ranges`
// Member `intensities`
#include "sensor_msgs/msg/detail/laser_echo__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__MultiEchoLaserScan__init(message_memory);
}

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_fini_function(void * message_memory)
{
  sensor_msgs__msg__MultiEchoLaserScan__fini(message_memory);
}

size_t sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__size_function__MultiEchoLaserScan__ranges(
  const void * untyped_member)
{
  const sensor_msgs__msg__LaserEcho__Sequence * member =
    (const sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__ranges(
  const void * untyped_member, size_t index)
{
  const sensor_msgs__msg__LaserEcho__Sequence * member =
    (const sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__ranges(
  void * untyped_member, size_t index)
{
  sensor_msgs__msg__LaserEcho__Sequence * member =
    (sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__fetch_function__MultiEchoLaserScan__ranges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const sensor_msgs__msg__LaserEcho * item =
    ((const sensor_msgs__msg__LaserEcho *)
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__ranges(untyped_member, index));
  sensor_msgs__msg__LaserEcho * value =
    (sensor_msgs__msg__LaserEcho *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__assign_function__MultiEchoLaserScan__ranges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  sensor_msgs__msg__LaserEcho * item =
    ((sensor_msgs__msg__LaserEcho *)
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__ranges(untyped_member, index));
  const sensor_msgs__msg__LaserEcho * value =
    (const sensor_msgs__msg__LaserEcho *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__resize_function__MultiEchoLaserScan__ranges(
  void * untyped_member, size_t size)
{
  sensor_msgs__msg__LaserEcho__Sequence * member =
    (sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  sensor_msgs__msg__LaserEcho__Sequence__fini(member);
  return sensor_msgs__msg__LaserEcho__Sequence__init(member, size);
}

size_t sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__size_function__MultiEchoLaserScan__intensities(
  const void * untyped_member)
{
  const sensor_msgs__msg__LaserEcho__Sequence * member =
    (const sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__intensities(
  const void * untyped_member, size_t index)
{
  const sensor_msgs__msg__LaserEcho__Sequence * member =
    (const sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__intensities(
  void * untyped_member, size_t index)
{
  sensor_msgs__msg__LaserEcho__Sequence * member =
    (sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__fetch_function__MultiEchoLaserScan__intensities(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const sensor_msgs__msg__LaserEcho * item =
    ((const sensor_msgs__msg__LaserEcho *)
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__intensities(untyped_member, index));
  sensor_msgs__msg__LaserEcho * value =
    (sensor_msgs__msg__LaserEcho *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__assign_function__MultiEchoLaserScan__intensities(
  void * untyped_member, size_t index, const void * untyped_value)
{
  sensor_msgs__msg__LaserEcho * item =
    ((sensor_msgs__msg__LaserEcho *)
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__intensities(untyped_member, index));
  const sensor_msgs__msg__LaserEcho * value =
    (const sensor_msgs__msg__LaserEcho *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__resize_function__MultiEchoLaserScan__intensities(
  void * untyped_member, size_t size)
{
  sensor_msgs__msg__LaserEcho__Sequence * member =
    (sensor_msgs__msg__LaserEcho__Sequence *)(untyped_member);
  sensor_msgs__msg__LaserEcho__Sequence__fini(member);
  return sensor_msgs__msg__LaserEcho__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_member_array[10] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, header),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, angle_min),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, angle_max),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, angle_increment),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, time_increment),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, scan_time),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, range_min),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, range_max),  // bytes offset in struct
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
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, ranges),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__size_function__MultiEchoLaserScan__ranges,  // size() function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__ranges,  // get_const(index) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__ranges,  // get(index) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__fetch_function__MultiEchoLaserScan__ranges,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__assign_function__MultiEchoLaserScan__ranges,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__resize_function__MultiEchoLaserScan__ranges  // resize(index) function pointer
  },
  {
    "intensities",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MultiEchoLaserScan, intensities),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__size_function__MultiEchoLaserScan__intensities,  // size() function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_const_function__MultiEchoLaserScan__intensities,  // get_const(index) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__get_function__MultiEchoLaserScan__intensities,  // get(index) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__fetch_function__MultiEchoLaserScan__intensities,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__assign_function__MultiEchoLaserScan__intensities,  // assign(index, value) function pointer
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__resize_function__MultiEchoLaserScan__intensities  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_members = {
  "sensor_msgs__msg",  // message namespace
  "MultiEchoLaserScan",  // message name
  10,  // number of fields
  sizeof(sensor_msgs__msg__MultiEchoLaserScan),
  false,  // has_any_key_member_
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_member_array,  // message members
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_type_support_handle = {
  0,
  &sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__MultiEchoLaserScan__get_type_hash,
  &sensor_msgs__msg__MultiEchoLaserScan__get_type_description,
  &sensor_msgs__msg__MultiEchoLaserScan__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, MultiEchoLaserScan)() {
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_member_array[8].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, LaserEcho)();
  sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_member_array[9].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, LaserEcho)();
  if (!sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__MultiEchoLaserScan__rosidl_typesupport_introspection_c__MultiEchoLaserScan_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
