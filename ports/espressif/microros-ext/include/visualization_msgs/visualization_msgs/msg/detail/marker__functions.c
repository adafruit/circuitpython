// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:msg/Marker.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/msg/detail/marker__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `ns`
// Member `texture_resource`
// Member `text`
// Member `mesh_resource`
#include "rosidl_runtime_c/string_functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"
// Member `scale`
#include "geometry_msgs/msg/detail/vector3__functions.h"
// Member `color`
// Member `colors`
#include "std_msgs/msg/detail/color_rgba__functions.h"
// Member `lifetime`
#include "builtin_interfaces/msg/detail/duration__functions.h"
// Member `points`
#include "geometry_msgs/msg/detail/point__functions.h"
// Member `texture`
#include "sensor_msgs/msg/detail/compressed_image__functions.h"
// Member `uv_coordinates`
#include "visualization_msgs/msg/detail/uv_coordinate__functions.h"
// Member `mesh_file`
#include "visualization_msgs/msg/detail/mesh_file__functions.h"

bool
visualization_msgs__msg__Marker__init(visualization_msgs__msg__Marker * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // ns
  if (!rosidl_runtime_c__String__init(&msg->ns)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // id
  // type
  // action
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // scale
  if (!geometry_msgs__msg__Vector3__init(&msg->scale)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // color
  if (!std_msgs__msg__ColorRGBA__init(&msg->color)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__init(&msg->lifetime)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // frame_locked
  // points
  if (!geometry_msgs__msg__Point__Sequence__init(&msg->points, 0)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // colors
  if (!std_msgs__msg__ColorRGBA__Sequence__init(&msg->colors, 0)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // texture_resource
  if (!rosidl_runtime_c__String__init(&msg->texture_resource)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // texture
  if (!sensor_msgs__msg__CompressedImage__init(&msg->texture)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // uv_coordinates
  if (!visualization_msgs__msg__UVCoordinate__Sequence__init(&msg->uv_coordinates, 0)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // text
  if (!rosidl_runtime_c__String__init(&msg->text)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // mesh_resource
  if (!rosidl_runtime_c__String__init(&msg->mesh_resource)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // mesh_file
  if (!visualization_msgs__msg__MeshFile__init(&msg->mesh_file)) {
    visualization_msgs__msg__Marker__fini(msg);
    return false;
  }
  // mesh_use_embedded_materials
  return true;
}

void
visualization_msgs__msg__Marker__fini(visualization_msgs__msg__Marker * msg)
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
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
  // scale
  geometry_msgs__msg__Vector3__fini(&msg->scale);
  // color
  std_msgs__msg__ColorRGBA__fini(&msg->color);
  // lifetime
  builtin_interfaces__msg__Duration__fini(&msg->lifetime);
  // frame_locked
  // points
  geometry_msgs__msg__Point__Sequence__fini(&msg->points);
  // colors
  std_msgs__msg__ColorRGBA__Sequence__fini(&msg->colors);
  // texture_resource
  rosidl_runtime_c__String__fini(&msg->texture_resource);
  // texture
  sensor_msgs__msg__CompressedImage__fini(&msg->texture);
  // uv_coordinates
  visualization_msgs__msg__UVCoordinate__Sequence__fini(&msg->uv_coordinates);
  // text
  rosidl_runtime_c__String__fini(&msg->text);
  // mesh_resource
  rosidl_runtime_c__String__fini(&msg->mesh_resource);
  // mesh_file
  visualization_msgs__msg__MeshFile__fini(&msg->mesh_file);
  // mesh_use_embedded_materials
}

bool
visualization_msgs__msg__Marker__are_equal(const visualization_msgs__msg__Marker * lhs, const visualization_msgs__msg__Marker * rhs)
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
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  // scale
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->scale), &(rhs->scale)))
  {
    return false;
  }
  // color
  if (!std_msgs__msg__ColorRGBA__are_equal(
      &(lhs->color), &(rhs->color)))
  {
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->lifetime), &(rhs->lifetime)))
  {
    return false;
  }
  // frame_locked
  if (lhs->frame_locked != rhs->frame_locked) {
    return false;
  }
  // points
  if (!geometry_msgs__msg__Point__Sequence__are_equal(
      &(lhs->points), &(rhs->points)))
  {
    return false;
  }
  // colors
  if (!std_msgs__msg__ColorRGBA__Sequence__are_equal(
      &(lhs->colors), &(rhs->colors)))
  {
    return false;
  }
  // texture_resource
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->texture_resource), &(rhs->texture_resource)))
  {
    return false;
  }
  // texture
  if (!sensor_msgs__msg__CompressedImage__are_equal(
      &(lhs->texture), &(rhs->texture)))
  {
    return false;
  }
  // uv_coordinates
  if (!visualization_msgs__msg__UVCoordinate__Sequence__are_equal(
      &(lhs->uv_coordinates), &(rhs->uv_coordinates)))
  {
    return false;
  }
  // text
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->text), &(rhs->text)))
  {
    return false;
  }
  // mesh_resource
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->mesh_resource), &(rhs->mesh_resource)))
  {
    return false;
  }
  // mesh_file
  if (!visualization_msgs__msg__MeshFile__are_equal(
      &(lhs->mesh_file), &(rhs->mesh_file)))
  {
    return false;
  }
  // mesh_use_embedded_materials
  if (lhs->mesh_use_embedded_materials != rhs->mesh_use_embedded_materials) {
    return false;
  }
  return true;
}

