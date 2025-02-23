// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:msg/ImageMarker.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/msg/detail/image_marker__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/msg/detail/image_marker__functions.h"
#include "visualization_msgs/msg/detail/image_marker__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `ns`
#include "rosidl_runtime_c/string_functions.h"
// Member `position`
// Member `points`
#include "geometry_msgs/msg/point.h"
// Member `position`
// Member `points`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"
// Member `outline_color`
// Member `fill_color`
// Member `outline_colors`
#include "std_msgs/msg/color_rgba.h"
// Member `outline_color`
// Member `fill_color`
// Member `outline_colors`
#include "std_msgs/msg/detail/color_rgba__rosidl_typesupport_introspection_c.h"
// Member `lifetime`
#include "builtin_interfaces/msg/duration.h"
// Member `lifetime`
#include "builtin_interfaces/msg/detail/duration__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__msg__ImageMarker__init(message_memory);
}

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_fini_function(void * message_memory)
{
  visualization_msgs__msg__ImageMarker__fini(message_memory);
}

size_t visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__size_function__ImageMarker__points(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__points(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__points(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__fetch_function__ImageMarker__points(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point * item =
    ((const geometry_msgs__msg__Point *)
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__points(untyped_member, index));
  geometry_msgs__msg__Point * value =
    (geometry_msgs__msg__Point *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__assign_function__ImageMarker__points(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point * item =
    ((geometry_msgs__msg__Point *)
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__points(untyped_member, index));
  const geometry_msgs__msg__Point * value =
    (const geometry_msgs__msg__Point *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__resize_function__ImageMarker__points(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  geometry_msgs__msg__Point__Sequence__fini(member);
  return geometry_msgs__msg__Point__Sequence__init(member, size);
}

size_t visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__size_function__ImageMarker__outline_colors(
  const void * untyped_member)
{
  const std_msgs__msg__ColorRGBA__Sequence * member =
    (const std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__outline_colors(
  const void * untyped_member, size_t index)
{
  const std_msgs__msg__ColorRGBA__Sequence * member =
    (const std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__outline_colors(
  void * untyped_member, size_t index)
{
  std_msgs__msg__ColorRGBA__Sequence * member =
    (std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__fetch_function__ImageMarker__outline_colors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const std_msgs__msg__ColorRGBA * item =
    ((const std_msgs__msg__ColorRGBA *)
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__outline_colors(untyped_member, index));
  std_msgs__msg__ColorRGBA * value =
    (std_msgs__msg__ColorRGBA *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__assign_function__ImageMarker__outline_colors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  std_msgs__msg__ColorRGBA * item =
    ((std_msgs__msg__ColorRGBA *)
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__outline_colors(untyped_member, index));
  const std_msgs__msg__ColorRGBA * value =
    (const std_msgs__msg__ColorRGBA *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__resize_function__ImageMarker__outline_colors(
  void * untyped_member, size_t size)
{
  std_msgs__msg__ColorRGBA__Sequence * member =
    (std_msgs__msg__ColorRGBA__Sequence *)(untyped_member);
  std_msgs__msg__ColorRGBA__Sequence__fini(member);
  return std_msgs__msg__ColorRGBA__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[13] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, header),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, ns),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, id),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, type),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, action),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, position),  // bytes offset in struct
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
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, scale),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "outline_color",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, outline_color),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "filled",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, filled),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "fill_color",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, fill_color),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, lifetime),  // bytes offset in struct
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
    offsetof(visualization_msgs__msg__ImageMarker, points),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__size_function__ImageMarker__points,  // size() function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__points,  // get_const(index) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__points,  // get(index) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__fetch_function__ImageMarker__points,  // fetch(index, &value) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__assign_function__ImageMarker__points,  // assign(index, value) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__resize_function__ImageMarker__points  // resize(index) function pointer
  },
  {
    "outline_colors",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__ImageMarker, outline_colors),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__size_function__ImageMarker__outline_colors,  // size() function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_const_function__ImageMarker__outline_colors,  // get_const(index) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__get_function__ImageMarker__outline_colors,  // get(index) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__fetch_function__ImageMarker__outline_colors,  // fetch(index, &value) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__assign_function__ImageMarker__outline_colors,  // assign(index, value) function pointer
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__resize_function__ImageMarker__outline_colors  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_members = {
  "visualization_msgs__msg",  // message namespace
  "ImageMarker",  // message name
  13,  // number of fields
  sizeof(visualization_msgs__msg__ImageMarker),
  false,  // has_any_key_member_
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array,  // message members
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_type_support_handle = {
  0,
  &visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__ImageMarker__get_type_hash,
  &visualization_msgs__msg__ImageMarker__get_type_description,
  &visualization_msgs__msg__ImageMarker__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, ImageMarker)() {
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, ColorRGBA)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[9].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, ColorRGBA)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[10].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[11].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_member_array[12].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, ColorRGBA)();
  if (!visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__msg__ImageMarker__rosidl_typesupport_introspection_c__ImageMarker_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
