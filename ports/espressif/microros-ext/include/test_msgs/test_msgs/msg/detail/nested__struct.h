// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:msg/Nested.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/msg/nested.h"


#ifndef TEST_MSGS__MSG__DETAIL__NESTED__STRUCT_H_
#define TEST_MSGS__MSG__DETAIL__NESTED__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'basic_types_value'
#include "test_msgs/msg/detail/basic_types__struct.h"

/// Struct defined in msg/Nested in the package test_msgs.
typedef struct test_msgs__msg__Nested
{
  test_msgs__msg__BasicTypes basic_types_value;
} test_msgs__msg__Nested;

// Struct for a sequence of test_msgs__msg__Nested.
typedef struct test_msgs__msg__Nested__Sequence
{
  test_msgs__msg__Nested * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__msg__Nested__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__MSG__DETAIL__NESTED__STRUCT_H_
