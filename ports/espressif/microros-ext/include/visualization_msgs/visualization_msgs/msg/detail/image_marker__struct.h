// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:msg/ImageMarker.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/image_marker.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__IMAGE_MARKER__STRUCT_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__IMAGE_MARKER__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'CIRCLE'.
enum
{
  visualization_msgs__msg__ImageMarker__CIRCLE = 0l
};

/// Constant 'LINE_STRIP'.
enum
{
  visualization_msgs__msg__ImageMarker__LINE_STRIP = 1l
};

/// Constant 'LINE_LIST'.
enum
{
  visualization_msgs__msg__ImageMarker__LINE_LIST = 2l
};

/// Constant 'POLYGON'.
enum
{
  visualization_msgs__msg__ImageMarker__POLYGON = 3l
};

/// Constant 'POINTS'.
enum
{
  visualization_msgs__msg__ImageMarker__POINTS = 4l
};

/// Constant 'ADD'.
enum
{
  visualization_msgs__msg__ImageMarker__ADD = 0l
};

/// Constant 'REMOVE'.
enum
{
  visualization_msgs__msg__ImageMarker__REMOVE = 1l
};

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'ns'
#include "rosidl_runtime_c/string.h"
// Member 'position'
// Member 'points'
#include "geometry_msgs/msg/detail/point__struct.h"
// Member 'outline_color'
// Member 'fill_color'
// Member 'outline_colors'
#include "std_msgs/msg/detail/color_rgba__struct.h"
// Member 'lifetime'
#include "builtin_interfaces/msg/detail/duration__struct.h"

/// Struct defined in msg/ImageMarker in the package visualization_msgs.
typedef struct visualization_msgs__msg__ImageMarker
{
  std_msgs__msg__Header header;
  /// Namespace which is used with the id to form a unique id.
  rosidl_runtime_c__String ns;
  /// Unique id within the namespace.
  int32_t id;
  /// One of the above types, e.g. CIRCLE, LINE_STRIP, etc.
  int32_t type;
  /// Either ADD or REMOVE.
  int32_t action;
  /// Two-dimensional coordinate position, in pixel-coordinates.
  geometry_msgs__msg__Point position;
  /// The scale of the object, e.g. the diameter for a CIRCLE.
  float scale;
  /// The outline color of the marker.
  std_msgs__msg__ColorRGBA outline_color;
  /// Whether or not to fill in the shape with color.
  uint8_t filled;
  /// Fill color; in the range:
  std_msgs__msg__ColorRGBA fill_color;
  /// How long the object should last before being automatically deleted.
  /// 0 indicates forever.
  builtin_interfaces__msg__Duration lifetime;
  /// Coordinates in 2D in pixel coords. Used for LINE_STRIP, LINE_LIST, POINTS, etc.
  geometry_msgs__msg__Point__Sequence points;
  /// The color for each line, point, etc. in the points field.
  std_msgs__msg__ColorRGBA__Sequence outline_colors;
} visualization_msgs__msg__ImageMarker;

// Struct for a sequence of visualization_msgs__msg__ImageMarker.
typedef struct visualization_msgs__msg__ImageMarker__Sequence
{
  visualization_msgs__msg__ImageMarker * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__msg__ImageMarker__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__IMAGE_MARKER__STRUCT_H_
