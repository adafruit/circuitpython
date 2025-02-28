// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Inertia.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/inertia.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__INERTIA__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__INERTIA__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'com'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/Inertia in the package geometry_msgs.
/**
  * Mass
 */
typedef struct geometry_msgs__msg__Inertia
{
  double m;
  /// Center of mass
  geometry_msgs__msg__Vector3 com;
  /// Inertia Tensor
  ///     | ixx ixy ixz |
  /// I = | ixy iyy iyz |
  ///     | ixz iyz izz |
  double ixx;
  double ixy;
  double ixz;
  double iyy;
  double iyz;
  double izz;
} geometry_msgs__msg__Inertia;

// Struct for a sequence of geometry_msgs__msg__Inertia.
typedef struct geometry_msgs__msg__Inertia__Sequence
{
  geometry_msgs__msg__Inertia * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Inertia__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__INERTIA__STRUCT_H_
