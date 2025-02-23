// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/TimeReference.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/time_reference__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `time_ref`
#include "builtin_interfaces/msg/detail/time__functions.h"
// Member `source`
#include "rosidl_runtime_c/string_functions.h"

bool
sensor_msgs__msg__TimeReference__init(sensor_msgs__msg__TimeReference * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    sensor_msgs__msg__TimeReference__fini(msg);
    return false;
  }
  // time_ref
  if (!builtin_interfaces__msg__Time__init(&msg->time_ref)) {
    sensor_msgs__msg__TimeReference__fini(msg);
    return false;
  }
  // source
  if (!rosidl_runtime_c__String__init(&msg->source)) {
    sensor_msgs__msg__TimeReference__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__msg__TimeReference__fini(sensor_msgs__msg__TimeReference * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // time_ref
  builtin_interfaces__msg__Time__fini(&msg->time_ref);
  // source
  rosidl_runtime_c__String__fini(&msg->source);
}

bool
sensor_msgs__msg__TimeReference__are_equal(const sensor_msgs__msg__TimeReference * lhs, const sensor_msgs__msg__TimeReference * rhs)
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
  // time_ref
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->time_ref), &(rhs->time_ref)))
  {
    return false;
  }
  // source
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->source), &(rhs->source)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__TimeReference__copy(
  const sensor_msgs__msg__TimeReference * input,
  sensor_msgs__msg__TimeReference * output)
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
  // time_ref
  if (!builtin_interfaces__msg__Time__copy(
      &(input->time_ref), &(output->time_ref)))
  {
    return false;
  }
  // source
  if (!rosidl_runtime_c__String__copy(
      &(input->source), &(output->source)))
  {
    return false;
  }
  return true;
}

sensor_msgs__msg__TimeReference *
sensor_msgs__msg__TimeReference__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__TimeReference * msg = (sensor_msgs__msg__TimeReference *)allocator.allocate(sizeof(sensor_msgs__msg__TimeReference), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__TimeReference));
  bool success = sensor_msgs__msg__TimeReference__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__TimeReference__destroy(sensor_msgs__msg__TimeReference * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__TimeReference__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__TimeReference__Sequence__init(sensor_msgs__msg__TimeReference__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__TimeReference * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__TimeReference *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__TimeReference), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__TimeReference__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__TimeReference__fini(&data[i - 1]);
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
sensor_msgs__msg__TimeReference__Sequence__fini(sensor_msgs__msg__TimeReference__Sequence * array)
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
      sensor_msgs__msg__TimeReference__fini(&array->data[i]);
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

sensor_msgs__msg__TimeReference__Sequence *
sensor_msgs__msg__TimeReference__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__TimeReference__Sequence * array = (sensor_msgs__msg__TimeReference__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__TimeReference__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__TimeReference__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__TimeReference__Sequence__destroy(sensor_msgs__msg__TimeReference__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__TimeReference__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__TimeReference__Sequence__are_equal(const sensor_msgs__msg__TimeReference__Sequence * lhs, const sensor_msgs__msg__TimeReference__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__TimeReference__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__TimeReference__Sequence__copy(
  const sensor_msgs__msg__TimeReference__Sequence * input,
  sensor_msgs__msg__TimeReference__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__TimeReference);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__TimeReference * data =
      (sensor_msgs__msg__TimeReference *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__TimeReference__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__TimeReference__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__TimeReference__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
