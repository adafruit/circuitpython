// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from example_interfaces:msg/Int32.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "example_interfaces/msg/int32.h"


#ifndef EXAMPLE_INTERFACES__MSG__DETAIL__INT32__STRUCT_H_
#define EXAMPLE_INTERFACES__MSG__DETAIL__INT32__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/Int32 in the package example_interfaces.
/**
  * This is an example message of using a primitive datatype, int32.
  * If you want to test with this that's fine, but if you are deploying
  * it into a system you should create a semantically meaningful message type.
  * If you want to embed it in another message, use the primitive data type instead.
 */
typedef struct example_interfaces__msg__Int32
{
  int32_t data;
} example_interfaces__msg__Int32;

// Struct for a sequence of example_interfaces__msg__Int32.
typedef struct example_interfaces__msg__Int32__Sequence
{
  example_interfaces__msg__Int32 * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} example_interfaces__msg__Int32__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // EXAMPLE_INTERFACES__MSG__DETAIL__INT32__STRUCT_H_
