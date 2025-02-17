// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/JoyFeedback.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/joy_feedback__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
sensor_msgs__msg__JoyFeedback__init(sensor_msgs__msg__JoyFeedback * msg)
{
  if (!msg) {
    return false;
  }
  // type
  // id
  // intensity
  return true;
}

void
sensor_msgs__msg__JoyFeedback__fini(sensor_msgs__msg__JoyFeedback * msg)
{
  if (!msg) {
    return;
  }
  // type
  // id
  // intensity
}

bool
sensor_msgs__msg__JoyFeedback__are_equal(const sensor_msgs__msg__JoyFeedback * lhs, const sensor_msgs__msg__JoyFeedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // type
  if (lhs->type != rhs->type) {
    return false;
  }
  // id
  if (lhs->id != rhs->id) {
    return false;
  }
  // intensity
  if (lhs->intensity != rhs->intensity) {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__JoyFeedback__copy(
  const sensor_msgs__msg__JoyFeedback * input,
  sensor_msgs__msg__JoyFeedback * output)
{
  if (!input || !output) {
    return false;
  }
  // type
  output->type = input->type;
  // id
  output->id = input->id;
  // intensity
  output->intensity = input->intensity;
  return true;
}

sensor_msgs__msg__JoyFeedback *
sensor_msgs__msg__JoyFeedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedback * msg = (sensor_msgs__msg__JoyFeedback *)allocator.allocate(sizeof(sensor_msgs__msg__JoyFeedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__JoyFeedback));
  bool success = sensor_msgs__msg__JoyFeedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__JoyFeedback__destroy(sensor_msgs__msg__JoyFeedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__JoyFeedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__JoyFeedback__Sequence__init(sensor_msgs__msg__JoyFeedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedback * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__JoyFeedback *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__JoyFeedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__JoyFeedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__JoyFeedback__fini(&data[i - 1]);
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
sensor_msgs__msg__JoyFeedback__Sequence__fini(sensor_msgs__msg__JoyFeedback__Sequence * array)
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
      sensor_msgs__msg__JoyFeedback__fini(&array->data[i]);
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

sensor_msgs__msg__JoyFeedback__Sequence *
sensor_msgs__msg__JoyFeedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedback__Sequence * array = (sensor_msgs__msg__JoyFeedback__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__JoyFeedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__JoyFeedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__JoyFeedback__Sequence__destroy(sensor_msgs__msg__JoyFeedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__JoyFeedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__JoyFeedback__Sequence__are_equal(const sensor_msgs__msg__JoyFeedback__Sequence * lhs, const sensor_msgs__msg__JoyFeedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__JoyFeedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__JoyFeedback__Sequence__copy(
  const sensor_msgs__msg__JoyFeedback__Sequence * input,
  sensor_msgs__msg__JoyFeedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__JoyFeedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__JoyFeedback * data =
      (sensor_msgs__msg__JoyFeedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__JoyFeedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__JoyFeedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__JoyFeedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
