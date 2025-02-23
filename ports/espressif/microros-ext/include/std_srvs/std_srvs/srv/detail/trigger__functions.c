// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from std_srvs:srv/Trigger.idl
// generated code does not contain a copyright notice
#include "std_srvs/srv/detail/trigger__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
std_srvs__srv__Trigger_Request__init(std_srvs__srv__Trigger_Request * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
std_srvs__srv__Trigger_Request__fini(std_srvs__srv__Trigger_Request * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
std_srvs__srv__Trigger_Request__are_equal(const std_srvs__srv__Trigger_Request * lhs, const std_srvs__srv__Trigger_Request * rhs)
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
std_srvs__srv__Trigger_Request__copy(
  const std_srvs__srv__Trigger_Request * input,
  std_srvs__srv__Trigger_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

std_srvs__srv__Trigger_Request *
std_srvs__srv__Trigger_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Request * msg = (std_srvs__srv__Trigger_Request *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(std_srvs__srv__Trigger_Request));
  bool success = std_srvs__srv__Trigger_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
std_srvs__srv__Trigger_Request__destroy(std_srvs__srv__Trigger_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    std_srvs__srv__Trigger_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
std_srvs__srv__Trigger_Request__Sequence__init(std_srvs__srv__Trigger_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Request * data = NULL;

  if (size) {
    data = (std_srvs__srv__Trigger_Request *)allocator.zero_allocate(size, sizeof(std_srvs__srv__Trigger_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = std_srvs__srv__Trigger_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        std_srvs__srv__Trigger_Request__fini(&data[i - 1]);
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
std_srvs__srv__Trigger_Request__Sequence__fini(std_srvs__srv__Trigger_Request__Sequence * array)
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
      std_srvs__srv__Trigger_Request__fini(&array->data[i]);
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

std_srvs__srv__Trigger_Request__Sequence *
std_srvs__srv__Trigger_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Request__Sequence * array = (std_srvs__srv__Trigger_Request__Sequence *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = std_srvs__srv__Trigger_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
std_srvs__srv__Trigger_Request__Sequence__destroy(std_srvs__srv__Trigger_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    std_srvs__srv__Trigger_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
std_srvs__srv__Trigger_Request__Sequence__are_equal(const std_srvs__srv__Trigger_Request__Sequence * lhs, const std_srvs__srv__Trigger_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!std_srvs__srv__Trigger_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
std_srvs__srv__Trigger_Request__Sequence__copy(
  const std_srvs__srv__Trigger_Request__Sequence * input,
  std_srvs__srv__Trigger_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(std_srvs__srv__Trigger_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    std_srvs__srv__Trigger_Request * data =
      (std_srvs__srv__Trigger_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!std_srvs__srv__Trigger_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          std_srvs__srv__Trigger_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!std_srvs__srv__Trigger_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `message`
#include "rosidl_runtime_c/string_functions.h"

bool
std_srvs__srv__Trigger_Response__init(std_srvs__srv__Trigger_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    std_srvs__srv__Trigger_Response__fini(msg);
    return false;
  }
  return true;
}

void
std_srvs__srv__Trigger_Response__fini(std_srvs__srv__Trigger_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // message
  rosidl_runtime_c__String__fini(&msg->message);
}

bool
std_srvs__srv__Trigger_Response__are_equal(const std_srvs__srv__Trigger_Response * lhs, const std_srvs__srv__Trigger_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  return true;
}

bool
std_srvs__srv__Trigger_Response__copy(
  const std_srvs__srv__Trigger_Response * input,
  std_srvs__srv__Trigger_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  return true;
}

std_srvs__srv__Trigger_Response *
std_srvs__srv__Trigger_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Response * msg = (std_srvs__srv__Trigger_Response *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(std_srvs__srv__Trigger_Response));
  bool success = std_srvs__srv__Trigger_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
std_srvs__srv__Trigger_Response__destroy(std_srvs__srv__Trigger_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    std_srvs__srv__Trigger_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
std_srvs__srv__Trigger_Response__Sequence__init(std_srvs__srv__Trigger_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Response * data = NULL;

  if (size) {
    data = (std_srvs__srv__Trigger_Response *)allocator.zero_allocate(size, sizeof(std_srvs__srv__Trigger_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = std_srvs__srv__Trigger_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        std_srvs__srv__Trigger_Response__fini(&data[i - 1]);
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
std_srvs__srv__Trigger_Response__Sequence__fini(std_srvs__srv__Trigger_Response__Sequence * array)
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
      std_srvs__srv__Trigger_Response__fini(&array->data[i]);
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

std_srvs__srv__Trigger_Response__Sequence *
std_srvs__srv__Trigger_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Response__Sequence * array = (std_srvs__srv__Trigger_Response__Sequence *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = std_srvs__srv__Trigger_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
std_srvs__srv__Trigger_Response__Sequence__destroy(std_srvs__srv__Trigger_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    std_srvs__srv__Trigger_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
std_srvs__srv__Trigger_Response__Sequence__are_equal(const std_srvs__srv__Trigger_Response__Sequence * lhs, const std_srvs__srv__Trigger_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!std_srvs__srv__Trigger_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
std_srvs__srv__Trigger_Response__Sequence__copy(
  const std_srvs__srv__Trigger_Response__Sequence * input,
  std_srvs__srv__Trigger_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(std_srvs__srv__Trigger_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    std_srvs__srv__Trigger_Response * data =
      (std_srvs__srv__Trigger_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!std_srvs__srv__Trigger_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          std_srvs__srv__Trigger_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!std_srvs__srv__Trigger_Response__copy(
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
// #include "std_srvs/srv/detail/trigger__functions.h"

bool
std_srvs__srv__Trigger_Event__init(std_srvs__srv__Trigger_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    std_srvs__srv__Trigger_Event__fini(msg);
    return false;
  }
  // request
  if (!std_srvs__srv__Trigger_Request__Sequence__init(&msg->request, 0)) {
    std_srvs__srv__Trigger_Event__fini(msg);
    return false;
  }
  // response
  if (!std_srvs__srv__Trigger_Response__Sequence__init(&msg->response, 0)) {
    std_srvs__srv__Trigger_Event__fini(msg);
    return false;
  }
  return true;
}

void
std_srvs__srv__Trigger_Event__fini(std_srvs__srv__Trigger_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  std_srvs__srv__Trigger_Request__Sequence__fini(&msg->request);
  // response
  std_srvs__srv__Trigger_Response__Sequence__fini(&msg->response);
}

bool
std_srvs__srv__Trigger_Event__are_equal(const std_srvs__srv__Trigger_Event * lhs, const std_srvs__srv__Trigger_Event * rhs)
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
  if (!std_srvs__srv__Trigger_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!std_srvs__srv__Trigger_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
std_srvs__srv__Trigger_Event__copy(
  const std_srvs__srv__Trigger_Event * input,
  std_srvs__srv__Trigger_Event * output)
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
  if (!std_srvs__srv__Trigger_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!std_srvs__srv__Trigger_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

std_srvs__srv__Trigger_Event *
std_srvs__srv__Trigger_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Event * msg = (std_srvs__srv__Trigger_Event *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(std_srvs__srv__Trigger_Event));
  bool success = std_srvs__srv__Trigger_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
std_srvs__srv__Trigger_Event__destroy(std_srvs__srv__Trigger_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    std_srvs__srv__Trigger_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
std_srvs__srv__Trigger_Event__Sequence__init(std_srvs__srv__Trigger_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Event * data = NULL;

  if (size) {
    data = (std_srvs__srv__Trigger_Event *)allocator.zero_allocate(size, sizeof(std_srvs__srv__Trigger_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = std_srvs__srv__Trigger_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        std_srvs__srv__Trigger_Event__fini(&data[i - 1]);
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
std_srvs__srv__Trigger_Event__Sequence__fini(std_srvs__srv__Trigger_Event__Sequence * array)
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
      std_srvs__srv__Trigger_Event__fini(&array->data[i]);
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

std_srvs__srv__Trigger_Event__Sequence *
std_srvs__srv__Trigger_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  std_srvs__srv__Trigger_Event__Sequence * array = (std_srvs__srv__Trigger_Event__Sequence *)allocator.allocate(sizeof(std_srvs__srv__Trigger_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = std_srvs__srv__Trigger_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
std_srvs__srv__Trigger_Event__Sequence__destroy(std_srvs__srv__Trigger_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    std_srvs__srv__Trigger_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
std_srvs__srv__Trigger_Event__Sequence__are_equal(const std_srvs__srv__Trigger_Event__Sequence * lhs, const std_srvs__srv__Trigger_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!std_srvs__srv__Trigger_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
std_srvs__srv__Trigger_Event__Sequence__copy(
  const std_srvs__srv__Trigger_Event__Sequence * input,
  std_srvs__srv__Trigger_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(std_srvs__srv__Trigger_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    std_srvs__srv__Trigger_Event * data =
      (std_srvs__srv__Trigger_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!std_srvs__srv__Trigger_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          std_srvs__srv__Trigger_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!std_srvs__srv__Trigger_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
