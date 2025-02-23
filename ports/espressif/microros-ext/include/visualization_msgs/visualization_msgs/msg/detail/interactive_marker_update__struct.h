// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/InteractiveMarkerUpdate.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/interactive_marker_update.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_UPDATE__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_UPDATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'KEEP_ALIVE'.
/**
  * Type holds the purpose of this message.  It must be one of UPDATE or KEEP_ALIVE.
  * UPDATE: Incremental update to previous state.
  *         The sequence number must be 1 higher than for
  *         the previous update.
  * KEEP_ALIVE: Indicates the that the server is still living.
  *             The sequence number does not increase.
  *             No payload data should be filled out (markers, poses, or erases).
 */
enum
{
  visualization_msgs__msg__InteractiveMarkerUpdate__KEEP_ALIVE = 0
};

/// Constant 'UPDATE'.
enum
{
  visualization_msgs__msg__InteractiveMarkerUpdate__UPDATE = 1
};

// Include directives for member types
// Member 'server_id'
// Member 'erases'
#include "rosidl_runtime_c/string.h"
// Member 'markers'
#include "visualization_msgs/msg/detail/interactive_marker__struct.h"
// Member 'poses'
#include "visualization_msgs/msg/detail/interactive_marker_pose__struct.h"

/// Struct defined in msg/InteractiveMarkerUpdate in the package visualization_msgs.
typedef struct visualization_msgs__msg__InteractiveMarkerUpdate
{
  /// Identifying string. Must be unique in the topic namespace
  /// that this server works on.
  rosidl_runtime_c__String server_id;
  /// Sequence number.
  /// The client will use this to detect if it has missed an update.
  uint64_t seq_num;
  uint8_t type;
  /// Note: No guarantees on the order of processing.
  ///       Contents must be kept consistent by sender.
  /// Markers to be added or updated
  visualization_msgs__msg__InteractiveMarker__Sequence markers;
  /// Poses of markers that should be moved
  visualization_msgs__msg__InteractiveMarkerPose__Sequence poses;
  /// Names of markers to be erased
  rosidl_runtime_c__String__Sequence erases;
} visualization_msgs__msg__InteractiveMarkerUpdate;

// Struct for a sequence of visualization_msgs__msg__InteractiveMarkerUpdate.
typedef struct visualization_msgs__msg__InteractiveMarkerUpdate__Sequence
{
  visualization_msgs__msg__InteractiveMarkerUpdate * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__InteractiveMarkerUpdate__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_UPDATE__STRUCT_H_
