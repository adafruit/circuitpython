// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/FluidPressure.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/fluid_pressure.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__FLUID_PRESSURE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__FLUID_PRESSURE__STRUCT_H_

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

/// Struct defined in msg/FluidPressure in the package sensor_msgs.
/**
  * Single pressure reading.  This message is appropriate for measuring the
  * pressure inside of a fluid (air, water, etc).  This also includes
  * atmospheric or barometric pressure.
  *
  * This message is not appropriate for force/pressure contact sensors.
 */
typedef struct sensor_msgs__msg__FluidPressure
{
  /// timestamp of the measurement
  /// frame_id is the location of the pressure sensor
  std_msgs__msg__Header header;
  /// Absolute pressure reading in Pascals.
  double fluid_pressure;
  /// 0 is interpreted as variance unknown
  double variance;
} sensor_msgs__msg__FluidPressure;

// Struct for a sequence of sensor_msgs__msg__FluidPressure.
typedef struct sensor_msgs__msg__FluidPressure__Sequence
{
  sensor_msgs__msg__FluidPressure * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__FluidPressure__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__FLUID_PRESSURE__STRUCT_H_
