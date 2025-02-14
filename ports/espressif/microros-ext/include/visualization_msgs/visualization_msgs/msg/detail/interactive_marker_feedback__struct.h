// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/InteractiveMarkerFeedback.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/interactive_marker_feedback.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_FEEDBACK__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_FEEDBACK__STRUCT_H_

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
  * Type of the event
  * KEEP_ALIVE: sent while dragging to keep up control of the marker
  * MENU_SELECT: a menu entry has been selected
  * BUTTON_CLICK: a button control has been clicked
  * POSE_UPDATE: the pose has been changed using one of the controls
 */
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__KEEP_ALIVE = 0
};

/// Constant 'POSE_UPDATE'.
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__POSE_UPDATE = 1
};

/// Constant 'MENU_SELECT'.
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__MENU_SELECT = 2
};

/// Constant 'BUTTON_CLICK'.
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__BUTTON_CLICK = 3
};

/// Constant 'MOUSE_DOWN'.
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__MOUSE_DOWN = 4
};

/// Constant 'MOUSE_UP'.
enum
{
  visualization_msgs__msg__InteractiveMarkerFeedback__MOUSE_UP = 5
};

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'client_id'
// Member 'marker_name'
// Member 'control_name'
#include "rosidl_runtime_c/string.h"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.h"
// Member 'mouse_point'
#include "geometry_msgs/msg/detail/point__struct.h"

/// Struct defined in msg/InteractiveMarkerFeedback in the package visualization_msgs.
/**
  * Time/frame info.
 */
typedef struct visualization_msgs__msg__InteractiveMarkerFeedback
{
  std_msgs__msg__Header header;
  /// Identifying string. Must be unique in the topic namespace.
  rosidl_runtime_c__String client_id;
  /// Feedback message sent back from the GUI, e.g.
  /// when the status of an interactive marker was modified by the user.
  /// Specifies which interactive marker and control this message refers to
  rosidl_runtime_c__String marker_name;
  rosidl_runtime_c__String control_name;
  uint8_t event_type;
  /// Current pose of the marker
  /// Note: Has to be valid for all feedback types.
  geometry_msgs__msg__Pose pose;
  /// Contains the ID of the selected menu entry
  /// Only valid for MENU_SELECT events.
  uint32_t menu_entry_id;
  /// If event_type is BUTTON_CLICK, MOUSE_DOWN, or MOUSE_UP, mouse_point
  /// may contain the 3 dimensional position of the event on the
  /// control.  If it does, mouse_point_valid will be true.  mouse_point
  /// will be relative to the frame listed in the header.
  geometry_msgs__msg__Point mouse_point;
  bool mouse_point_valid;
} visualization_msgs__msg__InteractiveMarkerFeedback;

// Struct for a sequence of visualization_msgs__msg__InteractiveMarkerFeedback.
typedef struct visualization_msgs__msg__InteractiveMarkerFeedback__Sequence
{
  visualization_msgs__msg__InteractiveMarkerFeedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__InteractiveMarkerFeedback__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_FEEDBACK__STRUCT_H_
