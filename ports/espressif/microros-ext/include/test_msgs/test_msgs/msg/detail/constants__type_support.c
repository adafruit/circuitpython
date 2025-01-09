// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from test_msgs:msg/Constants.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "test_msgs/msg/detail/constants__rosidl_typesupport_introspection_c.h"
#include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "test_msgs/msg/detail/constants__functions.h"
#include "test_msgs/msg/detail/constants__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__msg__Constants__init(message_memory);
}

void test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_fini_function(void * message_memory)
{
  test_msgs__msg__Constants__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_member_array[1] = {
  {
    "structure_needs_at_least_one_member",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Constants, structure_needs_at_least_one_member),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_members = {
  "test_msgs__msg",  // message namespace
  "Constants",  // message name
  1,  // number of fields
  sizeof(test_msgs__msg__Constants),
  false,  // has_any_key_member_
  test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_member_array,  // message members
  test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_type_support_handle = {
  0,
  &test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__msg__Constants__get_type_hash,
  &test_msgs__msg__Constants__get_type_description,
  &test_msgs__msg__Constants__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, Constants)() {
  if (!test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_type_support_handle.typesupport_identifier) {
    test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__msg__Constants__rosidl_typesupport_introspection_c__Constants_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
