// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/Builtins.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/builtins__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `duration_value`
#include "builtin_interfaces/msg/detail/duration__functions.h"
// Member `time_value`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
test_msgs__msg__Builtins__init(test_msgs__msg__Builtins * msg)
{
  if (!msg) {
    return false;
  }
  // duration_value
  if (!builtin_interfaces__msg__Duration__init(&msg->duration_value)) {
    test_msgs__msg__Builtins__fini(msg);
    return false;
  }
  // time_value
  if (!builtin_interfaces__msg__Time__init(&msg->time_value)) {
    test_msgs__msg__Builtins__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__msg__Builtins__fini(test_msgs__msg__Builtins * msg)
{
  if (!msg) {
    return;
  }
  // duration_value
  builtin_interfaces__msg__Duration__fini(&msg->duration_value);
  // time_value
  builtin_interfaces__msg__Time__fini(&msg->time_value);
}

bool
test_msgs__msg__Builtins__are_equal(const test_msgs__msg__Builtins * lhs, const test_msgs__msg__Builtins * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // duration_value
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->duration_value), &(rhs->duration_value)))
  {
    return false;
  }
  // time_value
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->time_value), &(rhs->time_value)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__msg__Builtins__copy(
  const test_msgs__msg__Builtins * input,
  test_msgs__msg__Builtins * output)
{
  if (!input || !output) {
    return false;
  }
  // duration_value
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->duration_value), &(output->duration_value)))
  {
    return false;
  }
  // time_value
  if (!builtin_interfaces__msg__Time__copy(
      &(input->time_value), &(output->time_value)))
  {
    return false;
  }
  return true;
}

test_msgs__msg__Builtins *
test_msgs__msg__Builtins__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Builtins * msg = (test_msgs__msg__Builtins *)allocator.allocate(sizeof(test_msgs__msg__Builtins), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__Builtins));
  bool success = test_msgs__msg__Builtins__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__Builtins__destroy(test_msgs__msg__Builtins * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__Builtins__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__Builtins__Sequence__init(test_msgs__msg__Builtins__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Builtins * data = NULL;

  if (size) {
    data = (test_msgs__msg__Builtins *)allocator.zero_allocate(size, sizeof(test_msgs__msg__Builtins), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__Builtins__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__Builtins__fini(&data[i - 1]);
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
test_msgs__msg__Builtins__Sequence__fini(test_msgs__msg__Builtins__Sequence * array)
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
      test_msgs__msg__Builtins__fini(&array->data[i]);
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

test_msgs__msg__Builtins__Sequence *
test_msgs__msg__Builtins__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__Builtins__Sequence * array = (test_msgs__msg__Builtins__Sequence *)allocator.allocate(sizeof(test_msgs__msg__Builtins__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__Builtins__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__Builtins__Sequence__destroy(test_msgs__msg__Builtins__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__Builtins__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__Builtins__Sequence__are_equal(const test_msgs__msg__Builtins__Sequence * lhs, const test_msgs__msg__Builtins__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__Builtins__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__Builtins__Sequence__copy(
  const test_msgs__msg__Builtins__Sequence * input,
  test_msgs__msg__Builtins__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__Builtins);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__Builtins * data =
      (test_msgs__msg__Builtins *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__Builtins__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__Builtins__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__Builtins__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
