// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/WStrings.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/w_strings__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `wstring_value`
// Member `wstring_value_default1`
// Member `wstring_value_default2`
// Member `wstring_value_default3`
// Member `array_of_wstrings`
// Member `bounded_sequence_of_wstrings`
// Member `unbounded_sequence_of_wstrings`
#include "rosidl_runtime_c/u16string_functions.h"

bool
test_msgs__msg__WStrings__init(test_msgs__msg__WStrings * msg)
{
  if (!msg) {
    return false;
  }
  // wstring_value
  if (!rosidl_runtime_c__U16String__init(&msg->wstring_value)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  // wstring_value_default1
  if (!rosidl_runtime_c__U16String__init(&msg->wstring_value_default1)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  {
    bool success = rosidl_runtime_c__U16String__assign(&msg->wstring_value_default1, u"Hello world!");
    if (!success) {
      goto abort_init_0;
    }
  }
  // wstring_value_default2
  if (!rosidl_runtime_c__U16String__init(&msg->wstring_value_default2)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  {
    bool success = rosidl_runtime_c__U16String__assign(&msg->wstring_value_default2, u"Hellö wörld!");
    if (!success) {
      goto abort_init_1;
    }
  }
  // wstring_value_default3
  if (!rosidl_runtime_c__U16String__init(&msg->wstring_value_default3)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  {
    bool success = rosidl_runtime_c__U16String__assign(&msg->wstring_value_default3, u"ハローワールド");
    if (!success) {
      goto abort_init_2;
    }
  }
  // array_of_wstrings
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__U16String__init(&msg->array_of_wstrings[i])) {
      test_msgs__msg__WStrings__fini(msg);
      return false;
    }
  }
  // bounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__init(&msg->bounded_sequence_of_wstrings, 0)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  // unbounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__init(&msg->unbounded_sequence_of_wstrings, 0)) {
    test_msgs__msg__WStrings__fini(msg);
    return false;
  }
  return true;
abort_init_2:
  rosidl_runtime_c__U16String__fini(&msg->wstring_value_default2);
abort_init_1:
  rosidl_runtime_c__U16String__fini(&msg->wstring_value_default1);
abort_init_0:
  return false;
}

void
test_msgs__msg__WStrings__fini(test_msgs__msg__WStrings * msg)
{
  if (!msg) {
    return;
  }
  // wstring_value
  rosidl_runtime_c__U16String__fini(&msg->wstring_value);
  // wstring_value_default1
  rosidl_runtime_c__U16String__fini(&msg->wstring_value_default1);
  // wstring_value_default2
  rosidl_runtime_c__U16String__fini(&msg->wstring_value_default2);
  // wstring_value_default3
  rosidl_runtime_c__U16String__fini(&msg->wstring_value_default3);
  // array_of_wstrings
  for (size_t i = 0; i < 3; ++i) {
    rosidl_runtime_c__U16String__fini(&msg->array_of_wstrings[i]);
  }
  // bounded_sequence_of_wstrings
  rosidl_runtime_c__U16String__Sequence__fini(&msg->bounded_sequence_of_wstrings);
  // unbounded_sequence_of_wstrings
  rosidl_runtime_c__U16String__Sequence__fini(&msg->unbounded_sequence_of_wstrings);
}

bool
test_msgs__msg__WStrings__are_equal(const test_msgs__msg__WStrings * lhs, const test_msgs__msg__WStrings * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // wstring_value
  if (!rosidl_runtime_c__U16String__are_equal(
      &(lhs->wstring_value), &(rhs->wstring_value)))
  {
    return false;
  }
  // wstring_value_default1
  if (!rosidl_runtime_c__U16String__are_equal(
      &(lhs->wstring_value_default1), &(rhs->wstring_value_default1)))
  {
    return false;
  }
  // wstring_value_default2
  if (!rosidl_runtime_c__U16String__are_equal(
      &(lhs->wstring_value_default2), &(rhs->wstring_value_default2)))
  {
    return false;
  }
  // wstring_value_default3
  if (!rosidl_runtime_c__U16String__are_equal(
      &(lhs->wstring_value_default3), &(rhs->wstring_value_default3)))
  {
    return false;
  }
  // array_of_wstrings
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__U16String__are_equal(
        &(lhs->array_of_wstrings[i]), &(rhs->array_of_wstrings[i])))
    {
      return false;
    }
  }
  // bounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__are_equal(
      &(lhs->bounded_sequence_of_wstrings), &(rhs->bounded_sequence_of_wstrings)))
  {
    return false;
  }
  // unbounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__are_equal(
      &(lhs->unbounded_sequence_of_wstrings), &(rhs->unbounded_sequence_of_wstrings)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__msg__WStrings__copy(
  const test_msgs__msg__WStrings * input,
  test_msgs__msg__WStrings * output)
{
  if (!input || !output) {
    return false;
  }
  // wstring_value
  if (!rosidl_runtime_c__U16String__copy(
      &(input->wstring_value), &(output->wstring_value)))
  {
    return false;
  }
  // wstring_value_default1
  if (!rosidl_runtime_c__U16String__copy(
      &(input->wstring_value_default1), &(output->wstring_value_default1)))
  {
    return false;
  }
  // wstring_value_default2
  if (!rosidl_runtime_c__U16String__copy(
      &(input->wstring_value_default2), &(output->wstring_value_default2)))
  {
    return false;
  }
  // wstring_value_default3
  if (!rosidl_runtime_c__U16String__copy(
      &(input->wstring_value_default3), &(output->wstring_value_default3)))
  {
    return false;
  }
  // array_of_wstrings
  for (size_t i = 0; i < 3; ++i) {
    if (!rosidl_runtime_c__U16String__copy(
        &(input->array_of_wstrings[i]), &(output->array_of_wstrings[i])))
    {
      return false;
    }
  }
  // bounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__copy(
      &(input->bounded_sequence_of_wstrings), &(output->bounded_sequence_of_wstrings)))
  {
    return false;
  }
  // unbounded_sequence_of_wstrings
  if (!rosidl_runtime_c__U16String__Sequence__copy(
      &(input->unbounded_sequence_of_wstrings), &(output->unbounded_sequence_of_wstrings)))
  {
    return false;
  }
  return true;
}

