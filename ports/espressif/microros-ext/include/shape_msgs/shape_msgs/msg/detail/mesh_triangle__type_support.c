// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from shape_msgs:msg/MeshTriangle.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "shape_msgs/msg/detail/mesh_triangle__rosidl_typesupport_introspection_c.h"
#include "shape_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "shape_msgs/msg/detail/mesh_triangle__functions.h"
#include "shape_msgs/msg/detail/mesh_triangle__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  shape_msgs__msg__MeshTriangle__init(message_memory);
}

void shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_fini_function(void * message_memory)
{
  shape_msgs__msg__MeshTriangle__fini(message_memory);
}

size_t shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__size_function__MeshTriangle__vertex_indices(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_const_function__MeshTriangle__vertex_indices(
  const void * untyped_member, size_t index)
{
  const uint32_t * member =
    (const uint32_t *)(untyped_member);
  return &member[index];
}

void * shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_function__MeshTriangle__vertex_indices(
  void * untyped_member, size_t index)
{
  uint32_t * member =
    (uint32_t *)(untyped_member);
  return &member[index];
}

void shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__fetch_function__MeshTriangle__vertex_indices(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint32_t * item =
    ((const uint32_t *)
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_const_function__MeshTriangle__vertex_indices(untyped_member, index));
  uint32_t * value =
    (uint32_t *)(untyped_value);
  *value = *item;
}

void shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__assign_function__MeshTriangle__vertex_indices(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint32_t * item =
    ((uint32_t *)
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_function__MeshTriangle__vertex_indices(untyped_member, index));
  const uint32_t * value =
    (const uint32_t *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_member_array[1] = {
  {
    "vertex_indices",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(shape_msgs__msg__MeshTriangle, vertex_indices),  // bytes offset in struct
    NULL,  // default value
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__size_function__MeshTriangle__vertex_indices,  // size() function pointer
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_const_function__MeshTriangle__vertex_indices,  // get_const(index) function pointer
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__get_function__MeshTriangle__vertex_indices,  // get(index) function pointer
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__fetch_function__MeshTriangle__vertex_indices,  // fetch(index, &value) function pointer
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__assign_function__MeshTriangle__vertex_indices,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_members = {
  "shape_msgs__msg",  // message namespace
  "MeshTriangle",  // message name
  1,  // number of fields
  sizeof(shape_msgs__msg__MeshTriangle),
  false,  // has_any_key_member_
  shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_member_array,  // message members
  shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_init_function,  // function to initialize message memory (memory has to be allocated)
  shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_type_support_handle = {
  0,
  &shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_members,
  get_message_typesupport_handle_function,
  &shape_msgs__msg__MeshTriangle__get_type_hash,
  &shape_msgs__msg__MeshTriangle__get_type_description,
  &shape_msgs__msg__MeshTriangle__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_shape_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, shape_msgs, msg, MeshTriangle)() {
  if (!shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_type_support_handle.typesupport_identifier) {
    shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &shape_msgs__msg__MeshTriangle__rosidl_typesupport_introspection_c__MeshTriangle_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
