// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from micro_ros_msgs:msg/Graph.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "micro_ros_msgs/msg/graph.h"


#ifndef MICRO_ROS_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_
#define MICRO_ROS_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'nodes'
#include "micro_ros_msgs/msg/detail/node__struct.h"

/// Struct defined in msg/Graph in the package micro_ros_msgs.
typedef struct micro_ros_msgs__msg__Graph
{
  micro_ros_msgs__msg__Node__Sequence nodes;
} micro_ros_msgs__msg__Graph;

// Struct for a sequence of micro_ros_msgs__msg__Graph.
typedef struct micro_ros_msgs__msg__Graph__Sequence
{
  micro_ros_msgs__msg__Graph * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} micro_ros_msgs__msg__Graph__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MICRO_ROS_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_
