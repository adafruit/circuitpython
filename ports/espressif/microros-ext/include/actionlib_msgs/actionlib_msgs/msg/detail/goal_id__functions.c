// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from actionlib_msgs:msg/GoalID.idl
// generated code does not contain a copyright notice
#include "actionlib_msgs/msg/detail/goal_id__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"
// Member `id`
#include "rosidl_runtime_c/string_functions.h"

bool
actionlib_msgs__msg__GoalID__init(actionlib_msgs__msg__GoalID * msg)
{
  if (!msg) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    actionlib_msgs__msg__GoalID__fini(msg);
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    actionlib_msgs__msg__GoalID__fini(msg);
    return false;
  }
  return true;
}

void
actionlib_msgs__msg__GoalID__fini(actionlib_msgs__msg__GoalID * msg)
{
  if (!msg) {
    return;
  }
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
  // id
  rosidl_runtime_c__String__fini(&msg->id);
}

bool
actionlib_msgs__msg__GoalID__are_equal(const actionlib_msgs__msg__GoalID * lhs, const actionlib_msgs__msg__GoalID * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  return true;
}

bool
actionlib_msgs__msg__GoalID__copy(
  const actionlib_msgs__msg__GoalID * input,
  actionlib_msgs__msg__GoalID * output)
{
  if (!input || !output) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  return true;
}

actionlib_msgs__msg__GoalID *
actionlib_msgs__msg__GoalID__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalID * msg = (actionlib_msgs__msg__GoalID *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalID), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(actionlib_msgs__msg__GoalID));
  bool success = actionlib_msgs__msg__GoalID__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
actionlib_msgs__msg__GoalID__destroy(actionlib_msgs__msg__GoalID * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    actionlib_msgs__msg__GoalID__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
actionlib_msgs__msg__GoalID__Sequence__init(actionlib_msgs__msg__GoalID__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalID * data = NULL;

  if (size) {
    data = (actionlib_msgs__msg__GoalID *)allocator.zero_allocate(size, sizeof(actionlib_msgs__msg__GoalID), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = actionlib_msgs__msg__GoalID__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        actionlib_msgs__msg__GoalID__fini(&data[i - 1]);
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
actionlib_msgs__msg__GoalID__Sequence__fini(actionlib_msgs__msg__GoalID__Sequence * array)
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
      actionlib_msgs__msg__GoalID__fini(&array->data[i]);
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

actionlib_msgs__msg__GoalID__Sequence *
actionlib_msgs__msg__GoalID__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  actionlib_msgs__msg__GoalID__Sequence * array = (actionlib_msgs__msg__GoalID__Sequence *)allocator.allocate(sizeof(actionlib_msgs__msg__GoalID__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = actionlib_msgs__msg__GoalID__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
actionlib_msgs__msg__GoalID__Sequence__destroy(actionlib_msgs__msg__GoalID__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    actionlib_msgs__msg__GoalID__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
actionlib_msgs__msg__GoalID__Sequence__are_equal(const actionlib_msgs__msg__GoalID__Sequence * lhs, const actionlib_msgs__msg__GoalID__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!actionlib_msgs__msg__GoalID__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
actionlib_msgs__msg__GoalID__Sequence__copy(
  const actionlib_msgs__msg__GoalID__Sequence * input,
  actionlib_msgs__msg__GoalID__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(actionlib_msgs__msg__GoalID);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    actionlib_msgs__msg__GoalID * data =
      (actionlib_msgs__msg__GoalID *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!actionlib_msgs__msg__GoalID__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          actionlib_msgs__msg__GoalID__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!actionlib_msgs__msg__GoalID__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
