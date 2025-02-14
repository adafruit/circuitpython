// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/CameraInfo.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/camera_info__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/camera_info__functions.h"
#include "sensor_msgs/msg/detail/camera_info__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `distortion_model`
#include "rosidl_runtime_c/string_functions.h"
// Member `d`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `roi`
#include "sensor_msgs/msg/region_of_interest.h"
// Member `roi`
#include "sensor_msgs/msg/detail/region_of_interest__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__CameraInfo__init(message_memory);
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_fini_function(void * message_memory)
{
  sensor_msgs__msg__CameraInfo__fini(message_memory);
}

size_t sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__d(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__d(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__d(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__d(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__d(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__d(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__d(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__resize_function__CameraInfo__d(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

size_t sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__k(
  const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__k(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__k(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__k(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__k(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__k(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__k(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

size_t sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__r(
  const void * untyped_member)
{
  (void)untyped_member;
  return 9;
}

const void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__r(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__r(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__r(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__r(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__r(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__r(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

size_t sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__p(
  const void * untyped_member)
{
  (void)untyped_member;
  return 12;
}

const void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__p(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__p(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__p(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__p(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__p(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__p(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_member_array[11] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, header),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__CameraInfo, height),  // bytes offset in struct
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
    offsetof(sensor_msgs__msg__CameraInfo, width),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "distortion_model",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, distortion_model),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "d",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, d),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__d,  // size() function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__d,  // get_const(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__d,  // get(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__d,  // fetch(index, &value) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__d,  // assign(index, value) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__resize_function__CameraInfo__d  // resize(index) function pointer
  },
  {
    "k",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, k),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__k,  // size() function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__k,  // get_const(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__k,  // get(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__k,  // fetch(index, &value) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__k,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "r",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    9,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, r),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__r,  // size() function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__r,  // get_const(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__r,  // get(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__r,  // fetch(index, &value) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__r,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "p",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    12,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, p),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__size_function__CameraInfo__p,  // size() function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_const_function__CameraInfo__p,  // get_const(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__get_function__CameraInfo__p,  // get(index) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__fetch_function__CameraInfo__p,  // fetch(index, &value) function pointer
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__assign_function__CameraInfo__p,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "binning_x",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, binning_x),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "binning_y",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, binning_y),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "roi",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__CameraInfo, roi),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_members = {
  "sensor_msgs__msg",  // message namespace
  "CameraInfo",  // message name
  11,  // number of fields
  sizeof(sensor_msgs__msg__CameraInfo),
  false,  // has_any_key_member_
  sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_member_array,  // message members
  sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_type_support_handle = {
  0,
  &sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__CameraInfo__get_type_hash,
  &sensor_msgs__msg__CameraInfo__get_type_description,
  &sensor_msgs__msg__CameraInfo__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, CameraInfo)() {
  sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_member_array[10].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, RegionOfInterest)();
  if (!sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__CameraInfo__rosidl_typesupport_introspection_c__CameraInfo_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
