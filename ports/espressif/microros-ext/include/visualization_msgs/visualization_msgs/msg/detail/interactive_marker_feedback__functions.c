// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarkerFeedback.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker_feedback__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `client_id`
// Member `marker_name`
// Member `control_name`
#include "rosidl_runtime_c/string_functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"
// Member `mouse_point`
#include "geometry_msgs/msg/detail/point__functions.h"

bool
visualization_msgs__msg__InteractiveMarkerFeedback__init(visualization_msgs__msg__InteractiveMarkerFeedback * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // client_id
  if (!rosidl_runtime_c__String__init(&msg->client_id)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // marker_name
  if (!rosidl_runtime_c__String__init(&msg->marker_name)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // control_name
  if (!rosidl_runtime_c__String__init(&msg->control_name)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // event_type
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // menu_entry_id
  // mouse_point
  if (!geometry_msgs__msg__Point__init(&msg->mouse_point)) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
    return false;
  }
  // mouse_point_valid
  return true;
}

void
visualization_msgs__msg__InteractiveMarkerFeedback__fini(visualization_msgs__msg__InteractiveMarkerFeedback * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // client_id
  rosidl_runtime_c__String__fini(&msg->client_id);
  // marker_name
  rosidl_runtime_c__String__fini(&msg->marker_name);
  // control_name
  rosidl_runtime_c__String__fini(&msg->control_name);
  // event_type
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
  // menu_entry_id
  // mouse_point
  geometry_msgs__msg__Point__fini(&msg->mouse_point);
  // mouse_point_valid
}

bool
visualization_msgs__msg__InteractiveMarkerFeedback__are_equal(const visualization_msgs__msg__InteractiveMarkerFeedback * lhs, const visualization_msgs__msg__InteractiveMarkerFeedback * rhs)
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
  // client_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->client_id), &(rhs->client_id)))
  {
    return false;
  }
  // marker_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->marker_name), &(rhs->marker_name)))
  {
    return false;
  }
  // control_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->control_name), &(rhs->control_name)))
  {
    return false;
  }
  // event_type
  if (lhs->event_type != rhs->event_type) {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  // menu_entry_id
  if (lhs->menu_entry_id != rhs->menu_entry_id) {
    return false;
  }
  // mouse_point
  if (!geometry_msgs__msg__Point__are_equal(
      &(lhs->mouse_point), &(rhs->mouse_point)))
  {
    return false;
  }
  // mouse_point_valid
  if (lhs->mouse_point_valid != rhs->mouse_point_valid) {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerFeedback__copy(
  const visualization_msgs__msg__InteractiveMarkerFeedback * input,
  visualization_msgs__msg__InteractiveMarkerFeedback * output)
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
  // client_id
  if (!rosidl_runtime_c__String__copy(
      &(input->client_id), &(output->client_id)))
  {
    return false;
  }
  // marker_name
  if (!rosidl_runtime_c__String__copy(
      &(input->marker_name), &(output->marker_name)))
  {
    return false;
  }
  // control_name
  if (!rosidl_runtime_c__String__copy(
      &(input->control_name), &(output->control_name)))
  {
    return false;
  }
  // event_type
  output->event_type = input->event_type;
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  // menu_entry_id
  output->menu_entry_id = input->menu_entry_id;
  // mouse_point
  if (!geometry_msgs__msg__Point__copy(
      &(input->mouse_point), &(output->mouse_point)))
  {
    return false;
  }
  // mouse_point_valid
  output->mouse_point_valid = input->mouse_point_valid;
  return true;
}

visualization_msgs__msg__InteractiveMarkerFeedback *
visualization_msgs__msg__InteractiveMarkerFeedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerFeedback * msg = (visualization_msgs__msg__InteractiveMarkerFeedback *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerFeedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarkerFeedback));
  bool success = visualization_msgs__msg__InteractiveMarkerFeedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarkerFeedback__destroy(visualization_msgs__msg__InteractiveMarkerFeedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarkerFeedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__init(visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerFeedback * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarkerFeedback *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarkerFeedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarkerFeedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarkerFeedback__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__fini(visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarkerFeedback__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarkerFeedback__Sequence *
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * array = (visualization_msgs__msg__InteractiveMarkerFeedback__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarkerFeedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__destroy(visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * lhs, const visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerFeedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarkerFeedback__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * input,
  visualization_msgs__msg__InteractiveMarkerFeedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarkerFeedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarkerFeedback * data =
      (visualization_msgs__msg__InteractiveMarkerFeedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarkerFeedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarkerFeedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarkerFeedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
