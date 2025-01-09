// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/JoyFeedbackArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/joy_feedback_array.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__JOY_FEEDBACK_ARRAY__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__JOY_FEEDBACK_ARRAY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'array'
#include "sensor_msgs/msg/detail/joy_feedback__struct.h"

/// Struct defined in msg/JoyFeedbackArray in the package sensor_msgs.
/**
  * This message publishes values for multiple feedback at once.
 */
typedef struct sensor_msgs__msg__JoyFeedbackArray
{
  sensor_msgs__msg__JoyFeedback__Sequence array;
} sensor_msgs__msg__JoyFeedbackArray;

// Struct for a sequence of sensor_msgs__msg__JoyFeedbackArray.
typedef struct sensor_msgs__msg__JoyFeedbackArray__Sequence
{
  sensor_msgs__msg__JoyFeedbackArray * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__JoyFeedbackArray__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__JOY_FEEDBACK_ARRAY__STRUCT_H_
