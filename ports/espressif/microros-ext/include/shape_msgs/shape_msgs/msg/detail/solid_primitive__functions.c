// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from shape_msgs:msg/SolidPrimitive.idl
// generated code does not contain a copyright notice
#include "shape_msgs/msg/detail/solid_primitive__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `dimensions`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `polygon`
#include "geometry_msgs/msg/detail/polygon__functions.h"

bool
shape_msgs__msg__SolidPrimitive__init(shape_msgs__msg__SolidPrimitive * msg)
{
  if (!msg) {
    return false;
  }
  // type
  // dimensions
  if (!rosidl_runtime_c__double__Sequence__init(&msg->dimensions, 0)) {
    shape_msgs__msg__SolidPrimitive__fini(msg);
    return false;
  }
  // polygon
  if (!geometry_msgs__msg__Polygon__init(&msg->polygon)) {
    shape_msgs__msg__SolidPrimitive__fini(msg);
    return false;
  }
  return true;
}

void
shape_msgs__msg__SolidPrimitive__fini(shape_msgs__msg__SolidPrimitive * msg)
{
  if (!msg) {
    return;
  }
  // type
  // dimensions
  rosidl_runtime_c__double__Sequence__fini(&msg->dimensions);
  // polygon
  geometry_msgs__msg__Polygon__fini(&msg->polygon);
}

bool
shape_msgs__msg__SolidPrimitive__are_equal(const shape_msgs__msg__SolidPrimitive * lhs, const shape_msgs__msg__SolidPrimitive * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // type
  if (lhs->type != rhs->type) {
    return false;
  }
  // dimensions
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->dimensions), &(rhs->dimensions)))
  {
    return false;
  }
  // polygon
  if (!geometry_msgs__msg__Polygon__are_equal(
      &(lhs->polygon), &(rhs->polygon)))
  {
    return false;
  }
  return true;
}

bool
shape_msgs__msg__SolidPrimitive__copy(
  const shape_msgs__msg__SolidPrimitive * input,
  shape_msgs__msg__SolidPrimitive * output)
{
  if (!input || !output) {
    return false;
  }
  // type
  output->type = input->type;
  // dimensions
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->dimensions), &(output->dimensions)))
  {
    return false;
  }
  // polygon
  if (!geometry_msgs__msg__Polygon__copy(
      &(input->polygon), &(output->polygon)))
  {
    return false;
  }
  return true;
}

shape_msgs__msg__SolidPrimitive *
shape_msgs__msg__SolidPrimitive__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__SolidPrimitive * msg = (shape_msgs__msg__SolidPrimitive *)allocator.allocate(sizeof(shape_msgs__msg__SolidPrimitive), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(shape_msgs__msg__SolidPrimitive));
  bool success = shape_msgs__msg__SolidPrimitive__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
shape_msgs__msg__SolidPrimitive__destroy(shape_msgs__msg__SolidPrimitive * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    shape_msgs__msg__SolidPrimitive__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
shape_msgs__msg__SolidPrimitive__Sequence__init(shape_msgs__msg__SolidPrimitive__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__SolidPrimitive * data = NULL;

  if (size) {
    data = (shape_msgs__msg__SolidPrimitive *)allocator.zero_allocate(size, sizeof(shape_msgs__msg__SolidPrimitive), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = shape_msgs__msg__SolidPrimitive__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        shape_msgs__msg__SolidPrimitive__fini(&data[i - 1]);
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
shape_msgs__msg__SolidPrimitive__Sequence__fini(shape_msgs__msg__SolidPrimitive__Sequence * array)
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
      shape_msgs__msg__SolidPrimitive__fini(&array->data[i]);
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

shape_msgs__msg__SolidPrimitive__Sequence *
shape_msgs__msg__SolidPrimitive__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  shape_msgs__msg__SolidPrimitive__Sequence * array = (shape_msgs__msg__SolidPrimitive__Sequence *)allocator.allocate(sizeof(shape_msgs__msg__SolidPrimitive__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = shape_msgs__msg__SolidPrimitive__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
shape_msgs__msg__SolidPrimitive__Sequence__destroy(shape_msgs__msg__SolidPrimitive__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    shape_msgs__msg__SolidPrimitive__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
shape_msgs__msg__SolidPrimitive__Sequence__are_equal(const shape_msgs__msg__SolidPrimitive__Sequence * lhs, const shape_msgs__msg__SolidPrimitive__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!shape_msgs__msg__SolidPrimitive__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
shape_msgs__msg__SolidPrimitive__Sequence__copy(
  const shape_msgs__msg__SolidPrimitive__Sequence * input,
  shape_msgs__msg__SolidPrimitive__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(shape_msgs__msg__SolidPrimitive);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    shape_msgs__msg__SolidPrimitive * data =
      (shape_msgs__msg__SolidPrimitive *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!shape_msgs__msg__SolidPrimitive__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          shape_msgs__msg__SolidPrimitive__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!shape_msgs__msg__SolidPrimitive__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
