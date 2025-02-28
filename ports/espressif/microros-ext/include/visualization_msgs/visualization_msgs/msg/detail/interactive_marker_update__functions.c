// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarkerUpdate.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_update__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `server_id`
// Member `erases`
#include "rosidl_runtime_c/string_functions.h"
// Member `markers`
#include "visualization_msgs/msg/detail/interactive_marker__functions.h"
// Member `poses`
#include "visualization_msgs/msg/detail/interactive_marker_pose__functions.h"

bool
visualization_msgs__msg__InteractiveMarkerUpdate__init(visualization_msgs__msg__InteractiveMarkerUpdate * msg)
{
  if (!msg) {
    return false;
  }
  // server_id
  if (!rosidl_runtime_c__String__init(&msg->server_id)) {
    visualization_msgs__msg__InteractiveMarkerUpdate__fini(msg);
    return false;
  }
  // seq_num
  // type
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__init(&msg->markers, 0)) {
    visualization_msgs__msg__InteractiveMarkerUpdate__fini(msg);
    return false;
  }
  // poses
  if (!visualization_msgs__msg__InteractiveMarkerPose__Sequence__init(&msg->poses, 0)) {
    visualization_msgs__msg__InteractiveMarkerUpdate__fini(msg);
    return false;
  }
  // erases
  if (!rosidl_runtime_c__String__Sequence__init(&msg->erases, 0)) {
    visualization_msgs__msg__InteractiveMarkerUpdate__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__InteractiveMarkerUpdate__fini(visualization_msgs__msg__InteractiveMarkerUpdate * msg)
{
  if (!msg) {
    return;
  }
  // server_id
  rosidl_runtime_c__String__fini(&msg->server_id);
  // seq_num
  // type
  // markers
  visualization_msgs__msg__InteractiveMarker__Sequence__fini(&msg->markers);
  // poses
  visualization_msgs__msg__InteractiveMarkerPose__Sequence__fini(&msg->poses);
  // erases
  rosidl_runtime_c__String__Sequence__fini(&msg->erases);
}

bool
visualization_msgs__msg__InteractiveMarkerUpdate__are_equal(const visualization_msgs__msg__InteractiveMarkerUpdate * lhs, const visualization_msgs__msg__InteractiveMarkerUpdate * rhs)
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
  // type
  if (lhs->type != rhs->type) {
    return false;
  }
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__are_equal(
      &(lhs->markers), &(rhs->markers)))
  {
    return false;
  }
  // poses
  if (!visualization_msgs__msg__InteractiveMarkerPose__Sequence__are_equal(
      &(lhs->poses), &(rhs->poses)))
  {
    return false;
  }
  // erases
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->erases), &(rhs->erases)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerUpdate__copy(
  const visualization_msgs__msg__InteractiveMarkerUpdate * input,
  visualization_msgs__msg__InteractiveMarkerUpdate * output)
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
  // type
  output->type = input->type;
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__copy(
      &(input->markers), &(output->markers)))
  {
    return false;
  }
  // poses
  if (!visualization_msgs__msg__InteractiveMarkerPose__Sequence__copy(
      &(input->poses), &(output->poses)))
  {
    return false;
  }
  // erases
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->erases), &(output->erases)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__InteractiveMarkerUpdate *
visualization_msgs__msg__InteractiveMarkerUpdate__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerUpdate * msg = (visualization_msgs__msg__InteractiveMarkerUpdate *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerUpdate), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarkerUpdate));
  bool success = visualization_msgs__msg__InteractiveMarkerUpdate__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarkerUpdate__destroy(visualization_msgs__msg__InteractiveMarkerUpdate * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarkerUpdate__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__init(visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerUpdate * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarkerUpdate *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarkerUpdate), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarkerUpdate__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarkerUpdate__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__fini(visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarkerUpdate__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarkerUpdate__Sequence *
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * array = (visualization_msgs__msg__InteractiveMarkerUpdate__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerUpdate__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__destroy(visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * lhs, const visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerUpdate__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerUpdate__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * input,
  visualization_msgs__msg__InteractiveMarkerUpdate__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarkerUpdate);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarkerUpdate * data =
      (visualization_msgs__msg__InteractiveMarkerUpdate *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarkerUpdate__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarkerUpdate__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerUpdate__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
