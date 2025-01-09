// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/Nested.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/nested__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `basic_types_value`
#include "test_msgs/msg/detail/basic_types__functions.h"

bool
test_msgs__msg__Nested__init(test_msgs__msg__Nested * msg)
{
  if (!msg) {
    return false;
  }
  // basic_types_value
  if (!test_msgs__msg__BasicTypes__init(&msg->basic_types_value)) {
    test_msgs__msg__Nested__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__msg__Nested__fini(test_msgs__msg__Nested * msg)
{
  if (!msg) {
    return;
  }
  // basic_types_value
  test_msgs__msg__BasicTypes__fini(&msg->basic_types_value);
}

bool
test_msgs__msg__Nested__are_equal(const test_msgs__msg__Nested * lhs, const test_msgs__msg__Nested * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // basic_types_value
  if (!test_msgs__msg__BasicTypes__are_equal(
      &(lhs->basic_types_value), &(rhs->basic_types_value)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__msg__Nested__copy(
  const test_msgs__msg__Nested * input,
  test_msgs__msg__Nested * output)
{
  if (!input || !output) {
    return false;
  }
  // basic_types_value
  if (!test_msgs__msg__BasicTypes__copy(
      &(input->basic_types_value), &(output->basic_types_value)))
  {
    return false;
  }
  return true;
}

test_msgs__msg__Nested *
test_msgs__msg__Nested__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Nested * msg = (test_msgs__msg__Nested *)allocator.allocate(sizeof(test_msgs__msg__Nested), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__Nested));
  bool success = test_msgs__msg__Nested__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__Nested__destroy(test_msgs__msg__Nested * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__Nested__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__Nested__Sequence__init(test_msgs__msg__Nested__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Nested * data = NULL;

  if (size) {
    data = (test_msgs__msg__Nested *)allocator.zero_allocate(size, sizeof(test_msgs__msg__Nested), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__Nested__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__Nested__fini(&data[i - 1]);
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
test_msgs__msg__Nested__Sequence__fini(test_msgs__msg__Nested__Sequence * array)
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
      test_msgs__msg__Nested__fini(&array->data[i]);
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

test_msgs__msg__Nested__Sequence *
test_msgs__msg__Nested__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Nested__Sequence * array = (test_msgs__msg__Nested__Sequence *)allocator.allocate(sizeof(test_msgs__msg__Nested__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__Nested__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__Nested__Sequence__destroy(test_msgs__msg__Nested__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__Nested__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__Nested__Sequence__are_equal(const test_msgs__msg__Nested__Sequence * lhs, const test_msgs__msg__Nested__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__Nested__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__Nested__Sequence__copy(
  const test_msgs__msg__Nested__Sequence * input,
  test_msgs__msg__Nested__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__Nested);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__Nested * data =
      (test_msgs__msg__Nested *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__Nested__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__Nested__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__Nested__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
