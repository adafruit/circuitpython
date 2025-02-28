// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from visualization_msgs:srv/GetInteractiveMarkers.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/srv/get_interactive_markers.h"


#ifndef VISUALIZATION_MSGS__SRV__DETAIL__GET_INTERACTIVE_MARKERS__STRUCT_H_
#define VISUALIZATION_MSGS__SRV__DETAIL__GET_INTERACTIVE_MARKERS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/GetInteractiveMarkers in the package visualization_msgs.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Request
{
  uint8_t structure_needs_at_least_one_member;
} visualization_msgs__srv__GetInteractiveMarkers_Request;

// Struct for a sequence of visualization_msgs__srv__GetInteractiveMarkers_Request.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence
{
  visualization_msgs__srv__GetInteractiveMarkers_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'markers'
#include "visualization_msgs/msg/detail/interactive_marker__struct.h"

/// Struct defined in srv/GetInteractiveMarkers in the package visualization_msgs.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Response
{
  uint64_t sequence_number;
  /// All interactive markers provided by the server.
  visualization_msgs__msg__InteractiveMarker__Sequence markers;
} visualization_msgs__srv__GetInteractiveMarkers_Response;

// Struct for a sequence of visualization_msgs__srv__GetInteractiveMarkers_Response.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence
{
  visualization_msgs__srv__GetInteractiveMarkers_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  visualization_msgs__srv__GetInteractiveMarkers_Event__request__MAX_SIZE = 1
};
// response
enum
{
  visualization_msgs__srv__GetInteractiveMarkers_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/GetInteractiveMarkers in the package visualization_msgs.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Event
{
  service_msgs__msg__ServiceEventInfo info;
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence request;
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence response;
} visualization_msgs__srv__GetInteractiveMarkers_Event;

// Struct for a sequence of visualization_msgs__srv__GetInteractiveMarkers_Event.
typedef struct visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence
{
  visualization_msgs__srv__GetInteractiveMarkers_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__SRV__DETAIL__GET_INTERACTIVE_MARKERS__STRUCT_H_
