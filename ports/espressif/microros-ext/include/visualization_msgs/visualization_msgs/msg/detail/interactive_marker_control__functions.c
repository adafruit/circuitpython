// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarkerControl.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_control__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
// Member `description`
#include "rosidl_runtime_c/string_functions.h"
// Member `orientation`
#include "geometry_msgs/msg/detail/quaternion__functions.h"
// Member `markers`
#include "visualization_msgs/msg/detail/marker__functions.h"

bool
visualization_msgs__msg__InteractiveMarkerControl__init(visualization_msgs__msg__InteractiveMarkerControl * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    visualization_msgs__msg__InteractiveMarkerControl__fini(msg);
    return false;
  }
  // orientation
  if (!geometry_msgs__msg__Quaternion__init(&msg->orientation)) {
    visualization_msgs__msg__InteractiveMarkerControl__fini(msg);
    return false;
  }
  // orientation_mode
  // interaction_mode
  // always_visible
  // markers
  if (!visualization_msgs__msg__Marker__Sequence__init(&msg->markers, 0)) {
    visualization_msgs__msg__InteractiveMarkerControl__fini(msg);
    return false;
  }
  // independent_marker_orientation
  // description
  if (!rosidl_runtime_c__String__init(&msg->description)) {
    visualization_msgs__msg__InteractiveMarkerControl__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__InteractiveMarkerControl__fini(visualization_msgs__msg__InteractiveMarkerControl * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // orientation
  geometry_msgs__msg__Quaternion__fini(&msg->orientation);
  // orientation_mode
  // interaction_mode
  // always_visible
  // markers
  visualization_msgs__msg__Marker__Sequence__fini(&msg->markers);
  // independent_marker_orientation
  // description
  rosidl_runtime_c__String__fini(&msg->description);
}

bool
visualization_msgs__msg__InteractiveMarkerControl__are_equal(const visualization_msgs__msg__InteractiveMarkerControl * lhs, const visualization_msgs__msg__InteractiveMarkerControl * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // orientation
  if (!geometry_msgs__msg__Quaternion__are_equal(
      &(lhs->orientation), &(rhs->orientation)))
  {
    return false;
  }
  // orientation_mode
  if (lhs->orientation_mode != rhs->orientation_mode) {
    return false;
  }
  // interaction_mode
  if (lhs->interaction_mode != rhs->interaction_mode) {
    return false;
  }
  // always_visible
  if (lhs->always_visible != rhs->always_visible) {
    return false;
  }
  // markers
  if (!visualization_msgs__msg__Marker__Sequence__are_equal(
      &(lhs->markers), &(rhs->markers)))
  {
    return false;
  }
  // independent_marker_orientation
  if (lhs->independent_marker_orientation != rhs->independent_marker_orientation) {
    return false;
  }
  // description
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->description), &(rhs->description)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerControl__copy(
  const visualization_msgs__msg__InteractiveMarkerControl * input,
  visualization_msgs__msg__InteractiveMarkerControl * output)
{
  if (!input || !output) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // orientation
  if (!geometry_msgs__msg__Quaternion__copy(
      &(input->orientation), &(output->orientation)))
  {
    return false;
  }
  // orientation_mode
  output->orientation_mode = input->orientation_mode;
  // interaction_mode
  output->interaction_mode = input->interaction_mode;
  // always_visible
  output->always_visible = input->always_visible;
  // markers
  if (!visualization_msgs__msg__Marker__Sequence__copy(
      &(input->markers), &(output->markers)))
  {
    return false;
  }
  // independent_marker_orientation
  output->independent_marker_orientation = input->independent_marker_orientation;
  // description
  if (!rosidl_runtime_c__String__copy(
      &(input->description), &(output->description)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__InteractiveMarkerControl *
visualization_msgs__msg__InteractiveMarkerControl__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerControl * msg = (visualization_msgs__msg__InteractiveMarkerControl *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerControl), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarkerControl));
  bool success = visualization_msgs__msg__InteractiveMarkerControl__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarkerControl__destroy(visualization_msgs__msg__InteractiveMarkerControl * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarkerControl__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarkerControl__Sequence__init(visualization_msgs__msg__InteractiveMarkerControl__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerControl * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarkerControl *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarkerControl), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarkerControl__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarkerControl__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarkerControl__Sequence__fini(visualization_msgs__msg__InteractiveMarkerControl__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarkerControl__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarkerControl__Sequence *
visualization_msgs__msg__InteractiveMarkerControl__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerControl__Sequence * array = (visualization_msgs__msg__InteractiveMarkerControl__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerControl__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarkerControl__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarkerControl__Sequence__destroy(visualization_msgs__msg__InteractiveMarkerControl__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarkerControl__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarkerControl__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarkerControl__Sequence * lhs, const visualization_msgs__msg__InteractiveMarkerControl__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerControl__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerControl__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarkerControl__Sequence * input,
  visualization_msgs__msg__InteractiveMarkerControl__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarkerControl);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarkerControl * data =
      (visualization_msgs__msg__InteractiveMarkerControl *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarkerControl__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarkerControl__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerControl__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
