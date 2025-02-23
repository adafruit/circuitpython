// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from nav_msgs:msg/MapMetaData.idl
// generated code does not contain a copyright notice
#include "nav_msgs/msg/detail/map_meta_data__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `map_load_time`
#include "builtin_interfaces/msg/detail/time__functions.h"
// Member `origin`
#include "geometry_msgs/msg/detail/pose__functions.h"

bool
nav_msgs__msg__MapMetaData__init(nav_msgs__msg__MapMetaData * msg)
{
  if (!msg) {
    return false;
  }
  // map_load_time
  if (!builtin_interfaces__msg__Time__init(&msg->map_load_time)) {
    nav_msgs__msg__MapMetaData__fini(msg);
    return false;
  }
  // resolution
  // width
  // height
  // origin
  if (!geometry_msgs__msg__Pose__init(&msg->origin)) {
    nav_msgs__msg__MapMetaData__fini(msg);
    return false;
  }
  return true;
}

void
nav_msgs__msg__MapMetaData__fini(nav_msgs__msg__MapMetaData * msg)
{
  if (!msg) {
    return;
  }
  // map_load_time
  builtin_interfaces__msg__Time__fini(&msg->map_load_time);
  // resolution
  // width
  // height
  // origin
  geometry_msgs__msg__Pose__fini(&msg->origin);
}

bool
nav_msgs__msg__MapMetaData__are_equal(const nav_msgs__msg__MapMetaData * lhs, const nav_msgs__msg__MapMetaData * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // map_load_time
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->map_load_time), &(rhs->map_load_time)))
  {
    return false;
  }
  // resolution
  if (lhs->resolution != rhs->resolution) {
    return false;
  }
  // width
  if (lhs->width != rhs->width) {
    return false;
  }
  // height
  if (lhs->height != rhs->height) {
    return false;
  }
  // origin
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->origin), &(rhs->origin)))
  {
    return false;
  }
  return true;
}

bool
nav_msgs__msg__MapMetaData__copy(
  const nav_msgs__msg__MapMetaData * input,
  nav_msgs__msg__MapMetaData * output)
{
  if (!input || !output) {
    return false;
  }
  // map_load_time
  if (!builtin_interfaces__msg__Time__copy(
      &(input->map_load_time), &(output->map_load_time)))
  {
    return false;
  }
  // resolution
  output->resolution = input->resolution;
  // width
  output->width = input->width;
  // height
  output->height = input->height;
  // origin
  if (!geometry_msgs__msg__Pose__copy(
      &(input->origin), &(output->origin)))
  {
    return false;
  }
  return true;
}

nav_msgs__msg__MapMetaData *
nav_msgs__msg__MapMetaData__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__MapMetaData * msg = (nav_msgs__msg__MapMetaData *)allocator.allocate(sizeof(nav_msgs__msg__MapMetaData), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(nav_msgs__msg__MapMetaData));
  bool success = nav_msgs__msg__MapMetaData__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
nav_msgs__msg__MapMetaData__destroy(nav_msgs__msg__MapMetaData * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    nav_msgs__msg__MapMetaData__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
nav_msgs__msg__MapMetaData__Sequence__init(nav_msgs__msg__MapMetaData__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__MapMetaData * data = NULL;

  if (size) {
    data = (nav_msgs__msg__MapMetaData *)allocator.zero_allocate(size, sizeof(nav_msgs__msg__MapMetaData), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = nav_msgs__msg__MapMetaData__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        nav_msgs__msg__MapMetaData__fini(&data[i - 1]);
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
nav_msgs__msg__MapMetaData__Sequence__fini(nav_msgs__msg__MapMetaData__Sequence * array)
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
      nav_msgs__msg__MapMetaData__fini(&array->data[i]);
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

nav_msgs__msg__MapMetaData__Sequence *
nav_msgs__msg__MapMetaData__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  nav_msgs__msg__MapMetaData__Sequence * array = (nav_msgs__msg__MapMetaData__Sequence *)allocator.allocate(sizeof(nav_msgs__msg__MapMetaData__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = nav_msgs__msg__MapMetaData__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
nav_msgs__msg__MapMetaData__Sequence__destroy(nav_msgs__msg__MapMetaData__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    nav_msgs__msg__MapMetaData__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
nav_msgs__msg__MapMetaData__Sequence__are_equal(const nav_msgs__msg__MapMetaData__Sequence * lhs, const nav_msgs__msg__MapMetaData__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!nav_msgs__msg__MapMetaData__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
nav_msgs__msg__MapMetaData__Sequence__copy(
  const nav_msgs__msg__MapMetaData__Sequence * input,
  nav_msgs__msg__MapMetaData__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(nav_msgs__msg__MapMetaData);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    nav_msgs__msg__MapMetaData * data =
      (nav_msgs__msg__MapMetaData *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!nav_msgs__msg__MapMetaData__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          nav_msgs__msg__MapMetaData__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!nav_msgs__msg__MapMetaData__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
