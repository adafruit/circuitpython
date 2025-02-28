// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from diagnostic_msgs:srv/SelfTest.idl
// generated code does not contain a copyright notice
#include "diagnostic_msgs/srv/detail/self_test__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
diagnostic_msgs__srv__SelfTest_Request__init(diagnostic_msgs__srv__SelfTest_Request * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
diagnostic_msgs__srv__SelfTest_Request__fini(diagnostic_msgs__srv__SelfTest_Request * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
diagnostic_msgs__srv__SelfTest_Request__are_equal(const diagnostic_msgs__srv__SelfTest_Request * lhs, const diagnostic_msgs__srv__SelfTest_Request * rhs)
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
diagnostic_msgs__srv__SelfTest_Request__copy(
  const diagnostic_msgs__srv__SelfTest_Request * input,
  diagnostic_msgs__srv__SelfTest_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

diagnostic_msgs__srv__SelfTest_Request *
diagnostic_msgs__srv__SelfTest_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Request * msg = (diagnostic_msgs__srv__SelfTest_Request *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__srv__SelfTest_Request));
  bool success = diagnostic_msgs__srv__SelfTest_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__srv__SelfTest_Request__destroy(diagnostic_msgs__srv__SelfTest_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__srv__SelfTest_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__srv__SelfTest_Request__Sequence__init(diagnostic_msgs__srv__SelfTest_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Request * data = NULL;

  if (size) {
    data = (diagnostic_msgs__srv__SelfTest_Request *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__srv__SelfTest_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__srv__SelfTest_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__srv__SelfTest_Request__fini(&data[i - 1]);
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
diagnostic_msgs__srv__SelfTest_Request__Sequence__fini(diagnostic_msgs__srv__SelfTest_Request__Sequence * array)
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
      diagnostic_msgs__srv__SelfTest_Request__fini(&array->data[i]);
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

diagnostic_msgs__srv__SelfTest_Request__Sequence *
diagnostic_msgs__srv__SelfTest_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Request__Sequence * array = (diagnostic_msgs__srv__SelfTest_Request__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__srv__SelfTest_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__srv__SelfTest_Request__Sequence__destroy(diagnostic_msgs__srv__SelfTest_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__srv__SelfTest_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__srv__SelfTest_Request__Sequence__are_equal(const diagnostic_msgs__srv__SelfTest_Request__Sequence * lhs, const diagnostic_msgs__srv__SelfTest_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__srv__SelfTest_Request__Sequence__copy(
  const diagnostic_msgs__srv__SelfTest_Request__Sequence * input,
  diagnostic_msgs__srv__SelfTest_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__srv__SelfTest_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__srv__SelfTest_Request * data =
      (diagnostic_msgs__srv__SelfTest_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__srv__SelfTest_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__srv__SelfTest_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `id`
#include "rosidl_runtime_c/string_functions.h"
// Member `status`
#include "diagnostic_msgs/msg/detail/diagnostic_status__functions.h"

bool
diagnostic_msgs__srv__SelfTest_Response__init(diagnostic_msgs__srv__SelfTest_Response * msg)
{
  if (!msg) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    diagnostic_msgs__srv__SelfTest_Response__fini(msg);
    return false;
  }
  // passed
  // status
  if (!diagnostic_msgs__msg__DiagnosticStatus__Sequence__init(&msg->status, 0)) {
    diagnostic_msgs__srv__SelfTest_Response__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__srv__SelfTest_Response__fini(diagnostic_msgs__srv__SelfTest_Response * msg)
{
  if (!msg) {
    return;
  }
  // id
  rosidl_runtime_c__String__fini(&msg->id);
  // passed
  // status
  diagnostic_msgs__msg__DiagnosticStatus__Sequence__fini(&msg->status);
}

bool
diagnostic_msgs__srv__SelfTest_Response__are_equal(const diagnostic_msgs__srv__SelfTest_Response * lhs, const diagnostic_msgs__srv__SelfTest_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  // passed
  if (lhs->passed != rhs->passed) {
    return false;
  }
  // status
  if (!diagnostic_msgs__msg__DiagnosticStatus__Sequence__are_equal(
      &(lhs->status), &(rhs->status)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__srv__SelfTest_Response__copy(
  const diagnostic_msgs__srv__SelfTest_Response * input,
  diagnostic_msgs__srv__SelfTest_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  // passed
  output->passed = input->passed;
  // status
  if (!diagnostic_msgs__msg__DiagnosticStatus__Sequence__copy(
      &(input->status), &(output->status)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__srv__SelfTest_Response *
diagnostic_msgs__srv__SelfTest_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Response * msg = (diagnostic_msgs__srv__SelfTest_Response *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__srv__SelfTest_Response));
  bool success = diagnostic_msgs__srv__SelfTest_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__srv__SelfTest_Response__destroy(diagnostic_msgs__srv__SelfTest_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__srv__SelfTest_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__srv__SelfTest_Response__Sequence__init(diagnostic_msgs__srv__SelfTest_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Response * data = NULL;

  if (size) {
    data = (diagnostic_msgs__srv__SelfTest_Response *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__srv__SelfTest_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__srv__SelfTest_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__srv__SelfTest_Response__fini(&data[i - 1]);
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
diagnostic_msgs__srv__SelfTest_Response__Sequence__fini(diagnostic_msgs__srv__SelfTest_Response__Sequence * array)
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
      diagnostic_msgs__srv__SelfTest_Response__fini(&array->data[i]);
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

diagnostic_msgs__srv__SelfTest_Response__Sequence *
diagnostic_msgs__srv__SelfTest_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Response__Sequence * array = (diagnostic_msgs__srv__SelfTest_Response__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__srv__SelfTest_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__srv__SelfTest_Response__Sequence__destroy(diagnostic_msgs__srv__SelfTest_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__srv__SelfTest_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__srv__SelfTest_Response__Sequence__are_equal(const diagnostic_msgs__srv__SelfTest_Response__Sequence * lhs, const diagnostic_msgs__srv__SelfTest_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__srv__SelfTest_Response__Sequence__copy(
  const diagnostic_msgs__srv__SelfTest_Response__Sequence * input,
  diagnostic_msgs__srv__SelfTest_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__srv__SelfTest_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__srv__SelfTest_Response * data =
      (diagnostic_msgs__srv__SelfTest_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__srv__SelfTest_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__srv__SelfTest_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Response__copy(
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
// #include "diagnostic_msgs/srv/detail/self_test__functions.h"

bool
diagnostic_msgs__srv__SelfTest_Event__init(diagnostic_msgs__srv__SelfTest_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    diagnostic_msgs__srv__SelfTest_Event__fini(msg);
    return false;
  }
  // request
  if (!diagnostic_msgs__srv__SelfTest_Request__Sequence__init(&msg->request, 0)) {
    diagnostic_msgs__srv__SelfTest_Event__fini(msg);
    return false;
  }
  // response
  if (!diagnostic_msgs__srv__SelfTest_Response__Sequence__init(&msg->response, 0)) {
    diagnostic_msgs__srv__SelfTest_Event__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__srv__SelfTest_Event__fini(diagnostic_msgs__srv__SelfTest_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  diagnostic_msgs__srv__SelfTest_Request__Sequence__fini(&msg->request);
  // response
  diagnostic_msgs__srv__SelfTest_Response__Sequence__fini(&msg->response);
}

bool
diagnostic_msgs__srv__SelfTest_Event__are_equal(const diagnostic_msgs__srv__SelfTest_Event * lhs, const diagnostic_msgs__srv__SelfTest_Event * rhs)
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
  if (!diagnostic_msgs__srv__SelfTest_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!diagnostic_msgs__srv__SelfTest_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__srv__SelfTest_Event__copy(
  const diagnostic_msgs__srv__SelfTest_Event * input,
  diagnostic_msgs__srv__SelfTest_Event * output)
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
  if (!diagnostic_msgs__srv__SelfTest_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!diagnostic_msgs__srv__SelfTest_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__srv__SelfTest_Event *
diagnostic_msgs__srv__SelfTest_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Event * msg = (diagnostic_msgs__srv__SelfTest_Event *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__srv__SelfTest_Event));
  bool success = diagnostic_msgs__srv__SelfTest_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__srv__SelfTest_Event__destroy(diagnostic_msgs__srv__SelfTest_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__srv__SelfTest_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__srv__SelfTest_Event__Sequence__init(diagnostic_msgs__srv__SelfTest_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Event * data = NULL;

  if (size) {
    data = (diagnostic_msgs__srv__SelfTest_Event *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__srv__SelfTest_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__srv__SelfTest_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__srv__SelfTest_Event__fini(&data[i - 1]);
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
diagnostic_msgs__srv__SelfTest_Event__Sequence__fini(diagnostic_msgs__srv__SelfTest_Event__Sequence * array)
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
      diagnostic_msgs__srv__SelfTest_Event__fini(&array->data[i]);
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

diagnostic_msgs__srv__SelfTest_Event__Sequence *
diagnostic_msgs__srv__SelfTest_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__SelfTest_Event__Sequence * array = (diagnostic_msgs__srv__SelfTest_Event__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__srv__SelfTest_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__srv__SelfTest_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__srv__SelfTest_Event__Sequence__destroy(diagnostic_msgs__srv__SelfTest_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__srv__SelfTest_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__srv__SelfTest_Event__Sequence__are_equal(const diagnostic_msgs__srv__SelfTest_Event__Sequence * lhs, const diagnostic_msgs__srv__SelfTest_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__srv__SelfTest_Event__Sequence__copy(
  const diagnostic_msgs__srv__SelfTest_Event__Sequence * input,
  diagnostic_msgs__srv__SelfTest_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__srv__SelfTest_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__srv__SelfTest_Event * data =
      (diagnostic_msgs__srv__SelfTest_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__srv__SelfTest_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__srv__SelfTest_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__srv__SelfTest_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
