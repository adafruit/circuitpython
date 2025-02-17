// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/Pose.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/pose.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POSE__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POSE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'position'
#include "geometry_msgs/msg/detail/point__struct.h"
// Member 'orientation'
#include "geometry_msgs/msg/detail/quaternion__struct.h"

/// Struct defined in msg/Pose in the package geometry_msgs.
/**
  * A representation of pose in free space, composed of position and orientation.
 */
typedef struct geometry_msgs__msg__Pose
{
  geometry_msgs__msg__Point position;
  geometry_msgs__msg__Quaternion orientation;
} geometry_msgs__msg__Pose;

// Struct for a sequence of geometry_msgs__msg__Pose.
typedef struct geometry_msgs__msg__Pose__Sequence
{
  geometry_msgs__msg__Pose * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__Pose__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POSE__STRUCT_H_
