// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from visualization_msgs:srv/GetInteractiveMarkers.idl
// generated code does not contain a copyright notice
#include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
visualization_msgs__srv__GetInteractiveMarkers_Request__init(visualization_msgs__srv__GetInteractiveMarkers_Request * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Request__fini(visualization_msgs__srv__GetInteractiveMarkers_Request * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Request__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Request * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // structure_needs_at_least_one_member
  if (lhs->structure_needs_at_least_one_member != rhs->structure_needs_at_least_one_member) {
    return false;
  }
  return true;
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Request__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Request * input,
  visualization_msgs__srv__GetInteractiveMarkers_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

visualization_msgs__srv__GetInteractiveMarkers_Request *
visualization_msgs__srv__GetInteractiveMarkers_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Request * msg = (visualization_msgs__srv__GetInteractiveMarkers_Request *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request));
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Request__destroy(visualization_msgs__srv__GetInteractiveMarkers_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__srv__GetInteractiveMarkers_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__init(visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Request * data = NULL;

  if (size) {
    data = (visualization_msgs__srv__GetInteractiveMarkers_Request *)allocator.zero_allocate(size, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__srv__GetInteractiveMarkers_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__srv__GetInteractiveMarkers_Request__fini(&data[i - 1]);
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
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__fini(visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * array)
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
      visualization_msgs__srv__GetInteractiveMarkers_Request__fini(&array->data[i]);
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

visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * array = (visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__destroy(visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * input,
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__srv__GetInteractiveMarkers_Request * data =
      (visualization_msgs__srv__GetInteractiveMarkers_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__srv__GetInteractiveMarkers_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__srv__GetInteractiveMarkers_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `markers`
#include "visualization_msgs/msg/detail/interactive_marker__functions.h"

bool
visualization_msgs__srv__GetInteractiveMarkers_Response__init(visualization_msgs__srv__GetInteractiveMarkers_Response * msg)
{
  if (!msg) {
    return false;
  }
  // sequence_number
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__init(&msg->markers, 0)) {
    visualization_msgs__srv__GetInteractiveMarkers_Response__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Response__fini(visualization_msgs__srv__GetInteractiveMarkers_Response * msg)
{
  if (!msg) {
    return;
  }
  // sequence_number
  // markers
  visualization_msgs__msg__InteractiveMarker__Sequence__fini(&msg->markers);
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Response__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Response * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // sequence_number
  if (lhs->sequence_number != rhs->sequence_number) {
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
visualization_msgs__srv__GetInteractiveMarkers_Response__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Response * input,
  visualization_msgs__srv__GetInteractiveMarkers_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // sequence_number
  output->sequence_number = input->sequence_number;
  // markers
  if (!visualization_msgs__msg__InteractiveMarker__Sequence__copy(
      &(input->markers), &(output->markers)))
  {
    return false;
  }
  return true;
}

visualization_msgs__srv__GetInteractiveMarkers_Response *
visualization_msgs__srv__GetInteractiveMarkers_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Response * msg = (visualization_msgs__srv__GetInteractiveMarkers_Response *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response));
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Response__destroy(visualization_msgs__srv__GetInteractiveMarkers_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__srv__GetInteractiveMarkers_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__init(visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Response * data = NULL;

  if (size) {
    data = (visualization_msgs__srv__GetInteractiveMarkers_Response *)allocator.zero_allocate(size, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__srv__GetInteractiveMarkers_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__srv__GetInteractiveMarkers_Response__fini(&data[i - 1]);
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
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__fini(visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * array)
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
      visualization_msgs__srv__GetInteractiveMarkers_Response__fini(&array->data[i]);
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

visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * array = (visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__destroy(visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * input,
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__srv__GetInteractiveMarkers_Response * data =
      (visualization_msgs__srv__GetInteractiveMarkers_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__srv__GetInteractiveMarkers_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__srv__GetInteractiveMarkers_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Response__copy(
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
// #include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"

bool
visualization_msgs__srv__GetInteractiveMarkers_Event__init(visualization_msgs__srv__GetInteractiveMarkers_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__fini(msg);
    return false;
  }
  // request
  if (!visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__init(&msg->request, 0)) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__fini(msg);
    return false;
  }
  // response
  if (!visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__init(&msg->response, 0)) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__fini(msg);
    return false;
  }
  return true;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Event__fini(visualization_msgs__srv__GetInteractiveMarkers_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__fini(&msg->request);
  // response
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__fini(&msg->response);
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Event__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Event * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Event * rhs)
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
  if (!visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Event__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Event * input,
  visualization_msgs__srv__GetInteractiveMarkers_Event * output)
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
  if (!visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

visualization_msgs__srv__GetInteractiveMarkers_Event *
visualization_msgs__srv__GetInteractiveMarkers_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Event * msg = (visualization_msgs__srv__GetInteractiveMarkers_Event *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event));
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Event__destroy(visualization_msgs__srv__GetInteractiveMarkers_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__init(visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Event * data = NULL;

  if (size) {
    data = (visualization_msgs__srv__GetInteractiveMarkers_Event *)allocator.zero_allocate(size, sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = visualization_msgs__srv__GetInteractiveMarkers_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        visualization_msgs__srv__GetInteractiveMarkers_Event__fini(&data[i - 1]);
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
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__fini(visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * array)
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
      visualization_msgs__srv__GetInteractiveMarkers_Event__fini(&array->data[i]);
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

visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence *
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * array = (visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence *)allocator.allocate(sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__destroy(visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__are_equal(const visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * lhs, const visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence__copy(
  const visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * input,
  visualization_msgs__srv__GetInteractiveMarkers_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    visualization_msgs__srv__GetInteractiveMarkers_Event * data =
      (visualization_msgs__srv__GetInteractiveMarkers_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!visualization_msgs__srv__GetInteractiveMarkers_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          visualization_msgs__srv__GetInteractiveMarkers_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!visualization_msgs__srv__GetInteractiveMarkers_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
