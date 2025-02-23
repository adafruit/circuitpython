// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/PolygonInstance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/polygon_instance.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'polygon'
#include "geometry_msgs/msg/detail/polygon__struct.h"

/// Struct defined in msg/PolygonInstance in the package geometry_msgs.
/**
  * A specification of a polygon where the first and last points are assumed to be connected
  * It includes a unique identification field for disambiguating multiple instances
 */
typedef struct geometry_msgs__msg__PolygonInstance
{
  geometry_msgs__msg__Polygon polygon;
  int64_t id;
} geometry_msgs__msg__PolygonInstance;

// Struct for a sequence of geometry_msgs__msg__PolygonInstance.
typedef struct geometry_msgs__msg__PolygonInstance__Sequence
{
  geometry_msgs__msg__PolygonInstance * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__PolygonInstance__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POLYGON_INSTANCE__STRUCT_H_
