// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/JoyFeedbackArray.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/joy_feedback_array__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `array`
#include "sensor_msgs/msg/detail/joy_feedback__functions.h"

bool
sensor_msgs__msg__JoyFeedbackArray__init(sensor_msgs__msg__JoyFeedbackArray * msg)
{
  if (!msg) {
    return false;
  }
  // array
  if (!sensor_msgs__msg__JoyFeedback__Sequence__init(&msg->array, 0)) {
    sensor_msgs__msg__JoyFeedbackArray__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__msg__JoyFeedbackArray__fini(sensor_msgs__msg__JoyFeedbackArray * msg)
{
  if (!msg) {
    return;
  }
  // array
  sensor_msgs__msg__JoyFeedback__Sequence__fini(&msg->array);
}

bool
sensor_msgs__msg__JoyFeedbackArray__are_equal(const sensor_msgs__msg__JoyFeedbackArray * lhs, const sensor_msgs__msg__JoyFeedbackArray * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // array
  if (!sensor_msgs__msg__JoyFeedback__Sequence__are_equal(
      &(lhs->array), &(rhs->array)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__JoyFeedbackArray__copy(
  const sensor_msgs__msg__JoyFeedbackArray * input,
  sensor_msgs__msg__JoyFeedbackArray * output)
{
  if (!input || !output) {
    return false;
  }
  // array
  if (!sensor_msgs__msg__JoyFeedback__Sequence__copy(
      &(input->array), &(output->array)))
  {
    return false;
  }
  return true;
}

sensor_msgs__msg__JoyFeedbackArray *
sensor_msgs__msg__JoyFeedbackArray__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedbackArray * msg = (sensor_msgs__msg__JoyFeedbackArray *)allocator.allocate(sizeof(sensor_msgs__msg__JoyFeedbackArray), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__JoyFeedbackArray));
  bool success = sensor_msgs__msg__JoyFeedbackArray__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__JoyFeedbackArray__destroy(sensor_msgs__msg__JoyFeedbackArray * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__JoyFeedbackArray__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__JoyFeedbackArray__Sequence__init(sensor_msgs__msg__JoyFeedbackArray__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedbackArray * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__JoyFeedbackArray *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__JoyFeedbackArray), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__JoyFeedbackArray__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__JoyFeedbackArray__fini(&data[i - 1]);
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
sensor_msgs__msg__JoyFeedbackArray__Sequence__fini(sensor_msgs__msg__JoyFeedbackArray__Sequence * array)
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
      sensor_msgs__msg__JoyFeedbackArray__fini(&array->data[i]);
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

sensor_msgs__msg__JoyFeedbackArray__Sequence *
sensor_msgs__msg__JoyFeedbackArray__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__JoyFeedbackArray__Sequence * array = (sensor_msgs__msg__JoyFeedbackArray__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__JoyFeedbackArray__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__JoyFeedbackArray__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__JoyFeedbackArray__Sequence__destroy(sensor_msgs__msg__JoyFeedbackArray__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__JoyFeedbackArray__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__JoyFeedbackArray__Sequence__are_equal(const sensor_msgs__msg__JoyFeedbackArray__Sequence * lhs, const sensor_msgs__msg__JoyFeedbackArray__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__JoyFeedbackArray__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__JoyFeedbackArray__Sequence__copy(
  const sensor_msgs__msg__JoyFeedbackArray__Sequence * input,
  sensor_msgs__msg__JoyFeedbackArray__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__JoyFeedbackArray);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__JoyFeedbackArray * data =
      (sensor_msgs__msg__JoyFeedbackArray *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__JoyFeedbackArray__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__JoyFeedbackArray__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__JoyFeedbackArray__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
