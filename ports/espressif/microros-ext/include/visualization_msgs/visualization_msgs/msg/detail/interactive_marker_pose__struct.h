// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/InteractiveMarkerPose.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/interactive_marker_pose.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_POSE__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_POSE__STRUCT_H_

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
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.h"
// Member 'name'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/InteractiveMarkerPose in the package visualization_msgs.
typedef struct visualization_msgs__msg__InteractiveMarkerPose
{
  /// Time/frame info.
  std_msgs__msg__Header header;
  /// Initial pose. Also, defines the pivot point for rotations.
  geometry_msgs__msg__Pose pose;
  /// Identifying string. Must be globally unique in
  /// the topic that this message is sent through.
  rosidl_runtime_c__String name;
} visualization_msgs__msg__InteractiveMarkerPose;

// Struct for a sequence of visualization_msgs__msg__InteractiveMarkerPose.
typedef struct visualization_msgs__msg__InteractiveMarkerPose__Sequence
{
  visualization_msgs__msg__InteractiveMarkerPose * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__InteractiveMarkerPose__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_POSE__STRUCT_H_
