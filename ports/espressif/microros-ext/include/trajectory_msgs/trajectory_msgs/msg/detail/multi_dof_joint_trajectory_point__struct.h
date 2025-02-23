// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from trajectory_msgs:msg/MultiDOFJointTrajectoryPoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "trajectory_msgs/msg/multi_dof_joint_trajectory_point.h"


#ifndef TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__STRUCT_H_
#define TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'transforms'
#include "geometry_msgs/msg/detail/transform__struct.h"
// Member 'velocities'
// Member 'accelerations'
#include "geometry_msgs/msg/detail/twist__struct.h"
// Member 'time_from_start'
#include "builtin_interfaces/msg/detail/duration__struct.h"

/// Struct defined in msg/MultiDOFJointTrajectoryPoint in the package trajectory_msgs.
/**
  * Each multi-dof joint can specify a transform (up to 6 DOF).
 */
typedef struct trajectory_msgs__msg__MultiDOFJointTrajectoryPoint
{
  geometry_msgs__msg__Transform__Sequence transforms;
  /// There can be a velocity specified for the origin of the joint.
  geometry_msgs__msg__Twist__Sequence velocities;
  /// There can be an acceleration specified for the origin of the joint.
  geometry_msgs__msg__Twist__Sequence accelerations;
  /// Desired time from the trajectory start to arrive at this trajectory point.
  builtin_interfaces__msg__Duration time_from_start;
} trajectory_msgs__msg__MultiDOFJointTrajectoryPoint;

// Struct for a sequence of trajectory_msgs__msg__MultiDOFJointTrajectoryPoint.
typedef struct trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence
{
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__STRUCT_H_
