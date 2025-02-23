// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from example_interfaces:msg/MultiArrayLayout.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "example_interfaces/msg/detail/multi_array_layout__rosidl_typesupport_introspection_c.h"
#include "example_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "example_interfaces/msg/detail/multi_array_layout__functions.h"
#include "example_interfaces/msg/detail/multi_array_layout__struct.h"


// Include directives for member types
// Member `dim`
#include "example_interfaces/msg/multi_array_dimension.h"
// Member `dim`
#include "example_interfaces/msg/detail/multi_array_dimension__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  example_interfaces__msg__MultiArrayLayout__init(message_memory);
}

void example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_fini_function(void * message_memory)
{
  example_interfaces__msg__MultiArrayLayout__fini(message_memory);
}

size_t example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__size_function__MultiArrayLayout__dim(
  const void * untyped_member)
{
  const example_interfaces__msg__MultiArrayDimension__Sequence * member =
    (const example_interfaces__msg__MultiArrayDimension__Sequence *)(untyped_member);
  return member->size;
}

const void * example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_const_function__MultiArrayLayout__dim(
  const void * untyped_member, size_t index)
{
  const example_interfaces__msg__MultiArrayDimension__Sequence * member =
    (const example_interfaces__msg__MultiArrayDimension__Sequence *)(untyped_member);
  return &member->data[index];
}

void * example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_function__MultiArrayLayout__dim(
  void * untyped_member, size_t index)
{
  example_interfaces__msg__MultiArrayDimension__Sequence * member =
    (example_interfaces__msg__MultiArrayDimension__Sequence *)(untyped_member);
  return &member->data[index];
}

void example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__fetch_function__MultiArrayLayout__dim(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const example_interfaces__msg__MultiArrayDimension * item =
    ((const example_interfaces__msg__MultiArrayDimension *)
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_const_function__MultiArrayLayout__dim(untyped_member, index));
  example_interfaces__msg__MultiArrayDimension * value =
    (example_interfaces__msg__MultiArrayDimension *)(untyped_value);
  *value = *item;
}

void example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__assign_function__MultiArrayLayout__dim(
  void * untyped_member, size_t index, const void * untyped_value)
{
  example_interfaces__msg__MultiArrayDimension * item =
    ((example_interfaces__msg__MultiArrayDimension *)
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_function__MultiArrayLayout__dim(untyped_member, index));
  const example_interfaces__msg__MultiArrayDimension * value =
    (const example_interfaces__msg__MultiArrayDimension *)(untyped_value);
  *item = *value;
}

bool example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__resize_function__MultiArrayLayout__dim(
  void * untyped_member, size_t size)
{
  example_interfaces__msg__MultiArrayDimension__Sequence * member =
    (example_interfaces__msg__MultiArrayDimension__Sequence *)(untyped_member);
  example_interfaces__msg__MultiArrayDimension__Sequence__fini(member);
  return example_interfaces__msg__MultiArrayDimension__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_member_array[2] = {
  {
    "dim",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__msg__MultiArrayLayout, dim),  // bytes offset in struct
    NULL,  // default value
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__size_function__MultiArrayLayout__dim,  // size() function pointer
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_const_function__MultiArrayLayout__dim,  // get_const(index) function pointer
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__get_function__MultiArrayLayout__dim,  // get(index) function pointer
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__fetch_function__MultiArrayLayout__dim,  // fetch(index, &value) function pointer
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__assign_function__MultiArrayLayout__dim,  // assign(index, value) function pointer
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__resize_function__MultiArrayLayout__dim  // resize(index) function pointer
  },
  {
    "data_offset",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__msg__MultiArrayLayout, data_offset),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_members = {
  "example_interfaces__msg",  // message namespace
  "MultiArrayLayout",  // message name
  2,  // number of fields
  sizeof(example_interfaces__msg__MultiArrayLayout),
  false,  // has_any_key_member_
  example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_member_array,  // message members
  example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_init_function,  // function to initialize message memory (memory has to be allocated)
  example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_type_support_handle = {
  0,
  &example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_members,
  get_message_typesupport_handle_function,
  &example_interfaces__msg__MultiArrayLayout__get_type_hash,
  &example_interfaces__msg__MultiArrayLayout__get_type_description,
  &example_interfaces__msg__MultiArrayLayout__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_example_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, msg, MultiArrayLayout)() {
  example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, msg, MultiArrayDimension)();
  if (!example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_type_support_handle.typesupport_identifier) {
    example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &example_interfaces__msg__MultiArrayLayout__rosidl_typesupport_introspection_c__MultiArrayLayout_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
