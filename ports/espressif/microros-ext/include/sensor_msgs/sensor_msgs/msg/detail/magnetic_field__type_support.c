// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/MagneticField.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/magnetic_field__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/magnetic_field__functions.h"
#include "sensor_msgs/msg/detail/magnetic_field__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `magnetic_field`
#include "geometry_msgs/msg/vector3.h"
// Member `magnetic_field`
#include "geometry_msgs/msg/detail/vector3__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__MagneticField__init(message_memory);
}

void sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_fini_function(void * message_memory)
{
  sensor_msgs__msg__MagneticField__fini(message_memory);
}

size_t sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__size_function__MagneticField__magnetic_field_covariance(
  const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_const_function__MagneticField__magnetic_field_covariance(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_function__MagneticField__magnetic_field_covariance(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__fetch_function__MagneticField__magnetic_field_covariance(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_const_function__MagneticField__magnetic_field_covariance(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__assign_function__MagneticField__magnetic_field_covariance(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_function__MagneticField__magnetic_field_covariance(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MagneticField, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "magnetic_field",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MagneticField, magnetic_field),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "magnetic_field_covariance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__MagneticField, magnetic_field_covariance),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__size_function__MagneticField__magnetic_field_covariance,  // size() function pointer
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_const_function__MagneticField__magnetic_field_covariance,  // get_const(index) function pointer
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__get_function__MagneticField__magnetic_field_covariance,  // get(index) function pointer
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__fetch_function__MagneticField__magnetic_field_covariance,  // fetch(index, &value) function pointer
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__assign_function__MagneticField__magnetic_field_covariance,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_members = {
  "sensor_msgs__msg",  // message namespace
  "MagneticField",  // message name
  3,  // number of fields
  sizeof(sensor_msgs__msg__MagneticField),
  false,  // has_any_key_member_
  sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_member_array,  // message members
  sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_type_support_handle = {
  0,
  &sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__MagneticField__get_type_hash,
  &sensor_msgs__msg__MagneticField__get_type_description,
  &sensor_msgs__msg__MagneticField__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, MagneticField)() {
  sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Vector3)();
  if (!sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__MagneticField__rosidl_typesupport_introspection_c__MagneticField_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
