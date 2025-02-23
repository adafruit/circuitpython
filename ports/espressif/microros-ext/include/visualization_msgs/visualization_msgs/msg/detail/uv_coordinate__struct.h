// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/UVCoordinate.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/uv_coordinate.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__UV_COORDINATE__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__UV_COORDINATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/UVCoordinate in the package visualization_msgs.
/**
  * Location of the pixel as a ratio of the width of a 2D texture.
  * Values should be in range:.
 */
typedef struct visualization_msgs__msg__UVCoordinate
{
  float u;
  float v;
} visualization_msgs__msg__UVCoordinate;

// Struct for a sequence of visualization_msgs__msg__UVCoordinate.
typedef struct visualization_msgs__msg__UVCoordinate__Sequence
{
  visualization_msgs__msg__UVCoordinate * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__UVCoordinate__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__UV_COORDINATE__STRUCT_H_
