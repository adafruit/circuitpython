// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/CameraInfo.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/camera_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `distortion_model`
#include "rosidl_runtime_c/string_functions.h"
// Member `d`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `roi`
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"

bool
sensor_msgs__msg__CameraInfo__init(sensor_msgs__msg__CameraInfo * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    sensor_msgs__msg__CameraInfo__fini(msg);
    return false;
  }
  // height
  // width
  // distortion_model
  if (!rosidl_runtime_c__String__init(&msg->distortion_model)) {
    sensor_msgs__msg__CameraInfo__fini(msg);
    return false;
  }
  // d
  if (!rosidl_runtime_c__double__Sequence__init(&msg->d, 0)) {
    sensor_msgs__msg__CameraInfo__fini(msg);
    return false;
  }
  // k
  // r
  // p
  // binning_x
  // binning_y
  // roi
  if (!sensor_msgs__msg__RegionOfInterest__init(&msg->roi)) {
    sensor_msgs__msg__CameraInfo__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__msg__CameraInfo__fini(sensor_msgs__msg__CameraInfo * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // height
  // width
  // distortion_model
  rosidl_runtime_c__String__fini(&msg->distortion_model);
  // d
  rosidl_runtime_c__double__Sequence__fini(&msg->d);
  // k
  // r
  // p
  // binning_x
  // binning_y
  // roi
  sensor_msgs__msg__RegionOfInterest__fini(&msg->roi);
}

bool
sensor_msgs__msg__CameraInfo__are_equal(const sensor_msgs__msg__CameraInfo * lhs, const sensor_msgs__msg__CameraInfo * rhs)
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
  // height
  if (lhs->height != rhs->height) {
    return false;
  }
  // width
  if (lhs->width != rhs->width) {
    return false;
  }
  // distortion_model
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->distortion_model), &(rhs->distortion_model)))
  {
    return false;
  }
  // d
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->d), &(rhs->d)))
  {
    return false;
  }
  // k
  for (size_t i = 0; i < 9; ++i) {
    if (lhs->k[i] != rhs->k[i]) {
      return false;
    }
  }
  // r
  for (size_t i = 0; i < 9; ++i) {
    if (lhs->r[i] != rhs->r[i]) {
      return false;
    }
  }
  // p
  for (size_t i = 0; i < 12; ++i) {
    if (lhs->p[i] != rhs->p[i]) {
      return false;
    }
  }
  // binning_x
  if (lhs->binning_x != rhs->binning_x) {
    return false;
  }
  // binning_y
  if (lhs->binning_y != rhs->binning_y) {
    return false;
  }
  // roi
  if (!sensor_msgs__msg__RegionOfInterest__are_equal(
      &(lhs->roi), &(rhs->roi)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__CameraInfo__copy(
  const sensor_msgs__msg__CameraInfo * input,
  sensor_msgs__msg__CameraInfo * output)
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
  // height
  output->height = input->height;
  // width
  output->width = input->width;
  // distortion_model
  if (!rosidl_runtime_c__String__copy(
      &(input->distortion_model), &(output->distortion_model)))
  {
    return false;
  }
  // d
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->d), &(output->d)))
  {
    return false;
  }
  // k
  for (size_t i = 0; i < 9; ++i) {
    output->k[i] = input->k[i];
  }
  // r
  for (size_t i = 0; i < 9; ++i) {
    output->r[i] = input->r[i];
  }
  // p
  for (size_t i = 0; i < 12; ++i) {
    output->p[i] = input->p[i];
  }
  // binning_x
  output->binning_x = input->binning_x;
  // binning_y
  output->binning_y = input->binning_y;
  // roi
  if (!sensor_msgs__msg__RegionOfInterest__copy(
      &(input->roi), &(output->roi)))
  {
    return false;
  }
  return true;
}

sensor_msgs__msg__CameraInfo *
sensor_msgs__msg__CameraInfo__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__CameraInfo * msg = (sensor_msgs__msg__CameraInfo *)allocator.allocate(sizeof(sensor_msgs__msg__CameraInfo), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__CameraInfo));
  bool success = sensor_msgs__msg__CameraInfo__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__CameraInfo__destroy(sensor_msgs__msg__CameraInfo * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__CameraInfo__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__CameraInfo__Sequence__init(sensor_msgs__msg__CameraInfo__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__CameraInfo * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__CameraInfo *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__CameraInfo), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__CameraInfo__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__CameraInfo__fini(&data[i - 1]);
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
sensor_msgs__msg__CameraInfo__Sequence__fini(sensor_msgs__msg__CameraInfo__Sequence * array)
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
      sensor_msgs__msg__CameraInfo__fini(&array->data[i]);
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

sensor_msgs__msg__CameraInfo__Sequence *
sensor_msgs__msg__CameraInfo__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__CameraInfo__Sequence * array = (sensor_msgs__msg__CameraInfo__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__CameraInfo__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__CameraInfo__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__CameraInfo__Sequence__destroy(sensor_msgs__msg__CameraInfo__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__CameraInfo__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__CameraInfo__Sequence__are_equal(const sensor_msgs__msg__CameraInfo__Sequence * lhs, const sensor_msgs__msg__CameraInfo__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__CameraInfo__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__CameraInfo__Sequence__copy(
  const sensor_msgs__msg__CameraInfo__Sequence * input,
  sensor_msgs__msg__CameraInfo__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__CameraInfo);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__CameraInfo * data =
      (sensor_msgs__msg__CameraInfo *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__CameraInfo__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__CameraInfo__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__CameraInfo__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
