// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:srv/SetCameraInfo.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/srv/detail/set_camera_info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `camera_info`
#include "sensor_msgs/msg/detail/camera_info__functions.h"

bool
sensor_msgs__srv__SetCameraInfo_Request__init(sensor_msgs__srv__SetCameraInfo_Request * msg)
{
  if (!msg) {
    return false;
  }
  // camera_info
  if (!sensor_msgs__msg__CameraInfo__init(&msg->camera_info)) {
    sensor_msgs__srv__SetCameraInfo_Request__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__srv__SetCameraInfo_Request__fini(sensor_msgs__srv__SetCameraInfo_Request * msg)
{
  if (!msg) {
    return;
  }
  // camera_info
  sensor_msgs__msg__CameraInfo__fini(&msg->camera_info);
}

bool
sensor_msgs__srv__SetCameraInfo_Request__are_equal(const sensor_msgs__srv__SetCameraInfo_Request * lhs, const sensor_msgs__srv__SetCameraInfo_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // camera_info
  if (!sensor_msgs__msg__CameraInfo__are_equal(
      &(lhs->camera_info), &(rhs->camera_info)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Request__copy(
  const sensor_msgs__srv__SetCameraInfo_Request * input,
  sensor_msgs__srv__SetCameraInfo_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // camera_info
  if (!sensor_msgs__msg__CameraInfo__copy(
      &(input->camera_info), &(output->camera_info)))
  {
    return false;
  }
  return true;
}

sensor_msgs__srv__SetCameraInfo_Request *
sensor_msgs__srv__SetCameraInfo_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Request * msg = (sensor_msgs__srv__SetCameraInfo_Request *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__srv__SetCameraInfo_Request));
  bool success = sensor_msgs__srv__SetCameraInfo_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__srv__SetCameraInfo_Request__destroy(sensor_msgs__srv__SetCameraInfo_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__srv__SetCameraInfo_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__srv__SetCameraInfo_Request__Sequence__init(sensor_msgs__srv__SetCameraInfo_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Request * data = NULL;

  if (size) {
    data = (sensor_msgs__srv__SetCameraInfo_Request *)allocator.zero_allocate(size, sizeof(sensor_msgs__srv__SetCameraInfo_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__srv__SetCameraInfo_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__srv__SetCameraInfo_Request__fini(&data[i - 1]);
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
sensor_msgs__srv__SetCameraInfo_Request__Sequence__fini(sensor_msgs__srv__SetCameraInfo_Request__Sequence * array)
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
      sensor_msgs__srv__SetCameraInfo_Request__fini(&array->data[i]);
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

sensor_msgs__srv__SetCameraInfo_Request__Sequence *
sensor_msgs__srv__SetCameraInfo_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Request__Sequence * array = (sensor_msgs__srv__SetCameraInfo_Request__Sequence *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__srv__SetCameraInfo_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__srv__SetCameraInfo_Request__Sequence__destroy(sensor_msgs__srv__SetCameraInfo_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__srv__SetCameraInfo_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__srv__SetCameraInfo_Request__Sequence__are_equal(const sensor_msgs__srv__SetCameraInfo_Request__Sequence * lhs, const sensor_msgs__srv__SetCameraInfo_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Request__Sequence__copy(
  const sensor_msgs__srv__SetCameraInfo_Request__Sequence * input,
  sensor_msgs__srv__SetCameraInfo_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__srv__SetCameraInfo_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__srv__SetCameraInfo_Request * data =
      (sensor_msgs__srv__SetCameraInfo_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__srv__SetCameraInfo_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__srv__SetCameraInfo_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `status_message`
#include "rosidl_runtime_c/string_functions.h"

bool
sensor_msgs__srv__SetCameraInfo_Response__init(sensor_msgs__srv__SetCameraInfo_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // status_message
  if (!rosidl_runtime_c__String__init(&msg->status_message)) {
    sensor_msgs__srv__SetCameraInfo_Response__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__srv__SetCameraInfo_Response__fini(sensor_msgs__srv__SetCameraInfo_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // status_message
  rosidl_runtime_c__String__fini(&msg->status_message);
}

bool
sensor_msgs__srv__SetCameraInfo_Response__are_equal(const sensor_msgs__srv__SetCameraInfo_Response * lhs, const sensor_msgs__srv__SetCameraInfo_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // status_message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->status_message), &(rhs->status_message)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Response__copy(
  const sensor_msgs__srv__SetCameraInfo_Response * input,
  sensor_msgs__srv__SetCameraInfo_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // status_message
  if (!rosidl_runtime_c__String__copy(
      &(input->status_message), &(output->status_message)))
  {
    return false;
  }
  return true;
}

sensor_msgs__srv__SetCameraInfo_Response *
sensor_msgs__srv__SetCameraInfo_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Response * msg = (sensor_msgs__srv__SetCameraInfo_Response *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__srv__SetCameraInfo_Response));
  bool success = sensor_msgs__srv__SetCameraInfo_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__srv__SetCameraInfo_Response__destroy(sensor_msgs__srv__SetCameraInfo_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__srv__SetCameraInfo_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__srv__SetCameraInfo_Response__Sequence__init(sensor_msgs__srv__SetCameraInfo_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Response * data = NULL;

  if (size) {
    data = (sensor_msgs__srv__SetCameraInfo_Response *)allocator.zero_allocate(size, sizeof(sensor_msgs__srv__SetCameraInfo_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__srv__SetCameraInfo_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__srv__SetCameraInfo_Response__fini(&data[i - 1]);
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
sensor_msgs__srv__SetCameraInfo_Response__Sequence__fini(sensor_msgs__srv__SetCameraInfo_Response__Sequence * array)
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
      sensor_msgs__srv__SetCameraInfo_Response__fini(&array->data[i]);
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

sensor_msgs__srv__SetCameraInfo_Response__Sequence *
sensor_msgs__srv__SetCameraInfo_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Response__Sequence * array = (sensor_msgs__srv__SetCameraInfo_Response__Sequence *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__srv__SetCameraInfo_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__srv__SetCameraInfo_Response__Sequence__destroy(sensor_msgs__srv__SetCameraInfo_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__srv__SetCameraInfo_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__srv__SetCameraInfo_Response__Sequence__are_equal(const sensor_msgs__srv__SetCameraInfo_Response__Sequence * lhs, const sensor_msgs__srv__SetCameraInfo_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Response__Sequence__copy(
  const sensor_msgs__srv__SetCameraInfo_Response__Sequence * input,
  sensor_msgs__srv__SetCameraInfo_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__srv__SetCameraInfo_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__srv__SetCameraInfo_Response * data =
      (sensor_msgs__srv__SetCameraInfo_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__srv__SetCameraInfo_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__srv__SetCameraInfo_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "sensor_msgs/srv/detail/set_camera_info__functions.h"

bool
sensor_msgs__srv__SetCameraInfo_Event__init(sensor_msgs__srv__SetCameraInfo_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    sensor_msgs__srv__SetCameraInfo_Event__fini(msg);
    return false;
  }
  // request
  if (!sensor_msgs__srv__SetCameraInfo_Request__Sequence__init(&msg->request, 0)) {
    sensor_msgs__srv__SetCameraInfo_Event__fini(msg);
    return false;
  }
  // response
  if (!sensor_msgs__srv__SetCameraInfo_Response__Sequence__init(&msg->response, 0)) {
    sensor_msgs__srv__SetCameraInfo_Event__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__srv__SetCameraInfo_Event__fini(sensor_msgs__srv__SetCameraInfo_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  sensor_msgs__srv__SetCameraInfo_Request__Sequence__fini(&msg->request);
  // response
  sensor_msgs__srv__SetCameraInfo_Response__Sequence__fini(&msg->response);
}

bool
sensor_msgs__srv__SetCameraInfo_Event__are_equal(const sensor_msgs__srv__SetCameraInfo_Event * lhs, const sensor_msgs__srv__SetCameraInfo_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!sensor_msgs__srv__SetCameraInfo_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!sensor_msgs__srv__SetCameraInfo_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Event__copy(
  const sensor_msgs__srv__SetCameraInfo_Event * input,
  sensor_msgs__srv__SetCameraInfo_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!sensor_msgs__srv__SetCameraInfo_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!sensor_msgs__srv__SetCameraInfo_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

sensor_msgs__srv__SetCameraInfo_Event *
sensor_msgs__srv__SetCameraInfo_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Event * msg = (sensor_msgs__srv__SetCameraInfo_Event *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__srv__SetCameraInfo_Event));
  bool success = sensor_msgs__srv__SetCameraInfo_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__srv__SetCameraInfo_Event__destroy(sensor_msgs__srv__SetCameraInfo_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__srv__SetCameraInfo_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__srv__SetCameraInfo_Event__Sequence__init(sensor_msgs__srv__SetCameraInfo_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Event * data = NULL;

  if (size) {
    data = (sensor_msgs__srv__SetCameraInfo_Event *)allocator.zero_allocate(size, sizeof(sensor_msgs__srv__SetCameraInfo_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__srv__SetCameraInfo_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__srv__SetCameraInfo_Event__fini(&data[i - 1]);
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
sensor_msgs__srv__SetCameraInfo_Event__Sequence__fini(sensor_msgs__srv__SetCameraInfo_Event__Sequence * array)
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
      sensor_msgs__srv__SetCameraInfo_Event__fini(&array->data[i]);
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

sensor_msgs__srv__SetCameraInfo_Event__Sequence *
sensor_msgs__srv__SetCameraInfo_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__srv__SetCameraInfo_Event__Sequence * array = (sensor_msgs__srv__SetCameraInfo_Event__Sequence *)allocator.allocate(sizeof(sensor_msgs__srv__SetCameraInfo_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__srv__SetCameraInfo_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__srv__SetCameraInfo_Event__Sequence__destroy(sensor_msgs__srv__SetCameraInfo_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__srv__SetCameraInfo_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__srv__SetCameraInfo_Event__Sequence__are_equal(const sensor_msgs__srv__SetCameraInfo_Event__Sequence * lhs, const sensor_msgs__srv__SetCameraInfo_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__srv__SetCameraInfo_Event__Sequence__copy(
  const sensor_msgs__srv__SetCameraInfo_Event__Sequence * input,
  sensor_msgs__srv__SetCameraInfo_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__srv__SetCameraInfo_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__srv__SetCameraInfo_Event * data =
      (sensor_msgs__srv__SetCameraInfo_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__srv__SetCameraInfo_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__srv__SetCameraInfo_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__srv__SetCameraInfo_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
