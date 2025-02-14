// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/Arrays.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/arrays__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `string_values`
// Member `string_values_default`
#include "rosidl_runtime_c/string_functions.h"
// Member `basic_types_values`
#include "test_msgs/msg/detail/basic_types__functions.h"
// Member `constants_values`
#include "test_msgs/msg/detail/constants__functions.h"
// Member `defaults_values`
#include "test_msgs/msg/detail/defaults__functions.h"

bool
test_msgs__msg__Arrays__init(test_msgs__msg__Arrays * msg)
{
  if (!msg) {
    return false;
  }
  // bool_values
  // byte_values
  // char_values
  // float32_values
  // float64_values
  // int8_values
  // uint8_values
  // int16_values
  // uint16_values
  // int32_values
  // uint32_values
  // int64_values
  // uint64_values
  // string_values
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__init(&msg->string_values[i])) {
      test_msgs__msg__Arrays__fini(msg);
      return false;
    }
  }
  // basic_types_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__BasicTypes__init(&msg->basic_types_values[i])) {
      test_msgs__msg__Arrays__fini(msg);
      return false;
    }
  }
  // constants_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Constants__init(&msg->constants_values[i])) {
      test_msgs__msg__Arrays__fini(msg);
      return false;
    }
  }
  // defaults_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Defaults__init(&msg->defaults_values[i])) {
      test_msgs__msg__Arrays__fini(msg);
      return false;
    }
  }
  // bool_values_default
  msg->bool_values_default[0] = false;
  msg->bool_values_default[1] = true;
  msg->bool_values_default[2] = false;
  // byte_values_default
  msg->byte_values_default[0] = 0;
  msg->byte_values_default[1] = 1;
  msg->byte_values_default[2] = 255;
  // char_values_default
  msg->char_values_default[0] = 0;
  msg->char_values_default[1] = 1;
  msg->char_values_default[2] = 127;
  // float32_values_default
  msg->float32_values_default[0] = 1.125f;
  msg->float32_values_default[1] = 0.0f;
  msg->float32_values_default[2] = -1.125f;
  // float64_values_default
  msg->float64_values_default[0] = 3.1415l;
  msg->float64_values_default[1] = 0.0l;
  msg->float64_values_default[2] = -3.1415l;
  // int8_values_default
  msg->int8_values_default[0] = 0;
  msg->int8_values_default[1] = 127;
  msg->int8_values_default[2] = -128;
  // uint8_values_default
  msg->uint8_values_default[0] = 0;
  msg->uint8_values_default[1] = 1;
  msg->uint8_values_default[2] = 255;
  // int16_values_default
  msg->int16_values_default[0] = 0;
  msg->int16_values_default[1] = 32767;
  msg->int16_values_default[2] = -32768;
  // uint16_values_default
  msg->uint16_values_default[0] = 0;
  msg->uint16_values_default[1] = 1;
  msg->uint16_values_default[2] = 65535;
  // int32_values_default
  msg->int32_values_default[0] = 0l;
  msg->int32_values_default[1] = 2147483647l;
  msg->int32_values_default[2] = (-2147483647l - 1);
  // uint32_values_default
  msg->uint32_values_default[0] = 0ul;
  msg->uint32_values_default[1] = 1ul;
  msg->uint32_values_default[2] = 4294967295ul;
  // int64_values_default
  msg->int64_values_default[0] = 0ll;
  msg->int64_values_default[1] = 9223372036854775807ll;
  msg->int64_values_default[2] = (-9223372036854775807ll - 1);
  // uint64_values_default
  msg->uint64_values_default[0] = 0ull;
  msg->uint64_values_default[1] = 1ull;
  msg->uint64_values_default[2] = 18446744073709551615ull;
  // string_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__init(&msg->string_values_default[i])) {
      test_msgs__msg__Arrays__fini(msg);
      return false;
    }
  }
  {
    bool success = rosidl_runtime_c__String__assign(&msg->string_values_default[0], "");
    if (!success) {
      goto abort_init_0;
    }
  }
  {
    bool success = rosidl_runtime_c__String__assign(&msg->string_values_default[1], "max value");
    if (!success) {
      goto abort_init_1;
    }
  }
  {
    bool success = rosidl_runtime_c__String__assign(&msg->string_values_default[2], "min value");
    if (!success) {
      goto abort_init_2;
    }
  }
  // alignment_check
  return true;
