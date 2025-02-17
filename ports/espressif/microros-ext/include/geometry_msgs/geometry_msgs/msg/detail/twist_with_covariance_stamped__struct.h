// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/TwistWithCovarianceStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/twist_with_covariance_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__TWIST_WITH_COVARIANCE_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__TWIST_WITH_COVARIANCE_STAMPED__STRUCT_H_

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
// Member 'twist'
#include "geometry_msgs/msg/detail/twist_with_covariance__struct.h"

/// Struct defined in msg/TwistWithCovarianceStamped in the package geometry_msgs.
/**
  * This represents an estimated twist with reference coordinate frame and timestamp.
 */
typedef struct geometry_msgs__msg__TwistWithCovarianceStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__TwistWithCovariance twist;
} geometry_msgs__msg__TwistWithCovarianceStamped;

// Struct for a sequence of geometry_msgs__msg__TwistWithCovarianceStamped.
typedef struct geometry_msgs__msg__TwistWithCovarianceStamped__Sequence
{
  geometry_msgs__msg__TwistWithCovarianceStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__TwistWithCovarianceStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__TWIST_WITH_COVARIANCE_STAMPED__STRUCT_H_
