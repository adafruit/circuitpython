// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:srv/BasicTypes.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/srv/basic_types.h"


#ifndef TEST_MSGS__SRV__DETAIL__BASIC_TYPES__STRUCT_H_
#define TEST_MSGS__SRV__DETAIL__BASIC_TYPES__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'string_value'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/BasicTypes in the package test_msgs.
typedef struct test_msgs__srv__BasicTypes_Request
{
  bool bool_value;
  uint8_t byte_value;
  uint8_t char_value;
  float float32_value;
  double float64_value;
  int8_t int8_value;
  uint8_t uint8_value;
  int16_t int16_value;
  uint16_t uint16_value;
  int32_t int32_value;
  uint32_t uint32_value;
  int64_t int64_value;
  uint64_t uint64_value;
  rosidl_runtime_c__String string_value;
} test_msgs__srv__BasicTypes_Request;

// Struct for a sequence of test_msgs__srv__BasicTypes_Request.
typedef struct test_msgs__srv__BasicTypes_Request__Sequence
{
  test_msgs__srv__BasicTypes_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__BasicTypes_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'string_value'
// already included above
// #include "rosidl_runtime_c/string.h"

/// Struct defined in srv/BasicTypes in the package test_msgs.
typedef struct test_msgs__srv__BasicTypes_Response
{
  bool bool_value;
  uint8_t byte_value;
  uint8_t char_value;
  float float32_value;
  double float64_value;
  int8_t int8_value;
  uint8_t uint8_value;
  int16_t int16_value;
  uint16_t uint16_value;
  int32_t int32_value;
  uint32_t uint32_value;
  int64_t int64_value;
  uint64_t uint64_value;
  rosidl_runtime_c__String string_value;
} test_msgs__srv__BasicTypes_Response;

// Struct for a sequence of test_msgs__srv__BasicTypes_Response.
typedef struct test_msgs__srv__BasicTypes_Response__Sequence
{
  test_msgs__srv__BasicTypes_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__BasicTypes_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  test_msgs__srv__BasicTypes_Event__request__MAX_SIZE = 1
};
// response
enum
{
  test_msgs__srv__BasicTypes_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/BasicTypes in the package test_msgs.
typedef struct test_msgs__srv__BasicTypes_Event
{
  service_msgs__msg__ServiceEventInfo info;
  test_msgs__srv__BasicTypes_Request__Sequence request;
  test_msgs__srv__BasicTypes_Response__Sequence response;
} test_msgs__srv__BasicTypes_Event;

// Struct for a sequence of test_msgs__srv__BasicTypes_Event.
typedef struct test_msgs__srv__BasicTypes_Event__Sequence
{
  test_msgs__srv__BasicTypes_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__BasicTypes_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__SRV__DETAIL__BASIC_TYPES__STRUCT_H_
