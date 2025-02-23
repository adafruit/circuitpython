// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/PolygonStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/polygon_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POLYGON_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POLYGON_STAMPED__STRUCT_H_

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
// Member 'polygon'
#include "geometry_msgs/msg/detail/polygon__struct.h"

/// Struct defined in msg/PolygonStamped in the package geometry_msgs.
/**
  * This represents a Polygon with reference coordinate frame and timestamp
 */
typedef struct geometry_msgs__msg__PolygonStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Polygon polygon;
} geometry_msgs__msg__PolygonStamped;

// Struct for a sequence of geometry_msgs__msg__PolygonStamped.
typedef struct geometry_msgs__msg__PolygonStamped__Sequence
{
  geometry_msgs__msg__PolygonStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__PolygonStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POLYGON_STAMPED__STRUCT_H_
