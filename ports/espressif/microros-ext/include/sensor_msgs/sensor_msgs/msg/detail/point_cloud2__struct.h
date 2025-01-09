// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/PointCloud2.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/point_cloud2.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD2__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD2__STRUCT_H_

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
// Member 'fields'
#include "sensor_msgs/msg/detail/point_field__struct.h"
// Member 'data'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/PointCloud2 in the package sensor_msgs.
/**
  * This message holds a collection of N-dimensional points, which may
  * contain additional information such as normals, intensity, etc. The
  * point data is stored as a binary blob, its layout described by the
  * contents of the "fields" array.
  *
  * The point cloud data may be organized 2d (image-like) or 1d (unordered).
  * Point clouds organized as 2d images may be produced by camera depth sensors
  * such as stereo or time-of-flight.
 */
typedef struct sensor_msgs__msg__PointCloud2
{
  /// Time of sensor data acquisition, and the coordinate frame ID (for 3d points).
  std_msgs__msg__Header header;
  /// 2D structure of the point cloud. If the cloud is unordered, height is
  /// 1 and width is the length of the point cloud.
  uint32_t height;
  uint32_t width;
  /// Describes the channels and their layout in the binary data blob.
  sensor_msgs__msg__PointField__Sequence fields;
  /// Is this data bigendian?
  bool is_bigendian;
  /// Length of a point in bytes
  uint32_t point_step;
  /// Length of a row in bytes
  uint32_t row_step;
  /// Actual point data, size is (row_step*height)
  rosidl_runtime_c__uint8__Sequence data;
  /// True if there are no invalid points
  bool is_dense;
} sensor_msgs__msg__PointCloud2;

// Struct for a sequence of sensor_msgs__msg__PointCloud2.
typedef struct sensor_msgs__msg__PointCloud2__Sequence
{
  sensor_msgs__msg__PointCloud2 * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__PointCloud2__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD2__STRUCT_H_
