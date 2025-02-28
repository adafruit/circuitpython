// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/Illuminance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/illuminance.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__ILLUMINANCE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__ILLUMINANCE__STRUCT_H_

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

/// Struct defined in msg/Illuminance in the package sensor_msgs.
/**
  * Single photometric illuminance measurement.  Light should be assumed to be
  * measured along the sensor's x-axis (the area of detection is the y-z plane).
  * The illuminance should have a 0 or positive value and be received with
  * the sensor's +X axis pointing toward the light source.
  *
  * Photometric illuminance is the measure of the human eye's sensitivity of the
  * intensity of light encountering or passing through a surface.
  *
  * All other Photometric and Radiometric measurements should not use this message.
  * This message cannot represent:
  *  - Luminous intensity (candela/light source output)
  *  - Luminance (nits/light output per area)
  *  - Irradiance (watt/area), etc.
 */
typedef struct sensor_msgs__msg__Illuminance
{
  /// timestamp is the time the illuminance was measured
  /// frame_id is the location and direction of the reading
  std_msgs__msg__Header header;
  /// Measurement of the Photometric Illuminance in Lux.
  double illuminance;
  /// 0 is interpreted as variance unknown
  double variance;
} sensor_msgs__msg__Illuminance;

// Struct for a sequence of sensor_msgs__msg__Illuminance.
typedef struct sensor_msgs__msg__Illuminance__Sequence
{
  sensor_msgs__msg__Illuminance * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__Illuminance__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__ILLUMINANCE__STRUCT_H_
