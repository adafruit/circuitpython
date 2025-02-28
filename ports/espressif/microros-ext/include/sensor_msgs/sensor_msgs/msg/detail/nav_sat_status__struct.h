// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/NavSatStatus.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/nav_sat_status.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__NAV_SAT_STATUS__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__NAV_SAT_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'STATUS_UNKNOWN'.
/**
  * status is not yet set
 */
enum
{
  sensor_msgs__msg__NavSatStatus__STATUS_UNKNOWN = -2
};

/// Constant 'STATUS_NO_FIX'.
/**
  * unable to fix position
 */
enum
{
  sensor_msgs__msg__NavSatStatus__STATUS_NO_FIX = -1
};

/// Constant 'STATUS_FIX'.
/**
  * unaugmented fix
 */
enum
{
  sensor_msgs__msg__NavSatStatus__STATUS_FIX = 0
};

/// Constant 'STATUS_SBAS_FIX'.
/**
  * with satellite-based augmentation
 */
enum
{
  sensor_msgs__msg__NavSatStatus__STATUS_SBAS_FIX = 1
};

/// Constant 'STATUS_GBAS_FIX'.
/**
  * with ground-based augmentation
 */
enum
{
  sensor_msgs__msg__NavSatStatus__STATUS_GBAS_FIX = 2
};

/// Constant 'SERVICE_UNKNOWN'.
/**
  * Bits defining which Global Navigation Satellite System signals were
  * used by the receiver.
  * Remember service is a bitfield, so checking (service & SERVICE_UNKNOWN) will not work. Use == instead.
 */
enum
{
  sensor_msgs__msg__NavSatStatus__SERVICE_UNKNOWN = 0
};

/// Constant 'SERVICE_GPS'.
enum
{
  sensor_msgs__msg__NavSatStatus__SERVICE_GPS = 1
};

/// Constant 'SERVICE_GLONASS'.
enum
{
  sensor_msgs__msg__NavSatStatus__SERVICE_GLONASS = 2
};

/// Constant 'SERVICE_COMPASS'.
/**
  * includes BeiDou.
 */
enum
{
  sensor_msgs__msg__NavSatStatus__SERVICE_COMPASS = 4
};

/// Constant 'SERVICE_GALILEO'.
enum
{
  sensor_msgs__msg__NavSatStatus__SERVICE_GALILEO = 8
};

/// Struct defined in msg/NavSatStatus in the package sensor_msgs.
/**
  * Navigation Satellite fix status for any Global Navigation Satellite System.
  *
  * Whether to output an augmented fix is determined by both the fix
  * type and the last time differential corrections were received.  A
  * fix is valid when status >= STATUS_FIX.
 */
typedef struct sensor_msgs__msg__NavSatStatus
{
  /// STATUS_UNKNOWN
  int8_t status;
  uint16_t service;
} sensor_msgs__msg__NavSatStatus;

// Struct for a sequence of sensor_msgs__msg__NavSatStatus.
typedef struct sensor_msgs__msg__NavSatStatus__Sequence
{
  sensor_msgs__msg__NavSatStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__NavSatStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__NAV_SAT_STATUS__STRUCT_H_
