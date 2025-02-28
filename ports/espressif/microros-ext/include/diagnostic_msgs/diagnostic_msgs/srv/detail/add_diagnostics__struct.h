// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from diagnostic_msgs:srv/AddDiagnostics.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "diagnostic_msgs/srv/add_diagnostics.h"


#ifndef DIAGNOSTIC_MSGS__SRV__DETAIL__ADD_DIAGNOSTICS__STRUCT_H_
#define DIAGNOSTIC_MSGS__SRV__DETAIL__ADD_DIAGNOSTICS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'load_namespace'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/AddDiagnostics in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Request
{
  /// The load_namespace parameter defines the namespace where parameters for the
  /// initialization of analyzers in the diagnostic aggregator have been loaded. The
  /// value should be a global name (i.e. /my/name/space), not a relative
  /// (my/name/space) or private (~my/name/space) name. Analyzers will not be added
  /// if a non-global name is used. The call will also fail if the namespace
  /// contains parameters that follow a namespace structure that does not conform to
  /// that expected by the analyzer definitions. See
  /// http://wiki.ros.org/diagnostics/Tutorials/Configuring%20Diagnostic%20Aggregators
  /// and http://wiki.ros.org/diagnostics/Tutorials/Using%20the%20GenericAnalyzer
  /// for examples of the structure of yaml files which are expected to have been
  /// loaded into the namespace.
  rosidl_runtime_c__String load_namespace;
} diagnostic_msgs__srv__AddDiagnostics_Request;

// Struct for a sequence of diagnostic_msgs__srv__AddDiagnostics_Request.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Request__Sequence
{
  diagnostic_msgs__srv__AddDiagnostics_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__AddDiagnostics_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'message'
// already included above
// #include "rosidl_runtime_c/string.h"

/// Struct defined in srv/AddDiagnostics in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Response
{
  /// True if diagnostic aggregator was updated with new diagnostics, False
  /// otherwise. A false return value means that either there is a bond in the
  /// aggregator which already used the requested namespace, or the initialization
  /// of analyzers failed.
  bool success;
  /// Message with additional information about the success or failure
  rosidl_runtime_c__String message;
} diagnostic_msgs__srv__AddDiagnostics_Response;

// Struct for a sequence of diagnostic_msgs__srv__AddDiagnostics_Response.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Response__Sequence
{
  diagnostic_msgs__srv__AddDiagnostics_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__AddDiagnostics_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  diagnostic_msgs__srv__AddDiagnostics_Event__request__MAX_SIZE = 1
};
// response
enum
{
  diagnostic_msgs__srv__AddDiagnostics_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/AddDiagnostics in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Event
{
  service_msgs__msg__ServiceEventInfo info;
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence request;
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence response;
} diagnostic_msgs__srv__AddDiagnostics_Event;

// Struct for a sequence of diagnostic_msgs__srv__AddDiagnostics_Event.
typedef struct diagnostic_msgs__srv__AddDiagnostics_Event__Sequence
{
  diagnostic_msgs__srv__AddDiagnostics_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__AddDiagnostics_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DIAGNOSTIC_MSGS__SRV__DETAIL__ADD_DIAGNOSTICS__STRUCT_H_
