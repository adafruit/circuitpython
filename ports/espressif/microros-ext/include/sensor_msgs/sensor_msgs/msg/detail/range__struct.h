// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/Range.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/range.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__RANGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'ULTRASOUND'.
/**
  * Radiation type enums
  * If you want a value added to this list, send an email to the ros-users list
 */
enum
{
  sensor_msgs__msg__Range__ULTRASOUND = 0
};

/// Constant 'INFRARED'.
enum
{
  sensor_msgs__msg__Range__INFRARED = 1
};

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"

/// Struct defined in msg/Range in the package sensor_msgs.
/**
  * Single range reading from an active ranger that emits energy and reports
  * one range reading that is valid along an arc at the distance measured.
  * This message is  not appropriate for laser scanners. See the LaserScan
  * message if you are working with a laser scanner.
  *
  * This message also can represent a fixed-distance (binary) ranger.  This
  * sensor will have min_range===max_range===distance of detection.
  * These sensors follow REP 117 and will output -Inf if the object is detected
  * and +Inf if the object is outside of the detection range.
 */
typedef struct sensor_msgs__msg__Range
{
  /// timestamp in the header is the time the ranger
  /// returned the distance reading
  std_msgs__msg__Header header;
  /// the type of radiation used by the sensor
  /// (sound, IR, etc)
  uint8_t radiation_type;
  /// the size of the arc that the distance reading is
  /// valid for
  /// the object causing the range reading may have
  /// been anywhere within -field_of_view/2 and
  /// field_of_view/2 at the measured range.
  /// 0 angle corresponds to the x-axis of the sensor.
  float field_of_view;
  /// minimum range value
  float min_range;
  /// maximum range value
  /// Fixed distance rangers require min_range==max_range
  float max_range;
  /// range data
  /// (Note: values < range_min or > range_max should be discarded)
  /// Fixed distance rangers only output -Inf or +Inf.
  /// -Inf represents a detection within fixed distance.
  /// (Detection too close to the sensor to quantify)
  /// +Inf represents no detection within the fixed distance.
  /// (Object out of range)
  float range;
  /// variance of the range sensor
  /// 0 is interpreted as variance unknown
  float variance;
} sensor_msgs__msg__Range;

// Struct for a sequence of sensor_msgs__msg__Range.
typedef struct sensor_msgs__msg__Range__Sequence
{
  sensor_msgs__msg__Range * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__Range__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__RANGE__STRUCT_H_
