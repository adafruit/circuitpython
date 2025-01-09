// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from geometry_msgs:msg/TwistWithCovariance.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "geometry_msgs/msg/detail/twist_with_covariance__rosidl_typesupport_introspection_c.h"
#include "geometry_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "geometry_msgs/msg/detail/twist_with_covariance__functions.h"
#include "geometry_msgs/msg/detail/twist_with_covariance__struct.h"


// Include directives for member types
// Member `twist`
#include "geometry_msgs/msg/twist.h"
// Member `twist`
#include "geometry_msgs/msg/detail/twist__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  geometry_msgs__msg__TwistWithCovariance__init(message_memory);
}

void geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_fini_function(void * message_memory)
{
  geometry_msgs__msg__TwistWithCovariance__fini(message_memory);
}

size_t geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__size_function__TwistWithCovariance__covariance(
  const void * untyped_member)
{
  (void)untyped_member;
  return 36;
}

const void * geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_const_function__TwistWithCovariance__covariance(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_function__TwistWithCovariance__covariance(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__fetch_function__TwistWithCovariance__covariance(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_const_function__TwistWithCovariance__covariance(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__assign_function__TwistWithCovariance__covariance(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_function__TwistWithCovariance__covariance(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_member_array[2] = {
  {
    "twist",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(geometry_msgs__msg__TwistWithCovariance, twist),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "covariance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    36,  // array size
    false,  // is upper bound
    offsetof(geometry_msgs__msg__TwistWithCovariance, covariance),  // bytes offset in struct
    NULL,  // default value
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__size_function__TwistWithCovariance__covariance,  // size() function pointer
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_const_function__TwistWithCovariance__covariance,  // get_const(index) function pointer
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__get_function__TwistWithCovariance__covariance,  // get(index) function pointer
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__fetch_function__TwistWithCovariance__covariance,  // fetch(index, &value) function pointer
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__assign_function__TwistWithCovariance__covariance,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_members = {
  "geometry_msgs__msg",  // message namespace
  "TwistWithCovariance",  // message name
  2,  // number of fields
  sizeof(geometry_msgs__msg__TwistWithCovariance),
  false,  // has_any_key_member_
  geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_member_array,  // message members
  geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_init_function,  // function to initialize message memory (memory has to be allocated)
  geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_type_support_handle = {
  0,
  &geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_members,
  get_message_typesupport_handle_function,
  &geometry_msgs__msg__TwistWithCovariance__get_type_hash,
  &geometry_msgs__msg__TwistWithCovariance__get_type_description,
  &geometry_msgs__msg__TwistWithCovariance__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_geometry_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, TwistWithCovariance)() {
  geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Twist)();
  if (!geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_type_support_handle.typesupport_identifier) {
    geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &geometry_msgs__msg__TwistWithCovariance__rosidl_typesupport_introspection_c__TwistWithCovariance_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