abort_init_2:
  rosidl_runtime_c__String__fini(&msg->string_values_default[1]);
abort_init_1:
  rosidl_runtime_c__String__fini(&msg->string_values_default[0]);
abort_init_0:
  return false;
}

void
test_msgs__msg__Arrays__fini(test_msgs__msg__Arrays * msg)
{
  if (!msg) {
    return;
  }
  // bool_values
  // byte_values
  // char_values
  // float32_values
  // float64_values
  // int8_values
  // uint8_values
  // int16_values
  // uint16_values
  // int32_values
  // uint32_values
  // int64_values
  // uint64_values
  // string_values
  for (size_t i = 0; i < 3; ++i) {
    rosidl_runtime_c__String__fini(&msg->string_values[i]);
  }
  // basic_types_values
  for (size_t i = 0; i < 3; ++i) {
    test_msgs__msg__BasicTypes__fini(&msg->basic_types_values[i]);
  }
  // constants_values
  for (size_t i = 0; i < 3; ++i) {
    test_msgs__msg__Constants__fini(&msg->constants_values[i]);
  }
  // defaults_values
  for (size_t i = 0; i < 3; ++i) {
    test_msgs__msg__Defaults__fini(&msg->defaults_values[i]);
  }
  // bool_values_default
  // byte_values_default
  // char_values_default
  // float32_values_default
  // float64_values_default
  // int8_values_default
  // uint8_values_default
  // int16_values_default
  // uint16_values_default
  // int32_values_default
  // uint32_values_default
  // int64_values_default
  // uint64_values_default
  // string_values_default
  for (size_t i = 0; i < 3; ++i) {
    rosidl_runtime_c__String__fini(&msg->string_values_default[i]);
  }
  // alignment_check
}

