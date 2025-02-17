// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from micro_ros_msgs:msg/Entity.idl
// generated code does not contain a copyright notice
#include "micro_ros_msgs/msg/detail/entity__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
// Member `types`
#include "rosidl_runtime_c/string_functions.h"

bool
micro_ros_msgs__msg__Entity__init(micro_ros_msgs__msg__Entity * msg)
{
  if (!msg) {
    return false;
  }
  // entity_type
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    micro_ros_msgs__msg__Entity__fini(msg);
    return false;
  }
  // types
  if (!rosidl_runtime_c__String__Sequence__init(&msg->types, 0)) {
    micro_ros_msgs__msg__Entity__fini(msg);
    return false;
  }
  return true;
}

void
micro_ros_msgs__msg__Entity__fini(micro_ros_msgs__msg__Entity * msg)
{
  if (!msg) {
    return;
  }
  // entity_type
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // types
  rosidl_runtime_c__String__Sequence__fini(&msg->types);
}

bool
micro_ros_msgs__msg__Entity__are_equal(const micro_ros_msgs__msg__Entity * lhs, const micro_ros_msgs__msg__Entity * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // entity_type
  if (lhs->entity_type != rhs->entity_type) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // types
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->types), &(rhs->types)))
  {
    return false;
  }
  return true;
}

bool
micro_ros_msgs__msg__Entity__copy(
  const micro_ros_msgs__msg__Entity * input,
  micro_ros_msgs__msg__Entity * output)
{
  if (!input || !output) {
    return false;
  }
  // entity_type
  output->entity_type = input->entity_type;
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // types
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->types), &(output->types)))
  {
    return false;
  }
  return true;
}

micro_ros_msgs__msg__Entity *
micro_ros_msgs__msg__Entity__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Entity * msg = (micro_ros_msgs__msg__Entity *)allocator.allocate(sizeof(micro_ros_msgs__msg__Entity), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(micro_ros_msgs__msg__Entity));
  bool success = micro_ros_msgs__msg__Entity__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
micro_ros_msgs__msg__Entity__destroy(micro_ros_msgs__msg__Entity * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    micro_ros_msgs__msg__Entity__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
micro_ros_msgs__msg__Entity__Sequence__init(micro_ros_msgs__msg__Entity__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Entity * data = NULL;

  if (size) {
    data = (micro_ros_msgs__msg__Entity *)allocator.zero_allocate(size, sizeof(micro_ros_msgs__msg__Entity), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = micro_ros_msgs__msg__Entity__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        micro_ros_msgs__msg__Entity__fini(&data[i - 1]);
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
micro_ros_msgs__msg__Entity__Sequence__fini(micro_ros_msgs__msg__Entity__Sequence * array)
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
      micro_ros_msgs__msg__Entity__fini(&array->data[i]);
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

micro_ros_msgs__msg__Entity__Sequence *
micro_ros_msgs__msg__Entity__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  micro_ros_msgs__msg__Entity__Sequence * array = (micro_ros_msgs__msg__Entity__Sequence *)allocator.allocate(sizeof(micro_ros_msgs__msg__Entity__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = micro_ros_msgs__msg__Entity__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
micro_ros_msgs__msg__Entity__Sequence__destroy(micro_ros_msgs__msg__Entity__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    micro_ros_msgs__msg__Entity__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
micro_ros_msgs__msg__Entity__Sequence__are_equal(const micro_ros_msgs__msg__Entity__Sequence * lhs, const micro_ros_msgs__msg__Entity__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!micro_ros_msgs__msg__Entity__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
micro_ros_msgs__msg__Entity__Sequence__copy(
  const micro_ros_msgs__msg__Entity__Sequence * input,
  micro_ros_msgs__msg__Entity__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(micro_ros_msgs__msg__Entity);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    micro_ros_msgs__msg__Entity * data =
      (micro_ros_msgs__msg__Entity *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!micro_ros_msgs__msg__Entity__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          micro_ros_msgs__msg__Entity__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!micro_ros_msgs__msg__Entity__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
