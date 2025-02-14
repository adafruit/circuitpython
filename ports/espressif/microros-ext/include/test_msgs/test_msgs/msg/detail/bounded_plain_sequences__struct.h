// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:msg/BoundedPlainSequences.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/msg/bounded_plain_sequences.h"


#ifndef TEST_MSGS__MSG__DETAIL__BOUNDED_PLAIN_SEQUENCES__STRUCT_H_
#define TEST_MSGS__MSG__DETAIL__BOUNDED_PLAIN_SEQUENCES__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'bool_values'
// Member 'byte_values'
// Member 'char_values'
// Member 'float32_values'
// Member 'float64_values'
// Member 'int8_values'
// Member 'uint8_values'
// Member 'int16_values'
// Member 'uint16_values'
// Member 'int32_values'
// Member 'uint32_values'
// Member 'int64_values'
// Member 'uint64_values'
// Member 'bool_values_default'
// Member 'byte_values_default'
// Member 'char_values_default'
// Member 'float32_values_default'
// Member 'float64_values_default'
// Member 'int8_values_default'
// Member 'uint8_values_default'
// Member 'int16_values_default'
// Member 'uint16_values_default'
// Member 'int32_values_default'
// Member 'uint32_values_default'
// Member 'int64_values_default'
// Member 'uint64_values_default'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'basic_types_values'
#include "test_msgs/msg/detail/basic_types__struct.h"
// Member 'constants_values'
#include "test_msgs/msg/detail/constants__struct.h"
// Member 'defaults_values'
#include "test_msgs/msg/detail/defaults__struct.h"

// constants for array fields with an upper bound
// bool_values
enum
{
  test_msgs__msg__BoundedPlainSequences__bool_values__MAX_SIZE = 3
};
// byte_values
enum
{
  test_msgs__msg__BoundedPlainSequences__byte_values__MAX_SIZE = 3
};
// char_values
enum
{
  test_msgs__msg__BoundedPlainSequences__char_values__MAX_SIZE = 3
};
// float32_values
enum
{
  test_msgs__msg__BoundedPlainSequences__float32_values__MAX_SIZE = 3
};
// float64_values
enum
{
  test_msgs__msg__BoundedPlainSequences__float64_values__MAX_SIZE = 3
};
// int8_values
enum
{
  test_msgs__msg__BoundedPlainSequences__int8_values__MAX_SIZE = 3
};
// uint8_values
enum
{
  test_msgs__msg__BoundedPlainSequences__uint8_values__MAX_SIZE = 3
};
// int16_values
enum
{
  test_msgs__msg__BoundedPlainSequences__int16_values__MAX_SIZE = 3
};
// uint16_values
enum
{
  test_msgs__msg__BoundedPlainSequences__uint16_values__MAX_SIZE = 3
};
// int32_values
enum
{
  test_msgs__msg__BoundedPlainSequences__int32_values__MAX_SIZE = 3
};
// uint32_values
enum
{
  test_msgs__msg__BoundedPlainSequences__uint32_values__MAX_SIZE = 3
};
// int64_values
enum
{
  test_msgs__msg__BoundedPlainSequences__int64_values__MAX_SIZE = 3
};
// uint64_values
enum
{
  test_msgs__msg__BoundedPlainSequences__uint64_values__MAX_SIZE = 3
};
// basic_types_values
enum
{
  test_msgs__msg__BoundedPlainSequences__basic_types_values__MAX_SIZE = 3
};
// constants_values
enum
{
  test_msgs__msg__BoundedPlainSequences__constants_values__MAX_SIZE = 3
};
// defaults_values
enum
{
  test_msgs__msg__BoundedPlainSequences__defaults_values__MAX_SIZE = 3
};
// bool_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__bool_values_default__MAX_SIZE = 3
};
// byte_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__byte_values_default__MAX_SIZE = 3
};
// char_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__char_values_default__MAX_SIZE = 3
};
// float32_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__float32_values_default__MAX_SIZE = 3
};
// float64_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__float64_values_default__MAX_SIZE = 3
};
// int8_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__int8_values_default__MAX_SIZE = 3
};
// uint8_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__uint8_values_default__MAX_SIZE = 3
};
// int16_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__int16_values_default__MAX_SIZE = 3
};
// uint16_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__uint16_values_default__MAX_SIZE = 3
};
// int32_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__int32_values_default__MAX_SIZE = 3
};
// uint32_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__uint32_values_default__MAX_SIZE = 3
};
// int64_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__int64_values_default__MAX_SIZE = 3
};
// uint64_values_default
enum
{
  test_msgs__msg__BoundedPlainSequences__uint64_values_default__MAX_SIZE = 3
};

/// Struct defined in msg/BoundedPlainSequences in the package test_msgs.
/**
  * Bounded sequences of different POD types
 */
typedef struct test_msgs__msg__BoundedPlainSequences
{
  rosidl_runtime_c__boolean__Sequence bool_values;
  rosidl_runtime_c__octet__Sequence byte_values;
  rosidl_runtime_c__uint8__Sequence char_values;
  rosidl_runtime_c__float__Sequence float32_values;
  rosidl_runtime_c__double__Sequence float64_values;
  rosidl_runtime_c__int8__Sequence int8_values;
  rosidl_runtime_c__uint8__Sequence uint8_values;
  rosidl_runtime_c__int16__Sequence int16_values;
  rosidl_runtime_c__uint16__Sequence uint16_values;
  rosidl_runtime_c__int32__Sequence int32_values;
  rosidl_runtime_c__uint32__Sequence uint32_values;
  rosidl_runtime_c__int64__Sequence int64_values;
  rosidl_runtime_c__uint64__Sequence uint64_values;
  test_msgs__msg__BasicTypes__Sequence basic_types_values;
  test_msgs__msg__Constants__Sequence constants_values;
  test_msgs__msg__Defaults__Sequence defaults_values;
  rosidl_runtime_c__boolean__Sequence bool_values_default;
  rosidl_runtime_c__octet__Sequence byte_values_default;
  rosidl_runtime_c__uint8__Sequence char_values_default;
  rosidl_runtime_c__float__Sequence float32_values_default;
  rosidl_runtime_c__double__Sequence float64_values_default;
  rosidl_runtime_c__int8__Sequence int8_values_default;
  rosidl_runtime_c__uint8__Sequence uint8_values_default;
  rosidl_runtime_c__int16__Sequence int16_values_default;
  rosidl_runtime_c__uint16__Sequence uint16_values_default;
  rosidl_runtime_c__int32__Sequence int32_values_default;
  rosidl_runtime_c__uint32__Sequence uint32_values_default;
  rosidl_runtime_c__int64__Sequence int64_values_default;
  rosidl_runtime_c__uint64__Sequence uint64_values_default;
  /// Regression test: check alignment of basic field after a sequence field is correct
  int32_t alignment_check;
} test_msgs__msg__BoundedPlainSequences;

// Struct for a sequence of test_msgs__msg__BoundedPlainSequences.
typedef struct test_msgs__msg__BoundedPlainSequences__Sequence
{
  test_msgs__msg__BoundedPlainSequences * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__msg__BoundedPlainSequences__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__MSG__DETAIL__BOUNDED_PLAIN_SEQUENCES__STRUCT_H_
