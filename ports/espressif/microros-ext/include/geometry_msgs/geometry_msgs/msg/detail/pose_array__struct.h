// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/PoseArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/pose_array.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__POSE_ARRAY__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__POSE_ARRAY__STRUCT_H_

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
// Member 'poses'
#include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in msg/PoseArray in the package geometry_msgs.
/**
  * An array of poses with a header for global reference.
 */
typedef struct geometry_msgs__msg__PoseArray
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Pose__Sequence poses;
} geometry_msgs__msg__PoseArray;

// Struct for a sequence of geometry_msgs__msg__PoseArray.
typedef struct geometry_msgs__msg__PoseArray__Sequence
{
  geometry_msgs__msg__PoseArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__PoseArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__POSE_ARRAY__STRUCT_H_
