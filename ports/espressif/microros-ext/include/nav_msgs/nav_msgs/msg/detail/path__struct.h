// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from nav_msgs:msg/Path.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "nav_msgs/msg/path.h"


#ifndef NAV_MSGS__MSG__DETAIL__PATH__STRUCT_H_
#define NAV_MSGS__MSG__DETAIL__PATH__STRUCT_H_

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
#include "geometry_msgs/msg/detail/pose_stamped__struct.h"

/// Struct defined in msg/Path in the package nav_msgs.
/**
  * An array of poses that represents a Path for a robot to follow.
 */
typedef struct nav_msgs__msg__Path
{
  /// Indicates the frame_id of the path.
  std_msgs__msg__Header header;
  /// Array of poses to follow.
  geometry_msgs__msg__PoseStamped__Sequence poses;
} nav_msgs__msg__Path;

// Struct for a sequence of nav_msgs__msg__Path.
typedef struct nav_msgs__msg__Path__Sequence
{
  nav_msgs__msg__Path * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__msg__Path__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // NAV_MSGS__MSG__DETAIL__PATH__STRUCT_H_