bool
test_msgs__msg__Arrays__are_equal(const test_msgs__msg__Arrays * lhs, const test_msgs__msg__Arrays * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // bool_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->bool_values[i] != rhs->bool_values[i]) {
      return false;
    }
  }
  // byte_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->byte_values[i] != rhs->byte_values[i]) {
      return false;
    }
  }
  // char_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->char_values[i] != rhs->char_values[i]) {
      return false;
    }
  }
  // float32_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->float32_values[i] != rhs->float32_values[i]) {
      return false;
    }
  }
  // float64_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->float64_values[i] != rhs->float64_values[i]) {
      return false;
    }
  }
  // int8_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int8_values[i] != rhs->int8_values[i]) {
      return false;
    }
  }
  // uint8_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint8_values[i] != rhs->uint8_values[i]) {
      return false;
    }
  }
  // int16_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int16_values[i] != rhs->int16_values[i]) {
      return false;
    }
  }
  // uint16_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint16_values[i] != rhs->uint16_values[i]) {
      return false;
    }
  }
  // int32_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int32_values[i] != rhs->int32_values[i]) {
      return false;
    }
  }
  // uint32_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint32_values[i] != rhs->uint32_values[i]) {
      return false;
    }
  }
  // int64_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int64_values[i] != rhs->int64_values[i]) {
      return false;
    }
  }
  // uint64_values
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint64_values[i] != rhs->uint64_values[i]) {
      return false;
    }
  }
  // string_values
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__are_equal(
        &(lhs->string_values[i]), &(rhs->string_values[i])))
    {
      return false;
    }
  }
  // basic_types_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__BasicTypes__are_equal(
        &(lhs->basic_types_values[i]), &(rhs->basic_types_values[i])))
    {
      return false;
    }
  }
  // constants_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Constants__are_equal(
        &(lhs->constants_values[i]), &(rhs->constants_values[i])))
    {
      return false;
    }
  }
  // defaults_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Defaults__are_equal(
        &(lhs->defaults_values[i]), &(rhs->defaults_values[i])))
    {
      return false;
    }
  }
  // bool_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->bool_values_default[i] != rhs->bool_values_default[i]) {
      return false;
    }
  }
  // byte_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->byte_values_default[i] != rhs->byte_values_default[i]) {
      return false;
    }
  }
  // char_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->char_values_default[i] != rhs->char_values_default[i]) {
      return false;
    }
  }
  // float32_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->float32_values_default[i] != rhs->float32_values_default[i]) {
      return false;
    }
  }
  // float64_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->float64_values_default[i] != rhs->float64_values_default[i]) {
      return false;
    }
  }
  // int8_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int8_values_default[i] != rhs->int8_values_default[i]) {
      return false;
    }
  }
  // uint8_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint8_values_default[i] != rhs->uint8_values_default[i]) {
      return false;
    }
  }
  // int16_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int16_values_default[i] != rhs->int16_values_default[i]) {
      return false;
    }
  }
  // uint16_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint16_values_default[i] != rhs->uint16_values_default[i]) {
      return false;
    }
  }
  // int32_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int32_values_default[i] != rhs->int32_values_default[i]) {
      return false;
    }
  }
  // uint32_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint32_values_default[i] != rhs->uint32_values_default[i]) {
      return false;
    }
  }
  // int64_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->int64_values_default[i] != rhs->int64_values_default[i]) {
      return false;
    }
  }
  // uint64_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (lhs->uint64_values_default[i] != rhs->uint64_values_default[i]) {
      return false;
    }
  }
  // string_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__are_equal(
        &(lhs->string_values_default[i]), &(rhs->string_values_default[i])))
    {
      return false;
    }
  }
  // alignment_check
  if (lhs->alignment_check != rhs->alignment_check) {
    return false;
  }
  return true;
}

