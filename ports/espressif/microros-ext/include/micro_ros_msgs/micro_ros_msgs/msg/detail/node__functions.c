// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from micro_ros_msgs:msg/Node.idl
// generated code does not contain a copyright notice
#include "micro_ros_msgs/msg/detail/node__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `node_namespace`
// Member `node_name`
#include "rosidl_runtime_c/string_functions.h"
// Member `entities`
#include "micro_ros_msgs/msg/detail/entity__functions.h"

bool
micro_ros_msgs__msg__Node__init(micro_ros_msgs__msg__Node * msg)
{
  if (!msg) {
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__init(&msg->node_namespace)) {
    micro_ros_msgs__msg__Node__fini(msg);
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__init(&msg->node_name)) {
    micro_ros_msgs__msg__Node__fini(msg);
    return false;
  }
  // entities
  if (!micro_ros_msgs__msg__Entity__Sequence__init(&msg->entities, 0)) {
    micro_ros_msgs__msg__Node__fini(msg);
    return false;
  }
  return true;
}

void
micro_ros_msgs__msg__Node__fini(micro_ros_msgs__msg__Node * msg)
{
  if (!msg) {
    return;
  }
  // node_namespace
  rosidl_runtime_c__String__fini(&msg->node_namespace);
  // node_name
  rosidl_runtime_c__String__fini(&msg->node_name);
  // entities
  micro_ros_msgs__msg__Entity__Sequence__fini(&msg->entities);
}

bool
micro_ros_msgs__msg__Node__are_equal(const micro_ros_msgs__msg__Node * lhs, const micro_ros_msgs__msg__Node * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->node_namespace), &(rhs->node_namespace)))
  {
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->node_name), &(rhs->node_name)))
  {
    return false;
  }
  // entities
  if (!micro_ros_msgs__msg__Entity__Sequence__are_equal(
      &(lhs->entities), &(rhs->entities)))
  {
    return false;
  }
  return true;
}

bool
micro_ros_msgs__msg__Node__copy(
  const micro_ros_msgs__msg__Node * input,
  micro_ros_msgs__msg__Node * output)
{
  if (!input || !output) {
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__copy(
      &(input->node_namespace), &(output->node_namespace)))
  {
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__copy(
      &(input->node_name), &(output->node_name)))
  {
    return false;
  }
  // entities
  if (!micro_ros_msgs__msg__Entity__Sequence__copy(
      &(input->entities), &(output->entities)))
  {
    return false;
  }
  return true;
}

micro_ros_msgs__msg__Node *
micro_ros_msgs__msg__Node__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Node * msg = (micro_ros_msgs__msg__Node *)allocator.allocate(sizeof(micro_ros_msgs__msg__Node), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(micro_ros_msgs__msg__Node));
  bool success = micro_ros_msgs__msg__Node__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
micro_ros_msgs__msg__Node__destroy(micro_ros_msgs__msg__Node * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    micro_ros_msgs__msg__Node__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
micro_ros_msgs__msg__Node__Sequence__init(micro_ros_msgs__msg__Node__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Node * data = NULL;

  if (size) {
    data = (micro_ros_msgs__msg__Node *)allocator.zero_allocate(size, sizeof(micro_ros_msgs__msg__Node), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = micro_ros_msgs__msg__Node__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        micro_ros_msgs__msg__Node__fini(&data[i - 1]);
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
micro_ros_msgs__msg__Node__Sequence__fini(micro_ros_msgs__msg__Node__Sequence * array)
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
      micro_ros_msgs__msg__Node__fini(&array->data[i]);
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

micro_ros_msgs__msg__Node__Sequence *
micro_ros_msgs__msg__Node__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Node__Sequence * array = (micro_ros_msgs__msg__Node__Sequence *)allocator.allocate(sizeof(micro_ros_msgs__msg__Node__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = micro_ros_msgs__msg__Node__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
micro_ros_msgs__msg__Node__Sequence__destroy(micro_ros_msgs__msg__Node__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    micro_ros_msgs__msg__Node__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
micro_ros_msgs__msg__Node__Sequence__are_equal(const micro_ros_msgs__msg__Node__Sequence * lhs, const micro_ros_msgs__msg__Node__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!micro_ros_msgs__msg__Node__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
micro_ros_msgs__msg__Node__Sequence__copy(
  const micro_ros_msgs__msg__Node__Sequence * input,
  micro_ros_msgs__msg__Node__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(micro_ros_msgs__msg__Node);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    micro_ros_msgs__msg__Node * data =
      (micro_ros_msgs__msg__Node *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!micro_ros_msgs__msg__Node__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          micro_ros_msgs__msg__Node__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!micro_ros_msgs__msg__Node__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
