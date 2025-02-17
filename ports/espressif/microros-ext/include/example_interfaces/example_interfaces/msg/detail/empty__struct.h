// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from example_interfaces:msg/Empty.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "example_interfaces/msg/empty.h"


#ifndef EXAMPLE_INTERFACES__MSG__DETAIL__EMPTY__STRUCT_H_
#define EXAMPLE_INTERFACES__MSG__DETAIL__EMPTY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/Empty in the package example_interfaces.
/**
  * If you want to test with this that's fine, but if you are deploying
  * it into a system you should create a semantically meaningful message type.
 */
typedef struct example_interfaces__msg__Empty
{
  uint8_t structure_needs_at_least_one_member;
} example_interfaces__msg__Empty;

// Struct for a sequence of example_interfaces__msg__Empty.
typedef struct example_interfaces__msg__Empty__Sequence
{
  example_interfaces__msg__Empty * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} example_interfaces__msg__Empty__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // EXAMPLE_INTERFACES__MSG__DETAIL__EMPTY__STRUCT_H_
