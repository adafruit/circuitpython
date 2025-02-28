// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actionlib_msgs:msg/GoalStatus.idl
// generated code does not contain a copyright notice
#include "actionlib_msgs/msg/detail/goal_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `goal_id`
#include "actionlib_msgs/msg/detail/goal_id__functions.h"
// Member `text`
#include "rosidl_runtime_c/string_functions.h"

bool
actionlib_msgs__msg__GoalStatus__init(actionlib_msgs__msg__GoalStatus * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!actionlib_msgs__msg__GoalID__init(&msg->goal_id)) {
    actionlib_msgs__msg__GoalStatus__fini(msg);
    return false;
  }
  // status
  // text
  if (!rosidl_runtime_c__String__init(&msg->text)) {
    actionlib_msgs__msg__GoalStatus__fini(msg);
    return false;
  }
  return true;
}

void
actionlib_msgs__msg__GoalStatus__fini(actionlib_msgs__msg__GoalStatus * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  actionlib_msgs__msg__GoalID__fini(&msg->goal_id);
  // status
  // text
  rosidl_runtime_c__String__fini(&msg->text);
}

bool
actionlib_msgs__msg__GoalStatus__are_equal(const actionlib_msgs__msg__GoalStatus * lhs, const actionlib_msgs__msg__GoalStatus * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!actionlib_msgs__msg__GoalID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // status
  if (lhs->status != rhs->status) {
    return false;
  }
  // text
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->text), &(rhs->text)))
  {
    return false;
  }
  return true;
}

bool
actionlib_msgs__msg__GoalStatus__copy(
  const actionlib_msgs__msg__GoalStatus * input,
  actionlib_msgs__msg__GoalStatus * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!actionlib_msgs__msg__GoalID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // status
  output->status = input->status;
  // text
  if (!rosidl_runtime_c__String__copy(
      &(input->text), &(output->text)))
  {
    return false;
  }
  return true;
}

actionlib_msgs__msg__GoalStatus *
actionlib_msgs__msg__GoalStatus__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatus * msg = (actionlib_msgs__msg__GoalStatus *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalStatus), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actionlib_msgs__msg__GoalStatus));
  bool success = actionlib_msgs__msg__GoalStatus__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actionlib_msgs__msg__GoalStatus__destroy(actionlib_msgs__msg__GoalStatus * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actionlib_msgs__msg__GoalStatus__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actionlib_msgs__msg__GoalStatus__Sequence__init(actionlib_msgs__msg__GoalStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatus * data = NULL;

  if (size) {
    data = (actionlib_msgs__msg__GoalStatus *)allocator.zero_allocate(size, sizeof(actionlib_msgs__msg__GoalStatus), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actionlib_msgs__msg__GoalStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actionlib_msgs__msg__GoalStatus__fini(&data[i - 1]);
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
actionlib_msgs__msg__GoalStatus__Sequence__fini(actionlib_msgs__msg__GoalStatus__Sequence * array)
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
      actionlib_msgs__msg__GoalStatus__fini(&array->data[i]);
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

actionlib_msgs__msg__GoalStatus__Sequence *
actionlib_msgs__msg__GoalStatus__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalStatus__Sequence * array = (actionlib_msgs__msg__GoalStatus__Sequence *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalStatus__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actionlib_msgs__msg__GoalStatus__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actionlib_msgs__msg__GoalStatus__Sequence__destroy(actionlib_msgs__msg__GoalStatus__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actionlib_msgs__msg__GoalStatus__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actionlib_msgs__msg__GoalStatus__Sequence__are_equal(const actionlib_msgs__msg__GoalStatus__Sequence * lhs, const actionlib_msgs__msg__GoalStatus__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actionlib_msgs__msg__GoalStatus__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actionlib_msgs__msg__GoalStatus__Sequence__copy(
  const actionlib_msgs__msg__GoalStatus__Sequence * input,
  actionlib_msgs__msg__GoalStatus__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actionlib_msgs__msg__GoalStatus);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actionlib_msgs__msg__GoalStatus * data =
      (actionlib_msgs__msg__GoalStatus *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actionlib_msgs__msg__GoalStatus__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actionlib_msgs__msg__GoalStatus__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actionlib_msgs__msg__GoalStatus__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
