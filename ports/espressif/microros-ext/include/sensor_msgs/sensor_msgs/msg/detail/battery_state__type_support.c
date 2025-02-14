// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from sensor_msgs:msg/BatteryState.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "sensor_msgs/msg/detail/battery_state__rosidl_typesupport_introspection_c.h"
#include "sensor_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "sensor_msgs/msg/detail/battery_state__functions.h"
#include "sensor_msgs/msg/detail/battery_state__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `cell_voltage`
// Member `cell_temperature`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `location`
// Member `serial_number`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  sensor_msgs__msg__BatteryState__init(message_memory);
}

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_fini_function(void * message_memory)
{
  sensor_msgs__msg__BatteryState__fini(message_memory);
}

size_t sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__size_function__BatteryState__cell_voltage(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_voltage(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_voltage(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__fetch_function__BatteryState__cell_voltage(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_voltage(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__assign_function__BatteryState__cell_voltage(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_voltage(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__resize_function__BatteryState__cell_voltage(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

size_t sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__size_function__BatteryState__cell_temperature(
  const void * untyped_member)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return member->size;
}

const void * sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_temperature(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__float__Sequence * member =
    (const rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void * sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_temperature(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  return &member->data[index];
}

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__fetch_function__BatteryState__cell_temperature(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_temperature(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__assign_function__BatteryState__cell_temperature(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_temperature(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

bool sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__resize_function__BatteryState__cell_temperature(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__float__Sequence * member =
    (rosidl_runtime_c__float__Sequence *)(untyped_member);
  rosidl_runtime_c__float__Sequence__fini(member);
  return rosidl_runtime_c__float__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_member_array[16] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "voltage",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, voltage),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "temperature",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, temperature),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "current",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, current),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "charge",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, charge),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "capacity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, capacity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "design_capacity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, design_capacity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "percentage",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, percentage),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "power_supply_status",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, power_supply_status),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "power_supply_health",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, power_supply_health),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "power_supply_technology",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, power_supply_technology),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "present",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, present),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "cell_voltage",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, cell_voltage),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__size_function__BatteryState__cell_voltage,  // size() function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_voltage,  // get_const(index) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_voltage,  // get(index) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__fetch_function__BatteryState__cell_voltage,  // fetch(index, &value) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__assign_function__BatteryState__cell_voltage,  // assign(index, value) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__resize_function__BatteryState__cell_voltage  // resize(index) function pointer
  },
  {
    "cell_temperature",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, cell_temperature),  // bytes offset in struct
    NULL,  // default value
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__size_function__BatteryState__cell_temperature,  // size() function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_const_function__BatteryState__cell_temperature,  // get_const(index) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__get_function__BatteryState__cell_temperature,  // get(index) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__fetch_function__BatteryState__cell_temperature,  // fetch(index, &value) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__assign_function__BatteryState__cell_temperature,  // assign(index, value) function pointer
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__resize_function__BatteryState__cell_temperature  // resize(index) function pointer
  },
  {
    "location",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, location),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "serial_number",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(sensor_msgs__msg__BatteryState, serial_number),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_members = {
  "sensor_msgs__msg",  // message namespace
  "BatteryState",  // message name
  16,  // number of fields
  sizeof(sensor_msgs__msg__BatteryState),
  false,  // has_any_key_member_
  sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_member_array,  // message members
  sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_init_function,  // function to initialize message memory (memory has to be allocated)
  sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_type_support_handle = {
  0,
  &sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_members,
  get_message_typesupport_handle_function,
  &sensor_msgs__msg__BatteryState__get_type_hash,
  &sensor_msgs__msg__BatteryState__get_type_description,
  &sensor_msgs__msg__BatteryState__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_sensor_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, BatteryState)() {
  sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_type_support_handle.typesupport_identifier) {
    sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &sensor_msgs__msg__BatteryState__rosidl_typesupport_introspection_c__BatteryState_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
