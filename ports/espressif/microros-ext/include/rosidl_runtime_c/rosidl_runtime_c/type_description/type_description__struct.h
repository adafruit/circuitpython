// DO NOT EDIT MANUALLY - this copied file managed by copy_type_description_generated_sources.bash
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from type_description_interfaces:msg/TypeDescription.idl
// generated code does not contain a copyright notice

#ifndef ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__TYPE_DESCRIPTION__STRUCT_H_
#define ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__TYPE_DESCRIPTION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'type_description'
// Member 'referenced_type_descriptions'
#include "rosidl_runtime_c/type_description/individual_type_description__struct.h"

/// Struct defined in msg/TypeDescription in the package type_description_interfaces.
/**
  * Represents a complete type description, including the type itself as well as the types it references.
 */
typedef struct rosidl_runtime_c__type_description__TypeDescription
{
  /// Description of the type.
  rosidl_runtime_c__type_description__IndividualTypeDescription type_description;
  /// Descriptions of all referenced types, recursively.
  rosidl_runtime_c__type_description__IndividualTypeDescription__Sequence referenced_type_descriptions;
} rosidl_runtime_c__type_description__TypeDescription;

// Struct for a sequence of rosidl_runtime_c__type_description__TypeDescription.
typedef struct rosidl_runtime_c__type_description__TypeDescription__Sequence
{
  rosidl_runtime_c__type_description__TypeDescription * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosidl_runtime_c__type_description__TypeDescription__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__TYPE_DESCRIPTION__STRUCT_H_
