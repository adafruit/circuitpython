// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/BoundedPlainSequences.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/bounded_plain_sequences__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `bool_values`
// Member `byte_values`
// Member `char_values`
// Member `float32_values`
// Member `float64_values`
// Member `int8_values`
// Member `uint8_values`
// Member `int16_values`
// Member `uint16_values`
// Member `int32_values`
// Member `uint32_values`
// Member `int64_values`
// Member `uint64_values`
// Member `bool_values_default`
// Member `byte_values_default`
// Member `char_values_default`
// Member `float32_values_default`
// Member `float64_values_default`
// Member `int8_values_default`
// Member `uint8_values_default`
// Member `int16_values_default`
// Member `uint16_values_default`
// Member `int32_values_default`
// Member `uint32_values_default`
// Member `int64_values_default`
// Member `uint64_values_default`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `basic_types_values`
#include "test_msgs/msg/detail/basic_types__functions.h"
// Member `constants_values`
#include "test_msgs/msg/detail/constants__functions.h"
// Member `defaults_values`
#include "test_msgs/msg/detail/defaults__functions.h"

bool
test_msgs__msg__BoundedPlainSequences__init(test_msgs__msg__BoundedPlainSequences * msg)
{
  if (!msg) {
    return false;
  }
  // bool_values
  if (!rosidl_runtime_c__boolean__Sequence__init(&msg->bool_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // byte_values
  if (!rosidl_runtime_c__octet__Sequence__init(&msg->byte_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // char_values
  if (!rosidl_runtime_c__uint8__Sequence__init(&msg->char_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // float32_values
  if (!rosidl_runtime_c__float__Sequence__init(&msg->float32_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // float64_values
  if (!rosidl_runtime_c__double__Sequence__init(&msg->float64_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // int8_values
  if (!rosidl_runtime_c__int8__Sequence__init(&msg->int8_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // uint8_values
  if (!rosidl_runtime_c__uint8__Sequence__init(&msg->uint8_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // int16_values
  if (!rosidl_runtime_c__int16__Sequence__init(&msg->int16_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // uint16_values
  if (!rosidl_runtime_c__uint16__Sequence__init(&msg->uint16_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // int32_values
  if (!rosidl_runtime_c__int32__Sequence__init(&msg->int32_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // uint32_values
  if (!rosidl_runtime_c__uint32__Sequence__init(&msg->uint32_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // int64_values
  if (!rosidl_runtime_c__int64__Sequence__init(&msg->int64_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // uint64_values
  if (!rosidl_runtime_c__uint64__Sequence__init(&msg->uint64_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // basic_types_values
  if (!test_msgs__msg__BasicTypes__Sequence__init(&msg->basic_types_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // constants_values
  if (!test_msgs__msg__Constants__Sequence__init(&msg->constants_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // defaults_values
  if (!test_msgs__msg__Defaults__Sequence__init(&msg->defaults_values, 0)) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
    return false;
  }
  // bool_values_default
  {
    bool success = rosidl_runtime_c__boolean__Sequence__init(&msg->bool_values_default, 3);
    if (!success) {
      goto abort_init_0;
    }
  }
  msg->bool_values_default.data[0] = false;
  msg->bool_values_default.data[1] = true;
  msg->bool_values_default.data[2] = false;
  // byte_values_default
  {
    bool success = rosidl_runtime_c__octet__Sequence__init(&msg->byte_values_default, 3);
    if (!success) {
      goto abort_init_1;
    }
  }
  msg->byte_values_default.data[0] = 0;
  msg->byte_values_default.data[1] = 1;
  msg->byte_values_default.data[2] = 255;
  // char_values_default
  {
    bool success = rosidl_runtime_c__uint8__Sequence__init(&msg->char_values_default, 3);
    if (!success) {
      goto abort_init_2;
    }
  }
  msg->char_values_default.data[0] = 0;
  msg->char_values_default.data[1] = 1;
  msg->char_values_default.data[2] = 127;
  // float32_values_default
  {
    bool success = rosidl_runtime_c__float__Sequence__init(&msg->float32_values_default, 3);
    if (!success) {
      goto abort_init_3;
    }
  }
  msg->float32_values_default.data[0] = 1.125f;
  msg->float32_values_default.data[1] = 0.0f;
  msg->float32_values_default.data[2] = -1.125f;
  // float64_values_default
  {
    bool success = rosidl_runtime_c__double__Sequence__init(&msg->float64_values_default, 3);
    if (!success) {
      goto abort_init_4;
    }
  }
  msg->float64_values_default.data[0] = 3.1415l;
  msg->float64_values_default.data[1] = 0.0l;
  msg->float64_values_default.data[2] = -3.1415l;
  // int8_values_default
  {
    bool success = rosidl_runtime_c__int8__Sequence__init(&msg->int8_values_default, 3);
    if (!success) {
      goto abort_init_5;
    }
  }
  msg->int8_values_default.data[0] = 0;
  msg->int8_values_default.data[1] = 127;
  msg->int8_values_default.data[2] = -128;
  // uint8_values_default
  {
    bool success = rosidl_runtime_c__uint8__Sequence__init(&msg->uint8_values_default, 3);
    if (!success) {
      goto abort_init_6;
    }
  }
  msg->uint8_values_default.data[0] = 0;
  msg->uint8_values_default.data[1] = 1;
  msg->uint8_values_default.data[2] = 255;
  // int16_values_default
  {
    bool success = rosidl_runtime_c__int16__Sequence__init(&msg->int16_values_default, 3);
    if (!success) {
      goto abort_init_7;
    }
  }
  msg->int16_values_default.data[0] = 0;
  msg->int16_values_default.data[1] = 32767;
  msg->int16_values_default.data[2] = -32768;
  // uint16_values_default
  {
    bool success = rosidl_runtime_c__uint16__Sequence__init(&msg->uint16_values_default, 3);
    if (!success) {
      goto abort_init_8;
    }
  }
  msg->uint16_values_default.data[0] = 0;
  msg->uint16_values_default.data[1] = 1;
  msg->uint16_values_default.data[2] = 65535;
  // int32_values_default
  {
    bool success = rosidl_runtime_c__int32__Sequence__init(&msg->int32_values_default, 3);
    if (!success) {
      goto abort_init_9;
    }
  }
  msg->int32_values_default.data[0] = 0l;
  msg->int32_values_default.data[1] = 2147483647l;
  msg->int32_values_default.data[2] = (-2147483647l - 1);
  // uint32_values_default
  {
    bool success = rosidl_runtime_c__uint32__Sequence__init(&msg->uint32_values_default, 3);
    if (!success) {
      goto abort_init_10;
    }
  }
  msg->uint32_values_default.data[0] = 0ul;
  msg->uint32_values_default.data[1] = 1ul;
  msg->uint32_values_default.data[2] = 4294967295ul;
  // int64_values_default
  {
    bool success = rosidl_runtime_c__int64__Sequence__init(&msg->int64_values_default, 3);
    if (!success) {
      goto abort_init_11;
    }
  }
  msg->int64_values_default.data[0] = 0ll;
  msg->int64_values_default.data[1] = 9223372036854775807ll;
  msg->int64_values_default.data[2] = (-9223372036854775807ll - 1);
  // uint64_values_default
  {
    bool success = rosidl_runtime_c__uint64__Sequence__init(&msg->uint64_values_default, 3);
    if (!success) {
      goto abort_init_12;
    }
  }
  msg->uint64_values_default.data[0] = 0ull;
  msg->uint64_values_default.data[1] = 1ull;
  msg->uint64_values_default.data[2] = 18446744073709551615ull;
  // alignment_check
  return true;
abort_init_12:
  rosidl_runtime_c__int64__Sequence__fini(&msg->int64_values_default);
abort_init_11:
  rosidl_runtime_c__uint32__Sequence__fini(&msg->uint32_values_default);
abort_init_10:
  rosidl_runtime_c__int32__Sequence__fini(&msg->int32_values_default);
abort_init_9:
  rosidl_runtime_c__uint16__Sequence__fini(&msg->uint16_values_default);
abort_init_8:
  rosidl_runtime_c__int16__Sequence__fini(&msg->int16_values_default);
abort_init_7:
  rosidl_runtime_c__uint8__Sequence__fini(&msg->uint8_values_default);
abort_init_6:
  rosidl_runtime_c__int8__Sequence__fini(&msg->int8_values_default);
abort_init_5:
  rosidl_runtime_c__double__Sequence__fini(&msg->float64_values_default);
abort_init_4:
  rosidl_runtime_c__float__Sequence__fini(&msg->float32_values_default);
abort_init_3:
  rosidl_runtime_c__uint8__Sequence__fini(&msg->char_values_default);
abort_init_2:
  rosidl_runtime_c__octet__Sequence__fini(&msg->byte_values_default);
abort_init_1:
  rosidl_runtime_c__boolean__Sequence__fini(&msg->bool_values_default);
abort_init_0:
  return false;
}

void
test_msgs__msg__BoundedPlainSequences__fini(test_msgs__msg__BoundedPlainSequences * msg)
{
  if (!msg) {
    return;
  }
  // bool_values
  rosidl_runtime_c__boolean__Sequence__fini(&msg->bool_values);
  // byte_values
  rosidl_runtime_c__octet__Sequence__fini(&msg->byte_values);
  // char_values
  rosidl_runtime_c__uint8__Sequence__fini(&msg->char_values);
  // float32_values
  rosidl_runtime_c__float__Sequence__fini(&msg->float32_values);
  // float64_values
  rosidl_runtime_c__double__Sequence__fini(&msg->float64_values);
  // int8_values
  rosidl_runtime_c__int8__Sequence__fini(&msg->int8_values);
  // uint8_values
  rosidl_runtime_c__uint8__Sequence__fini(&msg->uint8_values);
  // int16_values
  rosidl_runtime_c__int16__Sequence__fini(&msg->int16_values);
  // uint16_values
  rosidl_runtime_c__uint16__Sequence__fini(&msg->uint16_values);
  // int32_values
  rosidl_runtime_c__int32__Sequence__fini(&msg->int32_values);
  // uint32_values
  rosidl_runtime_c__uint32__Sequence__fini(&msg->uint32_values);
  // int64_values
  rosidl_runtime_c__int64__Sequence__fini(&msg->int64_values);
  // uint64_values
  rosidl_runtime_c__uint64__Sequence__fini(&msg->uint64_values);
  // basic_types_values
  test_msgs__msg__BasicTypes__Sequence__fini(&msg->basic_types_values);
  // constants_values
  test_msgs__msg__Constants__Sequence__fini(&msg->constants_values);
  // defaults_values
  test_msgs__msg__Defaults__Sequence__fini(&msg->defaults_values);
  // bool_values_default
  rosidl_runtime_c__boolean__Sequence__fini(&msg->bool_values_default);
  // byte_values_default
  rosidl_runtime_c__octet__Sequence__fini(&msg->byte_values_default);
  // char_values_default
  rosidl_runtime_c__uint8__Sequence__fini(&msg->char_values_default);
  // float32_values_default
  rosidl_runtime_c__float__Sequence__fini(&msg->float32_values_default);
  // float64_values_default
  rosidl_runtime_c__double__Sequence__fini(&msg->float64_values_default);
  // int8_values_default
  rosidl_runtime_c__int8__Sequence__fini(&msg->int8_values_default);
  // uint8_values_default
  rosidl_runtime_c__uint8__Sequence__fini(&msg->uint8_values_default);
  // int16_values_default
  rosidl_runtime_c__int16__Sequence__fini(&msg->int16_values_default);
  // uint16_values_default
  rosidl_runtime_c__uint16__Sequence__fini(&msg->uint16_values_default);
  // int32_values_default
  rosidl_runtime_c__int32__Sequence__fini(&msg->int32_values_default);
  // uint32_values_default
  rosidl_runtime_c__uint32__Sequence__fini(&msg->uint32_values_default);
  // int64_values_default
  rosidl_runtime_c__int64__Sequence__fini(&msg->int64_values_default);
  // uint64_values_default
  rosidl_runtime_c__uint64__Sequence__fini(&msg->uint64_values_default);
  // alignment_check
}

bool
test_msgs__msg__BoundedPlainSequences__are_equal(const test_msgs__msg__BoundedPlainSequences * lhs, const test_msgs__msg__BoundedPlainSequences * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // bool_values
  if (!rosidl_runtime_c__boolean__Sequence__are_equal(
      &(lhs->bool_values), &(rhs->bool_values)))
  {
    return false;
  }
  // byte_values
  if (!rosidl_runtime_c__octet__Sequence__are_equal(
      &(lhs->byte_values), &(rhs->byte_values)))
  {
    return false;
  }
  // char_values
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->char_values), &(rhs->char_values)))
  {
    return false;
  }
  // float32_values
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->float32_values), &(rhs->float32_values)))
  {
    return false;
  }
  // float64_values
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->float64_values), &(rhs->float64_values)))
  {
    return false;
  }
  // int8_values
  if (!rosidl_runtime_c__int8__Sequence__are_equal(
      &(lhs->int8_values), &(rhs->int8_values)))
  {
    return false;
  }
  // uint8_values
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->uint8_values), &(rhs->uint8_values)))
  {
    return false;
  }
  // int16_values
  if (!rosidl_runtime_c__int16__Sequence__are_equal(
      &(lhs->int16_values), &(rhs->int16_values)))
  {
    return false;
  }
  // uint16_values
  if (!rosidl_runtime_c__uint16__Sequence__are_equal(
      &(lhs->uint16_values), &(rhs->uint16_values)))
  {
    return false;
  }
  // int32_values
  if (!rosidl_runtime_c__int32__Sequence__are_equal(
      &(lhs->int32_values), &(rhs->int32_values)))
  {
    return false;
  }
  // uint32_values
  if (!rosidl_runtime_c__uint32__Sequence__are_equal(
      &(lhs->uint32_values), &(rhs->uint32_values)))
  {
    return false;
  }
  // int64_values
  if (!rosidl_runtime_c__int64__Sequence__are_equal(
      &(lhs->int64_values), &(rhs->int64_values)))
  {
    return false;
  }
  // uint64_values
  if (!rosidl_runtime_c__uint64__Sequence__are_equal(
      &(lhs->uint64_values), &(rhs->uint64_values)))
  {
    return false;
  }
  // basic_types_values
  if (!test_msgs__msg__BasicTypes__Sequence__are_equal(
      &(lhs->basic_types_values), &(rhs->basic_types_values)))
  {
    return false;
  }
  // constants_values
  if (!test_msgs__msg__Constants__Sequence__are_equal(
      &(lhs->constants_values), &(rhs->constants_values)))
  {
    return false;
  }
  // defaults_values
  if (!test_msgs__msg__Defaults__Sequence__are_equal(
      &(lhs->defaults_values), &(rhs->defaults_values)))
  {
    return false;
  }
  // bool_values_default
  if (!rosidl_runtime_c__boolean__Sequence__are_equal(
      &(lhs->bool_values_default), &(rhs->bool_values_default)))
  {
    return false;
  }
  // byte_values_default
  if (!rosidl_runtime_c__octet__Sequence__are_equal(
      &(lhs->byte_values_default), &(rhs->byte_values_default)))
  {
    return false;
  }
  // char_values_default
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->char_values_default), &(rhs->char_values_default)))
  {
    return false;
  }
  // float32_values_default
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->float32_values_default), &(rhs->float32_values_default)))
  {
    return false;
  }
  // float64_values_default
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->float64_values_default), &(rhs->float64_values_default)))
  {
    return false;
  }
  // int8_values_default
  if (!rosidl_runtime_c__int8__Sequence__are_equal(
      &(lhs->int8_values_default), &(rhs->int8_values_default)))
  {
    return false;
  }
  // uint8_values_default
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->uint8_values_default), &(rhs->uint8_values_default)))
  {
    return false;
  }
  // int16_values_default
  if (!rosidl_runtime_c__int16__Sequence__are_equal(
      &(lhs->int16_values_default), &(rhs->int16_values_default)))
  {
    return false;
  }
  // uint16_values_default
  if (!rosidl_runtime_c__uint16__Sequence__are_equal(
      &(lhs->uint16_values_default), &(rhs->uint16_values_default)))
  {
    return false;
  }
  // int32_values_default
  if (!rosidl_runtime_c__int32__Sequence__are_equal(
      &(lhs->int32_values_default), &(rhs->int32_values_default)))
  {
    return false;
  }
  // uint32_values_default
  if (!rosidl_runtime_c__uint32__Sequence__are_equal(
      &(lhs->uint32_values_default), &(rhs->uint32_values_default)))
  {
    return false;
  }
  // int64_values_default
  if (!rosidl_runtime_c__int64__Sequence__are_equal(
      &(lhs->int64_values_default), &(rhs->int64_values_default)))
  {
    return false;
  }
  // uint64_values_default
  if (!rosidl_runtime_c__uint64__Sequence__are_equal(
      &(lhs->uint64_values_default), &(rhs->uint64_values_default)))
  {
    return false;
  }
  // alignment_check
  if (lhs->alignment_check != rhs->alignment_check) {
    return false;
  }
  return true;
}

bool
test_msgs__msg__BoundedPlainSequences__copy(
  const test_msgs__msg__BoundedPlainSequences * input,
  test_msgs__msg__BoundedPlainSequences * output)
{
  if (!input || !output) {
    return false;
  }
  // bool_values
  if (!rosidl_runtime_c__boolean__Sequence__copy(
      &(input->bool_values), &(output->bool_values)))
  {
    return false;
  }
  // byte_values
  if (!rosidl_runtime_c__octet__Sequence__copy(
      &(input->byte_values), &(output->byte_values)))
  {
    return false;
  }
  // char_values
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->char_values), &(output->char_values)))
  {
    return false;
  }
  // float32_values
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->float32_values), &(output->float32_values)))
  {
    return false;
  }
  // float64_values
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->float64_values), &(output->float64_values)))
  {
    return false;
  }
  // int8_values
  if (!rosidl_runtime_c__int8__Sequence__copy(
      &(input->int8_values), &(output->int8_values)))
  {
    return false;
  }
  // uint8_values
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->uint8_values), &(output->uint8_values)))
  {
    return false;
  }
  // int16_values
  if (!rosidl_runtime_c__int16__Sequence__copy(
      &(input->int16_values), &(output->int16_values)))
  {
    return false;
  }
  // uint16_values
  if (!rosidl_runtime_c__uint16__Sequence__copy(
      &(input->uint16_values), &(output->uint16_values)))
  {
    return false;
  }
  // int32_values
  if (!rosidl_runtime_c__int32__Sequence__copy(
      &(input->int32_values), &(output->int32_values)))
  {
    return false;
  }
  // uint32_values
  if (!rosidl_runtime_c__uint32__Sequence__copy(
      &(input->uint32_values), &(output->uint32_values)))
  {
    return false;
  }
  // int64_values
  if (!rosidl_runtime_c__int64__Sequence__copy(
      &(input->int64_values), &(output->int64_values)))
  {
    return false;
  }
  // uint64_values
  if (!rosidl_runtime_c__uint64__Sequence__copy(
      &(input->uint64_values), &(output->uint64_values)))
  {
    return false;
  }
  // basic_types_values
  if (!test_msgs__msg__BasicTypes__Sequence__copy(
      &(input->basic_types_values), &(output->basic_types_values)))
  {
    return false;
  }
  // constants_values
  if (!test_msgs__msg__Constants__Sequence__copy(
      &(input->constants_values), &(output->constants_values)))
  {
    return false;
  }
  // defaults_values
  if (!test_msgs__msg__Defaults__Sequence__copy(
      &(input->defaults_values), &(output->defaults_values)))
  {
    return false;
  }
  // bool_values_default
  if (!rosidl_runtime_c__boolean__Sequence__copy(
      &(input->bool_values_default), &(output->bool_values_default)))
  {
    return false;
  }
  // byte_values_default
  if (!rosidl_runtime_c__octet__Sequence__copy(
      &(input->byte_values_default), &(output->byte_values_default)))
  {
    return false;
  }
  // char_values_default
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->char_values_default), &(output->char_values_default)))
  {
    return false;
  }
  // float32_values_default
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->float32_values_default), &(output->float32_values_default)))
  {
    return false;
  }
  // float64_values_default
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->float64_values_default), &(output->float64_values_default)))
  {
    return false;
  }
  // int8_values_default
  if (!rosidl_runtime_c__int8__Sequence__copy(
      &(input->int8_values_default), &(output->int8_values_default)))
  {
    return false;
  }
  // uint8_values_default
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->uint8_values_default), &(output->uint8_values_default)))
  {
    return false;
  }
  // int16_values_default
  if (!rosidl_runtime_c__int16__Sequence__copy(
      &(input->int16_values_default), &(output->int16_values_default)))
  {
    return false;
  }
  // uint16_values_default
  if (!rosidl_runtime_c__uint16__Sequence__copy(
      &(input->uint16_values_default), &(output->uint16_values_default)))
  {
    return false;
  }
  // int32_values_default
  if (!rosidl_runtime_c__int32__Sequence__copy(
      &(input->int32_values_default), &(output->int32_values_default)))
  {
    return false;
  }
  // uint32_values_default
  if (!rosidl_runtime_c__uint32__Sequence__copy(
      &(input->uint32_values_default), &(output->uint32_values_default)))
  {
    return false;
  }
  // int64_values_default
  if (!rosidl_runtime_c__int64__Sequence__copy(
      &(input->int64_values_default), &(output->int64_values_default)))
  {
    return false;
  }
  // uint64_values_default
  if (!rosidl_runtime_c__uint64__Sequence__copy(
      &(input->uint64_values_default), &(output->uint64_values_default)))
  {
    return false;
  }
  // alignment_check
  output->alignment_check = input->alignment_check;
  return true;
}

