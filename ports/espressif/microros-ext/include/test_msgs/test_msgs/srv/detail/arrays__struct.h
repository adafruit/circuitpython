// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:srv/Arrays.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/srv/arrays.h"


#ifndef TEST_MSGS__SRV__DETAIL__ARRAYS__STRUCT_H_
#define TEST_MSGS__SRV__DETAIL__ARRAYS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'string_values'
// Member 'string_values_default'
#include "rosidl_runtime_c/string.h"
// Member 'basic_types_values'
#include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'constants_values'
#include "test_msgs/msg/detail/constants__struct.h"
// Member 'defaults_values'
#include "test_msgs/msg/detail/defaults__struct.h"

/// Struct defined in srv/Arrays in the package test_msgs.
typedef struct test_msgs__srv__Arrays_Request
{
  bool bool_values[3];
  uint8_t byte_values[3];
  uint8_t char_values[3];
  float float32_values[3];
  double float64_values[3];
  int8_t int8_values[3];
  uint8_t uint8_values[3];
  int16_t int16_values[3];
  uint16_t uint16_values[3];
  int32_t int32_values[3];
  uint32_t uint32_values[3];
  int64_t int64_values[3];
  uint64_t uint64_values[3];
  rosidl_runtime_c__String string_values[3];
  test_msgs__msg__BasicTypes basic_types_values[3];
  test_msgs__msg__Constants constants_values[3];
  test_msgs__msg__Defaults defaults_values[3];
  bool bool_values_default[3];
  uint8_t byte_values_default[3];
  uint8_t char_values_default[3];
  float float32_values_default[3];
  double float64_values_default[3];
  int8_t int8_values_default[3];
  uint8_t uint8_values_default[3];
  int16_t int16_values_default[3];
  uint16_t uint16_values_default[3];
  int32_t int32_values_default[3];
  uint32_t uint32_values_default[3];
  int64_t int64_values_default[3];
  uint64_t uint64_values_default[3];
  rosidl_runtime_c__String string_values_default[3];
} test_msgs__srv__Arrays_Request;

// Struct for a sequence of test_msgs__srv__Arrays_Request.
typedef struct test_msgs__srv__Arrays_Request__Sequence
{
  test_msgs__srv__Arrays_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__Arrays_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'string_values'
// Member 'string_values_default'
// already included above
// #include "rosidl_runtime_c/string.h"
// Member 'basic_types_values'
// already included above
// #include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'constants_values'
// already included above
// #include "test_msgs/msg/detail/constants__struct.h"
// Member 'defaults_values'
// already included above
// #include "test_msgs/msg/detail/defaults__struct.h"

/// Struct defined in srv/Arrays in the package test_msgs.
typedef struct test_msgs__srv__Arrays_Response
{
  bool bool_values[3];
  uint8_t byte_values[3];
  uint8_t char_values[3];
  float float32_values[3];
  double float64_values[3];
  int8_t int8_values[3];
  uint8_t uint8_values[3];
  int16_t int16_values[3];
  uint16_t uint16_values[3];
  int32_t int32_values[3];
  uint32_t uint32_values[3];
  int64_t int64_values[3];
  uint64_t uint64_values[3];
  rosidl_runtime_c__String string_values[3];
  test_msgs__msg__BasicTypes basic_types_values[3];
  test_msgs__msg__Constants constants_values[3];
  test_msgs__msg__Defaults defaults_values[3];
  bool bool_values_default[3];
  uint8_t byte_values_default[3];
  uint8_t char_values_default[3];
  float float32_values_default[3];
  double float64_values_default[3];
  int8_t int8_values_default[3];
  uint8_t uint8_values_default[3];
  int16_t int16_values_default[3];
  uint16_t uint16_values_default[3];
  int32_t int32_values_default[3];
  uint32_t uint32_values_default[3];
  int64_t int64_values_default[3];
  uint64_t uint64_values_default[3];
  rosidl_runtime_c__String string_values_default[3];
} test_msgs__srv__Arrays_Response;

// Struct for a sequence of test_msgs__srv__Arrays_Response.
typedef struct test_msgs__srv__Arrays_Response__Sequence
{
  test_msgs__srv__Arrays_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__Arrays_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  test_msgs__srv__Arrays_Event__request__MAX_SIZE = 1
};
// response
enum
{
  test_msgs__srv__Arrays_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/Arrays in the package test_msgs.
typedef struct test_msgs__srv__Arrays_Event
{
  service_msgs__msg__ServiceEventInfo info;
  test_msgs__srv__Arrays_Request__Sequence request;
  test_msgs__srv__Arrays_Response__Sequence response;
} test_msgs__srv__Arrays_Event;

// Struct for a sequence of test_msgs__srv__Arrays_Event.
typedef struct test_msgs__srv__Arrays_Event__Sequence
{
  test_msgs__srv__Arrays_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__srv__Arrays_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__SRV__DETAIL__ARRAYS__STRUCT_H_
