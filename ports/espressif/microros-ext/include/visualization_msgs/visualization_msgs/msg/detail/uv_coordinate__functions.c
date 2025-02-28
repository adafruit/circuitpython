// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/UVCoordinate.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/uv_coordinate__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
visualization_msgs__msg__UVCoordinate__init(visualization_msgs__msg__UVCoordinate * msg)
{
  if (!msg) {
    return false;
  }
  // u
  // v
  return true;
}

void
visualization_msgs__msg__UVCoordinate__fini(visualization_msgs__msg__UVCoordinate * msg)
{
  if (!msg) {
    return;
  }
  // u
  // v
}

bool
visualization_msgs__msg__UVCoordinate__are_equal(const visualization_msgs__msg__UVCoordinate * lhs, const visualization_msgs__msg__UVCoordinate * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // u
  if (lhs->u != rhs->u) {
    return false;
  }
  // v
  if (lhs->v != rhs->v) {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__UVCoordinate__copy(
  const visualization_msgs__msg__UVCoordinate * input,
  visualization_msgs__msg__UVCoordinate * output)
{
  if (!input || !output) {
    return false;
  }
  // u
  output->u = input->u;
  // v
  output->v = input->v;
  return true;
}

visualization_msgs__msg__UVCoordinate *
visualization_msgs__msg__UVCoordinate__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__UVCoordinate * msg = (visualization_msgs__msg__UVCoordinate *)allocator.allocate(sizeof(visualization_msgs__msg__UVCoordinate), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__UVCoordinate));
  bool success = visualization_msgs__msg__UVCoordinate__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__UVCoordinate__destroy(visualization_msgs__msg__UVCoordinate * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__UVCoordinate__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__UVCoordinate__Sequence__init(visualization_msgs__msg__UVCoordinate__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__UVCoordinate * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__UVCoordinate *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__UVCoordinate), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__UVCoordinate__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__UVCoordinate__fini(&data[i - 1]);
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
visualization_msgs__msg__UVCoordinate__Sequence__fini(visualization_msgs__msg__UVCoordinate__Sequence * array)
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
      visualization_msgs__msg__UVCoordinate__fini(&array->data[i]);
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

visualization_msgs__msg__UVCoordinate__Sequence *
visualization_msgs__msg__UVCoordinate__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__UVCoordinate__Sequence * array = (visualization_msgs__msg__UVCoordinate__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__UVCoordinate__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__UVCoordinate__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__UVCoordinate__Sequence__destroy(visualization_msgs__msg__UVCoordinate__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__UVCoordinate__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__UVCoordinate__Sequence__are_equal(const visualization_msgs__msg__UVCoordinate__Sequence * lhs, const visualization_msgs__msg__UVCoordinate__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__UVCoordinate__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__UVCoordinate__Sequence__copy(
  const visualization_msgs__msg__UVCoordinate__Sequence * input,
  visualization_msgs__msg__UVCoordinate__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__UVCoordinate);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__UVCoordinate * data =
      (visualization_msgs__msg__UVCoordinate *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__UVCoordinate__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__UVCoordinate__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__UVCoordinate__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
