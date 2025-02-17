// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__MESSAGE_TYPE_SUPPORT_H_
#define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__MESSAGE_TYPE_SUPPORT_H_

#include "rosidl_runtime_c/message_type_support_struct.h"

#include <ucdr/microcdr.h>

typedef struct message_type_support_callbacks_t
{
  const char * message_namespace_;
  const char * message_name_;

  // Function for message serialization
  bool (* cdr_serialize)(
    const void * untyped_ros_message,
    ucdrBuffer * cdr);

  // Function for message deserialization
  bool (* cdr_deserialize)(
    ucdrBuffer * cdr,
    void * untyped_ros_message);

  // Function to get size of data
  uint32_t (* get_serialized_size)(
    const void *);

  // Function to get size of data with initial alignment
  size_t (* get_serialized_size_with_initial_alignment)(
    const void *,
    size_t);

  // Function for type support initialization
  size_t (* max_serialized_size)();

} message_type_support_callbacks_t;

#endif  // ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__MESSAGE_TYPE_SUPPORT_H_
