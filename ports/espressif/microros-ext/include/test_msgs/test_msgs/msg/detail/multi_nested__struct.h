// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from test_msgs:msg/MultiNested.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/msg/multi_nested.h"


#ifndef TEST_MSGS__MSG__DETAIL__MULTI_NESTED__STRUCT_H_
#define TEST_MSGS__MSG__DETAIL__MULTI_NESTED__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'array_of_arrays'
// Member 'bounded_sequence_of_arrays'
// Member 'unbounded_sequence_of_arrays'
#include "test_msgs/msg/detail/arrays__struct.h"
// Member 'array_of_bounded_sequences'
// Member 'bounded_sequence_of_bounded_sequences'
// Member 'unbounded_sequence_of_bounded_sequences'
#include "test_msgs/msg/detail/bounded_sequences__struct.h"
// Member 'array_of_unbounded_sequences'
// Member 'bounded_sequence_of_unbounded_sequences'
// Member 'unbounded_sequence_of_unbounded_sequences'
#include "test_msgs/msg/detail/unbounded_sequences__struct.h"

// constants for array fields with an upper bound
// bounded_sequence_of_arrays
enum
{
  test_msgs__msg__MultiNested__bounded_sequence_of_arrays__MAX_SIZE = 3
};
// bounded_sequence_of_bounded_sequences
enum
{
  test_msgs__msg__MultiNested__bounded_sequence_of_bounded_sequences__MAX_SIZE = 3
};
// bounded_sequence_of_unbounded_sequences
enum
{
  test_msgs__msg__MultiNested__bounded_sequence_of_unbounded_sequences__MAX_SIZE = 3
};

/// Struct defined in msg/MultiNested in the package test_msgs.
/**
  * Mulitple levels of nested messages
 */
typedef struct test_msgs__msg__MultiNested
{
  test_msgs__msg__Arrays array_of_arrays[3];
  test_msgs__msg__BoundedSequences array_of_bounded_sequences[3];
  test_msgs__msg__UnboundedSequences array_of_unbounded_sequences[3];
  test_msgs__msg__Arrays__Sequence bounded_sequence_of_arrays;
  test_msgs__msg__BoundedSequences__Sequence bounded_sequence_of_bounded_sequences;
  test_msgs__msg__UnboundedSequences__Sequence bounded_sequence_of_unbounded_sequences;
  test_msgs__msg__Arrays__Sequence unbounded_sequence_of_arrays;
  test_msgs__msg__BoundedSequences__Sequence unbounded_sequence_of_bounded_sequences;
  test_msgs__msg__UnboundedSequences__Sequence unbounded_sequence_of_unbounded_sequences;
} test_msgs__msg__MultiNested;

// Struct for a sequence of test_msgs__msg__MultiNested.
typedef struct test_msgs__msg__MultiNested__Sequence
{
  test_msgs__msg__MultiNested * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} test_msgs__msg__MultiNested__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__MSG__DETAIL__MULTI_NESTED__STRUCT_H_
