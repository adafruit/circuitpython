// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from diagnostic_msgs:srv/SelfTest.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "diagnostic_msgs/srv/self_test.h"


#ifndef DIAGNOSTIC_MSGS__SRV__DETAIL__SELF_TEST__STRUCT_H_
#define DIAGNOSTIC_MSGS__SRV__DETAIL__SELF_TEST__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/SelfTest in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__SelfTest_Request
{
  uint8_t structure_needs_at_least_one_member;
} diagnostic_msgs__srv__SelfTest_Request;

// Struct for a sequence of diagnostic_msgs__srv__SelfTest_Request.
typedef struct diagnostic_msgs__srv__SelfTest_Request__Sequence
{
  diagnostic_msgs__srv__SelfTest_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__SelfTest_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'id'
#include "rosidl_runtime_c/string.h"
// Member 'status'
#include "diagnostic_msgs/msg/detail/diagnostic_status__struct.h"

/// Struct defined in srv/SelfTest in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__SelfTest_Response
{
  rosidl_runtime_c__String id;
  uint8_t passed;
  diagnostic_msgs__msg__DiagnosticStatus__Sequence status;
} diagnostic_msgs__srv__SelfTest_Response;

// Struct for a sequence of diagnostic_msgs__srv__SelfTest_Response.
typedef struct diagnostic_msgs__srv__SelfTest_Response__Sequence
{
  diagnostic_msgs__srv__SelfTest_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__SelfTest_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  diagnostic_msgs__srv__SelfTest_Event__request__MAX_SIZE = 1
};
// response
enum
{
  diagnostic_msgs__srv__SelfTest_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/SelfTest in the package diagnostic_msgs.
typedef struct diagnostic_msgs__srv__SelfTest_Event
{
  service_msgs__msg__ServiceEventInfo info;
  diagnostic_msgs__srv__SelfTest_Request__Sequence request;
  diagnostic_msgs__srv__SelfTest_Response__Sequence response;
} diagnostic_msgs__srv__SelfTest_Event;

// Struct for a sequence of diagnostic_msgs__srv__SelfTest_Event.
typedef struct diagnostic_msgs__srv__SelfTest_Event__Sequence
{
  diagnostic_msgs__srv__SelfTest_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diagnostic_msgs__srv__SelfTest_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DIAGNOSTIC_MSGS__SRV__DETAIL__SELF_TEST__STRUCT_H_
