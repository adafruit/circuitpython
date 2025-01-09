// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from shape_msgs:msg/SolidPrimitive.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "shape_msgs/msg/solid_primitive.h"


#ifndef SHAPE_MSGS__MSG__DETAIL__SOLID_PRIMITIVE__STRUCT_H_
#define SHAPE_MSGS__MSG__DETAIL__SOLID_PRIMITIVE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'BOX'.
enum
{
  shape_msgs__msg__SolidPrimitive__BOX = 1
};

/// Constant 'SPHERE'.
enum
{
  shape_msgs__msg__SolidPrimitive__SPHERE = 2
};

/// Constant 'CYLINDER'.
enum
{
  shape_msgs__msg__SolidPrimitive__CYLINDER = 3
};

/// Constant 'CONE'.
enum
{
  shape_msgs__msg__SolidPrimitive__CONE = 4
};

/// Constant 'PRISM'.
enum
{
  shape_msgs__msg__SolidPrimitive__PRISM = 5
};

/// Constant 'BOX_X'.
/**
  * The meaning of the shape dimensions: each constant defines the index in the 'dimensions' array.
  * For type BOX, the X, Y, and Z dimensions are the length of the corresponding sides of the box.
 */
enum
{
  shape_msgs__msg__SolidPrimitive__BOX_X = 0
};

/// Constant 'BOX_Y'.
enum
{
  shape_msgs__msg__SolidPrimitive__BOX_Y = 1
};

/// Constant 'BOX_Z'.
enum
{
  shape_msgs__msg__SolidPrimitive__BOX_Z = 2
};

/// Constant 'SPHERE_RADIUS'.
/**
  * For the SPHERE type, only one component is used, and it gives the radius of the sphere.
 */
enum
{
  shape_msgs__msg__SolidPrimitive__SPHERE_RADIUS = 0
};

/// Constant 'CYLINDER_HEIGHT'.
/**
  * For the CYLINDER and CONE types, the center line is oriented along the Z axis.
  * Therefore the CYLINDER_HEIGHT (CONE_HEIGHT) component of dimensions gives the
  * height of the cylinder (cone).
  * The CYLINDER_RADIUS (CONE_RADIUS) component of dimensions gives the radius of
  * the base of the cylinder (cone).
  * Cone and cylinder primitives are defined to be circular. The tip of the cone
  * is pointing up, along +Z axis.
 */
enum
{
  shape_msgs__msg__SolidPrimitive__CYLINDER_HEIGHT = 0
};

/// Constant 'CYLINDER_RADIUS'.
enum
{
  shape_msgs__msg__SolidPrimitive__CYLINDER_RADIUS = 1
};

/// Constant 'CONE_HEIGHT'.
enum
{
  shape_msgs__msg__SolidPrimitive__CONE_HEIGHT = 0
};

/// Constant 'CONE_RADIUS'.
enum
{
  shape_msgs__msg__SolidPrimitive__CONE_RADIUS = 1
};

/// Constant 'PRISM_HEIGHT'.
/**
  * For the type PRISM, the center line is oriented along Z axis.
  * The PRISM_HEIGHT component of dimensions gives the
  * height of the prism.
  * The polygon defines the Z axis centered base of the prism.
  * The prism is constructed by extruding the base in +Z and -Z
  * directions by half of the PRISM_HEIGHT
  * Only x and y fields of the points are used in the polygon.
  * Points of the polygon are ordered counter-clockwise.
 */
enum
{
  shape_msgs__msg__SolidPrimitive__PRISM_HEIGHT = 0
};

// Include directives for member types
// Member 'dimensions'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'polygon'
#include "geometry_msgs/msg/detail/polygon__struct.h"

// constants for array fields with an upper bound
// dimensions
enum
{
  shape_msgs__msg__SolidPrimitive__dimensions__MAX_SIZE = 3
};

/// Struct defined in msg/SolidPrimitive in the package shape_msgs.
/**
  * Defines box, sphere, cylinder, cone and prism.
  * All shapes are defined to have their bounding boxes centered around 0,0,0.
 */
typedef struct shape_msgs__msg__SolidPrimitive
{
  /// The type of the shape
  uint8_t type;
  /// The dimensions of the shape
  /// At no point will dimensions have a length > 3.
  rosidl_runtime_c__double__Sequence dimensions;
  geometry_msgs__msg__Polygon polygon;
} shape_msgs__msg__SolidPrimitive;

// Struct for a sequence of shape_msgs__msg__SolidPrimitive.
typedef struct shape_msgs__msg__SolidPrimitive__Sequence
{
  shape_msgs__msg__SolidPrimitive * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} shape_msgs__msg__SolidPrimitive__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SHAPE_MSGS__MSG__DETAIL__SOLID_PRIMITIVE__STRUCT_H_
