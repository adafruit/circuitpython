// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/ChannelFloat32.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/channel_float32.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__CHANNEL_FLOAT32__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__CHANNEL_FLOAT32__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"
// Member 'values'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/ChannelFloat32 in the package sensor_msgs.
/**
  * This message is used by the PointCloud message to hold optional data
  * associated with each point in the cloud. The length of the values
  * array should be the same as the length of the points array in the
  * PointCloud, and each value should be associated with the corresponding
  * point.
  *
  * Channel names in existing practice include:
  *   "u", "v" - row and column (respectively) in the left stereo image.
  *              This is opposite to usual conventions but remains for
  *              historical reasons. The newer PointCloud2 message has no
  *              such problem.
  *   "rgb" - For point clouds produced by color stereo cameras. uint8
  *           (R,G,B) values packed into the least significant 24 bits,
  *           in order.
  *   "intensity" - laser or pixel intensity.
  *   "distance"
 */
typedef struct sensor_msgs__msg__ChannelFloat32
{
  /// The channel name should give semantics of the channel (e.g.
  /// "intensity" instead of "value").
  rosidl_runtime_c__String name;
  /// The values array should be 1-1 with the elements of the associated
  /// PointCloud.
  rosidl_runtime_c__float__Sequence values;
} sensor_msgs__msg__ChannelFloat32;

// Struct for a sequence of sensor_msgs__msg__ChannelFloat32.
typedef struct sensor_msgs__msg__ChannelFloat32__Sequence
{
  sensor_msgs__msg__ChannelFloat32 * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__ChannelFloat32__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__CHANNEL_FLOAT32__STRUCT_H_
