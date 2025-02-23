// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from shape_msgs:msg/Plane.idl
// generated code does not contain a copyright notice
#include "shape_msgs/msg/detail/plane__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
shape_msgs__msg__Plane__init(shape_msgs__msg__Plane * msg)
{
  if (!msg) {
    return false;
  }
  // coef
  return true;
}

void
shape_msgs__msg__Plane__fini(shape_msgs__msg__Plane * msg)
{
  if (!msg) {
    return;
  }
  // coef
}

bool
shape_msgs__msg__Plane__are_equal(const shape_msgs__msg__Plane * lhs, const shape_msgs__msg__Plane * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // coef
  for (size_t i = 0; i < 4; ++i) {
    if (lhs->coef[i] != rhs->coef[i]) {
      return false;
    }
  }
  return true;
}

bool
shape_msgs__msg__Plane__copy(
  const shape_msgs__msg__Plane * input,
  shape_msgs__msg__Plane * output)
{
  if (!input || !output) {
    return false;
  }
  // coef
  for (size_t i = 0; i < 4; ++i) {
    output->coef[i] = input->coef[i];
  }
  return true;
}

shape_msgs__msg__Plane *
shape_msgs__msg__Plane__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__Plane * msg = (shape_msgs__msg__Plane *)allocator.allocate(sizeof(shape_msgs__msg__Plane), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(shape_msgs__msg__Plane));
  bool success = shape_msgs__msg__Plane__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
shape_msgs__msg__Plane__destroy(shape_msgs__msg__Plane * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    shape_msgs__msg__Plane__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
shape_msgs__msg__Plane__Sequence__init(shape_msgs__msg__Plane__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__Plane * data = NULL;

  if (size) {
    data = (shape_msgs__msg__Plane *)allocator.zero_allocate(size, sizeof(shape_msgs__msg__Plane), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = shape_msgs__msg__Plane__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        shape_msgs__msg__Plane__fini(&data[i - 1]);
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
shape_msgs__msg__Plane__Sequence__fini(shape_msgs__msg__Plane__Sequence * array)
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
      shape_msgs__msg__Plane__fini(&array->data[i]);
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

shape_msgs__msg__Plane__Sequence *
shape_msgs__msg__Plane__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__Plane__Sequence * array = (shape_msgs__msg__Plane__Sequence *)allocator.allocate(sizeof(shape_msgs__msg__Plane__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = shape_msgs__msg__Plane__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
shape_msgs__msg__Plane__Sequence__destroy(shape_msgs__msg__Plane__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    shape_msgs__msg__Plane__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
shape_msgs__msg__Plane__Sequence__are_equal(const shape_msgs__msg__Plane__Sequence * lhs, const shape_msgs__msg__Plane__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!shape_msgs__msg__Plane__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
shape_msgs__msg__Plane__Sequence__copy(
  const shape_msgs__msg__Plane__Sequence * input,
  shape_msgs__msg__Plane__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(shape_msgs__msg__Plane);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    shape_msgs__msg__Plane * data =
      (shape_msgs__msg__Plane *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!shape_msgs__msg__Plane__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          shape_msgs__msg__Plane__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!shape_msgs__msg__Plane__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
