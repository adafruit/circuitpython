// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:action/NestedMessage.idl
// generated code does not contain a copyright notice
#include "test_msgs/action/detail/nested_message__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `nested_field_no_pkg`
#include "test_msgs/msg/detail/builtins__functions.h"
// Member `nested_field`
#include "test_msgs/msg/detail/basic_types__functions.h"
// Member `nested_different_pkg`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
test_msgs__action__NestedMessage_Goal__init(test_msgs__action__NestedMessage_Goal * msg)
{
  if (!msg) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__init(&msg->nested_field_no_pkg)) {
    test_msgs__action__NestedMessage_Goal__fini(msg);
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__init(&msg->nested_field)) {
    test_msgs__action__NestedMessage_Goal__fini(msg);
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__init(&msg->nested_different_pkg)) {
    test_msgs__action__NestedMessage_Goal__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_Goal__fini(test_msgs__action__NestedMessage_Goal * msg)
{
  if (!msg) {
    return;
  }
  // nested_field_no_pkg
  test_msgs__msg__Builtins__fini(&msg->nested_field_no_pkg);
  // nested_field
  test_msgs__msg__BasicTypes__fini(&msg->nested_field);
  // nested_different_pkg
  builtin_interfaces__msg__Time__fini(&msg->nested_different_pkg);
}

bool
test_msgs__action__NestedMessage_Goal__are_equal(const test_msgs__action__NestedMessage_Goal * lhs, const test_msgs__action__NestedMessage_Goal * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__are_equal(
      &(lhs->nested_field_no_pkg), &(rhs->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__are_equal(
      &(lhs->nested_field), &(rhs->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->nested_different_pkg), &(rhs->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Goal__copy(
  const test_msgs__action__NestedMessage_Goal * input,
  test_msgs__action__NestedMessage_Goal * output)
{
  if (!input || !output) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__copy(
      &(input->nested_field_no_pkg), &(output->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__copy(
      &(input->nested_field), &(output->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__copy(
      &(input->nested_different_pkg), &(output->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_Goal *
test_msgs__action__NestedMessage_Goal__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Goal * msg = (test_msgs__action__NestedMessage_Goal *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Goal), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_Goal));
  bool success = test_msgs__action__NestedMessage_Goal__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_Goal__destroy(test_msgs__action__NestedMessage_Goal * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_Goal__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_Goal__Sequence__init(test_msgs__action__NestedMessage_Goal__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Goal * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_Goal *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_Goal), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_Goal__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_Goal__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_Goal__Sequence__fini(test_msgs__action__NestedMessage_Goal__Sequence * array)
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
      test_msgs__action__NestedMessage_Goal__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_Goal__Sequence *
test_msgs__action__NestedMessage_Goal__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Goal__Sequence * array = (test_msgs__action__NestedMessage_Goal__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Goal__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_Goal__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_Goal__Sequence__destroy(test_msgs__action__NestedMessage_Goal__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_Goal__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_Goal__Sequence__are_equal(const test_msgs__action__NestedMessage_Goal__Sequence * lhs, const test_msgs__action__NestedMessage_Goal__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_Goal__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Goal__Sequence__copy(
  const test_msgs__action__NestedMessage_Goal__Sequence * input,
  test_msgs__action__NestedMessage_Goal__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_Goal);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_Goal * data =
      (test_msgs__action__NestedMessage_Goal *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_Goal__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_Goal__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_Goal__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `nested_field_no_pkg`
// already included above
// #include "test_msgs/msg/detail/builtins__functions.h"
// Member `nested_field`
// already included above
// #include "test_msgs/msg/detail/basic_types__functions.h"
// Member `nested_different_pkg`
// already included above
// #include "builtin_interfaces/msg/detail/time__functions.h"

bool
test_msgs__action__NestedMessage_Result__init(test_msgs__action__NestedMessage_Result * msg)
{
  if (!msg) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__init(&msg->nested_field_no_pkg)) {
    test_msgs__action__NestedMessage_Result__fini(msg);
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__init(&msg->nested_field)) {
    test_msgs__action__NestedMessage_Result__fini(msg);
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__init(&msg->nested_different_pkg)) {
    test_msgs__action__NestedMessage_Result__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_Result__fini(test_msgs__action__NestedMessage_Result * msg)
{
  if (!msg) {
    return;
  }
  // nested_field_no_pkg
  test_msgs__msg__Builtins__fini(&msg->nested_field_no_pkg);
  // nested_field
  test_msgs__msg__BasicTypes__fini(&msg->nested_field);
  // nested_different_pkg
  builtin_interfaces__msg__Time__fini(&msg->nested_different_pkg);
}

bool
test_msgs__action__NestedMessage_Result__are_equal(const test_msgs__action__NestedMessage_Result * lhs, const test_msgs__action__NestedMessage_Result * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__are_equal(
      &(lhs->nested_field_no_pkg), &(rhs->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__are_equal(
      &(lhs->nested_field), &(rhs->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->nested_different_pkg), &(rhs->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Result__copy(
  const test_msgs__action__NestedMessage_Result * input,
  test_msgs__action__NestedMessage_Result * output)
{
  if (!input || !output) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__copy(
      &(input->nested_field_no_pkg), &(output->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__copy(
      &(input->nested_field), &(output->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__copy(
      &(input->nested_different_pkg), &(output->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_Result *
test_msgs__action__NestedMessage_Result__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Result * msg = (test_msgs__action__NestedMessage_Result *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Result), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_Result));
  bool success = test_msgs__action__NestedMessage_Result__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_Result__destroy(test_msgs__action__NestedMessage_Result * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_Result__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_Result__Sequence__init(test_msgs__action__NestedMessage_Result__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Result * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_Result *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_Result), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_Result__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_Result__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_Result__Sequence__fini(test_msgs__action__NestedMessage_Result__Sequence * array)
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
      test_msgs__action__NestedMessage_Result__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_Result__Sequence *
test_msgs__action__NestedMessage_Result__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Result__Sequence * array = (test_msgs__action__NestedMessage_Result__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Result__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_Result__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_Result__Sequence__destroy(test_msgs__action__NestedMessage_Result__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_Result__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_Result__Sequence__are_equal(const test_msgs__action__NestedMessage_Result__Sequence * lhs, const test_msgs__action__NestedMessage_Result__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_Result__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Result__Sequence__copy(
  const test_msgs__action__NestedMessage_Result__Sequence * input,
  test_msgs__action__NestedMessage_Result__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_Result);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_Result * data =
      (test_msgs__action__NestedMessage_Result *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_Result__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_Result__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_Result__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `nested_field_no_pkg`
// already included above
// #include "test_msgs/msg/detail/builtins__functions.h"
// Member `nested_field`
// already included above
// #include "test_msgs/msg/detail/basic_types__functions.h"
// Member `nested_different_pkg`
// already included above
// #include "builtin_interfaces/msg/detail/time__functions.h"

bool
test_msgs__action__NestedMessage_Feedback__init(test_msgs__action__NestedMessage_Feedback * msg)
{
  if (!msg) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__init(&msg->nested_field_no_pkg)) {
    test_msgs__action__NestedMessage_Feedback__fini(msg);
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__init(&msg->nested_field)) {
    test_msgs__action__NestedMessage_Feedback__fini(msg);
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__init(&msg->nested_different_pkg)) {
    test_msgs__action__NestedMessage_Feedback__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_Feedback__fini(test_msgs__action__NestedMessage_Feedback * msg)
{
  if (!msg) {
    return;
  }
  // nested_field_no_pkg
  test_msgs__msg__Builtins__fini(&msg->nested_field_no_pkg);
  // nested_field
  test_msgs__msg__BasicTypes__fini(&msg->nested_field);
  // nested_different_pkg
  builtin_interfaces__msg__Time__fini(&msg->nested_different_pkg);
}

bool
test_msgs__action__NestedMessage_Feedback__are_equal(const test_msgs__action__NestedMessage_Feedback * lhs, const test_msgs__action__NestedMessage_Feedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__are_equal(
      &(lhs->nested_field_no_pkg), &(rhs->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__are_equal(
      &(lhs->nested_field), &(rhs->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->nested_different_pkg), &(rhs->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Feedback__copy(
  const test_msgs__action__NestedMessage_Feedback * input,
  test_msgs__action__NestedMessage_Feedback * output)
{
  if (!input || !output) {
    return false;
  }
  // nested_field_no_pkg
  if (!test_msgs__msg__Builtins__copy(
      &(input->nested_field_no_pkg), &(output->nested_field_no_pkg)))
  {
    return false;
  }
  // nested_field
  if (!test_msgs__msg__BasicTypes__copy(
      &(input->nested_field), &(output->nested_field)))
  {
    return false;
  }
  // nested_different_pkg
  if (!builtin_interfaces__msg__Time__copy(
      &(input->nested_different_pkg), &(output->nested_different_pkg)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_Feedback *
test_msgs__action__NestedMessage_Feedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Feedback * msg = (test_msgs__action__NestedMessage_Feedback *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Feedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_Feedback));
  bool success = test_msgs__action__NestedMessage_Feedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_Feedback__destroy(test_msgs__action__NestedMessage_Feedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_Feedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_Feedback__Sequence__init(test_msgs__action__NestedMessage_Feedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Feedback * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_Feedback *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_Feedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_Feedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_Feedback__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_Feedback__Sequence__fini(test_msgs__action__NestedMessage_Feedback__Sequence * array)
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
      test_msgs__action__NestedMessage_Feedback__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_Feedback__Sequence *
test_msgs__action__NestedMessage_Feedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_Feedback__Sequence * array = (test_msgs__action__NestedMessage_Feedback__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_Feedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_Feedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_Feedback__Sequence__destroy(test_msgs__action__NestedMessage_Feedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_Feedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_Feedback__Sequence__are_equal(const test_msgs__action__NestedMessage_Feedback__Sequence * lhs, const test_msgs__action__NestedMessage_Feedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_Feedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_Feedback__Sequence__copy(
  const test_msgs__action__NestedMessage_Feedback__Sequence * input,
  test_msgs__action__NestedMessage_Feedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_Feedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_Feedback * data =
      (test_msgs__action__NestedMessage_Feedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_Feedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_Feedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_Feedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `goal`
// already included above
// #include "test_msgs/action/detail/nested_message__functions.h"

bool
test_msgs__action__NestedMessage_SendGoal_Request__init(test_msgs__action__NestedMessage_SendGoal_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    test_msgs__action__NestedMessage_SendGoal_Request__fini(msg);
    return false;
  }
  // goal
  if (!test_msgs__action__NestedMessage_Goal__init(&msg->goal)) {
    test_msgs__action__NestedMessage_SendGoal_Request__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_SendGoal_Request__fini(test_msgs__action__NestedMessage_SendGoal_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // goal
  test_msgs__action__NestedMessage_Goal__fini(&msg->goal);
}

bool
test_msgs__action__NestedMessage_SendGoal_Request__are_equal(const test_msgs__action__NestedMessage_SendGoal_Request * lhs, const test_msgs__action__NestedMessage_SendGoal_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // goal
  if (!test_msgs__action__NestedMessage_Goal__are_equal(
      &(lhs->goal), &(rhs->goal)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Request__copy(
  const test_msgs__action__NestedMessage_SendGoal_Request * input,
  test_msgs__action__NestedMessage_SendGoal_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // goal
  if (!test_msgs__action__NestedMessage_Goal__copy(
      &(input->goal), &(output->goal)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_SendGoal_Request *
test_msgs__action__NestedMessage_SendGoal_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Request * msg = (test_msgs__action__NestedMessage_SendGoal_Request *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_SendGoal_Request));
  bool success = test_msgs__action__NestedMessage_SendGoal_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_SendGoal_Request__destroy(test_msgs__action__NestedMessage_SendGoal_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_SendGoal_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__init(test_msgs__action__NestedMessage_SendGoal_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Request * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_SendGoal_Request *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_SendGoal_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_SendGoal_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_SendGoal_Request__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__fini(test_msgs__action__NestedMessage_SendGoal_Request__Sequence * array)
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
      test_msgs__action__NestedMessage_SendGoal_Request__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_SendGoal_Request__Sequence *
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Request__Sequence * array = (test_msgs__action__NestedMessage_SendGoal_Request__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_SendGoal_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__destroy(test_msgs__action__NestedMessage_SendGoal_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_SendGoal_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__are_equal(const test_msgs__action__NestedMessage_SendGoal_Request__Sequence * lhs, const test_msgs__action__NestedMessage_SendGoal_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Request__Sequence__copy(
  const test_msgs__action__NestedMessage_SendGoal_Request__Sequence * input,
  test_msgs__action__NestedMessage_SendGoal_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_SendGoal_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_SendGoal_Request * data =
      (test_msgs__action__NestedMessage_SendGoal_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_SendGoal_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_SendGoal_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `stamp`
// already included above
// #include "builtin_interfaces/msg/detail/time__functions.h"

bool
test_msgs__action__NestedMessage_SendGoal_Response__init(test_msgs__action__NestedMessage_SendGoal_Response * msg)
{
  if (!msg) {
    return false;
  }
  // accepted
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    test_msgs__action__NestedMessage_SendGoal_Response__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_SendGoal_Response__fini(test_msgs__action__NestedMessage_SendGoal_Response * msg)
{
  if (!msg) {
    return;
  }
  // accepted
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
test_msgs__action__NestedMessage_SendGoal_Response__are_equal(const test_msgs__action__NestedMessage_SendGoal_Response * lhs, const test_msgs__action__NestedMessage_SendGoal_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // accepted
  if (lhs->accepted != rhs->accepted) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Response__copy(
  const test_msgs__action__NestedMessage_SendGoal_Response * input,
  test_msgs__action__NestedMessage_SendGoal_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // accepted
  output->accepted = input->accepted;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_SendGoal_Response *
test_msgs__action__NestedMessage_SendGoal_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Response * msg = (test_msgs__action__NestedMessage_SendGoal_Response *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_SendGoal_Response));
  bool success = test_msgs__action__NestedMessage_SendGoal_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_SendGoal_Response__destroy(test_msgs__action__NestedMessage_SendGoal_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_SendGoal_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__init(test_msgs__action__NestedMessage_SendGoal_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Response * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_SendGoal_Response *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_SendGoal_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_SendGoal_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_SendGoal_Response__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__fini(test_msgs__action__NestedMessage_SendGoal_Response__Sequence * array)
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
      test_msgs__action__NestedMessage_SendGoal_Response__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_SendGoal_Response__Sequence *
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Response__Sequence * array = (test_msgs__action__NestedMessage_SendGoal_Response__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_SendGoal_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__destroy(test_msgs__action__NestedMessage_SendGoal_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_SendGoal_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__are_equal(const test_msgs__action__NestedMessage_SendGoal_Response__Sequence * lhs, const test_msgs__action__NestedMessage_SendGoal_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Response__Sequence__copy(
  const test_msgs__action__NestedMessage_SendGoal_Response__Sequence * input,
  test_msgs__action__NestedMessage_SendGoal_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_SendGoal_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_SendGoal_Response * data =
      (test_msgs__action__NestedMessage_SendGoal_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_SendGoal_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_SendGoal_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Response__copy(
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
// #include "test_msgs/action/detail/nested_message__functions.h"

bool
test_msgs__action__NestedMessage_SendGoal_Event__init(test_msgs__action__NestedMessage_SendGoal_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    test_msgs__action__NestedMessage_SendGoal_Event__fini(msg);
    return false;
  }
  // request
  if (!test_msgs__action__NestedMessage_SendGoal_Request__Sequence__init(&msg->request, 0)) {
    test_msgs__action__NestedMessage_SendGoal_Event__fini(msg);
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_SendGoal_Response__Sequence__init(&msg->response, 0)) {
    test_msgs__action__NestedMessage_SendGoal_Event__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_SendGoal_Event__fini(test_msgs__action__NestedMessage_SendGoal_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  test_msgs__action__NestedMessage_SendGoal_Request__Sequence__fini(&msg->request);
  // response
  test_msgs__action__NestedMessage_SendGoal_Response__Sequence__fini(&msg->response);
}

bool
test_msgs__action__NestedMessage_SendGoal_Event__are_equal(const test_msgs__action__NestedMessage_SendGoal_Event * lhs, const test_msgs__action__NestedMessage_SendGoal_Event * rhs)
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
  if (!test_msgs__action__NestedMessage_SendGoal_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_SendGoal_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Event__copy(
  const test_msgs__action__NestedMessage_SendGoal_Event * input,
  test_msgs__action__NestedMessage_SendGoal_Event * output)
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
  if (!test_msgs__action__NestedMessage_SendGoal_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_SendGoal_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_SendGoal_Event *
test_msgs__action__NestedMessage_SendGoal_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Event * msg = (test_msgs__action__NestedMessage_SendGoal_Event *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_SendGoal_Event));
  bool success = test_msgs__action__NestedMessage_SendGoal_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_SendGoal_Event__destroy(test_msgs__action__NestedMessage_SendGoal_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_SendGoal_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__init(test_msgs__action__NestedMessage_SendGoal_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Event * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_SendGoal_Event *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_SendGoal_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_SendGoal_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_SendGoal_Event__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__fini(test_msgs__action__NestedMessage_SendGoal_Event__Sequence * array)
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
      test_msgs__action__NestedMessage_SendGoal_Event__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_SendGoal_Event__Sequence *
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_SendGoal_Event__Sequence * array = (test_msgs__action__NestedMessage_SendGoal_Event__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_SendGoal_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_SendGoal_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__destroy(test_msgs__action__NestedMessage_SendGoal_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_SendGoal_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__are_equal(const test_msgs__action__NestedMessage_SendGoal_Event__Sequence * lhs, const test_msgs__action__NestedMessage_SendGoal_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_SendGoal_Event__Sequence__copy(
  const test_msgs__action__NestedMessage_SendGoal_Event__Sequence * input,
  test_msgs__action__NestedMessage_SendGoal_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_SendGoal_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_SendGoal_Event * data =
      (test_msgs__action__NestedMessage_SendGoal_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_SendGoal_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_SendGoal_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_SendGoal_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"

bool
test_msgs__action__NestedMessage_GetResult_Request__init(test_msgs__action__NestedMessage_GetResult_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    test_msgs__action__NestedMessage_GetResult_Request__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_GetResult_Request__fini(test_msgs__action__NestedMessage_GetResult_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
}

bool
test_msgs__action__NestedMessage_GetResult_Request__are_equal(const test_msgs__action__NestedMessage_GetResult_Request * lhs, const test_msgs__action__NestedMessage_GetResult_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Request__copy(
  const test_msgs__action__NestedMessage_GetResult_Request * input,
  test_msgs__action__NestedMessage_GetResult_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_GetResult_Request *
test_msgs__action__NestedMessage_GetResult_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Request * msg = (test_msgs__action__NestedMessage_GetResult_Request *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_GetResult_Request));
  bool success = test_msgs__action__NestedMessage_GetResult_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_GetResult_Request__destroy(test_msgs__action__NestedMessage_GetResult_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_GetResult_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_GetResult_Request__Sequence__init(test_msgs__action__NestedMessage_GetResult_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Request * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_GetResult_Request *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_GetResult_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_GetResult_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_GetResult_Request__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_GetResult_Request__Sequence__fini(test_msgs__action__NestedMessage_GetResult_Request__Sequence * array)
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
      test_msgs__action__NestedMessage_GetResult_Request__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_GetResult_Request__Sequence *
test_msgs__action__NestedMessage_GetResult_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Request__Sequence * array = (test_msgs__action__NestedMessage_GetResult_Request__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_GetResult_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_GetResult_Request__Sequence__destroy(test_msgs__action__NestedMessage_GetResult_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_GetResult_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_GetResult_Request__Sequence__are_equal(const test_msgs__action__NestedMessage_GetResult_Request__Sequence * lhs, const test_msgs__action__NestedMessage_GetResult_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Request__Sequence__copy(
  const test_msgs__action__NestedMessage_GetResult_Request__Sequence * input,
  test_msgs__action__NestedMessage_GetResult_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_GetResult_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_GetResult_Request * data =
      (test_msgs__action__NestedMessage_GetResult_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_GetResult_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_GetResult_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `result`
// already included above
// #include "test_msgs/action/detail/nested_message__functions.h"

bool
test_msgs__action__NestedMessage_GetResult_Response__init(test_msgs__action__NestedMessage_GetResult_Response * msg)
{
  if (!msg) {
    return false;
  }
  // status
  // result
  if (!test_msgs__action__NestedMessage_Result__init(&msg->result)) {
    test_msgs__action__NestedMessage_GetResult_Response__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_GetResult_Response__fini(test_msgs__action__NestedMessage_GetResult_Response * msg)
{
  if (!msg) {
    return;
  }
  // status
  // result
  test_msgs__action__NestedMessage_Result__fini(&msg->result);
}

bool
test_msgs__action__NestedMessage_GetResult_Response__are_equal(const test_msgs__action__NestedMessage_GetResult_Response * lhs, const test_msgs__action__NestedMessage_GetResult_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // status
  if (lhs->status != rhs->status) {
    return false;
  }
  // result
  if (!test_msgs__action__NestedMessage_Result__are_equal(
      &(lhs->result), &(rhs->result)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Response__copy(
  const test_msgs__action__NestedMessage_GetResult_Response * input,
  test_msgs__action__NestedMessage_GetResult_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // status
  output->status = input->status;
  // result
  if (!test_msgs__action__NestedMessage_Result__copy(
      &(input->result), &(output->result)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_GetResult_Response *
test_msgs__action__NestedMessage_GetResult_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Response * msg = (test_msgs__action__NestedMessage_GetResult_Response *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_GetResult_Response));
  bool success = test_msgs__action__NestedMessage_GetResult_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_GetResult_Response__destroy(test_msgs__action__NestedMessage_GetResult_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_GetResult_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_GetResult_Response__Sequence__init(test_msgs__action__NestedMessage_GetResult_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Response * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_GetResult_Response *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_GetResult_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_GetResult_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_GetResult_Response__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_GetResult_Response__Sequence__fini(test_msgs__action__NestedMessage_GetResult_Response__Sequence * array)
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
      test_msgs__action__NestedMessage_GetResult_Response__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_GetResult_Response__Sequence *
test_msgs__action__NestedMessage_GetResult_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Response__Sequence * array = (test_msgs__action__NestedMessage_GetResult_Response__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_GetResult_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_GetResult_Response__Sequence__destroy(test_msgs__action__NestedMessage_GetResult_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_GetResult_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_GetResult_Response__Sequence__are_equal(const test_msgs__action__NestedMessage_GetResult_Response__Sequence * lhs, const test_msgs__action__NestedMessage_GetResult_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Response__Sequence__copy(
  const test_msgs__action__NestedMessage_GetResult_Response__Sequence * input,
  test_msgs__action__NestedMessage_GetResult_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_GetResult_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_GetResult_Response * data =
      (test_msgs__action__NestedMessage_GetResult_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_GetResult_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_GetResult_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
// already included above
// #include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "test_msgs/action/detail/nested_message__functions.h"

bool
test_msgs__action__NestedMessage_GetResult_Event__init(test_msgs__action__NestedMessage_GetResult_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    test_msgs__action__NestedMessage_GetResult_Event__fini(msg);
    return false;
  }
  // request
  if (!test_msgs__action__NestedMessage_GetResult_Request__Sequence__init(&msg->request, 0)) {
    test_msgs__action__NestedMessage_GetResult_Event__fini(msg);
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_GetResult_Response__Sequence__init(&msg->response, 0)) {
    test_msgs__action__NestedMessage_GetResult_Event__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_GetResult_Event__fini(test_msgs__action__NestedMessage_GetResult_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  test_msgs__action__NestedMessage_GetResult_Request__Sequence__fini(&msg->request);
  // response
  test_msgs__action__NestedMessage_GetResult_Response__Sequence__fini(&msg->response);
}

bool
test_msgs__action__NestedMessage_GetResult_Event__are_equal(const test_msgs__action__NestedMessage_GetResult_Event * lhs, const test_msgs__action__NestedMessage_GetResult_Event * rhs)
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
  if (!test_msgs__action__NestedMessage_GetResult_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_GetResult_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Event__copy(
  const test_msgs__action__NestedMessage_GetResult_Event * input,
  test_msgs__action__NestedMessage_GetResult_Event * output)
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
  if (!test_msgs__action__NestedMessage_GetResult_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!test_msgs__action__NestedMessage_GetResult_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_GetResult_Event *
test_msgs__action__NestedMessage_GetResult_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Event * msg = (test_msgs__action__NestedMessage_GetResult_Event *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_GetResult_Event));
  bool success = test_msgs__action__NestedMessage_GetResult_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_GetResult_Event__destroy(test_msgs__action__NestedMessage_GetResult_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_GetResult_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_GetResult_Event__Sequence__init(test_msgs__action__NestedMessage_GetResult_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Event * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_GetResult_Event *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_GetResult_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_GetResult_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_GetResult_Event__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_GetResult_Event__Sequence__fini(test_msgs__action__NestedMessage_GetResult_Event__Sequence * array)
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
      test_msgs__action__NestedMessage_GetResult_Event__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_GetResult_Event__Sequence *
test_msgs__action__NestedMessage_GetResult_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_GetResult_Event__Sequence * array = (test_msgs__action__NestedMessage_GetResult_Event__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_GetResult_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_GetResult_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_GetResult_Event__Sequence__destroy(test_msgs__action__NestedMessage_GetResult_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_GetResult_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_GetResult_Event__Sequence__are_equal(const test_msgs__action__NestedMessage_GetResult_Event__Sequence * lhs, const test_msgs__action__NestedMessage_GetResult_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_GetResult_Event__Sequence__copy(
  const test_msgs__action__NestedMessage_GetResult_Event__Sequence * input,
  test_msgs__action__NestedMessage_GetResult_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_GetResult_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_GetResult_Event * data =
      (test_msgs__action__NestedMessage_GetResult_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_GetResult_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_GetResult_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_GetResult_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `feedback`
// already included above
// #include "test_msgs/action/detail/nested_message__functions.h"

bool
test_msgs__action__NestedMessage_FeedbackMessage__init(test_msgs__action__NestedMessage_FeedbackMessage * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    test_msgs__action__NestedMessage_FeedbackMessage__fini(msg);
    return false;
  }
  // feedback
  if (!test_msgs__action__NestedMessage_Feedback__init(&msg->feedback)) {
    test_msgs__action__NestedMessage_FeedbackMessage__fini(msg);
    return false;
  }
  return true;
}

void
test_msgs__action__NestedMessage_FeedbackMessage__fini(test_msgs__action__NestedMessage_FeedbackMessage * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // feedback
  test_msgs__action__NestedMessage_Feedback__fini(&msg->feedback);
}

bool
test_msgs__action__NestedMessage_FeedbackMessage__are_equal(const test_msgs__action__NestedMessage_FeedbackMessage * lhs, const test_msgs__action__NestedMessage_FeedbackMessage * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // feedback
  if (!test_msgs__action__NestedMessage_Feedback__are_equal(
      &(lhs->feedback), &(rhs->feedback)))
  {
    return false;
  }
  return true;
}

bool
test_msgs__action__NestedMessage_FeedbackMessage__copy(
  const test_msgs__action__NestedMessage_FeedbackMessage * input,
  test_msgs__action__NestedMessage_FeedbackMessage * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // feedback
  if (!test_msgs__action__NestedMessage_Feedback__copy(
      &(input->feedback), &(output->feedback)))
  {
    return false;
  }
  return true;
}

test_msgs__action__NestedMessage_FeedbackMessage *
test_msgs__action__NestedMessage_FeedbackMessage__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_FeedbackMessage * msg = (test_msgs__action__NestedMessage_FeedbackMessage *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_FeedbackMessage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__action__NestedMessage_FeedbackMessage));
  bool success = test_msgs__action__NestedMessage_FeedbackMessage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__action__NestedMessage_FeedbackMessage__destroy(test_msgs__action__NestedMessage_FeedbackMessage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__action__NestedMessage_FeedbackMessage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__init(test_msgs__action__NestedMessage_FeedbackMessage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_FeedbackMessage * data = NULL;

  if (size) {
    data = (test_msgs__action__NestedMessage_FeedbackMessage *)allocator.zero_allocate(size, sizeof(test_msgs__action__NestedMessage_FeedbackMessage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__action__NestedMessage_FeedbackMessage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__action__NestedMessage_FeedbackMessage__fini(&data[i - 1]);
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
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__fini(test_msgs__action__NestedMessage_FeedbackMessage__Sequence * array)
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
      test_msgs__action__NestedMessage_FeedbackMessage__fini(&array->data[i]);
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

test_msgs__action__NestedMessage_FeedbackMessage__Sequence *
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__action__NestedMessage_FeedbackMessage__Sequence * array = (test_msgs__action__NestedMessage_FeedbackMessage__Sequence *)allocator.allocate(sizeof(test_msgs__action__NestedMessage_FeedbackMessage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__action__NestedMessage_FeedbackMessage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__destroy(test_msgs__action__NestedMessage_FeedbackMessage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__action__NestedMessage_FeedbackMessage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__are_equal(const test_msgs__action__NestedMessage_FeedbackMessage__Sequence * lhs, const test_msgs__action__NestedMessage_FeedbackMessage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__action__NestedMessage_FeedbackMessage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__action__NestedMessage_FeedbackMessage__Sequence__copy(
  const test_msgs__action__NestedMessage_FeedbackMessage__Sequence * input,
  test_msgs__action__NestedMessage_FeedbackMessage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__action__NestedMessage_FeedbackMessage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__action__NestedMessage_FeedbackMessage * data =
      (test_msgs__action__NestedMessage_FeedbackMessage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__action__NestedMessage_FeedbackMessage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__action__NestedMessage_FeedbackMessage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__action__NestedMessage_FeedbackMessage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
