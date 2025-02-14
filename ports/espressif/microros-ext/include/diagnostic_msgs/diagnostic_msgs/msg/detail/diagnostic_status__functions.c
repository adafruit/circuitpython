// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from diagnostic_msgs:msg/DiagnosticStatus.idl
// generated code does not contain a copyright notice
#include "diagnostic_msgs/msg/detail/diagnostic_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
// Member `message`
// Member `hardware_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `values`
#include "diagnostic_msgs/msg/detail/key_value__functions.h"

bool
diagnostic_msgs__msg__DiagnosticStatus__init(diagnostic_msgs__msg__DiagnosticStatus * msg)
{
  if (!msg) {
    return false;
  }
  // level
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    diagnostic_msgs__msg__DiagnosticStatus__fini(msg);
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    diagnostic_msgs__msg__DiagnosticStatus__fini(msg);
    return false;
  }
  // hardware_id
  if (!rosidl_runtime_c__String__init(&msg->hardware_id)) {
    diagnostic_msgs__msg__DiagnosticStatus__fini(msg);
    return false;
  }
  // values
  if (!diagnostic_msgs__msg__KeyValue__Sequence__init(&msg->values, 0)) {
    diagnostic_msgs__msg__DiagnosticStatus__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__msg__DiagnosticStatus__fini(diagnostic_msgs__msg__DiagnosticStatus * msg)
{
  if (!msg) {
    return;
  }
  // level
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // message
  rosidl_runtime_c__String__fini(&msg->message);
  // hardware_id
  rosidl_runtime_c__String__fini(&msg->hardware_id);
  // values
  diagnostic_msgs__msg__KeyValue__Sequence__fini(&msg->values);
}

bool
diagnostic_msgs__msg__DiagnosticStatus__are_equal(const diagnostic_msgs__msg__DiagnosticStatus * lhs, const diagnostic_msgs__msg__DiagnosticStatus * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // level
  if (lhs->level != rhs->level) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  // hardware_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->hardware_id), &(rhs->hardware_id)))
  {
    return false;
  }
  // values
  if (!diagnostic_msgs__msg__KeyValue__Sequence__are_equal(
      &(lhs->values), &(rhs->values)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__msg__DiagnosticStatus__copy(
  const diagnostic_msgs__msg__DiagnosticStatus * input,
  diagnostic_msgs__msg__DiagnosticStatus * output)
{
  if (!input || !output) {
    return false;
  }
  // level
  output->level = input->level;
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  // hardware_id
  if (!rosidl_runtime_c__String__copy(
      &(input->hardware_id), &(output->hardware_id)))
  {
    return false;
  }
  // values
  if (!diagnostic_msgs__msg__KeyValue__Sequence__copy(
      &(input->values), &(output->values)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__msg__DiagnosticStatus *
diagnostic_msgs__msg__DiagnosticStatus__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__DiagnosticStatus * msg = (diagnostic_msgs__msg__DiagnosticStatus *)allocator.allocate(sizeof(diagnostic_msgs__msg__DiagnosticStatus), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__msg__DiagnosticStatus));
  bool success = diagnostic_msgs__msg__DiagnosticStatus__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__msg__DiagnosticStatus__destroy(diagnostic_msgs__msg__DiagnosticStatus * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__msg__DiagnosticStatus__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__msg__DiagnosticStatus__Sequence__init(diagnostic_msgs__msg__DiagnosticStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__DiagnosticStatus * data = NULL;

  if (size) {
    data = (diagnostic_msgs__msg__DiagnosticStatus *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__msg__DiagnosticStatus), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__msg__DiagnosticStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__msg__DiagnosticStatus__fini(&data[i - 1]);
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
diagnostic_msgs__msg__DiagnosticStatus__Sequence__fini(diagnostic_msgs__msg__DiagnosticStatus__Sequence * array)
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
      diagnostic_msgs__msg__DiagnosticStatus__fini(&array->data[i]);
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

diagnostic_msgs__msg__DiagnosticStatus__Sequence *
diagnostic_msgs__msg__DiagnosticStatus__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__msg__DiagnosticStatus__Sequence * array = (diagnostic_msgs__msg__DiagnosticStatus__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__msg__DiagnosticStatus__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__msg__DiagnosticStatus__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__msg__DiagnosticStatus__Sequence__destroy(diagnostic_msgs__msg__DiagnosticStatus__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__msg__DiagnosticStatus__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__msg__DiagnosticStatus__Sequence__are_equal(const diagnostic_msgs__msg__DiagnosticStatus__Sequence * lhs, const diagnostic_msgs__msg__DiagnosticStatus__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__msg__DiagnosticStatus__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__msg__DiagnosticStatus__Sequence__copy(
  const diagnostic_msgs__msg__DiagnosticStatus__Sequence * input,
  diagnostic_msgs__msg__DiagnosticStatus__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__msg__DiagnosticStatus);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__msg__DiagnosticStatus * data =
      (diagnostic_msgs__msg__DiagnosticStatus *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__msg__DiagnosticStatus__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__msg__DiagnosticStatus__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__msg__DiagnosticStatus__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
