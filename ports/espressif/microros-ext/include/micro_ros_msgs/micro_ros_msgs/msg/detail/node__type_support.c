// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from micro_ros_msgs:msg/Node.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "micro_ros_msgs/msg/detail/node__rosidl_typesupport_introspection_c.h"
#include "micro_ros_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "micro_ros_msgs/msg/detail/node__functions.h"
#include "micro_ros_msgs/msg/detail/node__struct.h"


// Include directives for member types
// Member `node_namespace`
// Member `node_name`
#include "rosidl_runtime_c/string_functions.h"
// Member `entities`
#include "micro_ros_msgs/msg/entity.h"
// Member `entities`
#include "micro_ros_msgs/msg/detail/entity__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  micro_ros_msgs__msg__Node__init(message_memory);
}

void micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_fini_function(void * message_memory)
{
  micro_ros_msgs__msg__Node__fini(message_memory);
}

size_t micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__entities(
  const void * untyped_member)
{
  const micro_ros_msgs__msg__Entity__Sequence * member =
    (const micro_ros_msgs__msg__Entity__Sequence *)(untyped_member);
  return member->size;
}

const void * micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__entities(
  const void * untyped_member, size_t index)
{
  const micro_ros_msgs__msg__Entity__Sequence * member =
    (const micro_ros_msgs__msg__Entity__Sequence *)(untyped_member);
  return &member->data[index];
}

void * micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__entities(
  void * untyped_member, size_t index)
{
  micro_ros_msgs__msg__Entity__Sequence * member =
    (micro_ros_msgs__msg__Entity__Sequence *)(untyped_member);
  return &member->data[index];
}

void micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__entities(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const micro_ros_msgs__msg__Entity * item =
    ((const micro_ros_msgs__msg__Entity *)
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__entities(untyped_member, index));
  micro_ros_msgs__msg__Entity * value =
    (micro_ros_msgs__msg__Entity *)(untyped_value);
  *value = *item;
}

void micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__entities(
  void * untyped_member, size_t index, const void * untyped_value)
{
  micro_ros_msgs__msg__Entity * item =
    ((micro_ros_msgs__msg__Entity *)
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__entities(untyped_member, index));
  const micro_ros_msgs__msg__Entity * value =
    (const micro_ros_msgs__msg__Entity *)(untyped_value);
  *item = *value;
}

bool micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__entities(
  void * untyped_member, size_t size)
{
  micro_ros_msgs__msg__Entity__Sequence * member =
    (micro_ros_msgs__msg__Entity__Sequence *)(untyped_member);
  micro_ros_msgs__msg__Entity__Sequence__fini(member);
  return micro_ros_msgs__msg__Entity__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[3] = {
  {
    "node_namespace",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    256,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Node, node_namespace),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "node_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    256,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Node, node_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "entities",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(micro_ros_msgs__msg__Node, entities),  // bytes offset in struct
    NULL,  // default value
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__entities,  // size() function pointer
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__entities,  // get_const(index) function pointer
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__entities,  // get(index) function pointer
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__entities,  // fetch(index, &value) function pointer
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__entities,  // assign(index, value) function pointer
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__entities  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_members = {
  "micro_ros_msgs__msg",  // message namespace
  "Node",  // message name
  3,  // number of fields
  sizeof(micro_ros_msgs__msg__Node),
  false,  // has_any_key_member_
  micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array,  // message members
  micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_init_function,  // function to initialize message memory (memory has to be allocated)
  micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle = {
  0,
  &micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_members,
  get_message_typesupport_handle_function,
  &micro_ros_msgs__msg__Node__get_type_hash,
  &micro_ros_msgs__msg__Node__get_type_description,
  &micro_ros_msgs__msg__Node__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_micro_ros_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, micro_ros_msgs, msg, Node)() {
  micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, micro_ros_msgs, msg, Entity)();
  if (!micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle.typesupport_identifier) {
    micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &micro_ros_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
