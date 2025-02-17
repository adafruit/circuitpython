// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarkerPose.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_pose__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"
// Member `name`
#include "rosidl_runtime_c/string_functions.h"

bool
visualization_msgs__msg__InteractiveMarkerPose__init(visualization_msgs__msg__InteractiveMarkerPose * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    visualization_msgs__msg__InteractiveMarkerPose__fini(msg);
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    visualization_msgs__msg__InteractiveMarkerPose__fini(msg);
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    visualization_msgs__msg__InteractiveMarkerPose__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__InteractiveMarkerPose__fini(visualization_msgs__msg__InteractiveMarkerPose * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
  // name
  rosidl_runtime_c__String__fini(&msg->name);
}

bool
visualization_msgs__msg__InteractiveMarkerPose__are_equal(const visualization_msgs__msg__InteractiveMarkerPose * lhs, const visualization_msgs__msg__InteractiveMarkerPose * rhs)
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
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerPose__copy(
  const visualization_msgs__msg__InteractiveMarkerPose * input,
  visualization_msgs__msg__InteractiveMarkerPose * output)
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
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__InteractiveMarkerPose *
visualization_msgs__msg__InteractiveMarkerPose__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerPose * msg = (visualization_msgs__msg__InteractiveMarkerPose *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerPose), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarkerPose));
  bool success = visualization_msgs__msg__InteractiveMarkerPose__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarkerPose__destroy(visualization_msgs__msg__InteractiveMarkerPose * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarkerPose__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarkerPose__Sequence__init(visualization_msgs__msg__InteractiveMarkerPose__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerPose * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarkerPose *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarkerPose), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarkerPose__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarkerPose__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarkerPose__Sequence__fini(visualization_msgs__msg__InteractiveMarkerPose__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarkerPose__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarkerPose__Sequence *
visualization_msgs__msg__InteractiveMarkerPose__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerPose__Sequence * array = (visualization_msgs__msg__InteractiveMarkerPose__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerPose__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarkerPose__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarkerPose__Sequence__destroy(visualization_msgs__msg__InteractiveMarkerPose__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarkerPose__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarkerPose__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarkerPose__Sequence * lhs, const visualization_msgs__msg__InteractiveMarkerPose__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerPose__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerPose__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarkerPose__Sequence * input,
  visualization_msgs__msg__InteractiveMarkerPose__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarkerPose);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarkerPose * data =
      (visualization_msgs__msg__InteractiveMarkerPose *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarkerPose__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarkerPose__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerPose__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
