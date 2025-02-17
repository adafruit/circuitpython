// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/PointStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/point_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POINT_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POINT_STAMPED__STRUCT_H_

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
// Member 'point'
#include "geometry_msgs/msg/detail/point__struct.h"

/// Struct defined in msg/PointStamped in the package geometry_msgs.
/**
  * This represents a Point with reference coordinate frame and timestamp
 */
typedef struct geometry_msgs__msg__PointStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Point point;
} geometry_msgs__msg__PointStamped;

// Struct for a sequence of geometry_msgs__msg__PointStamped.
typedef struct geometry_msgs__msg__PointStamped__Sequence
{
  geometry_msgs__msg__PointStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__PointStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POINT_STAMPED__STRUCT_H_
