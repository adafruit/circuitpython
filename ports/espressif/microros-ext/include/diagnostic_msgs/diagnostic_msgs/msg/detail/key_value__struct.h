// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from diagnostic_msgs:msg/KeyValue.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "diagnostic_msgs/msg/key_value.h"


#ifndef DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__STRUCT_H_
#define DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'key'
// Member 'value'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/KeyValue in the package diagnostic_msgs.
/**
  * What to label this value when viewing.
 */
typedef struct diagnostic_msgs__msg__KeyValue
{
  rosidl_runtime_c__String key;
  /// A value to track over time.
  rosidl_runtime_c__String value;
} diagnostic_msgs__msg__KeyValue;

// Struct for a sequence of diagnostic_msgs__msg__KeyValue.
typedef struct diagnostic_msgs__msg__KeyValue__Sequence
{
  diagnostic_msgs__msg__KeyValue * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__msg__KeyValue__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DIAGNOSTIC_MSGS__MSG__DETAIL__KEY_VALUE__STRUCT_H_
