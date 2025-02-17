// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/InteractiveMarkerControl.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/interactive_marker_control.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_CONTROL__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_CONTROL__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'INHERIT'.
/**
  * Orientation mode: controls how orientation changes.
  * INHERIT: Follow orientation of interactive marker
  * FIXED: Keep orientation fixed at initial state
  * VIEW_FACING: Align y-z plane with screen (x: forward, y:left, z:up).
 */
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__INHERIT = 0
};

/// Constant 'FIXED'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__FIXED = 1
};

/// Constant 'VIEW_FACING'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__VIEW_FACING = 2
};

/// Constant 'NONE'.
/**
  * Interaction mode for this control
  *
  * NONE: This control is only meant for visualization; no context menu.
  * MENU: Like NONE, but right-click menu is active.
  * BUTTON: Element can be left-clicked.
  * MOVE_AXIS: Translate along local x-axis.
  * MOVE_PLANE: Translate in local y-z plane.
  * ROTATE_AXIS: Rotate around local x-axis.
  * MOVE_ROTATE: Combines MOVE_PLANE and ROTATE_AXIS.
 */
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__NONE = 0
};

/// Constant 'MENU'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MENU = 1
};

/// Constant 'BUTTON'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__BUTTON = 2
};

/// Constant 'MOVE_AXIS'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MOVE_AXIS = 3
};

/// Constant 'MOVE_PLANE'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MOVE_PLANE = 4
};

/// Constant 'ROTATE_AXIS'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__ROTATE_AXIS = 5
};

/// Constant 'MOVE_ROTATE'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MOVE_ROTATE = 6
};

/// Constant 'MOVE_3D'.
/**
  * "3D" interaction modes work with the mouse+SHIFT+CTRL or with 3D cursors.
  * MOVE_3D: Translate freely in 3D space.
  * ROTATE_3D: Rotate freely in 3D space about the origin of parent frame.
  * MOVE_ROTATE_3D: Full 6-DOF freedom of translation and rotation about the cursor origin.
 */
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MOVE_3D = 7
};

/// Constant 'ROTATE_3D'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__ROTATE_3D = 8
};

/// Constant 'MOVE_ROTATE_3D'.
enum
{
  visualization_msgs__msg__InteractiveMarkerControl__MOVE_ROTATE_3D = 9
};

// Include directives for member types
// Member 'name'
// Member 'description'
#include "rosidl_runtime_c/string.h"
// Member 'orientation'
#include "geometry_msgs/msg/detail/quaternion__struct.h"
// Member 'markers'
#include "visualization_msgs/msg/detail/marker__struct.h"

/// Struct defined in msg/InteractiveMarkerControl in the package visualization_msgs.
/**
  * Represents a control that is to be displayed together with an interactive marker
 */
typedef struct visualization_msgs__msg__InteractiveMarkerControl
{
  /// Identifying string for this control.
  /// You need to assign a unique value to this to receive feedback from the GUI
  /// on what actions the user performs on this control (e.g. a button click).
  rosidl_runtime_c__String name;
  /// Defines the local coordinate frame (relative to the pose of the parent
  /// interactive marker) in which is being rotated and translated.
  /// Default: Identity
  geometry_msgs__msg__Quaternion orientation;
  uint8_t orientation_mode;
  uint8_t interaction_mode;
  /// If true, the contained markers will also be visible
  /// when the gui is not in interactive mode.
  bool always_visible;
  /// Markers to be displayed as custom visual representation.
  /// Leave this empty to use the default control handles.
  ///
  /// Note:
  /// - The markers can be defined in an arbitrary coordinate frame,
  ///   but will be transformed into the local frame of the interactive marker.
  /// - If the header of a marker is empty, its pose will be interpreted as
  ///   relative to the pose of the parent interactive marker.
  visualization_msgs__msg__Marker__Sequence markers;
  /// In VIEW_FACING mode, set this to true if you don't want the markers
  /// to be aligned with the camera view point. The markers will show up
  /// as in INHERIT mode.
  bool independent_marker_orientation;
  /// Short description (< 40 characters) of what this control does,
  /// e.g. "Move the robot".
  /// Default: A generic description based on the interaction mode
  rosidl_runtime_c__String description;
} visualization_msgs__msg__InteractiveMarkerControl;

// Struct for a sequence of visualization_msgs__msg__InteractiveMarkerControl.
typedef struct visualization_msgs__msg__InteractiveMarkerControl__Sequence
{
  visualization_msgs__msg__InteractiveMarkerControl * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__InteractiveMarkerControl__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__INTERACTIVE_MARKER_CONTROL__STRUCT_H_
