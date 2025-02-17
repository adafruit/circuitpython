// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:msg/BatteryState.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/msg/battery_state.h"


#ifndef SENSOR_MSGS__MSG__DETAIL__BATTERY_STATE__STRUCT_H_
#define SENSOR_MSGS__MSG__DETAIL__BATTERY_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'POWER_SUPPLY_STATUS_UNKNOWN'.
/**
  * Constants are chosen to match the enums in the linux kernel
  * defined in include/linux/power_supply.h as of version 3.7
  * The one difference is for style reasons the constants are
  * all uppercase not mixed case.
  * Power supply status constants
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_UNKNOWN = 0
};

/// Constant 'POWER_SUPPLY_STATUS_CHARGING'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_CHARGING = 1
};

/// Constant 'POWER_SUPPLY_STATUS_DISCHARGING'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_DISCHARGING = 2
};

/// Constant 'POWER_SUPPLY_STATUS_NOT_CHARGING'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_NOT_CHARGING = 3
};

/// Constant 'POWER_SUPPLY_STATUS_FULL'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_FULL = 4
};

/// Constant 'POWER_SUPPLY_HEALTH_UNKNOWN'.
/**
  * Power supply health constants
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_UNKNOWN = 0
};

/// Constant 'POWER_SUPPLY_HEALTH_GOOD'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_GOOD = 1
};

/// Constant 'POWER_SUPPLY_HEALTH_OVERHEAT'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_OVERHEAT = 2
};

/// Constant 'POWER_SUPPLY_HEALTH_DEAD'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_DEAD = 3
};

/// Constant 'POWER_SUPPLY_HEALTH_OVERVOLTAGE'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4
};

/// Constant 'POWER_SUPPLY_HEALTH_UNSPEC_FAILURE'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5
};

/// Constant 'POWER_SUPPLY_HEALTH_COLD'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_COLD = 6
};

/// Constant 'POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7
};

/// Constant 'POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE'.
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_UNKNOWN'.
/**
  * Power supply technology (chemistry) constants
  * Unknown battery technology
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_NIMH'.
/**
  * Nickel-Metal Hydride battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_NIMH = 1
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_LION'.
/**
  * Lithium-ion battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LION = 2
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_LIPO'.
/**
  * Lithium Polymer battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LIPO = 3
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_LIFE'.
/**
  * Lithium Iron Phosphate battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LIFE = 4
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_NICD'.
/**
  * Nickel-Cadmium battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_NICD = 5
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_LIMN'.
/**
  * Lithium Manganese Dioxide battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LIMN = 6
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_TERNARY'.
/**
  * Ternary Lithium battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_TERNARY = 7
};

/// Constant 'POWER_SUPPLY_TECHNOLOGY_VRLA'.
/**
  * Valve Regulated Lead-Acid battery
 */
enum
{
  sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_VRLA = 8
};

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'cell_voltage'
// Member 'cell_temperature'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'location'
// Member 'serial_number'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/BatteryState in the package sensor_msgs.
typedef struct sensor_msgs__msg__BatteryState
{
  std_msgs__msg__Header header;
  /// Voltage in Volts (Mandatory)
  float voltage;
  /// Temperature in Degrees Celsius (If unmeasured NaN)
  float temperature;
  /// Negative when discharging (A)  (If unmeasured NaN)
  float current;
  /// Current charge in Ah  (If unmeasured NaN)
  float charge;
  /// Capacity in Ah (last full capacity)  (If unmeasured NaN)
  float capacity;
  /// Capacity in Ah (design capacity)  (If unmeasured NaN)
  float design_capacity;
  /// Charge percentage on 0 to 1 range  (If unmeasured NaN)
  float percentage;
  /// The charging status as reported. Values defined above
  uint8_t power_supply_status;
  /// The battery health metric. Values defined above
  uint8_t power_supply_health;
  /// The battery chemistry. Values defined above
  uint8_t power_supply_technology;
  /// True if the battery is present
  bool present;
  /// An array of individual cell voltages for each cell in the pack
  /// If individual voltages unknown but number of cells known set each to NaN
  rosidl_runtime_c__float__Sequence cell_voltage;
  /// An array of individual cell temperatures for each cell in the pack
  /// If individual temperatures unknown but number of cells known set each to NaN
  rosidl_runtime_c__float__Sequence cell_temperature;
  /// The location into which the battery is inserted. (slot number or plug)
  rosidl_runtime_c__String location;
  /// The best approximation of the battery serial number
  rosidl_runtime_c__String serial_number;
} sensor_msgs__msg__BatteryState;

// Struct for a sequence of sensor_msgs__msg__BatteryState.
typedef struct sensor_msgs__msg__BatteryState__Sequence
{
  sensor_msgs__msg__BatteryState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__msg__BatteryState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__MSG__DETAIL__BATTERY_STATE__STRUCT_H_
