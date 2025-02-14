// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/PointCloud2.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/point_cloud2__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `fields`
#include "sensor_msgs/msg/detail/point_field__functions.h"
// Member `data`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
sensor_msgs__msg__PointCloud2__init(sensor_msgs__msg__PointCloud2 * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    sensor_msgs__msg__PointCloud2__fini(msg);
    return false;
  }
  // height
  // width
  // fields
  if (!sensor_msgs__msg__PointField__Sequence__init(&msg->fields, 0)) {
    sensor_msgs__msg__PointCloud2__fini(msg);
    return false;
  }
  // is_bigendian
  // point_step
  // row_step
  // data
  if (!rosidl_runtime_c__uint8__Sequence__init(&msg->data, 0)) {
    sensor_msgs__msg__PointCloud2__fini(msg);
    return false;
  }
  // is_dense
  return true;
}

void
sensor_msgs__msg__PointCloud2__fini(sensor_msgs__msg__PointCloud2 * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // height
  // width
  // fields
  sensor_msgs__msg__PointField__Sequence__fini(&msg->fields);
  // is_bigendian
  // point_step
  // row_step
  // data
  rosidl_runtime_c__uint8__Sequence__fini(&msg->data);
  // is_dense
}

bool
sensor_msgs__msg__PointCloud2__are_equal(const sensor_msgs__msg__PointCloud2 * lhs, const sensor_msgs__msg__PointCloud2 * rhs)
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
  // fields
  if (!sensor_msgs__msg__PointField__Sequence__are_equal(
      &(lhs->fields), &(rhs->fields)))
  {
    return false;
  }
  // is_bigendian
  if (lhs->is_bigendian != rhs->is_bigendian) {
    return false;
  }
  // point_step
  if (lhs->point_step != rhs->point_step) {
    return false;
  }
  // row_step
  if (lhs->row_step != rhs->row_step) {
    return false;
  }
  // data
  if (!rosidl_runtime_c__uint8__Sequence__are_equal(
      &(lhs->data), &(rhs->data)))
  {
    return false;
  }
  // is_dense
  if (lhs->is_dense != rhs->is_dense) {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__PointCloud2__copy(
  const sensor_msgs__msg__PointCloud2 * input,
  sensor_msgs__msg__PointCloud2 * output)
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
  // fields
  if (!sensor_msgs__msg__PointField__Sequence__copy(
      &(input->fields), &(output->fields)))
  {
    return false;
  }
  // is_bigendian
  output->is_bigendian = input->is_bigendian;
  // point_step
  output->point_step = input->point_step;
  // row_step
  output->row_step = input->row_step;
  // data
  if (!rosidl_runtime_c__uint8__Sequence__copy(
      &(input->data), &(output->data)))
  {
    return false;
  }
  // is_dense
  output->is_dense = input->is_dense;
  return true;
}

sensor_msgs__msg__PointCloud2 *
sensor_msgs__msg__PointCloud2__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__PointCloud2 * msg = (sensor_msgs__msg__PointCloud2 *)allocator.allocate(sizeof(sensor_msgs__msg__PointCloud2), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__PointCloud2));
  bool success = sensor_msgs__msg__PointCloud2__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__PointCloud2__destroy(sensor_msgs__msg__PointCloud2 * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__PointCloud2__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__PointCloud2__Sequence__init(sensor_msgs__msg__PointCloud2__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__PointCloud2 * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__PointCloud2 *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__PointCloud2), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__PointCloud2__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__PointCloud2__fini(&data[i - 1]);
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
sensor_msgs__msg__PointCloud2__Sequence__fini(sensor_msgs__msg__PointCloud2__Sequence * array)
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
      sensor_msgs__msg__PointCloud2__fini(&array->data[i]);
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

sensor_msgs__msg__PointCloud2__Sequence *
sensor_msgs__msg__PointCloud2__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__PointCloud2__Sequence * array = (sensor_msgs__msg__PointCloud2__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__PointCloud2__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__PointCloud2__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__PointCloud2__Sequence__destroy(sensor_msgs__msg__PointCloud2__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__PointCloud2__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__PointCloud2__Sequence__are_equal(const sensor_msgs__msg__PointCloud2__Sequence * lhs, const sensor_msgs__msg__PointCloud2__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__PointCloud2__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__PointCloud2__Sequence__copy(
  const sensor_msgs__msg__PointCloud2__Sequence * input,
  sensor_msgs__msg__PointCloud2__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__PointCloud2);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__PointCloud2 * data =
      (sensor_msgs__msg__PointCloud2 *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__PointCloud2__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__PointCloud2__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__PointCloud2__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
