// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/RegionOfInterest.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
sensor_msgs__msg__RegionOfInterest__init(sensor_msgs__msg__RegionOfInterest * msg)
{
  if (!msg) {
    return false;
  }
  // x_offset
  // y_offset
  // height
  // width
  // do_rectify
  return true;
}

void
sensor_msgs__msg__RegionOfInterest__fini(sensor_msgs__msg__RegionOfInterest * msg)
{
  if (!msg) {
    return;
  }
  // x_offset
  // y_offset
  // height
  // width
  // do_rectify
}

bool
sensor_msgs__msg__RegionOfInterest__are_equal(const sensor_msgs__msg__RegionOfInterest * lhs, const sensor_msgs__msg__RegionOfInterest * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // x_offset
  if (lhs->x_offset != rhs->x_offset) {
    return false;
  }
  // y_offset
  if (lhs->y_offset != rhs->y_offset) {
    return false;
  }
  // height
  if (lhs->height != rhs->height) {
    return false;
  }
  // width
  if (lhs->width != rhs->width) {
    return false;
  }
  // do_rectify
  if (lhs->do_rectify != rhs->do_rectify) {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__RegionOfInterest__copy(
  const sensor_msgs__msg__RegionOfInterest * input,
  sensor_msgs__msg__RegionOfInterest * output)
{
  if (!input || !output) {
    return false;
  }
  // x_offset
  output->x_offset = input->x_offset;
  // y_offset
  output->y_offset = input->y_offset;
  // height
  output->height = input->height;
  // width
  output->width = input->width;
  // do_rectify
  output->do_rectify = input->do_rectify;
  return true;
}

sensor_msgs__msg__RegionOfInterest *
sensor_msgs__msg__RegionOfInterest__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__RegionOfInterest * msg = (sensor_msgs__msg__RegionOfInterest *)allocator.allocate(sizeof(sensor_msgs__msg__RegionOfInterest), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__RegionOfInterest));
  bool success = sensor_msgs__msg__RegionOfInterest__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__RegionOfInterest__destroy(sensor_msgs__msg__RegionOfInterest * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__RegionOfInterest__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__RegionOfInterest__Sequence__init(sensor_msgs__msg__RegionOfInterest__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__RegionOfInterest * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__RegionOfInterest *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__RegionOfInterest), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__RegionOfInterest__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__RegionOfInterest__fini(&data[i - 1]);
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
sensor_msgs__msg__RegionOfInterest__Sequence__fini(sensor_msgs__msg__RegionOfInterest__Sequence * array)
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
      sensor_msgs__msg__RegionOfInterest__fini(&array->data[i]);
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

sensor_msgs__msg__RegionOfInterest__Sequence *
sensor_msgs__msg__RegionOfInterest__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__RegionOfInterest__Sequence * array = (sensor_msgs__msg__RegionOfInterest__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__RegionOfInterest__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__RegionOfInterest__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__RegionOfInterest__Sequence__destroy(sensor_msgs__msg__RegionOfInterest__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__RegionOfInterest__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__RegionOfInterest__Sequence__are_equal(const sensor_msgs__msg__RegionOfInterest__Sequence * lhs, const sensor_msgs__msg__RegionOfInterest__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__RegionOfInterest__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__RegionOfInterest__Sequence__copy(
  const sensor_msgs__msg__RegionOfInterest__Sequence * input,
  sensor_msgs__msg__RegionOfInterest__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__RegionOfInterest);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__RegionOfInterest * data =
      (sensor_msgs__msg__RegionOfInterest *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__RegionOfInterest__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__RegionOfInterest__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__RegionOfInterest__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
