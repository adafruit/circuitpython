// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/PointCloud.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/point_cloud.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD__STRUCT_H_

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
// Member 'points'
#include "geometry_msgs/msg/detail/point32__struct.h"
// Member 'channels'
#include "sensor_msgs/msg/detail/channel_float32__struct.h"

/// Struct defined in msg/PointCloud in the package sensor_msgs.
/**
  * THIS MESSAGE IS DEPRECATED AS OF FOXY
  * Please use sensor_msgs/PointCloud2
 */
typedef struct sensor_msgs__msg__PointCloud
{
  /// This message holds a collection of 3d points, plus optional additional
  /// information about each point.
  /// Time of sensor data acquisition, coordinate frame ID.
  std_msgs__msg__Header header;
  /// Array of 3d points. Each Point32 should be interpreted as a 3d point
  /// in the frame given in the header.
  geometry_msgs__msg__Point32__Sequence points;
  /// Each channel should have the same number of elements as points array,
  /// and the data in each channel should correspond 1:1 with each point.
  /// Channel names in common practice are listed in ChannelFloat32.msg.
  sensor_msgs__msg__ChannelFloat32__Sequence channels;
} sensor_msgs__msg__PointCloud;

// Struct for a sequence of sensor_msgs__msg__PointCloud.
typedef struct sensor_msgs__msg__PointCloud__Sequence
{
  sensor_msgs__msg__PointCloud * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__PointCloud__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__POINT_CLOUD__STRUCT_H_
