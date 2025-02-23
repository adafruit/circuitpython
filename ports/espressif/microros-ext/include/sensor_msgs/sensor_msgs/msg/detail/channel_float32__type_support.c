// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/ChannelFloat32.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/channel_float32__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/channel_float32__functions.h"
#include "sensor_msgs/msg/detail/channel_float32__struct.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `values`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__ChannelFloat32__init(message_memory);
}

void sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_fini_function(void * message_memory)
{
  sensor_msgs__msg__ChannelFloat32__fini(message_memory);
}

size_t sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__size_function__ChannelFloat32__values(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_const_function__ChannelFloat32__values(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_function__ChannelFloat32__values(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__fetch_function__ChannelFloat32__values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_const_function__ChannelFloat32__values(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__assign_function__ChannelFloat32__values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_function__ChannelFloat32__values(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__resize_function__ChannelFloat32__values(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_member_array[2] = {
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__ChannelFloat32, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__ChannelFloat32, values),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__size_function__ChannelFloat32__values,  // size() function pointer
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_const_function__ChannelFloat32__values,  // get_const(index) function pointer
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__get_function__ChannelFloat32__values,  // get(index) function pointer
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__fetch_function__ChannelFloat32__values,  // fetch(index, &value) function pointer
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__assign_function__ChannelFloat32__values,  // assign(index, value) function pointer
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__resize_function__ChannelFloat32__values  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_members = {
  "sensor_msgs__msg",  // message namespace
  "ChannelFloat32",  // message name
  2,  // number of fields
  sizeof(sensor_msgs__msg__ChannelFloat32),
  false,  // has_any_key_member_
  sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_member_array,  // message members
  sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_type_support_handle = {
  0,
  &sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__ChannelFloat32__get_type_hash,
  &sensor_msgs__msg__ChannelFloat32__get_type_description,
  &sensor_msgs__msg__ChannelFloat32__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, ChannelFloat32)() {
  if (!sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__ChannelFloat32__rosidl_typesupport_introspection_c__ChannelFloat32_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
