// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/MagneticField.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/magnetic_field__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `magnetic_field`
#include "geometry_msgs/msg/detail/vector3__functions.h"

bool
sensor_msgs__msg__MagneticField__init(sensor_msgs__msg__MagneticField * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    sensor_msgs__msg__MagneticField__fini(msg);
    return false;
  }
  // magnetic_field
  if (!geometry_msgs__msg__Vector3__init(&msg->magnetic_field)) {
    sensor_msgs__msg__MagneticField__fini(msg);
    return false;
  }
  // magnetic_field_covariance
  return true;
}

void
sensor_msgs__msg__MagneticField__fini(sensor_msgs__msg__MagneticField * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // magnetic_field
  geometry_msgs__msg__Vector3__fini(&msg->magnetic_field);
  // magnetic_field_covariance
}

bool
sensor_msgs__msg__MagneticField__are_equal(const sensor_msgs__msg__MagneticField * lhs, const sensor_msgs__msg__MagneticField * rhs)
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
  // magnetic_field
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->magnetic_field), &(rhs->magnetic_field)))
  {
    return false;
  }
  // magnetic_field_covariance
  for (size_t i = 0; i < 9; ++i) {
    if (lhs->magnetic_field_covariance[i] != rhs->magnetic_field_covariance[i]) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__MagneticField__copy(
  const sensor_msgs__msg__MagneticField * input,
  sensor_msgs__msg__MagneticField * output)
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
  // magnetic_field
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->magnetic_field), &(output->magnetic_field)))
  {
    return false;
  }
  // magnetic_field_covariance
  for (size_t i = 0; i < 9; ++i) {
    output->magnetic_field_covariance[i] = input->magnetic_field_covariance[i];
  }
  return true;
}

sensor_msgs__msg__MagneticField *
sensor_msgs__msg__MagneticField__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__MagneticField * msg = (sensor_msgs__msg__MagneticField *)allocator.allocate(sizeof(sensor_msgs__msg__MagneticField), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__MagneticField));
  bool success = sensor_msgs__msg__MagneticField__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__MagneticField__destroy(sensor_msgs__msg__MagneticField * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__MagneticField__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__MagneticField__Sequence__init(sensor_msgs__msg__MagneticField__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__MagneticField * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__MagneticField *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__MagneticField), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__MagneticField__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__MagneticField__fini(&data[i - 1]);
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
sensor_msgs__msg__MagneticField__Sequence__fini(sensor_msgs__msg__MagneticField__Sequence * array)
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
      sensor_msgs__msg__MagneticField__fini(&array->data[i]);
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

sensor_msgs__msg__MagneticField__Sequence *
sensor_msgs__msg__MagneticField__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__MagneticField__Sequence * array = (sensor_msgs__msg__MagneticField__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__MagneticField__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__MagneticField__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__MagneticField__Sequence__destroy(sensor_msgs__msg__MagneticField__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__MagneticField__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__MagneticField__Sequence__are_equal(const sensor_msgs__msg__MagneticField__Sequence * lhs, const sensor_msgs__msg__MagneticField__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__MagneticField__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__MagneticField__Sequence__copy(
  const sensor_msgs__msg__MagneticField__Sequence * input,
  sensor_msgs__msg__MagneticField__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__MagneticField);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__MagneticField * data =
      (sensor_msgs__msg__MagneticField *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__MagneticField__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__MagneticField__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__MagneticField__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
