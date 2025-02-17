// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from nav_msgs:msg/OccupancyGrid.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "nav_msgs/msg/detail/occupancy_grid__rosidl_typesupport_introspection_c.h"
#include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "nav_msgs/msg/detail/occupancy_grid__functions.h"
#include "nav_msgs/msg/detail/occupancy_grid__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `info`
#include "nav_msgs/msg/map_meta_data.h"
// Member `info`
#include "nav_msgs/msg/detail/map_meta_data__rosidl_typesupport_introspection_c.h"
// Member `data`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__msg__OccupancyGrid__init(message_memory);
}

void nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_fini_function(void * message_memory)
{
  nav_msgs__msg__OccupancyGrid__fini(message_memory);
}

size_t nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__size_function__OccupancyGrid__data(
  const void * untyped_member)
{
  const rosidl_runtime_c__int8__Sequence * member =
    (const rosidl_runtime_c__int8__Sequence *)(untyped_member);
  return member->size;
}

const void * nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_const_function__OccupancyGrid__data(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__int8__Sequence * member =
    (const rosidl_runtime_c__int8__Sequence *)(untyped_member);
  return &member->data[index];
}

void * nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_function__OccupancyGrid__data(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__int8__Sequence * member =
    (rosidl_runtime_c__int8__Sequence *)(untyped_member);
  return &member->data[index];
}

void nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__fetch_function__OccupancyGrid__data(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int8_t * item =
    ((const int8_t *)
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_const_function__OccupancyGrid__data(untyped_member, index));
  int8_t * value =
    (int8_t *)(untyped_value);
  *value = *item;
}

void nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__assign_function__OccupancyGrid__data(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int8_t * item =
    ((int8_t *)
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_function__OccupancyGrid__data(untyped_member, index));
  const int8_t * value =
    (const int8_t *)(untyped_value);
  *item = *value;
}

bool nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__resize_function__OccupancyGrid__data(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__int8__Sequence * member =
    (rosidl_runtime_c__int8__Sequence *)(untyped_member);
  rosidl_runtime_c__int8__Sequence__fini(member);
  return rosidl_runtime_c__int8__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__OccupancyGrid, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__OccupancyGrid, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "data",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__msg__OccupancyGrid, data),  // bytes offset in struct
    NULL,  // default value
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__size_function__OccupancyGrid__data,  // size() function pointer
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_const_function__OccupancyGrid__data,  // get_const(index) function pointer
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__get_function__OccupancyGrid__data,  // get(index) function pointer
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__fetch_function__OccupancyGrid__data,  // fetch(index, &value) function pointer
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__assign_function__OccupancyGrid__data,  // assign(index, value) function pointer
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__resize_function__OccupancyGrid__data  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_members = {
  "nav_msgs__msg",  // message namespace
  "OccupancyGrid",  // message name
  3,  // number of fields
  sizeof(nav_msgs__msg__OccupancyGrid),
  false,  // has_any_key_member_
  nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_member_array,  // message members
  nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_type_support_handle = {
  0,
  &nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_members,
  get_message_typesupport_handle_function,
  &nav_msgs__msg__OccupancyGrid__get_type_hash,
  &nav_msgs__msg__OccupancyGrid__get_type_description,
  &nav_msgs__msg__OccupancyGrid__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, OccupancyGrid)() {
  nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, MapMetaData)();
  if (!nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_type_support_handle.typesupport_identifier) {
    nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__msg__OccupancyGrid__rosidl_typesupport_introspection_c__OccupancyGrid_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
