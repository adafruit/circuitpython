// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/InertiaStamped.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "geometry_msgs/msg/inertia_stamped.h"


#ifndef GEOMETRY_MSGS__MSG__DETAIL__INERTIA_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__INERTIA_STAMPED__STRUCT_H_

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
// Member 'inertia'
#include "geometry_msgs/msg/detail/inertia__struct.h"

/// Struct defined in msg/InertiaStamped in the package geometry_msgs.
/**
  * An Inertia with a time stamp and reference frame.
 */
typedef struct geometry_msgs__msg__InertiaStamped
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Inertia inertia;
} geometry_msgs__msg__InertiaStamped;

// Struct for a sequence of geometry_msgs__msg__InertiaStamped.
typedef struct geometry_msgs__msg__InertiaStamped__Sequence
{
  geometry_msgs__msg__InertiaStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__InertiaStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__INERTIA_STAMPED__STRUCT_H_
