// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sensor_msgs:msg/BatteryState.idl
// generated code does not contain a copyright notice

#include "sensor_msgs/msg/detail/battery_state__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sensor_msgs
const rosidl_type_hash_t *
sensor_msgs__msg__BatteryState__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4b, 0xee, 0x5d, 0xfc, 0xe9, 0x81, 0xc9, 0x8f,
      0xaa, 0x68, 0x28, 0xb8, 0x68, 0x30, 0x7a, 0x0a,
      0x73, 0xf9, 0x92, 0xed, 0x07, 0x89, 0xf3, 0x74,
      0xee, 0x96, 0xc8, 0xf8, 0x40, 0xe6, 0x97, 0x41,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "std_msgs/msg/detail/header__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t std_msgs__msg__Header__EXPECTED_HASH = {1, {
    0xf4, 0x9f, 0xb3, 0xae, 0x2c, 0xf0, 0x70, 0xf7,
    0x93, 0x64, 0x5f, 0xf7, 0x49, 0x68, 0x3a, 0xc6,
    0xb0, 0x62, 0x03, 0xe4, 0x1c, 0x89, 0x1e, 0x17,
    0x70, 0x1b, 0x1c, 0xb5, 0x97, 0xce, 0x6a, 0x01,
  }};
#endif

static char sensor_msgs__msg__BatteryState__TYPE_NAME[] = "sensor_msgs/msg/BatteryState";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char std_msgs__msg__Header__TYPE_NAME[] = "std_msgs/msg/Header";

// Define type names, field names, and default values
static char sensor_msgs__msg__BatteryState__FIELD_NAME__header[] = "header";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__voltage[] = "voltage";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__temperature[] = "temperature";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__current[] = "current";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__charge[] = "charge";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__capacity[] = "capacity";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__design_capacity[] = "design_capacity";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__percentage[] = "percentage";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_status[] = "power_supply_status";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_health[] = "power_supply_health";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_technology[] = "power_supply_technology";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__present[] = "present";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__cell_voltage[] = "cell_voltage";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__cell_temperature[] = "cell_temperature";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__location[] = "location";
static char sensor_msgs__msg__BatteryState__FIELD_NAME__serial_number[] = "serial_number";

static rosidl_runtime_c__type_description__Field sensor_msgs__msg__BatteryState__FIELDS[] = {
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__header, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__voltage, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__temperature, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__current, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__charge, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__capacity, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__design_capacity, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__percentage, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_status, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_health, 19, 19},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__power_supply_technology, 23, 23},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__present, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__cell_voltage, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__cell_temperature, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sensor_msgs__msg__BatteryState__FIELD_NAME__serial_number, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sensor_msgs__msg__BatteryState__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {std_msgs__msg__Header__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sensor_msgs__msg__BatteryState__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sensor_msgs__msg__BatteryState__TYPE_NAME, 28, 28},
      {sensor_msgs__msg__BatteryState__FIELDS, 16, 16},
    },
    {sensor_msgs__msg__BatteryState__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&std_msgs__msg__Header__EXPECTED_HASH, std_msgs__msg__Header__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = std_msgs__msg__Header__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "\n"
  "# Constants are chosen to match the enums in the linux kernel\n"
  "# defined in include/linux/power_supply.h as of version 3.7\n"
  "# The one difference is for style reasons the constants are\n"
  "# all uppercase not mixed case.\n"
  "\n"
  "# Power supply status constants\n"
  "uint8 POWER_SUPPLY_STATUS_UNKNOWN = 0\n"
  "uint8 POWER_SUPPLY_STATUS_CHARGING = 1\n"
  "uint8 POWER_SUPPLY_STATUS_DISCHARGING = 2\n"
  "uint8 POWER_SUPPLY_STATUS_NOT_CHARGING = 3\n"
  "uint8 POWER_SUPPLY_STATUS_FULL = 4\n"
  "\n"
  "# Power supply health constants\n"
  "uint8 POWER_SUPPLY_HEALTH_UNKNOWN = 0\n"
  "uint8 POWER_SUPPLY_HEALTH_GOOD = 1\n"
  "uint8 POWER_SUPPLY_HEALTH_OVERHEAT = 2\n"
  "uint8 POWER_SUPPLY_HEALTH_DEAD = 3\n"
  "uint8 POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4\n"
  "uint8 POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5\n"
  "uint8 POWER_SUPPLY_HEALTH_COLD = 6\n"
  "uint8 POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7\n"
  "uint8 POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8\n"
  "\n"
  "# Power supply technology (chemistry) constants\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0 # Unknown battery technology\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_NIMH = 1    # Nickel-Metal Hydride battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_LION = 2    # Lithium-ion battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_LIPO = 3    # Lithium Polymer battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_LIFE = 4    # Lithium Iron Phosphate battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_NICD = 5    # Nickel-Cadmium battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_LIMN = 6    # Lithium Manganese Dioxide battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_TERNARY = 7 # Ternary Lithium battery\n"
  "uint8 POWER_SUPPLY_TECHNOLOGY_VRLA = 8    # Valve Regulated Lead-Acid battery\n"
  "\n"
  "std_msgs/Header  header\n"
  "float32 voltage          # Voltage in Volts (Mandatory)\n"
  "float32 temperature      # Temperature in Degrees Celsius (If unmeasured NaN)\n"
  "float32 current          # Negative when discharging (A)  (If unmeasured NaN)\n"
  "float32 charge           # Current charge in Ah  (If unmeasured NaN)\n"
  "float32 capacity         # Capacity in Ah (last full capacity)  (If unmeasured NaN)\n"
  "float32 design_capacity  # Capacity in Ah (design capacity)  (If unmeasured NaN)\n"
  "float32 percentage       # Charge percentage on 0 to 1 range  (If unmeasured NaN)\n"
  "uint8   power_supply_status     # The charging status as reported. Values defined above\n"
  "uint8   power_supply_health     # The battery health metric. Values defined above\n"
  "uint8   power_supply_technology # The battery chemistry. Values defined above\n"
  "bool    present          # True if the battery is present\n"
  "\n"
  "float32[] cell_voltage   # An array of individual cell voltages for each cell in the pack\n"
  "                         # If individual voltages unknown but number of cells known set each to NaN\n"
  "float32[] cell_temperature # An array of individual cell temperatures for each cell in the pack\n"
  "                           # If individual temperatures unknown but number of cells known set each to NaN\n"
  "string location          # The location into which the battery is inserted. (slot number or plug)\n"
  "string serial_number     # The best approximation of the battery serial number";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sensor_msgs__msg__BatteryState__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sensor_msgs__msg__BatteryState__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 2969, 2969},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sensor_msgs__msg__BatteryState__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sensor_msgs__msg__BatteryState__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *std_msgs__msg__Header__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
