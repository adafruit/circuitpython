// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/PointCloud.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/point_cloud__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/point_cloud__functions.h"
#include "sensor_msgs/msg/detail/point_cloud__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `points`
#include "geometry_msgs/msg/point32.h"
// Member `points`
#include "geometry_msgs/msg/detail/point32__rosidl_typesupport_introspection_c.h"
// Member `channels`
#include "sensor_msgs/msg/channel_float32.h"
// Member `channels`
#include "sensor_msgs/msg/detail/channel_float32__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__PointCloud__init(message_memory);
}

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_fini_function(void * message_memory)
{
  sensor_msgs__msg__PointCloud__fini(message_memory);
}

size_t sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__size_function__PointCloud__points(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point32__Sequence * member =
    (const geometry_msgs__msg__Point32__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__points(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point32__Sequence * member =
    (const geometry_msgs__msg__Point32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__points(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point32__Sequence * member =
    (geometry_msgs__msg__Point32__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__fetch_function__PointCloud__points(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point32 * item =
    ((const geometry_msgs__msg__Point32 *)
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__points(untyped_member, index));
  geometry_msgs__msg__Point32 * value =
    (geometry_msgs__msg__Point32 *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__assign_function__PointCloud__points(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point32 * item =
    ((geometry_msgs__msg__Point32 *)
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__points(untyped_member, index));
  const geometry_msgs__msg__Point32 * value =
    (const geometry_msgs__msg__Point32 *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__resize_function__PointCloud__points(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point32__Sequence * member =
    (geometry_msgs__msg__Point32__Sequence *)(untyped_member);
  geometry_msgs__msg__Point32__Sequence__fini(member);
  return geometry_msgs__msg__Point32__Sequence__init(member, size);
}

size_t sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__size_function__PointCloud__channels(
  const void * untyped_member)
{
  const sensor_msgs__msg__ChannelFloat32__Sequence * member =
    (const sensor_msgs__msg__ChannelFloat32__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__channels(
  const void * untyped_member, size_t index)
{
  const sensor_msgs__msg__ChannelFloat32__Sequence * member =
    (const sensor_msgs__msg__ChannelFloat32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__channels(
  void * untyped_member, size_t index)
{
  sensor_msgs__msg__ChannelFloat32__Sequence * member =
    (sensor_msgs__msg__ChannelFloat32__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__fetch_function__PointCloud__channels(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const sensor_msgs__msg__ChannelFloat32 * item =
    ((const sensor_msgs__msg__ChannelFloat32 *)
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__channels(untyped_member, index));
  sensor_msgs__msg__ChannelFloat32 * value =
    (sensor_msgs__msg__ChannelFloat32 *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__assign_function__PointCloud__channels(
  void * untyped_member, size_t index, const void * untyped_value)
{
  sensor_msgs__msg__ChannelFloat32 * item =
    ((sensor_msgs__msg__ChannelFloat32 *)
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__channels(untyped_member, index));
  const sensor_msgs__msg__ChannelFloat32 * value =
    (const sensor_msgs__msg__ChannelFloat32 *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__resize_function__PointCloud__channels(
  void * untyped_member, size_t size)
{
  sensor_msgs__msg__ChannelFloat32__Sequence * member =
    (sensor_msgs__msg__ChannelFloat32__Sequence *)(untyped_member);
  sensor_msgs__msg__ChannelFloat32__Sequence__fini(member);
  return sensor_msgs__msg__ChannelFloat32__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__PointCloud, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
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
    offsetof(sensor_msgs__msg__PointCloud, points),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__size_function__PointCloud__points,  // size() function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__points,  // get_const(index) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__points,  // get(index) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__fetch_function__PointCloud__points,  // fetch(index, &value) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__assign_function__PointCloud__points,  // assign(index, value) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__resize_function__PointCloud__points  // resize(index) function pointer
  },
  {
    "channels",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__PointCloud, channels),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__size_function__PointCloud__channels,  // size() function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_const_function__PointCloud__channels,  // get_const(index) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__get_function__PointCloud__channels,  // get(index) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__fetch_function__PointCloud__channels,  // fetch(index, &value) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__assign_function__PointCloud__channels,  // assign(index, value) function pointer
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__resize_function__PointCloud__channels  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_members = {
  "sensor_msgs__msg",  // message namespace
  "PointCloud",  // message name
  3,  // number of fields
  sizeof(sensor_msgs__msg__PointCloud),
  false,  // has_any_key_member_
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_member_array,  // message members
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_type_support_handle = {
  0,
  &sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__PointCloud__get_type_hash,
  &sensor_msgs__msg__PointCloud__get_type_description,
  &sensor_msgs__msg__PointCloud__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, PointCloud)() {
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point32)();
  sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, ChannelFloat32)();
  if (!sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__PointCloud__rosidl_typesupport_introspection_c__PointCloud_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
