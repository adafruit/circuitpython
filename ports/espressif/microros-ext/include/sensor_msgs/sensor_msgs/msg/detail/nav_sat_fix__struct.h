// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/NavSatFix.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/nav_sat_fix.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__NAV_SAT_FIX__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__NAV_SAT_FIX__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'COVARIANCE_TYPE_UNKNOWN'.
/**
  * If the covariance of the fix is known, fill it in completely. If the
  * GPS receiver provides the variance of each measurement, put them
  * along the diagonal. If only Dilution of Precision is available,
  * estimate an approximate covariance from that.
 */
enum
{
  sensor_msgs__msg__NavSatFix__COVARIANCE_TYPE_UNKNOWN = 0
};

/// Constant 'COVARIANCE_TYPE_APPROXIMATED'.
enum
{
  sensor_msgs__msg__NavSatFix__COVARIANCE_TYPE_APPROXIMATED = 1
};

/// Constant 'COVARIANCE_TYPE_DIAGONAL_KNOWN'.
enum
{
  sensor_msgs__msg__NavSatFix__COVARIANCE_TYPE_DIAGONAL_KNOWN = 2
};

/// Constant 'COVARIANCE_TYPE_KNOWN'.
enum
{
  sensor_msgs__msg__NavSatFix__COVARIANCE_TYPE_KNOWN = 3
};

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'status'
#include "sensor_msgs/msg/detail/nav_sat_status__struct.h"

/// Struct defined in msg/NavSatFix in the package sensor_msgs.
/**
  * Navigation Satellite fix for any Global Navigation Satellite System
  *
  * Specified using the WGS 84 reference ellipsoid
 */
typedef struct sensor_msgs__msg__NavSatFix
{
  /// header.stamp specifies the ROS time for this measurement (the
  ///        corresponding satellite time may be reported using the
  ///        sensor_msgs/TimeReference message).
  ///
  /// header.frame_id is the frame of reference reported by the satellite
  ///        receiver, usually the location of the antenna.  This is a
  ///        Euclidean frame relative to the vehicle, not a reference
  ///        ellipsoid.
  std_msgs__msg__Header header;
  /// Satellite fix status information.
  sensor_msgs__msg__NavSatStatus status;
  /// Latitude. Positive is north of equator; negative is south.
  double latitude;
  /// Longitude. Positive is east of prime meridian; negative is west.
  double longitude;
  /// Altitude. Positive is above the WGS 84 ellipsoid
  /// (quiet NaN if no altitude is available).
  double altitude;
  /// Position covariance defined relative to a tangential plane
  /// through the reported position. The components are East, North, and
  /// Up (ENU), in row-major order.
  ///
  /// Beware: this coordinate system exhibits singularities at the poles.
  double position_covariance[9];
  uint8_t position_covariance_type;
} sensor_msgs__msg__NavSatFix;

// Struct for a sequence of sensor_msgs__msg__NavSatFix.
typedef struct sensor_msgs__msg__NavSatFix__Sequence
{
  sensor_msgs__msg__NavSatFix * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__NavSatFix__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__NAV_SAT_FIX__STRUCT_H_
