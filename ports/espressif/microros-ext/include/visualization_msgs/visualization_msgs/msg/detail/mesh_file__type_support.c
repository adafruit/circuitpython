// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:msg/MeshFile.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/msg/detail/mesh_file__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/msg/detail/mesh_file__functions.h"
#include "visualization_msgs/msg/detail/mesh_file__struct.h"


// Include directives for member types
// Member `filename`
#include "rosidl_runtime_c/string_functions.h"
// Member `data`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__msg__MeshFile__init(message_memory);
}

void visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_fini_function(void * message_memory)
{
  visualization_msgs__msg__MeshFile__fini(message_memory);
}

size_t visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__size_function__MeshFile__data(
  const void * untyped_member)
{
  const rosidl_runtime_c__uint8__Sequence * member =
    (const rosidl_runtime_c__uint8__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_const_function__MeshFile__data(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__uint8__Sequence * member =
    (const rosidl_runtime_c__uint8__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_function__MeshFile__data(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__uint8__Sequence * member =
    (rosidl_runtime_c__uint8__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__fetch_function__MeshFile__data(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_const_function__MeshFile__data(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__assign_function__MeshFile__data(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_function__MeshFile__data(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__resize_function__MeshFile__data(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__uint8__Sequence * member =
    (rosidl_runtime_c__uint8__Sequence *)(untyped_member);
  rosidl_runtime_c__uint8__Sequence__fini(member);
  return rosidl_runtime_c__uint8__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_member_array[2] = {
  {
    "filename",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__MeshFile, filename),  // bytes offset in struct
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
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__MeshFile, data),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__size_function__MeshFile__data,  // size() function pointer
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_const_function__MeshFile__data,  // get_const(index) function pointer
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__get_function__MeshFile__data,  // get(index) function pointer
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__fetch_function__MeshFile__data,  // fetch(index, &value) function pointer
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__assign_function__MeshFile__data,  // assign(index, value) function pointer
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__resize_function__MeshFile__data  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_members = {
  "visualization_msgs__msg",  // message namespace
  "MeshFile",  // message name
  2,  // number of fields
  sizeof(visualization_msgs__msg__MeshFile),
  false,  // has_any_key_member_
  visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_member_array,  // message members
  visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_type_support_handle = {
  0,
  &visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__MeshFile__get_type_hash,
  &visualization_msgs__msg__MeshFile__get_type_description,
  &visualization_msgs__msg__MeshFile__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, MeshFile)() {
  if (!visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__msg__MeshFile__rosidl_typesupport_introspection_c__MeshFile_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
