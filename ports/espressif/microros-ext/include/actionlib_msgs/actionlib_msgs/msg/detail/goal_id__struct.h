// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actionlib_msgs:msg/GoalID.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "actionlib_msgs/msg/goal_id.h"


#ifndef ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__STRUCT_H_
#define ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"
// Member 'id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/GoalID in the package actionlib_msgs.
typedef struct actionlib_msgs__msg__GoalID
{
  /// The stamp should store the time at which this goal was requested.
  /// It is used by an action server when it tries to preempt all
  /// goals that were requested before a certain time
  builtin_interfaces__msg__Time stamp;
  /// The id provides a way to associate feedback and
  /// result message with specific goal requests. The id
  /// specified must be unique.
  rosidl_runtime_c__String id;
} actionlib_msgs__msg__GoalID;

// Struct for a sequence of actionlib_msgs__msg__GoalID.
typedef struct actionlib_msgs__msg__GoalID__Sequence
{
  actionlib_msgs__msg__GoalID * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actionlib_msgs__msg__GoalID__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__STRUCT_H_
