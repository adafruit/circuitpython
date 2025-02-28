// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Transform.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/transform.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'translation'
#include "geometry_msgs/msg/detail/vector3__struct.h"
// Member 'rotation'
#include "geometry_msgs/msg/detail/quaternion__struct.h"

/// Struct defined in msg/Transform in the package geometry_msgs.
/**
  * This represents the transform between two coordinate frames in free space.
 */
typedef struct geometry_msgs__msg__Transform
{
  geometry_msgs__msg__Vector3 translation;
  geometry_msgs__msg__Quaternion rotation;
} geometry_msgs__msg__Transform;

// Struct for a sequence of geometry_msgs__msg__Transform.
typedef struct geometry_msgs__msg__Transform__Sequence
{
  geometry_msgs__msg__Transform * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Transform__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM__STRUCT_H_
