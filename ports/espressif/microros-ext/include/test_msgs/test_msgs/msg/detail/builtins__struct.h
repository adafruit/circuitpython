// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:msg/Builtins.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/msg/builtins.h"


#ifndef TEST_MSGS__MSG__DETAIL__BUILTINS__STRUCT_H_
#define TEST_MSGS__MSG__DETAIL__BUILTINS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'duration_value'
#include "builtin_interfaces/msg/detail/duration__struct.h"
// Member 'time_value'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/Builtins in the package test_msgs.
typedef struct test_msgs__msg__Builtins
{
  builtin_interfaces__msg__Duration duration_value;
  builtin_interfaces__msg__Time time_value;
} test_msgs__msg__Builtins;

// Struct for a sequence of test_msgs__msg__Builtins.
typedef struct test_msgs__msg__Builtins__Sequence
{
  test_msgs__msg__Builtins * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__msg__Builtins__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__MSG__DETAIL__BUILTINS__STRUCT_H_
