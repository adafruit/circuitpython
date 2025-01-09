// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from shape_msgs:msg/Mesh.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "shape_msgs/msg/detail/mesh__rosidl_typesupport_introspection_c.h"
#include "shape_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "shape_msgs/msg/detail/mesh__functions.h"
#include "shape_msgs/msg/detail/mesh__struct.h"


// Include directives for member types
// Member `triangles`
#include "shape_msgs/msg/mesh_triangle.h"
// Member `triangles`
#include "shape_msgs/msg/detail/mesh_triangle__rosidl_typesupport_introspection_c.h"
// Member `vertices`
#include "geometry_msgs/msg/point.h"
// Member `vertices`
#include "geometry_msgs/msg/detail/point__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  shape_msgs__msg__Mesh__init(message_memory);
}

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_fini_function(void * message_memory)
{
  shape_msgs__msg__Mesh__fini(message_memory);
}

size_t shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__size_function__Mesh__triangles(
  const void * untyped_member)
{
  const shape_msgs__msg__MeshTriangle__Sequence * member =
    (const shape_msgs__msg__MeshTriangle__Sequence *)(untyped_member);
  return member->size;
}

const void * shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__triangles(
  const void * untyped_member, size_t index)
{
  const shape_msgs__msg__MeshTriangle__Sequence * member =
    (const shape_msgs__msg__MeshTriangle__Sequence *)(untyped_member);
  return &member->data[index];
}

void * shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__triangles(
  void * untyped_member, size_t index)
{
  shape_msgs__msg__MeshTriangle__Sequence * member =
    (shape_msgs__msg__MeshTriangle__Sequence *)(untyped_member);
  return &member->data[index];
}

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__fetch_function__Mesh__triangles(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const shape_msgs__msg__MeshTriangle * item =
    ((const shape_msgs__msg__MeshTriangle *)
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__triangles(untyped_member, index));
  shape_msgs__msg__MeshTriangle * value =
    (shape_msgs__msg__MeshTriangle *)(untyped_value);
  *value = *item;
}

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__assign_function__Mesh__triangles(
  void * untyped_member, size_t index, const void * untyped_value)
{
  shape_msgs__msg__MeshTriangle * item =
    ((shape_msgs__msg__MeshTriangle *)
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__triangles(untyped_member, index));
  const shape_msgs__msg__MeshTriangle * value =
    (const shape_msgs__msg__MeshTriangle *)(untyped_value);
  *item = *value;
}

bool shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__resize_function__Mesh__triangles(
  void * untyped_member, size_t size)
{
  shape_msgs__msg__MeshTriangle__Sequence * member =
    (shape_msgs__msg__MeshTriangle__Sequence *)(untyped_member);
  shape_msgs__msg__MeshTriangle__Sequence__fini(member);
  return shape_msgs__msg__MeshTriangle__Sequence__init(member, size);
}

size_t shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__size_function__Mesh__vertices(
  const void * untyped_member)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return member->size;
}

const void * shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__vertices(
  const void * untyped_member, size_t index)
{
  const geometry_msgs__msg__Point__Sequence * member =
    (const geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void * shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__vertices(
  void * untyped_member, size_t index)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  return &member->data[index];
}

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__fetch_function__Mesh__vertices(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const geometry_msgs__msg__Point * item =
    ((const geometry_msgs__msg__Point *)
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__vertices(untyped_member, index));
  geometry_msgs__msg__Point * value =
    (geometry_msgs__msg__Point *)(untyped_value);
  *value = *item;
}

void shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__assign_function__Mesh__vertices(
  void * untyped_member, size_t index, const void * untyped_value)
{
  geometry_msgs__msg__Point * item =
    ((geometry_msgs__msg__Point *)
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__vertices(untyped_member, index));
  const geometry_msgs__msg__Point * value =
    (const geometry_msgs__msg__Point *)(untyped_value);
  *item = *value;
}

bool shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__resize_function__Mesh__vertices(
  void * untyped_member, size_t size)
{
  geometry_msgs__msg__Point__Sequence * member =
    (geometry_msgs__msg__Point__Sequence *)(untyped_member);
  geometry_msgs__msg__Point__Sequence__fini(member);
  return geometry_msgs__msg__Point__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_member_array[2] = {
  {
    "triangles",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(shape_msgs__msg__Mesh, triangles),  // bytes offset in struct
    NULL,  // default value
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__size_function__Mesh__triangles,  // size() function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__triangles,  // get_const(index) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__triangles,  // get(index) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__fetch_function__Mesh__triangles,  // fetch(index, &value) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__assign_function__Mesh__triangles,  // assign(index, value) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__resize_function__Mesh__triangles  // resize(index) function pointer
  },
  {
    "vertices",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(shape_msgs__msg__Mesh, vertices),  // bytes offset in struct
    NULL,  // default value
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__size_function__Mesh__vertices,  // size() function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_const_function__Mesh__vertices,  // get_const(index) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__get_function__Mesh__vertices,  // get(index) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__fetch_function__Mesh__vertices,  // fetch(index, &value) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__assign_function__Mesh__vertices,  // assign(index, value) function pointer
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__resize_function__Mesh__vertices  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_members = {
  "shape_msgs__msg",  // message namespace
  "Mesh",  // message name
  2,  // number of fields
  sizeof(shape_msgs__msg__Mesh),
  false,  // has_any_key_member_
  shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_member_array,  // message members
  shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_init_function,  // function to initialize message memory (memory has to be allocated)
  shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_type_support_handle = {
  0,
  &shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_members,
  get_message_typesupport_handle_function,
  &shape_msgs__msg__Mesh__get_type_hash,
  &shape_msgs__msg__Mesh__get_type_description,
  &shape_msgs__msg__Mesh__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_shape_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, shape_msgs, msg, Mesh)() {
  shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, shape_msgs, msg, MeshTriangle)();
  shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Point)();
  if (!shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_type_support_handle.typesupport_identifier) {
    shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &shape_msgs__msg__Mesh__rosidl_typesupport_introspection_c__Mesh_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
