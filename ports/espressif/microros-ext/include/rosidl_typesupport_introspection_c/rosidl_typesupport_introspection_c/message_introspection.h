// Copyright 2014-2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_C__MESSAGE_INTROSPECTION_H_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_C__MESSAGE_INTROSPECTION_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rosidl_runtime_c/message_initialization.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/type_hash.h"

#include "rosidl_typesupport_introspection_c/visibility_control.h"

/// Structure used to describe a single field of an interface type.
typedef struct rosidl_typesupport_introspection_c__MessageMember_s
{
  /// The name of the field.
  const char * name_;
  /// The type of the field as a value of the field types enum
  /// rosidl_typesupport_introspection_c_field_types.
  /// e.g. rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT
  uint8_t type_id_;
  /// If the field is a string, the upper bound on the length of the string.
  size_t string_upper_bound_;
  /// If the type_id_ value is rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,
  /// this points to an array describing the fields of the sub-interface.
  const rosidl_message_type_support_t * members_;
  /// True if this field is annotated as `@key`, false otherwise.
  bool is_key_;
  /// True if this field is an array type, false if it is any other type. An
  /// array has the same value for / type_id_.
  bool is_array_;
  /// If is_array_ is true, this contains the number of members in the array.
  size_t array_size_;
  /// If is_array_ is true, this specifies if the array has a maximum size. If it is true, the
  /// value in array_size_ is the maximum size.
  bool is_upper_bound_;
  /// The bytes into the interface's in-memory representation that this field can be found at.
  uint32_t offset_;
  /// If the interface has a default value, this points to it.
  const void * default_value_;
  /// If is_array_ is true, a pointer to a function that gives the size of one member of the array.
  size_t (* size_function)(const void *);
  /// If is_array_ is true, a pointer to a function that gives a const pointer to the member of the
  /// array indicated by index.
  const void * (*get_const_function)(const void *, size_t index);
  /// If is_array_ is true, a pointer to a function that gives a pointer to the member of the
  /// array indicated by index.
  void * (*get_function)(void *, size_t index);
  /// Pointer to a function that fetches (i.e. copies) an item from
  /// an array or sequence member. It takes a pointer to the member,
  /// an index (which is assumed to be valid), and a pointer to a
  /// pre-allocated value (which is assumed to be of the correct type).
  ///
  /// Available for array and sequence members.
  void (* fetch_function)(const void *, size_t index, void *);
  /// Pointer to a function that assigns (i.e. copies) a value to an
  /// item in an array or sequence member. It takes a pointer to the
  /// member, an index (which is assumed to be valid), and a pointer
  /// to an initialized value (which is assumed to be of the correct
  /// type).
  ///
  /// Available for array and sequence members.
  void (* assign_function)(void *, size_t index, const void *);
  /// If is_array_ is true, a pointer to a function that resizes the array.
  bool (* resize_function)(void *, size_t size);
} rosidl_typesupport_introspection_c__MessageMember;

/// Structure used to describe all fields of a single interface type.
typedef struct rosidl_typesupport_introspection_c__MessageMembers_s
{
  /// The namespace in which the interface resides, e.g. "example_messages__msg" for
  /// example_messages/msg
  const char * message_namespace_;
  /// The name of the interface, e.g. "Int16"
  const char * message_name_;
  /// The number of fields in the interface
  uint32_t member_count_;
  /// The size of the interface structure in memory
  size_t size_of_;
  /// A boolean value indicating if there are any members annotated as `@key` in the structure.
  bool has_any_key_member_;
  /// A pointer to the array that describes each field of the interface
  const rosidl_typesupport_introspection_c__MessageMember * members_;
  /// The function used to initialise the interface's in-memory representation
  void (* init_function)(void *, enum rosidl_runtime_c__message_initialization);
  /// The function used to clean up the interface's in-memory representation
  void (* fini_function)(void *);
} rosidl_typesupport_introspection_c__MessageMembers;

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_C__MESSAGE_INTROSPECTION_H_
