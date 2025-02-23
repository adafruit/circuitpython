// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from diagnostic_msgs:msg/DiagnosticArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "diagnostic_msgs/msg/diagnostic_array.h"


#ifndef DIAGNOSTIC_MSGS__MSG__DETAIL__DIAGNOSTIC_ARRAY__STRUCT_H_
#define DIAGNOSTIC_MSGS__MSG__DETAIL__DIAGNOSTIC_ARRAY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'status'
#include "diagnostic_msgs/msg/detail/diagnostic_status__struct.h"

/// Struct defined in msg/DiagnosticArray in the package diagnostic_msgs.
/**
  * This message is used to send diagnostic information about the state of the robot.
 */
typedef struct diagnostic_msgs__msg__DiagnosticArray
{
  /// for timestamp
  std_msgs__msg__Header header;
  /// an array of components being reported on
  diagnostic_msgs__msg__DiagnosticStatus__Sequence status;
} diagnostic_msgs__msg__DiagnosticArray;

// Struct for a sequence of diagnostic_msgs__msg__DiagnosticArray.
typedef struct diagnostic_msgs__msg__DiagnosticArray__Sequence
{
  diagnostic_msgs__msg__DiagnosticArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__msg__DiagnosticArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DIAGNOSTIC_MSGS__MSG__DETAIL__DIAGNOSTIC_ARRAY__STRUCT_H_