test_msgs__msg__WStrings *
test_msgs__msg__WStrings__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__WStrings * msg = (test_msgs__msg__WStrings *)allocator.allocate(sizeof(test_msgs__msg__WStrings), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__WStrings));
  bool success = test_msgs__msg__WStrings__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__WStrings__destroy(test_msgs__msg__WStrings * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__WStrings__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__WStrings__Sequence__init(test_msgs__msg__WStrings__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__WStrings * data = NULL;

  if (size) {
    data = (test_msgs__msg__WStrings *)allocator.zero_allocate(size, sizeof(test_msgs__msg__WStrings), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__WStrings__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__WStrings__fini(&data[i - 1]);
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
test_msgs__msg__WStrings__Sequence__fini(test_msgs__msg__WStrings__Sequence * array)
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
      test_msgs__msg__WStrings__fini(&array->data[i]);
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

test_msgs__msg__WStrings__Sequence *
test_msgs__msg__WStrings__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__WStrings__Sequence * array = (test_msgs__msg__WStrings__Sequence *)allocator.allocate(sizeof(test_msgs__msg__WStrings__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__WStrings__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__WStrings__Sequence__destroy(test_msgs__msg__WStrings__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__WStrings__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__WStrings__Sequence__are_equal(const test_msgs__msg__WStrings__Sequence * lhs, const test_msgs__msg__WStrings__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__WStrings__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__WStrings__Sequence__copy(
  const test_msgs__msg__WStrings__Sequence * input,
  test_msgs__msg__WStrings__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__WStrings);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__WStrings * data =
      (test_msgs__msg__WStrings *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__WStrings__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__WStrings__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__WStrings__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
