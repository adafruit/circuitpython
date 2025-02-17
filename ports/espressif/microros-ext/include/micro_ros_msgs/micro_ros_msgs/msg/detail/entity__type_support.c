// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from micro_ros_msgs:msg/Entity.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "micro_ros_msgs/msg/detail/entity__rosidl_typesupport_introspection_c.h"
#include "micro_ros_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "micro_ros_msgs/msg/detail/entity__functions.h"
#include "micro_ros_msgs/msg/detail/entity__struct.h"


// Include directives for member types
// Member `name`
// Member `types`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  micro_ros_msgs__msg__Entity__init(message_memory);
}

void micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_fini_function(void * message_memory)
{
  micro_ros_msgs__msg__Entity__fini(message_memory);
}

size_t micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__size_function__Entity__types(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_const_function__Entity__types(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_function__Entity__types(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__fetch_function__Entity__types(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_const_function__Entity__types(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__assign_function__Entity__types(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_function__Entity__types(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__resize_function__Entity__types(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_member_array[3] = {
  {
    "entity_type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_OCTET,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Entity, entity_type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    256,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Entity, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "types",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    256,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Entity, types),  // bytes offset in struct
    NULL,  // default value
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__size_function__Entity__types,  // size() function pointer
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_const_function__Entity__types,  // get_const(index) function pointer
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__get_function__Entity__types,  // get(index) function pointer
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__fetch_function__Entity__types,  // fetch(index, &value) function pointer
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__assign_function__Entity__types,  // assign(index, value) function pointer
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__resize_function__Entity__types  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_members = {
  "micro_ros_msgs__msg",  // message namespace
  "Entity",  // message name
  3,  // number of fields
  sizeof(micro_ros_msgs__msg__Entity),
  false,  // has_any_key_member_
  micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_member_array,  // message members
  micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_init_function,  // function to initialize message memory (memory has to be allocated)
  micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_type_support_handle = {
  0,
  &micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_members,
  get_message_typesupport_handle_function,
  &micro_ros_msgs__msg__Entity__get_type_hash,
  &micro_ros_msgs__msg__Entity__get_type_description,
  &micro_ros_msgs__msg__Entity__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_micro_ros_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, micro_ros_msgs, msg, Entity)() {
  if (!micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_type_support_handle.typesupport_identifier) {
    micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &micro_ros_msgs__msg__Entity__rosidl_typesupport_introspection_c__Entity_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
