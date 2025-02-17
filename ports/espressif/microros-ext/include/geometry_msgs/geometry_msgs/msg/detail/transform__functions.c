// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from geometry_msgs:msg/Transform.idl
// generated code does not contain a copyright notice
#include "geometry_msgs/msg/detail/transform__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `translation`
#include "geometry_msgs/msg/detail/vector3__functions.h"
// Member `rotation`
#include "geometry_msgs/msg/detail/quaternion__functions.h"

bool
geometry_msgs__msg__Transform__init(geometry_msgs__msg__Transform * msg)
{
  if (!msg) {
    return false;
  }
  // translation
  if (!geometry_msgs__msg__Vector3__init(&msg->translation)) {
    geometry_msgs__msg__Transform__fini(msg);
    return false;
  }
  // rotation
  if (!geometry_msgs__msg__Quaternion__init(&msg->rotation)) {
    geometry_msgs__msg__Transform__fini(msg);
    return false;
  }
  return true;
}

void
geometry_msgs__msg__Transform__fini(geometry_msgs__msg__Transform * msg)
{
  if (!msg) {
    return;
  }
  // translation
  geometry_msgs__msg__Vector3__fini(&msg->translation);
  // rotation
  geometry_msgs__msg__Quaternion__fini(&msg->rotation);
}

bool
geometry_msgs__msg__Transform__are_equal(const geometry_msgs__msg__Transform * lhs, const geometry_msgs__msg__Transform * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // translation
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->translation), &(rhs->translation)))
  {
    return false;
  }
  // rotation
  if (!geometry_msgs__msg__Quaternion__are_equal(
      &(lhs->rotation), &(rhs->rotation)))
  {
    return false;
  }
  return true;
}

bool
geometry_msgs__msg__Transform__copy(
  const geometry_msgs__msg__Transform * input,
  geometry_msgs__msg__Transform * output)
{
  if (!input || !output) {
    return false;
  }
  // translation
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->translation), &(output->translation)))
  {
    return false;
  }
  // rotation
  if (!geometry_msgs__msg__Quaternion__copy(
      &(input->rotation), &(output->rotation)))
  {
    return false;
  }
  return true;
}

geometry_msgs__msg__Transform *
geometry_msgs__msg__Transform__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  geometry_msgs__msg__Transform * msg = (geometry_msgs__msg__Transform *)allocator.allocate(sizeof(geometry_msgs__msg__Transform), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(geometry_msgs__msg__Transform));
  bool success = geometry_msgs__msg__Transform__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
geometry_msgs__msg__Transform__destroy(geometry_msgs__msg__Transform * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    geometry_msgs__msg__Transform__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
geometry_msgs__msg__Transform__Sequence__init(geometry_msgs__msg__Transform__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  geometry_msgs__msg__Transform * data = NULL;

  if (size) {
    data = (geometry_msgs__msg__Transform *)allocator.zero_allocate(size, sizeof(geometry_msgs__msg__Transform), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = geometry_msgs__msg__Transform__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        geometry_msgs__msg__Transform__fini(&data[i - 1]);
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
geometry_msgs__msg__Transform__Sequence__fini(geometry_msgs__msg__Transform__Sequence * array)
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
      geometry_msgs__msg__Transform__fini(&array->data[i]);
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

geometry_msgs__msg__Transform__Sequence *
geometry_msgs__msg__Transform__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  geometry_msgs__msg__Transform__Sequence * array = (geometry_msgs__msg__Transform__Sequence *)allocator.allocate(sizeof(geometry_msgs__msg__Transform__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = geometry_msgs__msg__Transform__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
geometry_msgs__msg__Transform__Sequence__destroy(geometry_msgs__msg__Transform__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    geometry_msgs__msg__Transform__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
geometry_msgs__msg__Transform__Sequence__are_equal(const geometry_msgs__msg__Transform__Sequence * lhs, const geometry_msgs__msg__Transform__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!geometry_msgs__msg__Transform__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
geometry_msgs__msg__Transform__Sequence__copy(
  const geometry_msgs__msg__Transform__Sequence * input,
  geometry_msgs__msg__Transform__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(geometry_msgs__msg__Transform);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    geometry_msgs__msg__Transform * data =
      (geometry_msgs__msg__Transform *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!geometry_msgs__msg__Transform__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          geometry_msgs__msg__Transform__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!geometry_msgs__msg__Transform__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
