// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from example_interfaces:msg/MultiArrayDimension.idl
// generated code does not contain a copyright notice
#include "example_interfaces/msg/detail/multi_array_dimension__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `label`
#include "rosidl_runtime_c/string_functions.h"

bool
example_interfaces__msg__MultiArrayDimension__init(example_interfaces__msg__MultiArrayDimension * msg)
{
  if (!msg) {
    return false;
  }
  // label
  if (!rosidl_runtime_c__String__init(&msg->label)) {
    example_interfaces__msg__MultiArrayDimension__fini(msg);
    return false;
  }
  // size
  // stride
  return true;
}

void
example_interfaces__msg__MultiArrayDimension__fini(example_interfaces__msg__MultiArrayDimension * msg)
{
  if (!msg) {
    return;
  }
  // label
  rosidl_runtime_c__String__fini(&msg->label);
  // size
  // stride
}

bool
example_interfaces__msg__MultiArrayDimension__are_equal(const example_interfaces__msg__MultiArrayDimension * lhs, const example_interfaces__msg__MultiArrayDimension * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // label
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->label), &(rhs->label)))
  {
    return false;
  }
  // size
  if (lhs->size != rhs->size) {
    return false;
  }
  // stride
  if (lhs->stride != rhs->stride) {
    return false;
  }
  return true;
}

bool
example_interfaces__msg__MultiArrayDimension__copy(
  const example_interfaces__msg__MultiArrayDimension * input,
  example_interfaces__msg__MultiArrayDimension * output)
{
  if (!input || !output) {
    return false;
  }
  // label
  if (!rosidl_runtime_c__String__copy(
      &(input->label), &(output->label)))
  {
    return false;
  }
  // size
  output->size = input->size;
  // stride
  output->stride = input->stride;
  return true;
}

example_interfaces__msg__MultiArrayDimension *
example_interfaces__msg__MultiArrayDimension__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  example_interfaces__msg__MultiArrayDimension * msg = (example_interfaces__msg__MultiArrayDimension *)allocator.allocate(sizeof(example_interfaces__msg__MultiArrayDimension), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(example_interfaces__msg__MultiArrayDimension));
  bool success = example_interfaces__msg__MultiArrayDimension__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
example_interfaces__msg__MultiArrayDimension__destroy(example_interfaces__msg__MultiArrayDimension * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    example_interfaces__msg__MultiArrayDimension__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
example_interfaces__msg__MultiArrayDimension__Sequence__init(example_interfaces__msg__MultiArrayDimension__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  example_interfaces__msg__MultiArrayDimension * data = NULL;

  if (size) {
    data = (example_interfaces__msg__MultiArrayDimension *)allocator.zero_allocate(size, sizeof(example_interfaces__msg__MultiArrayDimension), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = example_interfaces__msg__MultiArrayDimension__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        example_interfaces__msg__MultiArrayDimension__fini(&data[i - 1]);
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
example_interfaces__msg__MultiArrayDimension__Sequence__fini(example_interfaces__msg__MultiArrayDimension__Sequence * array)
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
      example_interfaces__msg__MultiArrayDimension__fini(&array->data[i]);
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

example_interfaces__msg__MultiArrayDimension__Sequence *
example_interfaces__msg__MultiArrayDimension__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  example_interfaces__msg__MultiArrayDimension__Sequence * array = (example_interfaces__msg__MultiArrayDimension__Sequence *)allocator.allocate(sizeof(example_interfaces__msg__MultiArrayDimension__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = example_interfaces__msg__MultiArrayDimension__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
example_interfaces__msg__MultiArrayDimension__Sequence__destroy(example_interfaces__msg__MultiArrayDimension__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    example_interfaces__msg__MultiArrayDimension__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
example_interfaces__msg__MultiArrayDimension__Sequence__are_equal(const example_interfaces__msg__MultiArrayDimension__Sequence * lhs, const example_interfaces__msg__MultiArrayDimension__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!example_interfaces__msg__MultiArrayDimension__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
example_interfaces__msg__MultiArrayDimension__Sequence__copy(
  const example_interfaces__msg__MultiArrayDimension__Sequence * input,
  example_interfaces__msg__MultiArrayDimension__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(example_interfaces__msg__MultiArrayDimension);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    example_interfaces__msg__MultiArrayDimension * data =
      (example_interfaces__msg__MultiArrayDimension *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!example_interfaces__msg__MultiArrayDimension__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          example_interfaces__msg__MultiArrayDimension__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!example_interfaces__msg__MultiArrayDimension__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
