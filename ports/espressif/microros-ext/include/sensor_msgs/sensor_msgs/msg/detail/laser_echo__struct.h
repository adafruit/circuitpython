// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/LaserEcho.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/laser_echo.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__LASER_ECHO__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__LASER_ECHO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'echoes'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/LaserEcho in the package sensor_msgs.
/**
  * This message is a submessage of MultiEchoLaserScan and is not intended
  * to be used separately.
 */
typedef struct sensor_msgs__msg__LaserEcho
{
  /// Multiple values of ranges or intensities.
  /// Each array represents data from the same angle increment.
  rosidl_runtime_c__float__Sequence echoes;
} sensor_msgs__msg__LaserEcho;

// Struct for a sequence of sensor_msgs__msg__LaserEcho.
typedef struct sensor_msgs__msg__LaserEcho__Sequence
{
  sensor_msgs__msg__LaserEcho * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__LaserEcho__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__LASER_ECHO__STRUCT_H_
