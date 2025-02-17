// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from micro_ros_msgs:msg/Entity.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "micro_ros_msgs/msg/entity.h"


#ifndef MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__STRUCT_H_
#define MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'PUBLISHER'.
enum
{
  micro_ros_msgs__msg__Entity__PUBLISHER = 0
};

/// Constant 'SUBSCRIBER'.
enum
{
  micro_ros_msgs__msg__Entity__SUBSCRIBER = 1
};

/// Constant 'SERVICE_SERVER'.
enum
{
  micro_ros_msgs__msg__Entity__SERVICE_SERVER = 2
};

/// Constant 'SERVICE_CLIENT'.
enum
{
  micro_ros_msgs__msg__Entity__SERVICE_CLIENT = 3
};

// Include directives for member types
// Member 'name'
// Member 'types'
#include "rosidl_runtime_c/string.h"

// constants for array fields with an upper bound
// name
enum
{
  micro_ros_msgs__msg__Entity__name__MAX_STRING_SIZE = 256
};
// types
enum
{
  micro_ros_msgs__msg__Entity__types__MAX_STRING_SIZE = 256
};

/// Struct defined in msg/Entity in the package micro_ros_msgs.
typedef struct micro_ros_msgs__msg__Entity
{
  uint8_t entity_type;
  rosidl_runtime_c__String name;
  rosidl_runtime_c__String__Sequence types;
} micro_ros_msgs__msg__Entity;

// Struct for a sequence of micro_ros_msgs__msg__Entity.
typedef struct micro_ros_msgs__msg__Entity__Sequence
{
  micro_ros_msgs__msg__Entity * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} micro_ros_msgs__msg__Entity__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__STRUCT_H_
