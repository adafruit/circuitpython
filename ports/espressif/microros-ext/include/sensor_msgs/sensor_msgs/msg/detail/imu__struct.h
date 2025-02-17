// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/Imu.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/imu.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__IMU__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__IMU__STRUCT_H_

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
// Member 'orientation'
#include "geometry_msgs/msg/detail/quaternion__struct.h"
// Member 'angular_velocity'
// Member 'linear_acceleration'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/Imu in the package sensor_msgs.
/**
  * This is a message to hold data from an IMU (Inertial Measurement Unit)
  *
  * Accelerations should be in m/s^2 (not in g's), and rotational velocity should be in rad/sec
  *
  * If the covariance of the measurement is known, it should be filled in (if all you know is the
  * variance of each measurement, e.g. from the datasheet, just put those along the diagonal)
  * A covariance matrix of all zeros will be interpreted as "covariance unknown", and to use the
  * data a covariance will have to be assumed or gotten from some other source
  *
  * If you have no estimate for one of the data elements (e.g. your IMU doesn't produce an
  * orientation estimate), please set element 0 of the associated covariance matrix to -1
  * If you are interpreting this message, please check for a value of -1 in the first element of each
  * covariance matrix, and disregard the associated estimate.
 */
typedef struct sensor_msgs__msg__Imu
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Quaternion orientation;
  /// Row major about x, y, z axes
  double orientation_covariance[9];
  geometry_msgs__msg__Vector3 angular_velocity;
  /// Row major about x, y, z axes
  double angular_velocity_covariance[9];
  geometry_msgs__msg__Vector3 linear_acceleration;
  /// Row major x, y z
  double linear_acceleration_covariance[9];
} sensor_msgs__msg__Imu;

// Struct for a sequence of sensor_msgs__msg__Imu.
typedef struct sensor_msgs__msg__Imu__Sequence
{
  sensor_msgs__msg__Imu * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__Imu__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__IMU__STRUCT_H_
