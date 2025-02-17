// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from nav_msgs:msg/OccupancyGrid.idl
// generated code does not contain a copyright notice
#include "nav_msgs/msg/detail/occupancy_grid__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `info`
#include "nav_msgs/msg/detail/map_meta_data__functions.h"
// Member `data`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
nav_msgs__msg__OccupancyGrid__init(nav_msgs__msg__OccupancyGrid * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    nav_msgs__msg__OccupancyGrid__fini(msg);
    return false;
  }
  // info
  if (!nav_msgs__msg__MapMetaData__init(&msg->info)) {
    nav_msgs__msg__OccupancyGrid__fini(msg);
    return false;
  }
  // data
  if (!rosidl_runtime_c__int8__Sequence__init(&msg->data, 0)) {
    nav_msgs__msg__OccupancyGrid__fini(msg);
    return false;
  }
  return true;
}

void
nav_msgs__msg__OccupancyGrid__fini(nav_msgs__msg__OccupancyGrid * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // info
  nav_msgs__msg__MapMetaData__fini(&msg->info);
  // data
  rosidl_runtime_c__int8__Sequence__fini(&msg->data);
}

bool
nav_msgs__msg__OccupancyGrid__are_equal(const nav_msgs__msg__OccupancyGrid * lhs, const nav_msgs__msg__OccupancyGrid * rhs)
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
  // info
  if (!nav_msgs__msg__MapMetaData__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // data
  if (!rosidl_runtime_c__int8__Sequence__are_equal(
      &(lhs->data), &(rhs->data)))
  {
    return false;
  }
  return true;
}

bool
nav_msgs__msg__OccupancyGrid__copy(
  const nav_msgs__msg__OccupancyGrid * input,
  nav_msgs__msg__OccupancyGrid * output)
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
  // info
  if (!nav_msgs__msg__MapMetaData__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // data
  if (!rosidl_runtime_c__int8__Sequence__copy(
      &(input->data), &(output->data)))
  {
    return false;
  }
  return true;
}

nav_msgs__msg__OccupancyGrid *
nav_msgs__msg__OccupancyGrid__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__OccupancyGrid * msg = (nav_msgs__msg__OccupancyGrid *)allocator.allocate(sizeof(nav_msgs__msg__OccupancyGrid), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(nav_msgs__msg__OccupancyGrid));
  bool success = nav_msgs__msg__OccupancyGrid__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
nav_msgs__msg__OccupancyGrid__destroy(nav_msgs__msg__OccupancyGrid * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    nav_msgs__msg__OccupancyGrid__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
nav_msgs__msg__OccupancyGrid__Sequence__init(nav_msgs__msg__OccupancyGrid__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__OccupancyGrid * data = NULL;

  if (size) {
    data = (nav_msgs__msg__OccupancyGrid *)allocator.zero_allocate(size, sizeof(nav_msgs__msg__OccupancyGrid), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = nav_msgs__msg__OccupancyGrid__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        nav_msgs__msg__OccupancyGrid__fini(&data[i - 1]);
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
nav_msgs__msg__OccupancyGrid__Sequence__fini(nav_msgs__msg__OccupancyGrid__Sequence * array)
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
      nav_msgs__msg__OccupancyGrid__fini(&array->data[i]);
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

nav_msgs__msg__OccupancyGrid__Sequence *
nav_msgs__msg__OccupancyGrid__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__OccupancyGrid__Sequence * array = (nav_msgs__msg__OccupancyGrid__Sequence *)allocator.allocate(sizeof(nav_msgs__msg__OccupancyGrid__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = nav_msgs__msg__OccupancyGrid__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
nav_msgs__msg__OccupancyGrid__Sequence__destroy(nav_msgs__msg__OccupancyGrid__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    nav_msgs__msg__OccupancyGrid__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
nav_msgs__msg__OccupancyGrid__Sequence__are_equal(const nav_msgs__msg__OccupancyGrid__Sequence * lhs, const nav_msgs__msg__OccupancyGrid__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!nav_msgs__msg__OccupancyGrid__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
nav_msgs__msg__OccupancyGrid__Sequence__copy(
  const nav_msgs__msg__OccupancyGrid__Sequence * input,
  nav_msgs__msg__OccupancyGrid__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(nav_msgs__msg__OccupancyGrid);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    nav_msgs__msg__OccupancyGrid * data =
      (nav_msgs__msg__OccupancyGrid *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!nav_msgs__msg__OccupancyGrid__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          nav_msgs__msg__OccupancyGrid__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!nav_msgs__msg__OccupancyGrid__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
