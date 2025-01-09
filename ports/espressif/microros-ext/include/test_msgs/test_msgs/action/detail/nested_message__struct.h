// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:action/NestedMessage.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/action/nested_message.h"


#ifndef TEST_MSGS__ACTION__DETAIL__NESTED_MESSAGE__STRUCT_H_
#define TEST_MSGS__ACTION__DETAIL__NESTED_MESSAGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'nested_field_no_pkg'
#include "test_msgs/msg/detail/builtins__struct.h"
// Member 'nested_field'
#include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'nested_different_pkg'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_Goal
{
  test_msgs__msg__Builtins nested_field_no_pkg;
  test_msgs__msg__BasicTypes nested_field;
  builtin_interfaces__msg__Time nested_different_pkg;
} test_msgs__action__NestedMessage_Goal;

// Struct for a sequence of test_msgs__action__NestedMessage_Goal.
typedef struct test_msgs__action__NestedMessage_Goal__Sequence
{
  test_msgs__action__NestedMessage_Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_Goal__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'nested_field_no_pkg'
// already included above
// #include "test_msgs/msg/detail/builtins__struct.h"
// Member 'nested_field'
// already included above
// #include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'nested_different_pkg'
// already included above
// #include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_Result
{
  test_msgs__msg__Builtins nested_field_no_pkg;
  test_msgs__msg__BasicTypes nested_field;
  builtin_interfaces__msg__Time nested_different_pkg;
} test_msgs__action__NestedMessage_Result;

// Struct for a sequence of test_msgs__action__NestedMessage_Result.
typedef struct test_msgs__action__NestedMessage_Result__Sequence
{
  test_msgs__action__NestedMessage_Result * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_Result__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'nested_field_no_pkg'
// already included above
// #include "test_msgs/msg/detail/builtins__struct.h"
// Member 'nested_field'
// already included above
// #include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'nested_different_pkg'
// already included above
// #include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_Feedback
{
  test_msgs__msg__Builtins nested_field_no_pkg;
  test_msgs__msg__BasicTypes nested_field;
  builtin_interfaces__msg__Time nested_different_pkg;
} test_msgs__action__NestedMessage_Feedback;

// Struct for a sequence of test_msgs__action__NestedMessage_Feedback.
typedef struct test_msgs__action__NestedMessage_Feedback__Sequence
{
  test_msgs__action__NestedMessage_Feedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_Feedback__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'goal'
#include "test_msgs/action/detail/nested_message__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_SendGoal_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
  test_msgs__action__NestedMessage_Goal goal;
} test_msgs__action__NestedMessage_SendGoal_Request;

// Struct for a sequence of test_msgs__action__NestedMessage_SendGoal_Request.
typedef struct test_msgs__action__NestedMessage_SendGoal_Request__Sequence
{
  test_msgs__action__NestedMessage_SendGoal_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_SendGoal_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'stamp'
// already included above
// #include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_SendGoal_Response
{
  bool accepted;
  builtin_interfaces__msg__Time stamp;
} test_msgs__action__NestedMessage_SendGoal_Response;

// Struct for a sequence of test_msgs__action__NestedMessage_SendGoal_Response.
typedef struct test_msgs__action__NestedMessage_SendGoal_Response__Sequence
{
  test_msgs__action__NestedMessage_SendGoal_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_SendGoal_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  test_msgs__action__NestedMessage_SendGoal_Event__request__MAX_SIZE = 1
};
// response
enum
{
  test_msgs__action__NestedMessage_SendGoal_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_SendGoal_Event
{
  service_msgs__msg__ServiceEventInfo info;
  test_msgs__action__NestedMessage_SendGoal_Request__Sequence request;
  test_msgs__action__NestedMessage_SendGoal_Response__Sequence response;
} test_msgs__action__NestedMessage_SendGoal_Event;

// Struct for a sequence of test_msgs__action__NestedMessage_SendGoal_Event.
typedef struct test_msgs__action__NestedMessage_SendGoal_Event__Sequence
{
  test_msgs__action__NestedMessage_SendGoal_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_SendGoal_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_GetResult_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
} test_msgs__action__NestedMessage_GetResult_Request;

// Struct for a sequence of test_msgs__action__NestedMessage_GetResult_Request.
typedef struct test_msgs__action__NestedMessage_GetResult_Request__Sequence
{
  test_msgs__action__NestedMessage_GetResult_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_GetResult_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'result'
// already included above
// #include "test_msgs/action/detail/nested_message__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_GetResult_Response
{
  int8_t status;
  test_msgs__action__NestedMessage_Result result;
} test_msgs__action__NestedMessage_GetResult_Response;

// Struct for a sequence of test_msgs__action__NestedMessage_GetResult_Response.
typedef struct test_msgs__action__NestedMessage_GetResult_Response__Sequence
{
  test_msgs__action__NestedMessage_GetResult_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_GetResult_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  test_msgs__action__NestedMessage_GetResult_Event__request__MAX_SIZE = 1
};
// response
enum
{
  test_msgs__action__NestedMessage_GetResult_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_GetResult_Event
{
  service_msgs__msg__ServiceEventInfo info;
  test_msgs__action__NestedMessage_GetResult_Request__Sequence request;
  test_msgs__action__NestedMessage_GetResult_Response__Sequence response;
} test_msgs__action__NestedMessage_GetResult_Event;

// Struct for a sequence of test_msgs__action__NestedMessage_GetResult_Event.
typedef struct test_msgs__action__NestedMessage_GetResult_Event__Sequence
{
  test_msgs__action__NestedMessage_GetResult_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_GetResult_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'feedback'
// already included above
// #include "test_msgs/action/detail/nested_message__struct.h"

/// Struct defined in action/NestedMessage in the package test_msgs.
typedef struct test_msgs__action__NestedMessage_FeedbackMessage
{
  unique_identifier_msgs__msg__UUID goal_id;
  test_msgs__action__NestedMessage_Feedback feedback;
} test_msgs__action__NestedMessage_FeedbackMessage;

// Struct for a sequence of test_msgs__action__NestedMessage_FeedbackMessage.
typedef struct test_msgs__action__NestedMessage_FeedbackMessage__Sequence
{
  test_msgs__action__NestedMessage_FeedbackMessage * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__action__NestedMessage_FeedbackMessage__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__ACTION__DETAIL__NESTED_MESSAGE__STRUCT_H_
