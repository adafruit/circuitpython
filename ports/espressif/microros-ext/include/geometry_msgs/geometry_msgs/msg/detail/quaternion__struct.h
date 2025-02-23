// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Quaternion.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/quaternion.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__QUATERNION__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__QUATERNION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/Quaternion in the package geometry_msgs.
/**
  * This represents an orientation in free space in quaternion form.
 */
typedef struct geometry_msgs__msg__Quaternion
{
  double x;
  double y;
  double z;
  double w;
} geometry_msgs__msg__Quaternion;

// Struct for a sequence of geometry_msgs__msg__Quaternion.
typedef struct geometry_msgs__msg__Quaternion__Sequence
{
  geometry_msgs__msg__Quaternion * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Quaternion__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__QUATERNION__STRUCT_H_
