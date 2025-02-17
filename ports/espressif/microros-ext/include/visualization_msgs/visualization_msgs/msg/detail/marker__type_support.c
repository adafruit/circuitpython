// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:msg/Marker.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/msg/detail/marker__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/msg/detail/marker__functions.h"
#include "visualization_msgs/msg/detail/marker__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `ns`
// Member `texture_resource`
// Member `text`
// Member `mesh_resource`
#include "rosidl_runtime_c/string_functions.h"
// Member `pose`
#include "geometry_msgs/msg/pose.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__rosidl_typesupport_introspection_c.h"
// Member `scale`
#include "geometry_msgs/msg/vector3.h"
// Member `scale`
#include "geometry_msgs/msg/detail/vector3__rosidl_typesupport_introspection_c.h"
// Member `color`
// Member `colors`
#include "std_msgs/msg/color_rgba.h"
// Member `color`
// Member `colors`
#include "std_msgs/msg/detail/color_rgba__rosidl_typesupport_introspection_c.h"
// Member `lifetime`
#include "builtin_interfaces/msg/duration.h"
// Member `lifetime`
#include "builtin_interfaces/msg/detail/duration__rosidl_typesupport_introspection_c.h"
// Member `points`
#include "geometry_msgs/msg/point.h"
// Member `points`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"
// Member `texture`
#include "sensor_msgs/msg/compressed_image.h"
// Member `texture`
#include "sensor_msgs/msg/detail/compressed_image__rosidl_typesupport_introspection_c.h"
// Member `uv_coordinates`
#include "visualization_msgs/msg/uv_coordinate.h"
// Member `uv_coordinates`
#include "visualization_msgs/msg/detail/uv_coordinate__rosidl_typesupport_introspection_c.h"
// Member `mesh_file`
#include "visualization_msgs/msg/mesh_file.h"
// Member `mesh_file`
#include "visualization_msgs/msg/detail/mesh_file__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__msg__Marker__init(message_memory);
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_fini_function(void * message_memory)
{
  visualization_msgs__msg__Marker__fini(message_memory);
}

size_t visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__points(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__points(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__points(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__points(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point * item =
    ((const geometry_msgs__msg__Point *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__points(untyped_member, index));
  geometry_msgs__msg__Point * value =
    (geometry_msgs__msg__Point *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__points(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point * item =
    ((geometry_msgs__msg__Point *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__points(untyped_member, index));
  const geometry_msgs__msg__Point * value =
    (const geometry_msgs__msg__Point *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__points(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  geometry_msgs__msg__Point__Sequence__fini(member);
  return geometry_msgs__msg__Point__Sequence__init(member, size);
}

size_t visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__colors(
  const void * untyped_member)
{
  const std_msgs__msg__ColorRGBA__Sequence * member =
    (const std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__colors(
  const void * untyped_member, size_t index)
{
  const std_msgs__msg__ColorRGBA__Sequence * member =
    (const std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__colors(
  void * untyped_member, size_t index)
{
  std_msgs__msg__ColorRGBA__Sequence * member =
    (std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__colors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const std_msgs__msg__ColorRGBA * item =
    ((const std_msgs__msg__ColorRGBA *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__colors(untyped_member, index));
  std_msgs__msg__ColorRGBA * value =
    (std_msgs__msg__ColorRGBA *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__colors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  std_msgs__msg__ColorRGBA * item =
    ((std_msgs__msg__ColorRGBA *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__colors(untyped_member, index));
  const std_msgs__msg__ColorRGBA * value =
    (const std_msgs__msg__ColorRGBA *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__colors(
  void * untyped_member, size_t size)
{
  std_msgs__msg__ColorRGBA__Sequence * member =
    (std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  std_msgs__msg__ColorRGBA__Sequence__fini(member);
  return std_msgs__msg__ColorRGBA__Sequence__init(member, size);
}

size_t visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__uv_coordinates(
  const void * untyped_member)
{
  const visualization_msgs__msg__UVCoordinate__Sequence * member =
    (const visualization_msgs__msg__UVCoordinate__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__uv_coordinates(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__msg__UVCoordinate__Sequence * member =
    (const visualization_msgs__msg__UVCoordinate__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__uv_coordinates(
  void * untyped_member, size_t index)
{
  visualization_msgs__msg__UVCoordinate__Sequence * member =
    (visualization_msgs__msg__UVCoordinate__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__uv_coordinates(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__msg__UVCoordinate * item =
    ((const visualization_msgs__msg__UVCoordinate *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__uv_coordinates(untyped_member, index));
  visualization_msgs__msg__UVCoordinate * value =
    (visualization_msgs__msg__UVCoordinate *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__uv_coordinates(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__msg__UVCoordinate * item =
    ((visualization_msgs__msg__UVCoordinate *)
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__uv_coordinates(untyped_member, index));
  const visualization_msgs__msg__UVCoordinate * value =
    (const visualization_msgs__msg__UVCoordinate *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__uv_coordinates(
  void * untyped_member, size_t size)
{
  visualization_msgs__msg__UVCoordinate__Sequence * member =
    (visualization_msgs__msg__UVCoordinate__Sequence *)(untyped_member);
  visualization_msgs__msg__UVCoordinate__Sequence__fini(member);
  return visualization_msgs__msg__UVCoordinate__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[19] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "ns",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, ns),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "action",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, action),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "pose",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, pose),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "scale",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, scale),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "color",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, color),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "lifetime",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, lifetime),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "frame_locked",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, frame_locked),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__Marker, points),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__points,  // size() function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__points,  // get_const(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__points,  // get(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__points,  // fetch(index, &value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__points,  // assign(index, value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__points  // resize(index) function pointer
  },
  {
    "colors",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, colors),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__colors,  // size() function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__colors,  // get_const(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__colors,  // get(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__colors,  // fetch(index, &value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__colors,  // assign(index, value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__colors  // resize(index) function pointer
  },
  {
    "texture_resource",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, texture_resource),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "texture",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, texture),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uv_coordinates",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, uv_coordinates),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__size_function__Marker__uv_coordinates,  // size() function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_const_function__Marker__uv_coordinates,  // get_const(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__get_function__Marker__uv_coordinates,  // get(index) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__fetch_function__Marker__uv_coordinates,  // fetch(index, &value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__assign_function__Marker__uv_coordinates,  // assign(index, value) function pointer
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__resize_function__Marker__uv_coordinates  // resize(index) function pointer
  },
  {
    "text",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, text),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "mesh_resource",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, mesh_resource),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "mesh_file",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, mesh_file),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "mesh_use_embedded_materials",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__Marker, mesh_use_embedded_materials),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_members = {
  "visualization_msgs__msg",  // message namespace
  "Marker",  // message name
  19,  // number of fields
  sizeof(visualization_msgs__msg__Marker),
  false,  // has_any_key_member_
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array,  // message members
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_type_support_handle = {
  0,
  &visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__Marker__get_type_hash,
  &visualization_msgs__msg__Marker__get_type_description,
  &visualization_msgs__msg__Marker__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, Marker)() {
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Pose)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Vector3)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, ColorRGBA)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[8].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[10].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[11].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, ColorRGBA)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[13].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, CompressedImage)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[14].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, UVCoordinate)();
  visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_member_array[17].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, MeshFile)();
  if (!visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__msg__Marker__rosidl_typesupport_introspection_c__Marker_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
