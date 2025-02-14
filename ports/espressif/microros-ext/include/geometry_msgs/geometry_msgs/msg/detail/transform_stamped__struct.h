// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/TransformStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/transform_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM_STAMPED__STRUCT_H_

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
// Member 'child_frame_id'
#include "rosidl_runtime_c/string.h"
// Member 'transform'
#include "geometry_msgs/msg/detail/transform__struct.h"

/// Struct defined in msg/TransformStamped in the package geometry_msgs.
/**
  * This expresses a transform from coordinate frame header.frame_id
  * to the coordinate frame child_frame_id at the time of header.stamp
  *
  * This message is mostly used by the
  * <a href="https://docs.ros.org/en/rolling/p/tf2/">tf2</a> package.
  * See its documentation for more information.
  *
  * The child_frame_id is necessary in addition to the frame_id
  * in the Header to communicate the full reference for the transform
  * in a self contained message.
 */
typedef struct geometry_msgs__msg__TransformStamped
{
  /// The frame id in the header is used as the reference frame of this transform.
  std_msgs__msg__Header header;
  /// The frame id of the child frame to which this transform points.
  rosidl_runtime_c__String child_frame_id;
  /// Translation and rotation in 3-dimensions of child_frame_id from header.frame_id.
  geometry_msgs__msg__Transform transform;
} geometry_msgs__msg__TransformStamped;

// Struct for a sequence of geometry_msgs__msg__TransformStamped.
typedef struct geometry_msgs__msg__TransformStamped__Sequence
{
  geometry_msgs__msg__TransformStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__TransformStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__TRANSFORM_STAMPED__STRUCT_H_
