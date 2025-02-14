// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from nav_msgs:srv/GetMap.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "nav_msgs/srv/get_map.h"


#ifndef NAV_MSGS__SRV__DETAIL__GET_MAP__STRUCT_H_
#define NAV_MSGS__SRV__DETAIL__GET_MAP__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/GetMap in the package nav_msgs.
typedef struct nav_msgs__srv__GetMap_Request
{
  uint8_t structure_needs_at_least_one_member;
} nav_msgs__srv__GetMap_Request;

// Struct for a sequence of nav_msgs__srv__GetMap_Request.
typedef struct nav_msgs__srv__GetMap_Request__Sequence
{
  nav_msgs__srv__GetMap_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetMap_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'map'
#include "nav_msgs/msg/detail/occupancy_grid__struct.h"

/// Struct defined in srv/GetMap in the package nav_msgs.
typedef struct nav_msgs__srv__GetMap_Response
{
  nav_msgs__msg__OccupancyGrid map;
} nav_msgs__srv__GetMap_Response;

// Struct for a sequence of nav_msgs__srv__GetMap_Response.
typedef struct nav_msgs__srv__GetMap_Response__Sequence
{
  nav_msgs__srv__GetMap_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetMap_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  nav_msgs__srv__GetMap_Event__request__MAX_SIZE = 1
};
// response
enum
{
  nav_msgs__srv__GetMap_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/GetMap in the package nav_msgs.
typedef struct nav_msgs__srv__GetMap_Event
{
  service_msgs__msg__ServiceEventInfo info;
  nav_msgs__srv__GetMap_Request__Sequence request;
  nav_msgs__srv__GetMap_Response__Sequence response;
} nav_msgs__srv__GetMap_Event;

// Struct for a sequence of nav_msgs__srv__GetMap_Event.
typedef struct nav_msgs__srv__GetMap_Event__Sequence
{
  nav_msgs__srv__GetMap_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetMap_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // NAV_MSGS__SRV__DETAIL__GET_MAP__STRUCT_H_
