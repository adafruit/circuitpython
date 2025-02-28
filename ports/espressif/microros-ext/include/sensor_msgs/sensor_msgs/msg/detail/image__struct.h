// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/Image.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/image.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__IMAGE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__IMAGE__STRUCT_H_

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
// Member 'encoding'
#include "rosidl_runtime_c/string.h"
// Member 'data'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/Image in the package sensor_msgs.
/**
  * This message contains an uncompressed image
  * (0, 0) is at top-left corner of image
 */
typedef struct sensor_msgs__msg__Image
{
  /// Header timestamp should be acquisition time of image
  /// Header frame_id should be optical frame of camera
  /// origin of frame should be optical center of cameara
  /// +x should point to the right in the image
  /// +y should point down in the image
  /// +z should point into to plane of the image
  /// If the frame_id here and the frame_id of the CameraInfo
  /// message associated with the image conflict
  /// the behavior is undefined
  std_msgs__msg__Header header;
  /// image height, that is, number of rows
  uint32_t height;
  /// image width, that is, number of columns
  uint32_t width;
  /// The legal values for encoding are in file include/sensor_msgs/image_encodings.hpp
  /// If you want to standardize a new string format, join
  /// ros-users@lists.ros.org and send an email proposing a new encoding.
  /// Encoding of pixels -- channel meaning, ordering, size
  /// taken from the list of strings in include/sensor_msgs/image_encodings.hpp
  rosidl_runtime_c__String encoding;
  /// is this data bigendian?
  uint8_t is_bigendian;
  /// Full row length in bytes
  uint32_t step;
  /// actual matrix data, size is (step * rows)
  rosidl_runtime_c__uint8__Sequence data;
} sensor_msgs__msg__Image;

// Struct for a sequence of sensor_msgs__msg__Image.
typedef struct sensor_msgs__msg__Image__Sequence
{
  sensor_msgs__msg__Image * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__Image__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__IMAGE__STRUCT_H_
