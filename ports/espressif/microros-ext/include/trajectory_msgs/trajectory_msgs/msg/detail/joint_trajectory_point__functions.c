// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from trajectory_msgs:msg/JointTrajectoryPoint.idl
// generated code does not contain a copyright notice
#include "trajectory_msgs/msg/detail/joint_trajectory_point__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `positions`
// Member `velocities`
// Member `accelerations`
// Member `effort`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `time_from_start`
#include "builtin_interfaces/msg/detail/duration__functions.h"

bool
trajectory_msgs__msg__JointTrajectoryPoint__init(trajectory_msgs__msg__JointTrajectoryPoint * msg)
{
  if (!msg) {
    return false;
  }
  // positions
  if (!rosidl_runtime_c__double__Sequence__init(&msg->positions, 0)) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
    return false;
  }
  // velocities
  if (!rosidl_runtime_c__double__Sequence__init(&msg->velocities, 0)) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
    return false;
  }
  // accelerations
  if (!rosidl_runtime_c__double__Sequence__init(&msg->accelerations, 0)) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
    return false;
  }
  // effort
  if (!rosidl_runtime_c__double__Sequence__init(&msg->effort, 0)) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
    return false;
  }
  // time_from_start
  if (!builtin_interfaces__msg__Duration__init(&msg->time_from_start)) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
    return false;
  }
  return true;
}

void
trajectory_msgs__msg__JointTrajectoryPoint__fini(trajectory_msgs__msg__JointTrajectoryPoint * msg)
{
  if (!msg) {
    return;
  }
  // positions
  rosidl_runtime_c__double__Sequence__fini(&msg->positions);
  // velocities
  rosidl_runtime_c__double__Sequence__fini(&msg->velocities);
  // accelerations
  rosidl_runtime_c__double__Sequence__fini(&msg->accelerations);
  // effort
  rosidl_runtime_c__double__Sequence__fini(&msg->effort);
  // time_from_start
  builtin_interfaces__msg__Duration__fini(&msg->time_from_start);
}

bool
trajectory_msgs__msg__JointTrajectoryPoint__are_equal(const trajectory_msgs__msg__JointTrajectoryPoint * lhs, const trajectory_msgs__msg__JointTrajectoryPoint * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // positions
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->positions), &(rhs->positions)))
  {
    return false;
  }
  // velocities
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->velocities), &(rhs->velocities)))
  {
    return false;
  }
  // accelerations
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->accelerations), &(rhs->accelerations)))
  {
    return false;
  }
  // effort
  if (!rosidl_runtime_c__double__Sequence__are_equal(
      &(lhs->effort), &(rhs->effort)))
  {
    return false;
  }
  // time_from_start
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->time_from_start), &(rhs->time_from_start)))
  {
    return false;
  }
  return true;
}

bool
trajectory_msgs__msg__JointTrajectoryPoint__copy(
  const trajectory_msgs__msg__JointTrajectoryPoint * input,
  trajectory_msgs__msg__JointTrajectoryPoint * output)
{
  if (!input || !output) {
    return false;
  }
  // positions
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->positions), &(output->positions)))
  {
    return false;
  }
  // velocities
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->velocities), &(output->velocities)))
  {
    return false;
  }
  // accelerations
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->accelerations), &(output->accelerations)))
  {
    return false;
  }
  // effort
  if (!rosidl_runtime_c__double__Sequence__copy(
      &(input->effort), &(output->effort)))
  {
    return false;
  }
  // time_from_start
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->time_from_start), &(output->time_from_start)))
  {
    return false;
  }
  return true;
}

trajectory_msgs__msg__JointTrajectoryPoint *
trajectory_msgs__msg__JointTrajectoryPoint__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__JointTrajectoryPoint * msg = (trajectory_msgs__msg__JointTrajectoryPoint *)allocator.allocate(sizeof(trajectory_msgs__msg__JointTrajectoryPoint), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(trajectory_msgs__msg__JointTrajectoryPoint));
  bool success = trajectory_msgs__msg__JointTrajectoryPoint__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
trajectory_msgs__msg__JointTrajectoryPoint__destroy(trajectory_msgs__msg__JointTrajectoryPoint * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    trajectory_msgs__msg__JointTrajectoryPoint__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__init(trajectory_msgs__msg__JointTrajectoryPoint__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__JointTrajectoryPoint * data = NULL;

  if (size) {
    data = (trajectory_msgs__msg__JointTrajectoryPoint *)allocator.zero_allocate(size, sizeof(trajectory_msgs__msg__JointTrajectoryPoint), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = trajectory_msgs__msg__JointTrajectoryPoint__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        trajectory_msgs__msg__JointTrajectoryPoint__fini(&data[i - 1]);
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
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__fini(trajectory_msgs__msg__JointTrajectoryPoint__Sequence * array)
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
      trajectory_msgs__msg__JointTrajectoryPoint__fini(&array->data[i]);
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

trajectory_msgs__msg__JointTrajectoryPoint__Sequence *
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__JointTrajectoryPoint__Sequence * array = (trajectory_msgs__msg__JointTrajectoryPoint__Sequence *)allocator.allocate(sizeof(trajectory_msgs__msg__JointTrajectoryPoint__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = trajectory_msgs__msg__JointTrajectoryPoint__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__destroy(trajectory_msgs__msg__JointTrajectoryPoint__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    trajectory_msgs__msg__JointTrajectoryPoint__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__are_equal(const trajectory_msgs__msg__JointTrajectoryPoint__Sequence * lhs, const trajectory_msgs__msg__JointTrajectoryPoint__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!trajectory_msgs__msg__JointTrajectoryPoint__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
trajectory_msgs__msg__JointTrajectoryPoint__Sequence__copy(
  const trajectory_msgs__msg__JointTrajectoryPoint__Sequence * input,
  trajectory_msgs__msg__JointTrajectoryPoint__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(trajectory_msgs__msg__JointTrajectoryPoint);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    trajectory_msgs__msg__JointTrajectoryPoint * data =
      (trajectory_msgs__msg__JointTrajectoryPoint *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!trajectory_msgs__msg__JointTrajectoryPoint__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          trajectory_msgs__msg__JointTrajectoryPoint__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!trajectory_msgs__msg__JointTrajectoryPoint__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
