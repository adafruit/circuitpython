// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from nav_msgs:srv/GetPlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "nav_msgs/srv/get_plan.h"


#ifndef NAV_MSGS__SRV__DETAIL__GET_PLAN__STRUCT_H_
#define NAV_MSGS__SRV__DETAIL__GET_PLAN__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'start'
// Member 'goal'
#include "geometry_msgs/msg/detail/pose_stamped__struct.h"

/// Struct defined in srv/GetPlan in the package nav_msgs.
typedef struct nav_msgs__srv__GetPlan_Request
{
  /// The start pose for the plan
  geometry_msgs__msg__PoseStamped start;
  /// The final pose of the goal position
  geometry_msgs__msg__PoseStamped goal;
  /// If the goal is obstructed, how many meters the planner can
  /// relax the constraint in x and y before failing.
  float tolerance;
} nav_msgs__srv__GetPlan_Request;

// Struct for a sequence of nav_msgs__srv__GetPlan_Request.
typedef struct nav_msgs__srv__GetPlan_Request__Sequence
{
  nav_msgs__srv__GetPlan_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetPlan_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'plan'
#include "nav_msgs/msg/detail/path__struct.h"

/// Struct defined in srv/GetPlan in the package nav_msgs.
typedef struct nav_msgs__srv__GetPlan_Response
{
  nav_msgs__msg__Path plan;
} nav_msgs__srv__GetPlan_Response;

// Struct for a sequence of nav_msgs__srv__GetPlan_Response.
typedef struct nav_msgs__srv__GetPlan_Response__Sequence
{
  nav_msgs__srv__GetPlan_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetPlan_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  nav_msgs__srv__GetPlan_Event__request__MAX_SIZE = 1
};
// response
enum
{
  nav_msgs__srv__GetPlan_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/GetPlan in the package nav_msgs.
typedef struct nav_msgs__srv__GetPlan_Event
{
  service_msgs__msg__ServiceEventInfo info;
  nav_msgs__srv__GetPlan_Request__Sequence request;
  nav_msgs__srv__GetPlan_Response__Sequence response;
} nav_msgs__srv__GetPlan_Event;

// Struct for a sequence of nav_msgs__srv__GetPlan_Event.
typedef struct nav_msgs__srv__GetPlan_Event__Sequence
{
  nav_msgs__srv__GetPlan_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} nav_msgs__srv__GetPlan_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // NAV_MSGS__SRV__DETAIL__GET_PLAN__STRUCT_H_
