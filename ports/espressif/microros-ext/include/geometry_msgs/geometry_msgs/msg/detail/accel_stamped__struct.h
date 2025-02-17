// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/AccelStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/accel_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__ACCEL_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__ACCEL_STAMPED__STRUCT_H_

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
#include "geometry_msgs/msg/detail/accel__struct.h"

/// Struct defined in msg/AccelStamped in the package geometry_msgs.
/**
  * An accel with reference coordinate frame and timestamp
 */
typedef struct geometry_msgs__msg__AccelStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Accel accel;
} geometry_msgs__msg__AccelStamped;

// Struct for a sequence of geometry_msgs__msg__AccelStamped.
typedef struct geometry_msgs__msg__AccelStamped__Sequence
{
  geometry_msgs__msg__AccelStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__AccelStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__ACCEL_STAMPED__STRUCT_H_
