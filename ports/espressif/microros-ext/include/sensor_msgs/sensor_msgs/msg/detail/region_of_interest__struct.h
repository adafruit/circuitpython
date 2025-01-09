// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/RegionOfInterest.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/region_of_interest.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__REGION_OF_INTEREST__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__REGION_OF_INTEREST__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/RegionOfInterest in the package sensor_msgs.
/**
  * This message is used to specify a region of interest within an image.
  *
  * When used to specify the ROI setting of the camera when the image was
  * taken, the height and width fields should either match the height and
  * width fields for the associated image; or height = width = 0
  * indicates that the full resolution image was captured.
 */
typedef struct sensor_msgs__msg__RegionOfInterest
{
  /// Leftmost pixel of the ROI
  /// (0 if the ROI includes the left edge of the image)
  uint32_t x_offset;
  /// Topmost pixel of the ROI
  /// (0 if the ROI includes the top edge of the image)
  uint32_t y_offset;
  /// Height of ROI
  uint32_t height;
  /// Width of ROI
  uint32_t width;
  /// True if a distinct rectified ROI should be calculated from the "raw"
  /// ROI in this message. Typically this should be False if the full image
  /// is captured (ROI not used), and True if a subwindow is captured (ROI
  /// used).
  bool do_rectify;
} sensor_msgs__msg__RegionOfInterest;

// Struct for a sequence of sensor_msgs__msg__RegionOfInterest.
typedef struct sensor_msgs__msg__RegionOfInterest__Sequence
{
  sensor_msgs__msg__RegionOfInterest * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__RegionOfInterest__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__REGION_OF_INTEREST__STRUCT_H_
