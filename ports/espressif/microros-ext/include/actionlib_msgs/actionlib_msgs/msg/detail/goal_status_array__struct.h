// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actionlib_msgs:msg/GoalStatusArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "actionlib_msgs/msg/goal_status_array.h"


#ifndef ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS_ARRAY__STRUCT_H_
#define ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS_ARRAY__STRUCT_H_

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
// Member 'status_list'
#include "actionlib_msgs/msg/detail/goal_status__struct.h"

/// Struct defined in msg/GoalStatusArray in the package actionlib_msgs.
/**
  * Stores the statuses for goals that are currently being tracked
  * by an action server
 */
typedef struct actionlib_msgs__msg__GoalStatusArray
{
  std_msgs__msg__Header header;
  actionlib_msgs__msg__GoalStatus__Sequence status_list;
} actionlib_msgs__msg__GoalStatusArray;

// Struct for a sequence of actionlib_msgs__msg__GoalStatusArray.
typedef struct actionlib_msgs__msg__GoalStatusArray__Sequence
{
  actionlib_msgs__msg__GoalStatusArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actionlib_msgs__msg__GoalStatusArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS_ARRAY__STRUCT_H_
