// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarkerInit.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_init__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `server_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `markers`
#include "visualization_msgs/msg/detail/interactive_marker__functions.h"

bool
visualization_msgs__msg__InteractiveMarkerInit__init(visualization_msgs__msg__InteractiveMarkerInit * msg)
{
  if (!msg) {
    return false;
  }
  // server_id
  if (!rosidl_runtime_c__String__init(&msg->server_id)) {
    visualization_msgs__msg__InteractiveMarkerInit__fini(msg);
    return false;
  }
  // seq_num
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__init(&msg->markers, 0)) {
    visualization_msgs__msg__InteractiveMarkerInit__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__InteractiveMarkerInit__fini(visualization_msgs__msg__InteractiveMarkerInit * msg)
{
  if (!msg) {
    return;
  }
  // server_id
  rosidl_runtime_c__String__fini(&msg->server_id);
  // seq_num
  // markers
  visualization_msgs__msg__InteractiveMarker__Sequence__fini(&msg->markers);
}

bool
visualization_msgs__msg__InteractiveMarkerInit__are_equal(const visualization_msgs__msg__InteractiveMarkerInit * lhs, const visualization_msgs__msg__InteractiveMarkerInit * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // server_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->server_id), &(rhs->server_id)))
  {
    return false;
  }
  // seq_num
  if (lhs->seq_num != rhs->seq_num) {
    return false;
  }
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__are_equal(
      &(lhs->markers), &(rhs->markers)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerInit__copy(
  const visualization_msgs__msg__InteractiveMarkerInit * input,
  visualization_msgs__msg__InteractiveMarkerInit * output)
{
  if (!input || !output) {
    return false;
  }
  // server_id
  if (!rosidl_runtime_c__String__copy(
      &(input->server_id), &(output->server_id)))
  {
    return false;
  }
  // seq_num
  output->seq_num = input->seq_num;
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__copy(
      &(input->markers), &(output->markers)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__InteractiveMarkerInit *
visualization_msgs__msg__InteractiveMarkerInit__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerInit * msg = (visualization_msgs__msg__InteractiveMarkerInit *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerInit), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarkerInit));
  bool success = visualization_msgs__msg__InteractiveMarkerInit__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarkerInit__destroy(visualization_msgs__msg__InteractiveMarkerInit * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarkerInit__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarkerInit__Sequence__init(visualization_msgs__msg__InteractiveMarkerInit__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerInit * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarkerInit *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarkerInit), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarkerInit__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarkerInit__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarkerInit__Sequence__fini(visualization_msgs__msg__InteractiveMarkerInit__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarkerInit__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarkerInit__Sequence *
visualization_msgs__msg__InteractiveMarkerInit__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerInit__Sequence * array = (visualization_msgs__msg__InteractiveMarkerInit__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerInit__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarkerInit__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarkerInit__Sequence__destroy(visualization_msgs__msg__InteractiveMarkerInit__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarkerInit__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarkerInit__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarkerInit__Sequence * lhs, const visualization_msgs__msg__InteractiveMarkerInit__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerInit__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerInit__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarkerInit__Sequence * input,
  visualization_msgs__msg__InteractiveMarkerInit__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarkerInit);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarkerInit * data =
      (visualization_msgs__msg__InteractiveMarkerInit *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarkerInit__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarkerInit__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerInit__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
