// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Wrench.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/wrench.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__WRENCH__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__WRENCH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'force'
// Member 'torque'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/Wrench in the package geometry_msgs.
/**
  * This represents force in free space, separated into its linear and angular parts.
 */
typedef struct geometry_msgs__msg__Wrench
{
  geometry_msgs__msg__Vector3 force;
  geometry_msgs__msg__Vector3 torque;
} geometry_msgs__msg__Wrench;

// Struct for a sequence of geometry_msgs__msg__Wrench.
typedef struct geometry_msgs__msg__Wrench__Sequence
{
  geometry_msgs__msg__Wrench * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Wrench__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__WRENCH__STRUCT_H_
