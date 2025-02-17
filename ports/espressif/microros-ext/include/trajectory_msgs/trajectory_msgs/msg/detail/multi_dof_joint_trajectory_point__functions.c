// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from trajectory_msgs:msg/MultiDOFJointTrajectoryPoint.idl
// generated code does not contain a copyright notice
#include "trajectory_msgs/msg/detail/multi_dof_joint_trajectory_point__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `transforms`
#include "geometry_msgs/msg/detail/transform__functions.h"
// Member `velocities`
// Member `accelerations`
#include "geometry_msgs/msg/detail/twist__functions.h"
// Member `time_from_start`
#include "builtin_interfaces/msg/detail/duration__functions.h"

bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg)
{
  if (!msg) {
    return false;
  }
  // transforms
  if (!geometry_msgs__msg__Transform__Sequence__init(&msg->transforms, 0)) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(msg);
    return false;
  }
  // velocities
  if (!geometry_msgs__msg__Twist__Sequence__init(&msg->velocities, 0)) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(msg);
    return false;
  }
  // accelerations
  if (!geometry_msgs__msg__Twist__Sequence__init(&msg->accelerations, 0)) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(msg);
    return false;
  }
  // time_from_start
  if (!builtin_interfaces__msg__Duration__init(&msg->time_from_start)) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(msg);
    return false;
  }
  return true;
}

void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg)
{
  if (!msg) {
    return;
  }
  // transforms
  geometry_msgs__msg__Transform__Sequence__fini(&msg->transforms);
  // velocities
  geometry_msgs__msg__Twist__Sequence__fini(&msg->velocities);
  // accelerations
  geometry_msgs__msg__Twist__Sequence__fini(&msg->accelerations);
  // time_from_start
  builtin_interfaces__msg__Duration__fini(&msg->time_from_start);
}

bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__are_equal(const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * lhs, const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // transforms
  if (!geometry_msgs__msg__Transform__Sequence__are_equal(
      &(lhs->transforms), &(rhs->transforms)))
  {
    return false;
  }
  // velocities
  if (!geometry_msgs__msg__Twist__Sequence__are_equal(
      &(lhs->velocities), &(rhs->velocities)))
  {
    return false;
  }
  // accelerations
  if (!geometry_msgs__msg__Twist__Sequence__are_equal(
      &(lhs->accelerations), &(rhs->accelerations)))
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
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__copy(
  const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * input,
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * output)
{
  if (!input || !output) {
    return false;
  }
  // transforms
  if (!geometry_msgs__msg__Transform__Sequence__copy(
      &(input->transforms), &(output->transforms)))
  {
    return false;
  }
  // velocities
  if (!geometry_msgs__msg__Twist__Sequence__copy(
      &(input->velocities), &(output->velocities)))
  {
    return false;
  }
  // accelerations
  if (!geometry_msgs__msg__Twist__Sequence__copy(
      &(input->accelerations), &(output->accelerations)))
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

trajectory_msgs__msg__MultiDOFJointTrajectoryPoint *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg = (trajectory_msgs__msg__MultiDOFJointTrajectoryPoint *)allocator.allocate(sizeof(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint));
  bool success = trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__destroy(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__init(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * data = NULL;

  if (size) {
    data = (trajectory_msgs__msg__MultiDOFJointTrajectoryPoint *)allocator.zero_allocate(size, sizeof(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(&data[i - 1]);
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
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__fini(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array)
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
      trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(&array->data[i]);
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

trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array = (trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence *)allocator.allocate(sizeof(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__destroy(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__are_equal(const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * lhs, const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__copy(
  const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * input,
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * data =
      (trajectory_msgs__msg__MultiDOFJointTrajectoryPoint *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
