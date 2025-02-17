// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from diagnostic_msgs:msg/KeyValue.idl
// generated code does not contain a copyright notice
#include "diagnostic_msgs/msg/detail/key_value__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `key`
// Member `value`
#include "rosidl_runtime_c/string_functions.h"

bool
diagnostic_msgs__msg__KeyValue__init(diagnostic_msgs__msg__KeyValue * msg)
{
  if (!msg) {
    return false;
  }
  // key
  if (!rosidl_runtime_c__String__init(&msg->key)) {
    diagnostic_msgs__msg__KeyValue__fini(msg);
    return false;
  }
  // value
  if (!rosidl_runtime_c__String__init(&msg->value)) {
    diagnostic_msgs__msg__KeyValue__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__msg__KeyValue__fini(diagnostic_msgs__msg__KeyValue * msg)
{
  if (!msg) {
    return;
  }
  // key
  rosidl_runtime_c__String__fini(&msg->key);
  // value
  rosidl_runtime_c__String__fini(&msg->value);
}

bool
diagnostic_msgs__msg__KeyValue__are_equal(const diagnostic_msgs__msg__KeyValue * lhs, const diagnostic_msgs__msg__KeyValue * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // key
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->key), &(rhs->key)))
  {
    return false;
  }
  // value
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->value), &(rhs->value)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__msg__KeyValue__copy(
  const diagnostic_msgs__msg__KeyValue * input,
  diagnostic_msgs__msg__KeyValue * output)
{
  if (!input || !output) {
    return false;
  }
  // key
  if (!rosidl_runtime_c__String__copy(
      &(input->key), &(output->key)))
  {
    return false;
  }
  // value
  if (!rosidl_runtime_c__String__copy(
      &(input->value), &(output->value)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__msg__KeyValue *
diagnostic_msgs__msg__KeyValue__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__KeyValue * msg = (diagnostic_msgs__msg__KeyValue *)allocator.allocate(sizeof(diagnostic_msgs__msg__KeyValue), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__msg__KeyValue));
  bool success = diagnostic_msgs__msg__KeyValue__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__msg__KeyValue__destroy(diagnostic_msgs__msg__KeyValue * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__msg__KeyValue__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__msg__KeyValue__Sequence__init(diagnostic_msgs__msg__KeyValue__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__KeyValue * data = NULL;

  if (size) {
    data = (diagnostic_msgs__msg__KeyValue *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__msg__KeyValue), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__msg__KeyValue__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__msg__KeyValue__fini(&data[i - 1]);
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
diagnostic_msgs__msg__KeyValue__Sequence__fini(diagnostic_msgs__msg__KeyValue__Sequence * array)
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
      diagnostic_msgs__msg__KeyValue__fini(&array->data[i]);
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

diagnostic_msgs__msg__KeyValue__Sequence *
diagnostic_msgs__msg__KeyValue__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__KeyValue__Sequence * array = (diagnostic_msgs__msg__KeyValue__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__msg__KeyValue__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__msg__KeyValue__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__msg__KeyValue__Sequence__destroy(diagnostic_msgs__msg__KeyValue__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__msg__KeyValue__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__msg__KeyValue__Sequence__are_equal(const diagnostic_msgs__msg__KeyValue__Sequence * lhs, const diagnostic_msgs__msg__KeyValue__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__msg__KeyValue__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__msg__KeyValue__Sequence__copy(
  const diagnostic_msgs__msg__KeyValue__Sequence * input,
  diagnostic_msgs__msg__KeyValue__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__msg__KeyValue);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__msg__KeyValue * data =
      (diagnostic_msgs__msg__KeyValue *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__msg__KeyValue__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__msg__KeyValue__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__msg__KeyValue__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
