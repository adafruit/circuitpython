// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from shape_msgs:msg/MeshTriangle.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "shape_msgs/msg/mesh_triangle.h"


#ifndef SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__STRUCT_H_
#define SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/MeshTriangle in the package shape_msgs.
/**
  * Definition of a triangle's vertices.
 */
typedef struct shape_msgs__msg__MeshTriangle
{
  uint32_t vertex_indices[3];
} shape_msgs__msg__MeshTriangle;

// Struct for a sequence of shape_msgs__msg__MeshTriangle.
typedef struct shape_msgs__msg__MeshTriangle__Sequence
{
  shape_msgs__msg__MeshTriangle * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} shape_msgs__msg__MeshTriangle__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__STRUCT_H_