bool
visualization_msgs__msg__Marker__copy(
  const visualization_msgs__msg__Marker * input,
  visualization_msgs__msg__Marker * output)
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
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  // scale
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->scale), &(output->scale)))
  {
    return false;
  }
  // color
  if (!std_msgs__msg__ColorRGBA__copy(
      &(input->color), &(output->color)))
  {
    return false;
  }
  // lifetime
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->lifetime), &(output->lifetime)))
  {
    return false;
  }
  // frame_locked
  output->frame_locked = input->frame_locked;
  // points
  if (!geometry_msgs__msg__Point__Sequence__copy(
      &(input->points), &(output->points)))
  {
    return false;
  }
  // colors
  if (!std_msgs__msg__ColorRGBA__Sequence__copy(
      &(input->colors), &(output->colors)))
  {
    return false;
  }
  // texture_resource
  if (!rosidl_runtime_c__String__copy(
      &(input->texture_resource), &(output->texture_resource)))
  {
    return false;
  }
  // texture
  if (!sensor_msgs__msg__CompressedImage__copy(
      &(input->texture), &(output->texture)))
  {
    return false;
  }
  // uv_coordinates
  if (!visualization_msgs__msg__UVCoordinate__Sequence__copy(
      &(input->uv_coordinates), &(output->uv_coordinates)))
  {
    return false;
  }
  // text
  if (!rosidl_runtime_c__String__copy(
      &(input->text), &(output->text)))
  {
    return false;
  }
  // mesh_resource
  if (!rosidl_runtime_c__String__copy(
      &(input->mesh_resource), &(output->mesh_resource)))
  {
    return false;
  }
  // mesh_file
  if (!visualization_msgs__msg__MeshFile__copy(
      &(input->mesh_file), &(output->mesh_file)))
  {
    return false;
  }
  // mesh_use_embedded_materials
  output->mesh_use_embedded_materials = input->mesh_use_embedded_materials;
  return true;
}

visualization_msgs__msg__Marker *
visualization_msgs__msg__Marker__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__Marker * msg = (visualization_msgs__msg__Marker *)allocator.allocate(sizeof(visualization_msgs__msg__Marker), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__msg__Marker));
  bool success = visualization_msgs__msg__Marker__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__msg__Marker__destroy(visualization_msgs__msg__Marker * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__msg__Marker__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__msg__Marker__Sequence__init(visualization_msgs__msg__Marker__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__Marker * data = NULL;

  if (size) {
    data = (visualization_msgs__msg__Marker *)allocator.zero_allocate(size, sizeof(visualization_msgs__msg__Marker), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__msg__Marker__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__msg__Marker__fini(&data[i - 1]);
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
visualization_msgs__msg__Marker__Sequence__fini(visualization_msgs__msg__Marker__Sequence * array)
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
      visualization_msgs__msg__Marker__fini(&array->data[i]);
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

visualization_msgs__msg__Marker__Sequence *
visualization_msgs__msg__Marker__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__msg__Marker__Sequence * array = (visualization_msgs__msg__Marker__Sequence *)allocator.allocate(sizeof(visualization_msgs__msg__Marker__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__msg__Marker__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__msg__Marker__Sequence__destroy(visualization_msgs__msg__Marker__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__msg__Marker__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__msg__Marker__Sequence__are_equal(const visualization_msgs__msg__Marker__Sequence * lhs, const visualization_msgs__msg__Marker__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__msg__Marker__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__msg__Marker__Sequence__copy(
  const visualization_msgs__msg__Marker__Sequence * input,
  visualization_msgs__msg__Marker__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__msg__Marker);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__msg__Marker * data =
      (visualization_msgs__msg__Marker *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__msg__Marker__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__msg__Marker__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__msg__Marker__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
