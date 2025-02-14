// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Polygon.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/polygon.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POLYGON__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POLYGON__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'points'
#include "geometry_msgs/msg/detail/point32__struct.h"

/// Struct defined in msg/Polygon in the package geometry_msgs.
/**
  * A specification of a polygon where the first and last points are assumed to be connected
 */
typedef struct geometry_msgs__msg__Polygon
{
  geometry_msgs__msg__Point32__Sequence points;
} geometry_msgs__msg__Polygon;

// Struct for a sequence of geometry_msgs__msg__Polygon.
typedef struct geometry_msgs__msg__Polygon__Sequence
{
  geometry_msgs__msg__Polygon * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Polygon__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POLYGON__STRUCT_H_
