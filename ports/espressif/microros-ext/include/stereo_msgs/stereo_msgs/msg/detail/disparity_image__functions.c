// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from stereo_msgs:msg/DisparityImage.idl
// generated code does not contain a copyright notice
#include "stereo_msgs/msg/detail/disparity_image__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `image`
#include "sensor_msgs/msg/detail/image__functions.h"
// Member `valid_window`
#include "sensor_msgs/msg/detail/region_of_interest__functions.h"

bool
stereo_msgs__msg__DisparityImage__init(stereo_msgs__msg__DisparityImage * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    stereo_msgs__msg__DisparityImage__fini(msg);
    return false;
  }
  // image
  if (!sensor_msgs__msg__Image__init(&msg->image)) {
    stereo_msgs__msg__DisparityImage__fini(msg);
    return false;
  }
  // f
  // t
  // valid_window
  if (!sensor_msgs__msg__RegionOfInterest__init(&msg->valid_window)) {
    stereo_msgs__msg__DisparityImage__fini(msg);
    return false;
  }
  // min_disparity
  // max_disparity
  // delta_d
  return true;
}

void
stereo_msgs__msg__DisparityImage__fini(stereo_msgs__msg__DisparityImage * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // image
  sensor_msgs__msg__Image__fini(&msg->image);
  // f
  // t
  // valid_window
  sensor_msgs__msg__RegionOfInterest__fini(&msg->valid_window);
  // min_disparity
  // max_disparity
  // delta_d
}

bool
stereo_msgs__msg__DisparityImage__are_equal(const stereo_msgs__msg__DisparityImage * lhs, const stereo_msgs__msg__DisparityImage * rhs)
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
  // image
  if (!sensor_msgs__msg__Image__are_equal(
      &(lhs->image), &(rhs->image)))
  {
    return false;
  }
  // f
  if (lhs->f != rhs->f) {
    return false;
  }
  // t
  if (lhs->t != rhs->t) {
    return false;
  }
  // valid_window
  if (!sensor_msgs__msg__RegionOfInterest__are_equal(
      &(lhs->valid_window), &(rhs->valid_window)))
  {
    return false;
  }
  // min_disparity
  if (lhs->min_disparity != rhs->min_disparity) {
    return false;
  }
  // max_disparity
  if (lhs->max_disparity != rhs->max_disparity) {
    return false;
  }
  // delta_d
  if (lhs->delta_d != rhs->delta_d) {
    return false;
  }
  return true;
}

bool
stereo_msgs__msg__DisparityImage__copy(
  const stereo_msgs__msg__DisparityImage * input,
  stereo_msgs__msg__DisparityImage * output)
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
  // image
  if (!sensor_msgs__msg__Image__copy(
      &(input->image), &(output->image)))
  {
    return false;
  }
  // f
  output->f = input->f;
  // t
  output->t = input->t;
  // valid_window
  if (!sensor_msgs__msg__RegionOfInterest__copy(
      &(input->valid_window), &(output->valid_window)))
  {
    return false;
  }
  // min_disparity
  output->min_disparity = input->min_disparity;
  // max_disparity
  output->max_disparity = input->max_disparity;
  // delta_d
  output->delta_d = input->delta_d;
  return true;
}

stereo_msgs__msg__DisparityImage *
stereo_msgs__msg__DisparityImage__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  stereo_msgs__msg__DisparityImage * msg = (stereo_msgs__msg__DisparityImage *)allocator.allocate(sizeof(stereo_msgs__msg__DisparityImage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(stereo_msgs__msg__DisparityImage));
  bool success = stereo_msgs__msg__DisparityImage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
stereo_msgs__msg__DisparityImage__destroy(stereo_msgs__msg__DisparityImage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    stereo_msgs__msg__DisparityImage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
stereo_msgs__msg__DisparityImage__Sequence__init(stereo_msgs__msg__DisparityImage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  stereo_msgs__msg__DisparityImage * data = NULL;

  if (size) {
    data = (stereo_msgs__msg__DisparityImage *)allocator.zero_allocate(size, sizeof(stereo_msgs__msg__DisparityImage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = stereo_msgs__msg__DisparityImage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        stereo_msgs__msg__DisparityImage__fini(&data[i - 1]);
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
stereo_msgs__msg__DisparityImage__Sequence__fini(stereo_msgs__msg__DisparityImage__Sequence * array)
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
      stereo_msgs__msg__DisparityImage__fini(&array->data[i]);
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

stereo_msgs__msg__DisparityImage__Sequence *
stereo_msgs__msg__DisparityImage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  stereo_msgs__msg__DisparityImage__Sequence * array = (stereo_msgs__msg__DisparityImage__Sequence *)allocator.allocate(sizeof(stereo_msgs__msg__DisparityImage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = stereo_msgs__msg__DisparityImage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
stereo_msgs__msg__DisparityImage__Sequence__destroy(stereo_msgs__msg__DisparityImage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    stereo_msgs__msg__DisparityImage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
stereo_msgs__msg__DisparityImage__Sequence__are_equal(const stereo_msgs__msg__DisparityImage__Sequence * lhs, const stereo_msgs__msg__DisparityImage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!stereo_msgs__msg__DisparityImage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
stereo_msgs__msg__DisparityImage__Sequence__copy(
  const stereo_msgs__msg__DisparityImage__Sequence * input,
  stereo_msgs__msg__DisparityImage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(stereo_msgs__msg__DisparityImage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    stereo_msgs__msg__DisparityImage * data =
      (stereo_msgs__msg__DisparityImage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!stereo_msgs__msg__DisparityImage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          stereo_msgs__msg__DisparityImage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!stereo_msgs__msg__DisparityImage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
