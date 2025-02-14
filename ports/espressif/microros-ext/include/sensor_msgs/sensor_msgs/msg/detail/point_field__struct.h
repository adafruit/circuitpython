// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/PointField.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/point_field.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__POINT_FIELD__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__POINT_FIELD__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'INT8'.
enum
{
  sensor_msgs__msg__PointField__INT8 = 1
};

/// Constant 'UINT8'.
enum
{
  sensor_msgs__msg__PointField__UINT8 = 2
};

/// Constant 'INT16'.
enum
{
  sensor_msgs__msg__PointField__INT16 = 3
};

/// Constant 'UINT16'.
enum
{
  sensor_msgs__msg__PointField__UINT16 = 4
};

/// Constant 'INT32'.
enum
{
  sensor_msgs__msg__PointField__INT32 = 5
};

/// Constant 'UINT32'.
enum
{
  sensor_msgs__msg__PointField__UINT32 = 6
};

/// Constant 'FLOAT32'.
enum
{
  sensor_msgs__msg__PointField__FLOAT32 = 7
};

/// Constant 'FLOAT64'.
enum
{
  sensor_msgs__msg__PointField__FLOAT64 = 8
};

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/PointField in the package sensor_msgs.
/**
  * This message holds the description of one point entry in the
  * PointCloud2 message format.
 */
typedef struct sensor_msgs__msg__PointField
{
  /// Common PointField names are x, y, z, intensity, rgb, rgba
  /// Name of field
  rosidl_runtime_c__String name;
  /// Offset from start of point struct
  uint32_t offset;
  /// Datatype enumeration, see above
  uint8_t datatype;
  /// How many elements in the field
  uint32_t count;
} sensor_msgs__msg__PointField;

// Struct for a sequence of sensor_msgs__msg__PointField.
typedef struct sensor_msgs__msg__PointField__Sequence
{
  sensor_msgs__msg__PointField * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__PointField__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__POINT_FIELD__STRUCT_H_
