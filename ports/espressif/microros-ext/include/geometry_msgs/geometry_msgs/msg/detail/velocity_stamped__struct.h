// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/VelocityStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/velocity_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_STAMPED__STRUCT_H_

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
// Member 'body_frame_id'
// Member 'reference_frame_id'
#include "rosidl_runtime_c/string.h"
// Member 'velocity'
#include "geometry_msgs/msg/detail/twist__struct.h"

/// Struct defined in msg/VelocityStamped in the package geometry_msgs.
/**
  * This expresses the timestamped velocity vector of a frame 'body_frame_id' in the reference frame 'reference_frame_id' expressed from arbitrary observation frame 'header.frame_id'.
  * - If the 'body_frame_id' and 'header.frame_id' are identical, the velocity is observed and defined in the local coordinates system of the body
  *   which is the usual use-case in mobile robotics and is also known as a body twist.
 */
typedef struct geometry_msgs__msg__VelocityStamped
{
  std_msgs__msg__Header header;
  rosidl_runtime_c__String body_frame_id;
  rosidl_runtime_c__String reference_frame_id;
  geometry_msgs__msg__Twist velocity;
} geometry_msgs__msg__VelocityStamped;

// Struct for a sequence of geometry_msgs__msg__VelocityStamped.
typedef struct geometry_msgs__msg__VelocityStamped__Sequence
{
  geometry_msgs__msg__VelocityStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__VelocityStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_STAMPED__STRUCT_H_
