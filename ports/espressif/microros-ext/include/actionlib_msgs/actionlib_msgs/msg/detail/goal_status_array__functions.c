// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actionlib_msgs:msg/GoalStatusArray.idl
// generated code does not contain a copyright notice
#include "actionlib_msgs/msg/detail/goal_status_array__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `status_list`
#include "actionlib_msgs/msg/detail/goal_status__functions.h"

bool
actionlib_msgs__msg__GoalStatusArray__init(actionlib_msgs__msg__GoalStatusArray * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    actionlib_msgs__msg__GoalStatusArray__fini(msg);
    return false;
  }
  // status_list
  if (!actionlib_msgs__msg__GoalStatus__Sequence__init(&msg->status_list, 0)) {
    actionlib_msgs__msg__GoalStatusArray__fini(msg);
    return false;
  }
  return true;
}

void
actionlib_msgs__msg__GoalStatusArray__fini(actionlib_msgs__msg__GoalStatusArray * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // status_list
  actionlib_msgs__msg__GoalStatus__Sequence__fini(&msg->status_list);
}

bool
actionlib_msgs__msg__GoalStatusArray__are_equal(const actionlib_msgs__msg__GoalStatusArray * lhs, const actionlib_msgs__msg__GoalStatusArray * rhs)
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
  // status_list
  if (!actionlib_msgs__msg__GoalStatus__Sequence__are_equal(
      &(lhs->status_list), &(rhs->status_list)))
  {
    return false;
  }
  return true;
}

bool
actionlib_msgs__msg__GoalStatusArray__copy(
  const actionlib_msgs__msg__GoalStatusArray * input,
  actionlib_msgs__msg__GoalStatusArray * output)
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
  // status_list
  if (!actionlib_msgs__msg__GoalStatus__Sequence__copy(
      &(input->status_list), &(output->status_list)))
  {
    return false;
  }
  return true;
}

actionlib_msgs__msg__GoalStatusArray *
actionlib_msgs__msg__GoalStatusArray__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatusArray * msg = (actionlib_msgs__msg__GoalStatusArray *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalStatusArray), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actionlib_msgs__msg__GoalStatusArray));
  bool success = actionlib_msgs__msg__GoalStatusArray__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actionlib_msgs__msg__GoalStatusArray__destroy(actionlib_msgs__msg__GoalStatusArray * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actionlib_msgs__msg__GoalStatusArray__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actionlib_msgs__msg__GoalStatusArray__Sequence__init(actionlib_msgs__msg__GoalStatusArray__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatusArray * data = NULL;

  if (size) {
    data = (actionlib_msgs__msg__GoalStatusArray *)allocator.zero_allocate(size, sizeof(actionlib_msgs__msg__GoalStatusArray), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actionlib_msgs__msg__GoalStatusArray__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actionlib_msgs__msg__GoalStatusArray__fini(&data[i - 1]);
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
actionlib_msgs__msg__GoalStatusArray__Sequence__fini(actionlib_msgs__msg__GoalStatusArray__Sequence * array)
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
      actionlib_msgs__msg__GoalStatusArray__fini(&array->data[i]);
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

actionlib_msgs__msg__GoalStatusArray__Sequence *
actionlib_msgs__msg__GoalStatusArray__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatusArray__Sequence * array = (actionlib_msgs__msg__GoalStatusArray__Sequence *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalStatusArray__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actionlib_msgs__msg__GoalStatusArray__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actionlib_msgs__msg__GoalStatusArray__Sequence__destroy(actionlib_msgs__msg__GoalStatusArray__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actionlib_msgs__msg__GoalStatusArray__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actionlib_msgs__msg__GoalStatusArray__Sequence__are_equal(const actionlib_msgs__msg__GoalStatusArray__Sequence * lhs, const actionlib_msgs__msg__GoalStatusArray__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actionlib_msgs__msg__GoalStatusArray__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actionlib_msgs__msg__GoalStatusArray__Sequence__copy(
  const actionlib_msgs__msg__GoalStatusArray__Sequence * input,
  actionlib_msgs__msg__GoalStatusArray__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actionlib_msgs__msg__GoalStatusArray);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actionlib_msgs__msg__GoalStatusArray * data =
      (actionlib_msgs__msg__GoalStatusArray *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actionlib_msgs__msg__GoalStatusArray__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actionlib_msgs__msg__GoalStatusArray__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actionlib_msgs__msg__GoalStatusArray__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
