// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from std_srvs:srv/Empty.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "std_srvs/srv/empty.h"


#ifndef STD_SRVS__SRV__DETAIL__EMPTY__STRUCT_H_
#define STD_SRVS__SRV__DETAIL__EMPTY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/Empty in the package std_srvs.
typedef struct std_srvs__srv__Empty_Request
{
  uint8_t structure_needs_at_least_one_member;
} std_srvs__srv__Empty_Request;

// Struct for a sequence of std_srvs__srv__Empty_Request.
typedef struct std_srvs__srv__Empty_Request__Sequence
{
  std_srvs__srv__Empty_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} std_srvs__srv__Empty_Request__Sequence;

// Constants defined in the message

/// Struct defined in srv/Empty in the package std_srvs.
typedef struct std_srvs__srv__Empty_Response
{
  uint8_t structure_needs_at_least_one_member;
} std_srvs__srv__Empty_Response;

// Struct for a sequence of std_srvs__srv__Empty_Response.
typedef struct std_srvs__srv__Empty_Response__Sequence
{
  std_srvs__srv__Empty_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} std_srvs__srv__Empty_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  std_srvs__srv__Empty_Event__request__MAX_SIZE = 1
};
// response
enum
{
  std_srvs__srv__Empty_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/Empty in the package std_srvs.
typedef struct std_srvs__srv__Empty_Event
{
  service_msgs__msg__ServiceEventInfo info;
  std_srvs__srv__Empty_Request__Sequence request;
  std_srvs__srv__Empty_Response__Sequence response;
} std_srvs__srv__Empty_Event;

// Struct for a sequence of std_srvs__srv__Empty_Event.
typedef struct std_srvs__srv__Empty_Event__Sequence
{
  std_srvs__srv__Empty_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} std_srvs__srv__Empty_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // STD_SRVS__SRV__DETAIL__EMPTY__STRUCT_H_
