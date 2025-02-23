// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from test_msgs:msg/WStrings.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "test_msgs/msg/detail/w_strings__rosidl_typesupport_introspection_c.h"
#include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "test_msgs/msg/detail/w_strings__functions.h"
#include "test_msgs/msg/detail/w_strings__struct.h"


// Include directives for member types
// Member `wstring_value`
// Member `wstring_value_default1`
// Member `wstring_value_default2`
// Member `wstring_value_default3`
// Member `array_of_wstrings`
// Member `bounded_sequence_of_wstrings`
// Member `unbounded_sequence_of_wstrings`
#include "rosidl_runtime_c/u16string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__msg__WStrings__init(message_memory);
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_fini_function(void * message_memory)
{
  test_msgs__msg__WStrings__fini(message_memory);
}

size_t test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__array_of_wstrings(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__array_of_wstrings(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__U16String * member =
    (const rosidl_runtime_c__U16String *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__array_of_wstrings(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__U16String * member =
    (rosidl_runtime_c__U16String *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__array_of_wstrings(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__U16String * item =
    ((const rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__array_of_wstrings(untyped_member, index));
  rosidl_runtime_c__U16String * value =
    (rosidl_runtime_c__U16String *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__array_of_wstrings(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__U16String * item =
    ((rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__array_of_wstrings(untyped_member, index));
  const rosidl_runtime_c__U16String * value =
    (const rosidl_runtime_c__U16String *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__bounded_sequence_of_wstrings(
  const void * untyped_member)
{
  const rosidl_runtime_c__U16String__Sequence * member =
    (const rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return member->size;
}

const void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__bounded_sequence_of_wstrings(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__U16String__Sequence * member =
    (const rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__bounded_sequence_of_wstrings(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__U16String__Sequence * member =
    (rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return &member->data[index];
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__bounded_sequence_of_wstrings(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__U16String * item =
    ((const rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__bounded_sequence_of_wstrings(untyped_member, index));
  rosidl_runtime_c__U16String * value =
    (rosidl_runtime_c__U16String *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__bounded_sequence_of_wstrings(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__U16String * item =
    ((rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__bounded_sequence_of_wstrings(untyped_member, index));
  const rosidl_runtime_c__U16String * value =
    (const rosidl_runtime_c__U16String *)(untyped_value);
  *item = *value;
}

bool test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__resize_function__WStrings__bounded_sequence_of_wstrings(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__U16String__Sequence * member =
    (rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  rosidl_runtime_c__U16String__Sequence__fini(member);
  return rosidl_runtime_c__U16String__Sequence__init(member, size);
}

size_t test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__unbounded_sequence_of_wstrings(
  const void * untyped_member)
{
  const rosidl_runtime_c__U16String__Sequence * member =
    (const rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return member->size;
}

const void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__unbounded_sequence_of_wstrings(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__U16String__Sequence * member =
    (const rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__unbounded_sequence_of_wstrings(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__U16String__Sequence * member =
    (rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  return &member->data[index];
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__unbounded_sequence_of_wstrings(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__U16String * item =
    ((const rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__unbounded_sequence_of_wstrings(untyped_member, index));
  rosidl_runtime_c__U16String * value =
    (rosidl_runtime_c__U16String *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__unbounded_sequence_of_wstrings(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__U16String * item =
    ((rosidl_runtime_c__U16String *)
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__unbounded_sequence_of_wstrings(untyped_member, index));
  const rosidl_runtime_c__U16String * value =
    (const rosidl_runtime_c__U16String *)(untyped_value);
  *item = *value;
}

bool test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__resize_function__WStrings__unbounded_sequence_of_wstrings(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__U16String__Sequence * member =
    (rosidl_runtime_c__U16String__Sequence *)(untyped_member);
  rosidl_runtime_c__U16String__Sequence__fini(member);
  return rosidl_runtime_c__U16String__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_member_array[7] = {
  {
    "wstring_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, wstring_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "wstring_value_default1",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, wstring_value_default1),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "wstring_value_default2",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, wstring_value_default2),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "wstring_value_default3",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, wstring_value_default3),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "array_of_wstrings",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, array_of_wstrings),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__array_of_wstrings,  // size() function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__array_of_wstrings,  // get_const(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__array_of_wstrings,  // get(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__array_of_wstrings,  // fetch(index, &value) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__array_of_wstrings,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "bounded_sequence_of_wstrings",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    true,  // is upper bound
    offsetof(test_msgs__msg__WStrings, bounded_sequence_of_wstrings),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__bounded_sequence_of_wstrings,  // size() function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__bounded_sequence_of_wstrings,  // get_const(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__bounded_sequence_of_wstrings,  // get(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__bounded_sequence_of_wstrings,  // fetch(index, &value) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__bounded_sequence_of_wstrings,  // assign(index, value) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__resize_function__WStrings__bounded_sequence_of_wstrings  // resize(index) function pointer
  },
  {
    "unbounded_sequence_of_wstrings",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__WStrings, unbounded_sequence_of_wstrings),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__size_function__WStrings__unbounded_sequence_of_wstrings,  // size() function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_const_function__WStrings__unbounded_sequence_of_wstrings,  // get_const(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__get_function__WStrings__unbounded_sequence_of_wstrings,  // get(index) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__fetch_function__WStrings__unbounded_sequence_of_wstrings,  // fetch(index, &value) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__assign_function__WStrings__unbounded_sequence_of_wstrings,  // assign(index, value) function pointer
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__resize_function__WStrings__unbounded_sequence_of_wstrings  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_members = {
  "test_msgs__msg",  // message namespace
  "WStrings",  // message name
  7,  // number of fields
  sizeof(test_msgs__msg__WStrings),
  false,  // has_any_key_member_
  test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_member_array,  // message members
  test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_type_support_handle = {
  0,
  &test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__msg__WStrings__get_type_hash,
  &test_msgs__msg__WStrings__get_type_description,
  &test_msgs__msg__WStrings__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, WStrings)() {
  if (!test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_type_support_handle.typesupport_identifier) {
    test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__msg__WStrings__rosidl_typesupport_introspection_c__WStrings_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
