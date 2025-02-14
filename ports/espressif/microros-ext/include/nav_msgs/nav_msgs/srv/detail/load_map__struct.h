// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from nav_msgs:srv/LoadMap.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "nav_msgs/srv/load_map.h"


#ifndef NAV_MSGS__SRV__DETAIL__LOAD_MAP__STRUCT_H_
#define NAV_MSGS__SRV__DETAIL__LOAD_MAP__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'map_url'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/LoadMap in the package nav_msgs.
typedef struct nav_msgs__srv__LoadMap_Request
{
  rosidl_runtime_c__String map_url;
} nav_msgs__srv__LoadMap_Request;

// Struct for a sequence of nav_msgs__srv__LoadMap_Request.
typedef struct nav_msgs__srv__LoadMap_Request__Sequence
{
  nav_msgs__srv__LoadMap_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__LoadMap_Request__Sequence;

// Constants defined in the message

/// Constant 'RESULT_SUCCESS'.
enum
{
  nav_msgs__srv__LoadMap_Response__RESULT_SUCCESS = 0
};

/// Constant 'RESULT_MAP_DOES_NOT_EXIST'.
enum
{
  nav_msgs__srv__LoadMap_Response__RESULT_MAP_DOES_NOT_EXIST = 1
};

/// Constant 'RESULT_INVALID_MAP_DATA'.
enum
{
  nav_msgs__srv__LoadMap_Response__RESULT_INVALID_MAP_DATA = 2
};

/// Constant 'RESULT_INVALID_MAP_METADATA'.
enum
{
  nav_msgs__srv__LoadMap_Response__RESULT_INVALID_MAP_METADATA = 3
};

/// Constant 'RESULT_UNDEFINED_FAILURE'.
enum
{
  nav_msgs__srv__LoadMap_Response__RESULT_UNDEFINED_FAILURE = 255
};

// Include directives for member types
// Member 'map'
#include "nav_msgs/msg/detail/occupancy_grid__struct.h"

/// Struct defined in srv/LoadMap in the package nav_msgs.
typedef struct nav_msgs__srv__LoadMap_Response
{
  /// Returned map is only valid if result equals RESULT_SUCCESS
  nav_msgs__msg__OccupancyGrid map;
  uint8_t result;
} nav_msgs__srv__LoadMap_Response;

// Struct for a sequence of nav_msgs__srv__LoadMap_Response.
typedef struct nav_msgs__srv__LoadMap_Response__Sequence
{
  nav_msgs__srv__LoadMap_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__LoadMap_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  nav_msgs__srv__LoadMap_Event__request__MAX_SIZE = 1
};
// response
enum
{
  nav_msgs__srv__LoadMap_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/LoadMap in the package nav_msgs.
typedef struct nav_msgs__srv__LoadMap_Event
{
  service_msgs__msg__ServiceEventInfo info;
  nav_msgs__srv__LoadMap_Request__Sequence request;
  nav_msgs__srv__LoadMap_Response__Sequence response;
} nav_msgs__srv__LoadMap_Event;

// Struct for a sequence of nav_msgs__srv__LoadMap_Event.
typedef struct nav_msgs__srv__LoadMap_Event__Sequence
{
  nav_msgs__srv__LoadMap_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__LoadMap_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // NAV_MSGS__SRV__DETAIL__LOAD_MAP__STRUCT_H_
