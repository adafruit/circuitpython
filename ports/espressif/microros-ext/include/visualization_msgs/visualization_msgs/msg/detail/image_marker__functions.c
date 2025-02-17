// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/ImageMarker.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/image_marker__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `ns`
#include "rosidl_runtime_c/string_functions.h"
// Member `position`
// Member `points`
#include "geometry_msgs/msg/detail/point__functions.h"
// Member `outline_color`
// Member `fill_color`
// Member `outline_colors`
#include "std_msgs/msg/detail/color_rgba__functions.h"
// Member `lifetime`
#include "builtin_interfaces/msg/detail/duration__functions.h"

bool
visualization_msgs__msg__ImageMarker__init(visualization_msgs__msg__ImageMarker * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // ns
  if (!rosidl_runtime_c__String__init(&msg->ns)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // id
  // type
  // action
  // position
  if (!geometry_msgs__msg__Point__init(&msg->position)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // scale
  // outline_color
  if (!std_msgs__msg__ColorRGBA__init(&msg->outline_color)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // filled
  // fill_color
  if (!std_msgs__msg__ColorRGBA__init(&msg->fill_color)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__init(&msg->lifetime)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // points
  if (!geometry_msgs__msg__Point__Sequence__init(&msg->points, 0)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  // outline_colors
  if (!std_msgs__msg__ColorRGBA__Sequence__init(&msg->outline_colors, 0)) {
    visualization_msgs__msg__ImageMarker__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__msg__ImageMarker__fini(visualization_msgs__msg__ImageMarker * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // ns
  rosidl_runtime_c__String__fini(&msg->ns);
  // id
  // type
  // action
  // position
  geometry_msgs__msg__Point__fini(&msg->position);
  // scale
  // outline_color
  std_msgs__msg__ColorRGBA__fini(&msg->outline_color);
  // filled
  // fill_color
  std_msgs__msg__ColorRGBA__fini(&msg->fill_color);
  // lifetime
  builtin_interfaces__msg__Duration__fini(&msg->lifetime);
  // points
  geometry_msgs__msg__Point__Sequence__fini(&msg->points);
  // outline_colors
  std_msgs__msg__ColorRGBA__Sequence__fini(&msg->outline_colors);
}

bool
visualization_msgs__msg__ImageMarker__are_equal(const visualization_msgs__msg__ImageMarker * lhs, const visualization_msgs__msg__ImageMarker * rhs)
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
  // ns
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->ns), &(rhs->ns)))
  {
    return false;
  }
  // id
  if (lhs->id != rhs->id) {
    return false;
  }
  // type
  if (lhs->type != rhs->type) {
    return false;
  }
  // action
  if (lhs->action != rhs->action) {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Point__are_equal(
      &(lhs->position), &(rhs->position)))
  {
    return false;
  }
  // scale
  if (lhs->scale != rhs->scale) {
    return false;
  }
  // outline_color
  if (!std_msgs__msg__ColorRGBA__are_equal(
      &(lhs->outline_color), &(rhs->outline_color)))
  {
    return false;
  }
  // filled
  if (lhs->filled != rhs->filled) {
    return false;
  }
  // fill_color
  if (!std_msgs__msg__ColorRGBA__are_equal(
      &(lhs->fill_color), &(rhs->fill_color)))
  {
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->lifetime), &(rhs->lifetime)))
  {
    return false;
  }
  // points
  if (!geometry_msgs__msg__Point__Sequence__are_equal(
      &(lhs->points), &(rhs->points)))
  {
    return false;
  }
  // outline_colors
  if (!std_msgs__msg__ColorRGBA__Sequence__are_equal(
      &(lhs->outline_colors), &(rhs->outline_colors)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__ImageMarker__copy(
  const visualization_msgs__msg__ImageMarker * input,
  visualization_msgs__msg__ImageMarker * output)
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
  // ns
  if (!rosidl_runtime_c__String__copy(
      &(input->ns), &(output->ns)))
  {
    return false;
  }
  // id
  output->id = input->id;
  // type
  output->type = input->type;
  // action
  output->action = input->action;
  // position
  if (!geometry_msgs__msg__Point__copy(
      &(input->position), &(output->position)))
  {
    return false;
  }
  // scale
  output->scale = input->scale;
  // outline_color
  if (!std_msgs__msg__ColorRGBA__copy(
      &(input->outline_color), &(output->outline_color)))
  {
    return false;
  }
  // filled
  output->filled = input->filled;
  // fill_color
  if (!std_msgs__msg__ColorRGBA__copy(
      &(input->fill_color), &(output->fill_color)))
  {
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->lifetime), &(output->lifetime)))
  {
    return false;
  }
  // points
  if (!geometry_msgs__msg__Point__Sequence__copy(
      &(input->points), &(output->points)))
  {
    return false;
  }
  // outline_colors
  if (!std_msgs__msg__ColorRGBA__Sequence__copy(
      &(input->outline_colors), &(output->outline_colors)))
  {
    return false;
  }
  return true;
}

visualization_msgs__msg__ImageMarker *
visualization_msgs__msg__ImageMarker__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__ImageMarker * msg = (visualization_msgs__msg__ImageMarker *)allocator.allocate(sizeof(visualization_msgs__msg__ImageMarker), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__ImageMarker));
  bool success = visualization_msgs__msg__ImageMarker__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__ImageMarker__destroy(visualization_msgs__msg__ImageMarker * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__ImageMarker__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__ImageMarker__Sequence__init(visualization_msgs__msg__ImageMarker__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__ImageMarker * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__ImageMarker *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__ImageMarker), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__ImageMarker__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__ImageMarker__fini(&data[i - 1]);
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
visualization_msgs__msg__ImageMarker__Sequence__fini(visualization_msgs__msg__ImageMarker__Sequence * array)
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
      visualization_msgs__msg__ImageMarker__fini(&array->data[i]);
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

visualization_msgs__msg__ImageMarker__Sequence *
visualization_msgs__msg__ImageMarker__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__ImageMarker__Sequence * array = (visualization_msgs__msg__ImageMarker__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__ImageMarker__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__ImageMarker__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__ImageMarker__Sequence__destroy(visualization_msgs__msg__ImageMarker__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__ImageMarker__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__ImageMarker__Sequence__are_equal(const visualization_msgs__msg__ImageMarker__Sequence * lhs, const visualization_msgs__msg__ImageMarker__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__ImageMarker__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__ImageMarker__Sequence__copy(
  const visualization_msgs__msg__ImageMarker__Sequence * input,
  visualization_msgs__msg__ImageMarker__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__ImageMarker);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__ImageMarker * data =
      (visualization_msgs__msg__ImageMarker *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__ImageMarker__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__ImageMarker__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__ImageMarker__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