test_msgs__msg__BoundedPlainSequences *
test_msgs__msg__BoundedPlainSequences__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__BoundedPlainSequences * msg = (test_msgs__msg__BoundedPlainSequences *)allocator.allocate(sizeof(test_msgs__msg__BoundedPlainSequences), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__BoundedPlainSequences));
  bool success = test_msgs__msg__BoundedPlainSequences__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__BoundedPlainSequences__destroy(test_msgs__msg__BoundedPlainSequences * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__BoundedPlainSequences__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__BoundedPlainSequences__Sequence__init(test_msgs__msg__BoundedPlainSequences__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__BoundedPlainSequences * data = NULL;

  if (size) {
    data = (test_msgs__msg__BoundedPlainSequences *)allocator.zero_allocate(size, sizeof(test_msgs__msg__BoundedPlainSequences), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__BoundedPlainSequences__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__BoundedPlainSequences__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
test_msgs__msg__BoundedPlainSequences__Sequence__fini(test_msgs__msg__BoundedPlainSequences__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      test_msgs__msg__BoundedPlainSequences__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

test_msgs__msg__BoundedPlainSequences__Sequence *
test_msgs__msg__BoundedPlainSequences__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__BoundedPlainSequences__Sequence * array = (test_msgs__msg__BoundedPlainSequences__Sequence *)allocator.allocate(sizeof(test_msgs__msg__BoundedPlainSequences__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__BoundedPlainSequences__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__BoundedPlainSequences__Sequence__destroy(test_msgs__msg__BoundedPlainSequences__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__BoundedPlainSequences__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__BoundedPlainSequences__Sequence__are_equal(const test_msgs__msg__BoundedPlainSequences__Sequence * lhs, const test_msgs__msg__BoundedPlainSequences__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__BoundedPlainSequences__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__BoundedPlainSequences__Sequence__copy(
  const test_msgs__msg__BoundedPlainSequences__Sequence * input,
  test_msgs__msg__BoundedPlainSequences__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__BoundedPlainSequences);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__BoundedPlainSequences * data =
      (test_msgs__msg__BoundedPlainSequences *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__BoundedPlainSequences__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__BoundedPlainSequences__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__BoundedPlainSequences__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
