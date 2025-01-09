// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from composition_interfaces:srv/LoadNode.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "composition_interfaces/srv/load_node.h"


#ifndef COMPOSITION_INTERFACES__SRV__DETAIL__LOAD_NODE__STRUCT_H_
#define COMPOSITION_INTERFACES__SRV__DETAIL__LOAD_NODE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'package_name'
// Member 'plugin_name'
// Member 'node_name'
// Member 'node_namespace'
// Member 'remap_rules'
#include "rosidl_runtime_c/string.h"
// Member 'parameters'
// Member 'extra_arguments'
#include "rcl_interfaces/msg/detail/parameter__struct.h"

/// Struct defined in srv/LoadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__LoadNode_Request
{
  rosidl_runtime_c__String package_name;
  /// A plugin within the ROS package "package_name".
  rosidl_runtime_c__String plugin_name;
  /// The assigned name of the composable node. Leave empty to use the node's
  /// default name.
  rosidl_runtime_c__String node_name;
  /// The assigned namespace of the composable node. Leave empty to use the node's
  /// default namespace.
  rosidl_runtime_c__String node_namespace;
  /// The assigned log level of the composable node. Enum values are found in
  /// message rcl_interfaces/Log.
  uint8_t log_level;
  /// Remapping rules for this composable node.
  ///
  /// For more info about static_remapping rules and their syntax, see
  /// https://design.ros2.org/articles/static_remapping.html
  /// TODO(sloretz) rcl_interfaces message for remap rules?
  rosidl_runtime_c__String__Sequence remap_rules;
  /// The Parameters of this composable node to set.
  rcl_interfaces__msg__Parameter__Sequence parameters;
  /// key/value arguments that are specific to a type of container process.
  rcl_interfaces__msg__Parameter__Sequence extra_arguments;
} composition_interfaces__srv__LoadNode_Request;

// Struct for a sequence of composition_interfaces__srv__LoadNode_Request.
typedef struct composition_interfaces__srv__LoadNode_Request__Sequence
{
  composition_interfaces__srv__LoadNode_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__LoadNode_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'error_message'
// Member 'full_node_name'
// already included above
// #include "rosidl_runtime_c/string.h"

/// Struct defined in srv/LoadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__LoadNode_Response
{
  bool success;
  /// Human readable error message if success is false, else empty string.
  rosidl_runtime_c__String error_message;
  /// Name of the loaded composable node (including namespace).
  rosidl_runtime_c__String full_node_name;
  /// A unique identifier for the loaded node.
  uint64_t unique_id;
} composition_interfaces__srv__LoadNode_Response;

// Struct for a sequence of composition_interfaces__srv__LoadNode_Response.
typedef struct composition_interfaces__srv__LoadNode_Response__Sequence
{
  composition_interfaces__srv__LoadNode_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__LoadNode_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  composition_interfaces__srv__LoadNode_Event__request__MAX_SIZE = 1
};
// response
enum
{
  composition_interfaces__srv__LoadNode_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/LoadNode in the package composition_interfaces.
typedef struct composition_interfaces__srv__LoadNode_Event
{
  service_msgs__msg__ServiceEventInfo info;
  composition_interfaces__srv__LoadNode_Request__Sequence request;
  composition_interfaces__srv__LoadNode_Response__Sequence response;
} composition_interfaces__srv__LoadNode_Event;

// Struct for a sequence of composition_interfaces__srv__LoadNode_Event.
typedef struct composition_interfaces__srv__LoadNode_Event__Sequence
{
  composition_interfaces__srv__LoadNode_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} composition_interfaces__srv__LoadNode_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // COMPOSITION_INTERFACES__SRV__DETAIL__LOAD_NODE__STRUCT_H_
