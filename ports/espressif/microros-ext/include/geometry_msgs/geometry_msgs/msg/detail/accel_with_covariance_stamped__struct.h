// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/AccelWithCovarianceStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/accel_with_covariance_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__ACCEL_WITH_COVARIANCE_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__ACCEL_WITH_COVARIANCE_STAMPED__STRUCT_H_

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
// Member 'accel'
#include "geometry_msgs/msg/detail/accel_with_covariance__struct.h"

/// Struct defined in msg/AccelWithCovarianceStamped in the package geometry_msgs.
/**
  * This represents an estimated accel with reference coordinate frame and timestamp.
 */
typedef struct geometry_msgs__msg__AccelWithCovarianceStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__AccelWithCovariance accel;
} geometry_msgs__msg__AccelWithCovarianceStamped;

// Struct for a sequence of geometry_msgs__msg__AccelWithCovarianceStamped.
typedef struct geometry_msgs__msg__AccelWithCovarianceStamped__Sequence
{
  geometry_msgs__msg__AccelWithCovarianceStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__AccelWithCovarianceStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__ACCEL_WITH_COVARIANCE_STAMPED__STRUCT_H_
