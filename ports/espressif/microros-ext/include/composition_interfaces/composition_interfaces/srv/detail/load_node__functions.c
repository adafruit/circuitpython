// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from composition_interfaces:srv/LoadNode.idl
// generated code does not contain a copyright notice
#include "composition_interfaces/srv/detail/load_node__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `package_name`
// Member `plugin_name`
// Member `node_name`
// Member `node_namespace`
// Member `remap_rules`
#include "rosidl_runtime_c/string_functions.h"
// Member `parameters`
// Member `extra_arguments`
#include "rcl_interfaces/msg/detail/parameter__functions.h"

bool
composition_interfaces__srv__LoadNode_Request__init(composition_interfaces__srv__LoadNode_Request * msg)
{
  if (!msg) {
    return false;
  }
  // package_name
  if (!rosidl_runtime_c__String__init(&msg->package_name)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // plugin_name
  if (!rosidl_runtime_c__String__init(&msg->plugin_name)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__init(&msg->node_name)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__init(&msg->node_namespace)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // log_level
  // remap_rules
  if (!rosidl_runtime_c__String__Sequence__init(&msg->remap_rules, 0)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__Parameter__Sequence__init(&msg->parameters, 0)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  // extra_arguments
  if (!rcl_interfaces__msg__Parameter__Sequence__init(&msg->extra_arguments, 0)) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
    return false;
  }
  return true;
}

void
composition_interfaces__srv__LoadNode_Request__fini(composition_interfaces__srv__LoadNode_Request * msg)
{
  if (!msg) {
    return;
  }
  // package_name
  rosidl_runtime_c__String__fini(&msg->package_name);
  // plugin_name
  rosidl_runtime_c__String__fini(&msg->plugin_name);
  // node_name
  rosidl_runtime_c__String__fini(&msg->node_name);
  // node_namespace
  rosidl_runtime_c__String__fini(&msg->node_namespace);
  // log_level
  // remap_rules
  rosidl_runtime_c__String__Sequence__fini(&msg->remap_rules);
  // parameters
  rcl_interfaces__msg__Parameter__Sequence__fini(&msg->parameters);
  // extra_arguments
  rcl_interfaces__msg__Parameter__Sequence__fini(&msg->extra_arguments);
}

bool
composition_interfaces__srv__LoadNode_Request__are_equal(const composition_interfaces__srv__LoadNode_Request * lhs, const composition_interfaces__srv__LoadNode_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // package_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->package_name), &(rhs->package_name)))
  {
    return false;
  }
  // plugin_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->plugin_name), &(rhs->plugin_name)))
  {
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->node_name), &(rhs->node_name)))
  {
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->node_namespace), &(rhs->node_namespace)))
  {
    return false;
  }
  // log_level
  if (lhs->log_level != rhs->log_level) {
    return false;
  }
  // remap_rules
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->remap_rules), &(rhs->remap_rules)))
  {
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__Parameter__Sequence__are_equal(
      &(lhs->parameters), &(rhs->parameters)))
  {
    return false;
  }
  // extra_arguments
  if (!rcl_interfaces__msg__Parameter__Sequence__are_equal(
      &(lhs->extra_arguments), &(rhs->extra_arguments)))
  {
    return false;
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Request__copy(
  const composition_interfaces__srv__LoadNode_Request * input,
  composition_interfaces__srv__LoadNode_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // package_name
  if (!rosidl_runtime_c__String__copy(
      &(input->package_name), &(output->package_name)))
  {
    return false;
  }
  // plugin_name
  if (!rosidl_runtime_c__String__copy(
      &(input->plugin_name), &(output->plugin_name)))
  {
    return false;
  }
  // node_name
  if (!rosidl_runtime_c__String__copy(
      &(input->node_name), &(output->node_name)))
  {
    return false;
  }
  // node_namespace
  if (!rosidl_runtime_c__String__copy(
      &(input->node_namespace), &(output->node_namespace)))
  {
    return false;
  }
  // log_level
  output->log_level = input->log_level;
  // remap_rules
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->remap_rules), &(output->remap_rules)))
  {
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__Parameter__Sequence__copy(
      &(input->parameters), &(output->parameters)))
  {
    return false;
  }
  // extra_arguments
  if (!rcl_interfaces__msg__Parameter__Sequence__copy(
      &(input->extra_arguments), &(output->extra_arguments)))
  {
    return false;
  }
  return true;
}

composition_interfaces__srv__LoadNode_Request *
composition_interfaces__srv__LoadNode_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Request * msg = (composition_interfaces__srv__LoadNode_Request *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(composition_interfaces__srv__LoadNode_Request));
  bool success = composition_interfaces__srv__LoadNode_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
