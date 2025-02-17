// DO NOT EDIT MANUALLY - this copied file managed by copy_type_description_generated_sources.bash
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from type_description_interfaces:msg/Field.idl
// generated code does not contain a copyright notice

#ifndef ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__FIELD__STRUCT_H_
#define ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__FIELD__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'name'
// Member 'default_value'
#include "rosidl_runtime_c/string.h"
// Member 'type'
#include "rosidl_runtime_c/type_description/field_type__struct.h"

/// Struct defined in msg/Field in the package type_description_interfaces.
/**
  * Represents a single field in a type.
 */
typedef struct rosidl_runtime_c__type_description__Field
{
  /// Name of the field.
  rosidl_runtime_c__String name;
  /// Type of the field, including details about the type like length, nested name, etc.
  rosidl_runtime_c__type_description__FieldType type;
  /// Literal default value of the field as a string, as it appeared in the original
  /// message description file, whether that be .msg/.srv/.action or .idl.
  rosidl_runtime_c__String default_value;
} rosidl_runtime_c__type_description__Field;

// Struct for a sequence of rosidl_runtime_c__type_description__Field.
typedef struct rosidl_runtime_c__type_description__Field__Sequence
{
  rosidl_runtime_c__type_description__Field * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosidl_runtime_c__type_description__Field__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__FIELD__STRUCT_H_
