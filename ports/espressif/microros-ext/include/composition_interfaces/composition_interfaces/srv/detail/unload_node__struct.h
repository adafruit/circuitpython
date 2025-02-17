// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from composition_interfaces:srv/UnloadNode.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "composition_interfaces/srv/unload_node.h"


#ifndef COMPOSITION_INTERFACES__SRV__DETAIL__UNLOAD_NODE__STRUCT_H_
#define COMPOSITION_INTERFACES__SRV__DETAIL__UNLOAD_NODE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/UnloadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__UnloadNode_Request
{
  uint64_t unique_id;
} composition_interfaces__srv__UnloadNode_Request;

// Struct for a sequence of composition_interfaces__srv__UnloadNode_Request.
typedef struct composition_interfaces__srv__UnloadNode_Request__Sequence
{
  composition_interfaces__srv__UnloadNode_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__UnloadNode_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'error_message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/UnloadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__UnloadNode_Response
{
  bool success;
  /// Human readable error message if success is false, else empty string.
  rosidl_runtime_c__String error_message;
} composition_interfaces__srv__UnloadNode_Response;

// Struct for a sequence of composition_interfaces__srv__UnloadNode_Response.
typedef struct composition_interfaces__srv__UnloadNode_Response__Sequence
{
  composition_interfaces__srv__UnloadNode_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__UnloadNode_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  composition_interfaces__srv__UnloadNode_Event__request__MAX_SIZE = 1
};
// response
enum
{
  composition_interfaces__srv__UnloadNode_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/UnloadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__UnloadNode_Event
{
  service_msgs__msg__ServiceEventInfo info;
  composition_interfaces__srv__UnloadNode_Request__Sequence request;
  composition_interfaces__srv__UnloadNode_Response__Sequence response;
} composition_interfaces__srv__UnloadNode_Event;

// Struct for a sequence of composition_interfaces__srv__UnloadNode_Event.
typedef struct composition_interfaces__srv__UnloadNode_Event__Sequence
{
  composition_interfaces__srv__UnloadNode_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__UnloadNode_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // COMPOSITION_INTERFACES__SRV__DETAIL__UNLOAD_NODE__STRUCT_H_
