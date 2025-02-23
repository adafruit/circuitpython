// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/NavSatFix.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/nav_sat_fix__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/nav_sat_fix__functions.h"
#include "sensor_msgs/msg/detail/nav_sat_fix__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `status`
#include "sensor_msgs/msg/nav_sat_status.h"
// Member `status`
#include "sensor_msgs/msg/detail/nav_sat_status__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__NavSatFix__init(message_memory);
}

void sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_fini_function(void * message_memory)
{
  sensor_msgs__msg__NavSatFix__fini(message_memory);
}

size_t sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__size_function__NavSatFix__position_covariance(
  const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_const_function__NavSatFix__position_covariance(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_function__NavSatFix__position_covariance(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__fetch_function__NavSatFix__position_covariance(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_const_function__NavSatFix__position_covariance(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__assign_function__NavSatFix__position_covariance(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_function__NavSatFix__position_covariance(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_member_array[7] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "status",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, status),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "latitude",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, latitude),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "longitude",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, longitude),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "altitude",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, altitude),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "position_covariance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, position_covariance),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__size_function__NavSatFix__position_covariance,  // size() function pointer
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_const_function__NavSatFix__position_covariance,  // get_const(index) function pointer
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__get_function__NavSatFix__position_covariance,  // get(index) function pointer
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__fetch_function__NavSatFix__position_covariance,  // fetch(index, &value) function pointer
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__assign_function__NavSatFix__position_covariance,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "position_covariance_type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__NavSatFix, position_covariance_type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_members = {
  "sensor_msgs__msg",  // message namespace
  "NavSatFix",  // message name
  7,  // number of fields
  sizeof(sensor_msgs__msg__NavSatFix),
  false,  // has_any_key_member_
  sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_member_array,  // message members
  sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_type_support_handle = {
  0,
  &sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__NavSatFix__get_type_hash,
  &sensor_msgs__msg__NavSatFix__get_type_description,
  &sensor_msgs__msg__NavSatFix__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, NavSatFix)() {
  sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, NavSatStatus)();
  if (!sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__NavSatFix__rosidl_typesupport_introspection_c__NavSatFix_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