composition_interfaces__srv__LoadNode_Request__destroy(composition_interfaces__srv__LoadNode_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    composition_interfaces__srv__LoadNode_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
composition_interfaces__srv__LoadNode_Request__Sequence__init(composition_interfaces__srv__LoadNode_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Request * data = NULL;

  if (size) {
    data = (composition_interfaces__srv__LoadNode_Request *)allocator.zero_allocate(size, sizeof(composition_interfaces__srv__LoadNode_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = composition_interfaces__srv__LoadNode_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        composition_interfaces__srv__LoadNode_Request__fini(&data[i - 1]);
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
composition_interfaces__srv__LoadNode_Request__Sequence__fini(composition_interfaces__srv__LoadNode_Request__Sequence * array)
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
      composition_interfaces__srv__LoadNode_Request__fini(&array->data[i]);
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

composition_interfaces__srv__LoadNode_Request__Sequence *
composition_interfaces__srv__LoadNode_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Request__Sequence * array = (composition_interfaces__srv__LoadNode_Request__Sequence *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = composition_interfaces__srv__LoadNode_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
composition_interfaces__srv__LoadNode_Request__Sequence__destroy(composition_interfaces__srv__LoadNode_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    composition_interfaces__srv__LoadNode_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
composition_interfaces__srv__LoadNode_Request__Sequence__are_equal(const composition_interfaces__srv__LoadNode_Request__Sequence * lhs, const composition_interfaces__srv__LoadNode_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Request__Sequence__copy(
  const composition_interfaces__srv__LoadNode_Request__Sequence * input,
  composition_interfaces__srv__LoadNode_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(composition_interfaces__srv__LoadNode_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    composition_interfaces__srv__LoadNode_Request * data =
      (composition_interfaces__srv__LoadNode_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!composition_interfaces__srv__LoadNode_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          composition_interfaces__srv__LoadNode_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `error_message`
// Member `full_node_name`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

bool
composition_interfaces__srv__LoadNode_Response__init(composition_interfaces__srv__LoadNode_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // error_message
  if (!rosidl_runtime_c__String__init(&msg->error_message)) {
    composition_interfaces__srv__LoadNode_Response__fini(msg);
    return false;
  }
  // full_node_name
  if (!rosidl_runtime_c__String__init(&msg->full_node_name)) {
    composition_interfaces__srv__LoadNode_Response__fini(msg);
    return false;
  }
  // unique_id
  return true;
}

void
composition_interfaces__srv__LoadNode_Response__fini(composition_interfaces__srv__LoadNode_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // error_message
  rosidl_runtime_c__String__fini(&msg->error_message);
  // full_node_name
  rosidl_runtime_c__String__fini(&msg->full_node_name);
  // unique_id
}

bool
composition_interfaces__srv__LoadNode_Response__are_equal(const composition_interfaces__srv__LoadNode_Response * lhs, const composition_interfaces__srv__LoadNode_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // error_message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->error_message), &(rhs->error_message)))
  {
    return false;
  }
  // full_node_name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->full_node_name), &(rhs->full_node_name)))
  {
    return false;
  }
  // unique_id
  if (lhs->unique_id != rhs->unique_id) {
    return false;
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Response__copy(
  const composition_interfaces__srv__LoadNode_Response * input,
  composition_interfaces__srv__LoadNode_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // error_message
  if (!rosidl_runtime_c__String__copy(
      &(input->error_message), &(output->error_message)))
  {
    return false;
  }
  // full_node_name
  if (!rosidl_runtime_c__String__copy(
      &(input->full_node_name), &(output->full_node_name)))
  {
    return false;
  }
  // unique_id
  output->unique_id = input->unique_id;
  return true;
}

composition_interfaces__srv__LoadNode_Response *
composition_interfaces__srv__LoadNode_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Response * msg = (composition_interfaces__srv__LoadNode_Response *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(composition_interfaces__srv__LoadNode_Response));
  bool success = composition_interfaces__srv__LoadNode_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
composition_interfaces__srv__LoadNode_Response__destroy(composition_interfaces__srv__LoadNode_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    composition_interfaces__srv__LoadNode_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
composition_interfaces__srv__LoadNode_Response__Sequence__init(composition_interfaces__srv__LoadNode_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Response * data = NULL;

  if (size) {
    data = (composition_interfaces__srv__LoadNode_Response *)allocator.zero_allocate(size, sizeof(composition_interfaces__srv__LoadNode_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = composition_interfaces__srv__LoadNode_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        composition_interfaces__srv__LoadNode_Response__fini(&data[i - 1]);
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
composition_interfaces__srv__LoadNode_Response__Sequence__fini(composition_interfaces__srv__LoadNode_Response__Sequence * array)
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
      composition_interfaces__srv__LoadNode_Response__fini(&array->data[i]);
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

composition_interfaces__srv__LoadNode_Response__Sequence *
composition_interfaces__srv__LoadNode_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Response__Sequence * array = (composition_interfaces__srv__LoadNode_Response__Sequence *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = composition_interfaces__srv__LoadNode_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
composition_interfaces__srv__LoadNode_Response__Sequence__destroy(composition_interfaces__srv__LoadNode_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    composition_interfaces__srv__LoadNode_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
composition_interfaces__srv__LoadNode_Response__Sequence__are_equal(const composition_interfaces__srv__LoadNode_Response__Sequence * lhs, const composition_interfaces__srv__LoadNode_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Response__Sequence__copy(
  const composition_interfaces__srv__LoadNode_Response__Sequence * input,
  composition_interfaces__srv__LoadNode_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(composition_interfaces__srv__LoadNode_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    composition_interfaces__srv__LoadNode_Response * data =
      (composition_interfaces__srv__LoadNode_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!composition_interfaces__srv__LoadNode_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          composition_interfaces__srv__LoadNode_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Response__copy(
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
// #include "composition_interfaces/srv/detail/load_node__functions.h"

bool
composition_interfaces__srv__LoadNode_Event__init(composition_interfaces__srv__LoadNode_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    composition_interfaces__srv__LoadNode_Event__fini(msg);
    return false;
  }
  // request
  if (!composition_interfaces__srv__LoadNode_Request__Sequence__init(&msg->request, 0)) {
    composition_interfaces__srv__LoadNode_Event__fini(msg);
    return false;
  }
  // response
  if (!composition_interfaces__srv__LoadNode_Response__Sequence__init(&msg->response, 0)) {
    composition_interfaces__srv__LoadNode_Event__fini(msg);
    return false;
  }
  return true;
}

void
composition_interfaces__srv__LoadNode_Event__fini(composition_interfaces__srv__LoadNode_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  composition_interfaces__srv__LoadNode_Request__Sequence__fini(&msg->request);
  // response
  composition_interfaces__srv__LoadNode_Response__Sequence__fini(&msg->response);
}

bool
composition_interfaces__srv__LoadNode_Event__are_equal(const composition_interfaces__srv__LoadNode_Event * lhs, const composition_interfaces__srv__LoadNode_Event * rhs)
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
  if (!composition_interfaces__srv__LoadNode_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!composition_interfaces__srv__LoadNode_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Event__copy(
  const composition_interfaces__srv__LoadNode_Event * input,
  composition_interfaces__srv__LoadNode_Event * output)
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
  if (!composition_interfaces__srv__LoadNode_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!composition_interfaces__srv__LoadNode_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

composition_interfaces__srv__LoadNode_Event *
composition_interfaces__srv__LoadNode_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Event * msg = (composition_interfaces__srv__LoadNode_Event *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(composition_interfaces__srv__LoadNode_Event));
  bool success = composition_interfaces__srv__LoadNode_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
composition_interfaces__srv__LoadNode_Event__destroy(composition_interfaces__srv__LoadNode_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    composition_interfaces__srv__LoadNode_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
composition_interfaces__srv__LoadNode_Event__Sequence__init(composition_interfaces__srv__LoadNode_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Event * data = NULL;

  if (size) {
    data = (composition_interfaces__srv__LoadNode_Event *)allocator.zero_allocate(size, sizeof(composition_interfaces__srv__LoadNode_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = composition_interfaces__srv__LoadNode_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        composition_interfaces__srv__LoadNode_Event__fini(&data[i - 1]);
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
composition_interfaces__srv__LoadNode_Event__Sequence__fini(composition_interfaces__srv__LoadNode_Event__Sequence * array)
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
      composition_interfaces__srv__LoadNode_Event__fini(&array->data[i]);
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

composition_interfaces__srv__LoadNode_Event__Sequence *
composition_interfaces__srv__LoadNode_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__LoadNode_Event__Sequence * array = (composition_interfaces__srv__LoadNode_Event__Sequence *)allocator.allocate(sizeof(composition_interfaces__srv__LoadNode_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = composition_interfaces__srv__LoadNode_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
composition_interfaces__srv__LoadNode_Event__Sequence__destroy(composition_interfaces__srv__LoadNode_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    composition_interfaces__srv__LoadNode_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
composition_interfaces__srv__LoadNode_Event__Sequence__are_equal(const composition_interfaces__srv__LoadNode_Event__Sequence * lhs, const composition_interfaces__srv__LoadNode_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
composition_interfaces__srv__LoadNode_Event__Sequence__copy(
  const composition_interfaces__srv__LoadNode_Event__Sequence * input,
  composition_interfaces__srv__LoadNode_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(composition_interfaces__srv__LoadNode_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    composition_interfaces__srv__LoadNode_Event * data =
      (composition_interfaces__srv__LoadNode_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!composition_interfaces__srv__LoadNode_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          composition_interfaces__srv__LoadNode_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!composition_interfaces__srv__LoadNode_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