bool
test_msgs__msg__Arrays__copy(
  const test_msgs__msg__Arrays * input,
  test_msgs__msg__Arrays * output)
{
  if (!input || !output) {
    return false;
  }
  // bool_values
  for (size_t i = 0; i < 3; ++i) {
    output->bool_values[i] = input->bool_values[i];
  }
  // byte_values
  for (size_t i = 0; i < 3; ++i) {
    output->byte_values[i] = input->byte_values[i];
  }
  // char_values
  for (size_t i = 0; i < 3; ++i) {
    output->char_values[i] = input->char_values[i];
  }
  // float32_values
  for (size_t i = 0; i < 3; ++i) {
    output->float32_values[i] = input->float32_values[i];
  }
  // float64_values
  for (size_t i = 0; i < 3; ++i) {
    output->float64_values[i] = input->float64_values[i];
  }
  // int8_values
  for (size_t i = 0; i < 3; ++i) {
    output->int8_values[i] = input->int8_values[i];
  }
  // uint8_values
  for (size_t i = 0; i < 3; ++i) {
    output->uint8_values[i] = input->uint8_values[i];
  }
  // int16_values
  for (size_t i = 0; i < 3; ++i) {
    output->int16_values[i] = input->int16_values[i];
  }
  // uint16_values
  for (size_t i = 0; i < 3; ++i) {
    output->uint16_values[i] = input->uint16_values[i];
  }
  // int32_values
  for (size_t i = 0; i < 3; ++i) {
    output->int32_values[i] = input->int32_values[i];
  }
  // uint32_values
  for (size_t i = 0; i < 3; ++i) {
    output->uint32_values[i] = input->uint32_values[i];
  }
  // int64_values
  for (size_t i = 0; i < 3; ++i) {
    output->int64_values[i] = input->int64_values[i];
  }
  // uint64_values
  for (size_t i = 0; i < 3; ++i) {
    output->uint64_values[i] = input->uint64_values[i];
  }
  // string_values
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__copy(
        &(input->string_values[i]), &(output->string_values[i])))
    {
      return false;
    }
  }
  // basic_types_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__BasicTypes__copy(
        &(input->basic_types_values[i]), &(output->basic_types_values[i])))
    {
      return false;
    }
  }
  // constants_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Constants__copy(
        &(input->constants_values[i]), &(output->constants_values[i])))
    {
      return false;
    }
  }
  // defaults_values
  for (size_t i = 0; i < 3; ++i) {
    if (!test_msgs__msg__Defaults__copy(
        &(input->defaults_values[i]), &(output->defaults_values[i])))
    {
      return false;
    }
  }
  // bool_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->bool_values_default[i] = input->bool_values_default[i];
  }
  // byte_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->byte_values_default[i] = input->byte_values_default[i];
  }
  // char_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->char_values_default[i] = input->char_values_default[i];
  }
  // float32_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->float32_values_default[i] = input->float32_values_default[i];
  }
  // float64_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->float64_values_default[i] = input->float64_values_default[i];
  }
  // int8_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->int8_values_default[i] = input->int8_values_default[i];
  }
  // uint8_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->uint8_values_default[i] = input->uint8_values_default[i];
  }
  // int16_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->int16_values_default[i] = input->int16_values_default[i];
  }
  // uint16_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->uint16_values_default[i] = input->uint16_values_default[i];
  }
  // int32_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->int32_values_default[i] = input->int32_values_default[i];
  }
  // uint32_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->uint32_values_default[i] = input->uint32_values_default[i];
  }
  // int64_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->int64_values_default[i] = input->int64_values_default[i];
  }
  // uint64_values_default
  for (size_t i = 0; i < 3; ++i) {
    output->uint64_values_default[i] = input->uint64_values_default[i];
  }
  // string_values_default
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__String__copy(
        &(input->string_values_default[i]), &(output->string_values_default[i])))
    {
      return false;
    }
  }
  // alignment_check
  output->alignment_check = input->alignment_check;
  return true;
}

test_msgs__msg__Arrays *
test_msgs__msg__Arrays__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Arrays * msg = (test_msgs__msg__Arrays *)allocator.allocate(sizeof(test_msgs__msg__Arrays), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__Arrays));
  bool success = test_msgs__msg__Arrays__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__Arrays__destroy(test_msgs__msg__Arrays * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__Arrays__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__Arrays__Sequence__init(test_msgs__msg__Arrays__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Arrays * data = NULL;

  if (size) {
    data = (test_msgs__msg__Arrays *)allocator.zero_allocate(size, sizeof(test_msgs__msg__Arrays), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__Arrays__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__Arrays__fini(&data[i - 1]);
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
test_msgs__msg__Arrays__Sequence__fini(test_msgs__msg__Arrays__Sequence * array)
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
      test_msgs__msg__Arrays__fini(&array->data[i]);
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

test_msgs__msg__Arrays__Sequence *
test_msgs__msg__Arrays__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Arrays__Sequence * array = (test_msgs__msg__Arrays__Sequence *)allocator.allocate(sizeof(test_msgs__msg__Arrays__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__Arrays__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__Arrays__Sequence__destroy(test_msgs__msg__Arrays__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__Arrays__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__Arrays__Sequence__are_equal(const test_msgs__msg__Arrays__Sequence * lhs, const test_msgs__msg__Arrays__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__Arrays__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__Arrays__Sequence__copy(
  const test_msgs__msg__Arrays__Sequence * input,
  test_msgs__msg__Arrays__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__Arrays);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__Arrays * data =
      (test_msgs__msg__Arrays *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__Arrays__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__Arrays__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__Arrays__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
