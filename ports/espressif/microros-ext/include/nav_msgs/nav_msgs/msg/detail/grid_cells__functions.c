// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from nav_msgs:msg/GridCells.idl
// generated code does not contain a copyright notice
#include "nav_msgs/msg/detail/grid_cells__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `cells`
#include "geometry_msgs/msg/detail/point__functions.h"

bool
nav_msgs__msg__GridCells__init(nav_msgs__msg__GridCells * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    nav_msgs__msg__GridCells__fini(msg);
    return false;
  }
  // cell_width
  // cell_height
  // cells
  if (!geometry_msgs__msg__Point__Sequence__init(&msg->cells, 0)) {
    nav_msgs__msg__GridCells__fini(msg);
    return false;
  }
  return true;
}

void
nav_msgs__msg__GridCells__fini(nav_msgs__msg__GridCells * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // cell_width
  // cell_height
  // cells
  geometry_msgs__msg__Point__Sequence__fini(&msg->cells);
}

bool
nav_msgs__msg__GridCells__are_equal(const nav_msgs__msg__GridCells * lhs, const nav_msgs__msg__GridCells * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // cell_width
  if (lhs->cell_width != rhs->cell_width) {
    return false;
  }
  // cell_height
  if (lhs->cell_height != rhs->cell_height) {
    return false;
  }
  // cells
  if (!geometry_msgs__msg__Point__Sequence__are_equal(
      &(lhs->cells), &(rhs->cells)))
  {
    return false;
  }
  return true;
}

bool
nav_msgs__msg__GridCells__copy(
  const nav_msgs__msg__GridCells * input,
  nav_msgs__msg__GridCells * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // cell_width
  output->cell_width = input->cell_width;
  // cell_height
  output->cell_height = input->cell_height;
  // cells
  if (!geometry_msgs__msg__Point__Sequence__copy(
      &(input->cells), &(output->cells)))
  {
    return false;
  }
  return true;
}

nav_msgs__msg__GridCells *
nav_msgs__msg__GridCells__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__GridCells * msg = (nav_msgs__msg__GridCells *)allocator.allocate(sizeof(nav_msgs__msg__GridCells), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(nav_msgs__msg__GridCells));
  bool success = nav_msgs__msg__GridCells__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
nav_msgs__msg__GridCells__destroy(nav_msgs__msg__GridCells * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    nav_msgs__msg__GridCells__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
nav_msgs__msg__GridCells__Sequence__init(nav_msgs__msg__GridCells__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__GridCells * data = NULL;

  if (size) {
    data = (nav_msgs__msg__GridCells *)allocator.zero_allocate(size, sizeof(nav_msgs__msg__GridCells), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = nav_msgs__msg__GridCells__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        nav_msgs__msg__GridCells__fini(&data[i - 1]);
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
nav_msgs__msg__GridCells__Sequence__fini(nav_msgs__msg__GridCells__Sequence * array)
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
      nav_msgs__msg__GridCells__fini(&array->data[i]);
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

nav_msgs__msg__GridCells__Sequence *
nav_msgs__msg__GridCells__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__GridCells__Sequence * array = (nav_msgs__msg__GridCells__Sequence *)allocator.allocate(sizeof(nav_msgs__msg__GridCells__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = nav_msgs__msg__GridCells__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
nav_msgs__msg__GridCells__Sequence__destroy(nav_msgs__msg__GridCells__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    nav_msgs__msg__GridCells__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
nav_msgs__msg__GridCells__Sequence__are_equal(const nav_msgs__msg__GridCells__Sequence * lhs, const nav_msgs__msg__GridCells__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!nav_msgs__msg__GridCells__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
nav_msgs__msg__GridCells__Sequence__copy(
  const nav_msgs__msg__GridCells__Sequence * input,
  nav_msgs__msg__GridCells__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(nav_msgs__msg__GridCells);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    nav_msgs__msg__GridCells * data =
      (nav_msgs__msg__GridCells *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!nav_msgs__msg__GridCells__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          nav_msgs__msg__GridCells__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!nav_msgs__msg__GridCells__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
