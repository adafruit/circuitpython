// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/MarkerArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/marker_array.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'markers'
#include "visualization_msgs/msg/detail/marker__struct.h"

/// Struct defined in msg/MarkerArray in the package visualization_msgs.
typedef struct visualization_msgs__msg__MarkerArray
{
  visualization_msgs__msg__Marker__Sequence markers;
} visualization_msgs__msg__MarkerArray;

// Struct for a sequence of visualization_msgs__msg__MarkerArray.
typedef struct visualization_msgs__msg__MarkerArray__Sequence
{
  visualization_msgs__msg__MarkerArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__MarkerArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__STRUCT_H_
