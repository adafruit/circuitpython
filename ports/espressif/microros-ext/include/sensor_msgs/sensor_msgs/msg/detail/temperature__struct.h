// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/Temperature.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/temperature.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__TEMPERATURE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__TEMPERATURE__STRUCT_H_

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

/// Struct defined in msg/Temperature in the package sensor_msgs.
/**
  * Single temperature reading.
 */
typedef struct sensor_msgs__msg__Temperature
{
  /// timestamp is the time the temperature was measured
  /// frame_id is the location of the temperature reading
  std_msgs__msg__Header header;
  /// Measurement of the Temperature in Degrees Celsius.
  double temperature;
  /// 0 is interpreted as variance unknown.
  double variance;
} sensor_msgs__msg__Temperature;

// Struct for a sequence of sensor_msgs__msg__Temperature.
typedef struct sensor_msgs__msg__Temperature__Sequence
{
  sensor_msgs__msg__Temperature * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__Temperature__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__TEMPERATURE__STRUCT_H_
