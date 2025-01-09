// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/InteractiveMarker.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/interactive_marker__functions.h"

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
// Member `description`
#include "rosidl_runtime_c/string_functions.h"
// Member `menu_entries`
#include "visualization_msgs/msg/detail/menu_entry__functions.h"
// Member `controls`
#include "visualization_msgs/msg/detail/interactive_marker_control__functions.h"

bool
visualization_msgs__msg__InteractiveMarker__init(visualization_msgs__msg__InteractiveMarker * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  // description
  if (!rosidl_runtime_c__String__init(&msg->description)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  // scale
  // menu_entries
  if (!visualization_msgs__msg__MenuEntry__Sequence__init(&msg->menu_entries, 0)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  // controls
  if (!visualization_msgs__msg__InteractiveMarkerControl__Sequence__init(&msg->controls, 0)) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__InteractiveMarker__fini(visualization_msgs__msg__InteractiveMarker * msg)
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
  // description
  rosidl_runtime_c__String__fini(&msg->description);
  // scale
  // menu_entries
  visualization_msgs__msg__MenuEntry__Sequence__fini(&msg->menu_entries);
  // controls
  visualization_msgs__msg__InteractiveMarkerControl__Sequence__fini(&msg->controls);
}

bool
visualization_msgs__msg__InteractiveMarker__are_equal(const visualization_msgs__msg__InteractiveMarker * lhs, const visualization_msgs__msg__InteractiveMarker * rhs)
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
  // description
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->description), &(rhs->description)))
  {
    return false;
  }
  // scale
  if (lhs->scale != rhs->scale) {
    return false;
  }
  // menu_entries
  if (!visualization_msgs__msg__MenuEntry__Sequence__are_equal(
      &(lhs->menu_entries), &(rhs->menu_entries)))
  {
    return false;
  }
  // controls
  if (!visualization_msgs__msg__InteractiveMarkerControl__Sequence__are_equal(
      &(lhs->controls), &(rhs->controls)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarker__copy(
  const visualization_msgs__msg__InteractiveMarker * input,
  visualization_msgs__msg__InteractiveMarker * output)
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
  // description
  if (!rosidl_runtime_c__String__copy(
      &(input->description), &(output->description)))
  {
    return false;
  }
  // scale
  output->scale = input->scale;
  // menu_entries
  if (!visualization_msgs__msg__MenuEntry__Sequence__copy(
      &(input->menu_entries), &(output->menu_entries)))
  {
    return false;
  }
  // controls
  if (!visualization_msgs__msg__InteractiveMarkerControl__Sequence__copy(
      &(input->controls), &(output->controls)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__InteractiveMarker *
visualization_msgs__msg__InteractiveMarker__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarker * msg = (visualization_msgs__msg__InteractiveMarker *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarker), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__InteractiveMarker));
  bool success = visualization_msgs__msg__InteractiveMarker__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__InteractiveMarker__destroy(visualization_msgs__msg__InteractiveMarker * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__InteractiveMarker__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__InteractiveMarker__Sequence__init(visualization_msgs__msg__InteractiveMarker__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarker * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__InteractiveMarker *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__InteractiveMarker), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__InteractiveMarker__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__InteractiveMarker__fini(&data[i - 1]);
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
visualization_msgs__msg__InteractiveMarker__Sequence__fini(visualization_msgs__msg__InteractiveMarker__Sequence * array)
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
      visualization_msgs__msg__InteractiveMarker__fini(&array->data[i]);
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

visualization_msgs__msg__InteractiveMarker__Sequence *
visualization_msgs__msg__InteractiveMarker__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__InteractiveMarker__Sequence * array = (visualization_msgs__msg__InteractiveMarker__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__InteractiveMarker__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__InteractiveMarker__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__InteractiveMarker__Sequence__destroy(visualization_msgs__msg__InteractiveMarker__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__InteractiveMarker__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__InteractiveMarker__Sequence__are_equal(const visualization_msgs__msg__InteractiveMarker__Sequence * lhs, const visualization_msgs__msg__InteractiveMarker__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarker__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__InteractiveMarker__Sequence__copy(
  const visualization_msgs__msg__InteractiveMarker__Sequence * input,
  visualization_msgs__msg__InteractiveMarker__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__InteractiveMarker);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__InteractiveMarker * data =
      (visualization_msgs__msg__InteractiveMarker *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__InteractiveMarker__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__InteractiveMarker__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__InteractiveMarker__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
