// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from actionlib_msgs:msg/GoalStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "actionlib_msgs/msg/goal_status.h"


#ifndef ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS__STRUCT_H_
#define ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'PENDING'.
/**
  * The goal has yet to be processed by the action server.
 */
enum
{
  actionlib_msgs__msg__GoalStatus__PENDING = 0
};

/// Constant 'ACTIVE'.
/**
  * The goal is currently being processed by the action server.
 */
enum
{
  actionlib_msgs__msg__GoalStatus__ACTIVE = 1
};

/// Constant 'PREEMPTED'.
/**
  * The goal received a cancel request after it started executing
  *   and has since completed its execution (Terminal State).
 */
enum
{
  actionlib_msgs__msg__GoalStatus__PREEMPTED = 2
};

/// Constant 'SUCCEEDED'.
/**
  * The goal was achieved successfully by the action server
  *   (Terminal State).
 */
enum
{
  actionlib_msgs__msg__GoalStatus__SUCCEEDED = 3
};

/// Constant 'ABORTED'.
/**
  * The goal was aborted during execution by the action server due
  *    to some failure (Terminal State).
 */
enum
{
  actionlib_msgs__msg__GoalStatus__ABORTED = 4
};

/// Constant 'REJECTED'.
/**
  * The goal was rejected by the action server without being processed,
  *    because the goal was unattainable or invalid (Terminal State).
 */
enum
{
  actionlib_msgs__msg__GoalStatus__REJECTED = 5
};

/// Constant 'PREEMPTING'.
/**
  * The goal received a cancel request after it started executing
  *    and has not yet completed execution.
 */
enum
{
  actionlib_msgs__msg__GoalStatus__PREEMPTING = 6
};

/// Constant 'RECALLING'.
/**
  * The goal received a cancel request before it started executing, but
  *    the action server has not yet confirmed that the goal is canceled.
 */
enum
{
  actionlib_msgs__msg__GoalStatus__RECALLING = 7
};

/// Constant 'RECALLED'.
/**
  * The goal received a cancel request before it started executing
  *    and was successfully cancelled (Terminal State).
 */
enum
{
  actionlib_msgs__msg__GoalStatus__RECALLED = 8
};

/// Constant 'LOST'.
/**
  * An action client can determine that a goal is LOST. This should not
  *    be sent over the wire by an action server.
 */
enum
{
  actionlib_msgs__msg__GoalStatus__LOST = 9
};

// Include directives for member types
// Member 'goal_id'
#include "actionlib_msgs/msg/detail/goal_id__struct.h"
// Member 'text'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/GoalStatus in the package actionlib_msgs.
typedef struct actionlib_msgs__msg__GoalStatus
{
  actionlib_msgs__msg__GoalID goal_id;
  uint8_t status;
  /// Allow for the user to associate a string with GoalStatus for debugging.
  rosidl_runtime_c__String text;
} actionlib_msgs__msg__GoalStatus;

// Struct for a sequence of actionlib_msgs__msg__GoalStatus.
typedef struct actionlib_msgs__msg__GoalStatus__Sequence
{
  actionlib_msgs__msg__GoalStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} actionlib_msgs__msg__GoalStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACTIONLIB_MSGS__MSG__DETAIL__GOAL_STATUS__STRUCT_H_
