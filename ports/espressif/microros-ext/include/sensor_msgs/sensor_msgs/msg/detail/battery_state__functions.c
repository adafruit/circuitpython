// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sensor_msgs:msg/BatteryState.idl
// generated code does not contain a copyright notice
#include "sensor_msgs/msg/detail/battery_state__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `cell_voltage`
// Member `cell_temperature`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `location`
// Member `serial_number`
#include "rosidl_runtime_c/string_functions.h"

bool
sensor_msgs__msg__BatteryState__init(sensor_msgs__msg__BatteryState * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    sensor_msgs__msg__BatteryState__fini(msg);
    return false;
  }
  // voltage
  // temperature
  // current
  // charge
  // capacity
  // design_capacity
  // percentage
  // power_supply_status
  // power_supply_health
  // power_supply_technology
  // present
  // cell_voltage
  if (!rosidl_runtime_c__float__Sequence__init(&msg->cell_voltage, 0)) {
    sensor_msgs__msg__BatteryState__fini(msg);
    return false;
  }
  // cell_temperature
  if (!rosidl_runtime_c__float__Sequence__init(&msg->cell_temperature, 0)) {
    sensor_msgs__msg__BatteryState__fini(msg);
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__init(&msg->location)) {
    sensor_msgs__msg__BatteryState__fini(msg);
    return false;
  }
  // serial_number
  if (!rosidl_runtime_c__String__init(&msg->serial_number)) {
    sensor_msgs__msg__BatteryState__fini(msg);
    return false;
  }
  return true;
}

void
sensor_msgs__msg__BatteryState__fini(sensor_msgs__msg__BatteryState * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // voltage
  // temperature
  // current
  // charge
  // capacity
  // design_capacity
  // percentage
  // power_supply_status
  // power_supply_health
  // power_supply_technology
  // present
  // cell_voltage
  rosidl_runtime_c__float__Sequence__fini(&msg->cell_voltage);
  // cell_temperature
  rosidl_runtime_c__float__Sequence__fini(&msg->cell_temperature);
  // location
  rosidl_runtime_c__String__fini(&msg->location);
  // serial_number
  rosidl_runtime_c__String__fini(&msg->serial_number);
}

bool
sensor_msgs__msg__BatteryState__are_equal(const sensor_msgs__msg__BatteryState * lhs, const sensor_msgs__msg__BatteryState * rhs)
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
  // voltage
  if (lhs->voltage != rhs->voltage) {
    return false;
  }
  // temperature
  if (lhs->temperature != rhs->temperature) {
    return false;
  }
  // current
  if (lhs->current != rhs->current) {
    return false;
  }
  // charge
  if (lhs->charge != rhs->charge) {
    return false;
  }
  // capacity
  if (lhs->capacity != rhs->capacity) {
    return false;
  }
  // design_capacity
  if (lhs->design_capacity != rhs->design_capacity) {
    return false;
  }
  // percentage
  if (lhs->percentage != rhs->percentage) {
    return false;
  }
  // power_supply_status
  if (lhs->power_supply_status != rhs->power_supply_status) {
    return false;
  }
  // power_supply_health
  if (lhs->power_supply_health != rhs->power_supply_health) {
    return false;
  }
  // power_supply_technology
  if (lhs->power_supply_technology != rhs->power_supply_technology) {
    return false;
  }
  // present
  if (lhs->present != rhs->present) {
    return false;
  }
  // cell_voltage
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->cell_voltage), &(rhs->cell_voltage)))
  {
    return false;
  }
  // cell_temperature
  if (!rosidl_runtime_c__float__Sequence__are_equal(
      &(lhs->cell_temperature), &(rhs->cell_temperature)))
  {
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->location), &(rhs->location)))
  {
    return false;
  }
  // serial_number
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->serial_number), &(rhs->serial_number)))
  {
    return false;
  }
  return true;
}

bool
sensor_msgs__msg__BatteryState__copy(
  const sensor_msgs__msg__BatteryState * input,
  sensor_msgs__msg__BatteryState * output)
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
  // voltage
  output->voltage = input->voltage;
  // temperature
  output->temperature = input->temperature;
  // current
  output->current = input->current;
  // charge
  output->charge = input->charge;
  // capacity
  output->capacity = input->capacity;
  // design_capacity
  output->design_capacity = input->design_capacity;
  // percentage
  output->percentage = input->percentage;
  // power_supply_status
  output->power_supply_status = input->power_supply_status;
  // power_supply_health
  output->power_supply_health = input->power_supply_health;
  // power_supply_technology
  output->power_supply_technology = input->power_supply_technology;
  // present
  output->present = input->present;
  // cell_voltage
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->cell_voltage), &(output->cell_voltage)))
  {
    return false;
  }
  // cell_temperature
  if (!rosidl_runtime_c__float__Sequence__copy(
      &(input->cell_temperature), &(output->cell_temperature)))
  {
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__copy(
      &(input->location), &(output->location)))
  {
    return false;
  }
  // serial_number
  if (!rosidl_runtime_c__String__copy(
      &(input->serial_number), &(output->serial_number)))
  {
    return false;
  }
  return true;
}

sensor_msgs__msg__BatteryState *
sensor_msgs__msg__BatteryState__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__BatteryState * msg = (sensor_msgs__msg__BatteryState *)allocator.allocate(sizeof(sensor_msgs__msg__BatteryState), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sensor_msgs__msg__BatteryState));
  bool success = sensor_msgs__msg__BatteryState__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sensor_msgs__msg__BatteryState__destroy(sensor_msgs__msg__BatteryState * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sensor_msgs__msg__BatteryState__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sensor_msgs__msg__BatteryState__Sequence__init(sensor_msgs__msg__BatteryState__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__BatteryState * data = NULL;

  if (size) {
    data = (sensor_msgs__msg__BatteryState *)allocator.zero_allocate(size, sizeof(sensor_msgs__msg__BatteryState), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sensor_msgs__msg__BatteryState__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sensor_msgs__msg__BatteryState__fini(&data[i - 1]);
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
sensor_msgs__msg__BatteryState__Sequence__fini(sensor_msgs__msg__BatteryState__Sequence * array)
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
      sensor_msgs__msg__BatteryState__fini(&array->data[i]);
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

sensor_msgs__msg__BatteryState__Sequence *
sensor_msgs__msg__BatteryState__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sensor_msgs__msg__BatteryState__Sequence * array = (sensor_msgs__msg__BatteryState__Sequence *)allocator.allocate(sizeof(sensor_msgs__msg__BatteryState__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sensor_msgs__msg__BatteryState__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sensor_msgs__msg__BatteryState__Sequence__destroy(sensor_msgs__msg__BatteryState__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sensor_msgs__msg__BatteryState__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sensor_msgs__msg__BatteryState__Sequence__are_equal(const sensor_msgs__msg__BatteryState__Sequence * lhs, const sensor_msgs__msg__BatteryState__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sensor_msgs__msg__BatteryState__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sensor_msgs__msg__BatteryState__Sequence__copy(
  const sensor_msgs__msg__BatteryState__Sequence * input,
  sensor_msgs__msg__BatteryState__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sensor_msgs__msg__BatteryState);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sensor_msgs__msg__BatteryState * data =
      (sensor_msgs__msg__BatteryState *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sensor_msgs__msg__BatteryState__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sensor_msgs__msg__BatteryState__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sensor_msgs__msg__BatteryState__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
