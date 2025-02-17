// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/RegionOfInterest.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/region_of_interest__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"
#include "sensor_msgs/msg/detail/region_of_interest__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__RegionOfInterest__init(message_memory);
}

void sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_fini_function(void * message_memory)
{
  sensor_msgs__msg__RegionOfInterest__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_member_array[5] = {
  {
    "x_offset",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__RegionOfInterest, x_offset),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "y_offset",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__RegionOfInterest, y_offset),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "height",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__RegionOfInterest, height),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "width",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__RegionOfInterest, width),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "do_rectify",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__RegionOfInterest, do_rectify),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_members = {
  "sensor_msgs__msg",  // message namespace
  "RegionOfInterest",  // message name
  5,  // number of fields
  sizeof(sensor_msgs__msg__RegionOfInterest),
  false,  // has_any_key_member_
  sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_member_array,  // message members
  sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_type_support_handle = {
  0,
  &sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__RegionOfInterest__get_type_hash,
  &sensor_msgs__msg__RegionOfInterest__get_type_description,
  &sensor_msgs__msg__RegionOfInterest__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, RegionOfInterest)() {
  if (!sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__RegionOfInterest__rosidl_typesupport_introspection_c__RegionOfInterest_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
