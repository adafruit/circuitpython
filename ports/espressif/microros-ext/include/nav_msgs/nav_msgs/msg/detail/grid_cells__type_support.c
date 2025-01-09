// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from nav_msgs:msg/GridCells.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "nav_msgs/msg/detail/grid_cells__rosidl_typesupport_introspection_c.h"
#include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "nav_msgs/msg/detail/grid_cells__functions.h"
#include "nav_msgs/msg/detail/grid_cells__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `cells`
#include "geometry_msgs/msg/point.h"
// Member `cells`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__msg__GridCells__init(message_memory);
}

void nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_fini_function(void * message_memory)
{
  nav_msgs__msg__GridCells__fini(message_memory);
}

size_t nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__size_function__GridCells__cells(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return member->size;
}

const void * nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_const_function__GridCells__cells(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void * nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_function__GridCells__cells(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__fetch_function__GridCells__cells(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point * item =
    ((const geometry_msgs__msg__Point *)
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_const_function__GridCells__cells(untyped_member, index));
  geometry_msgs__msg__Point * value =
    (geometry_msgs__msg__Point *)(untyped_value);
  *value = *item;
}

void nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__assign_function__GridCells__cells(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point * item =
    ((geometry_msgs__msg__Point *)
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_function__GridCells__cells(untyped_member, index));
  const geometry_msgs__msg__Point * value =
    (const geometry_msgs__msg__Point *)(untyped_value);
  *item = *value;
}

bool nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__resize_function__GridCells__cells(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  geometry_msgs__msg__Point__Sequence__fini(member);
  return geometry_msgs__msg__Point__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_member_array[4] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__GridCells, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "cell_width",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__GridCells, cell_width),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "cell_height",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__GridCells, cell_height),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "cells",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__GridCells, cells),  // bytes offset in struct
    NULL,  // default value
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__size_function__GridCells__cells,  // size() function pointer
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_const_function__GridCells__cells,  // get_const(index) function pointer
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__get_function__GridCells__cells,  // get(index) function pointer
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__fetch_function__GridCells__cells,  // fetch(index, &value) function pointer
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__assign_function__GridCells__cells,  // assign(index, value) function pointer
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__resize_function__GridCells__cells  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_members = {
  "nav_msgs__msg",  // message namespace
  "GridCells",  // message name
  4,  // number of fields
  sizeof(nav_msgs__msg__GridCells),
  false,  // has_any_key_member_
  nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_member_array,  // message members
  nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_type_support_handle = {
  0,
  &nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_members,
  get_message_typesupport_handle_function,
  &nav_msgs__msg__GridCells__get_type_hash,
  &nav_msgs__msg__GridCells__get_type_description,
  &nav_msgs__msg__GridCells__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, GridCells)() {
  nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  if (!nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_type_support_handle.typesupport_identifier) {
    nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__msg__GridCells__rosidl_typesupport_introspection_c__GridCells_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
