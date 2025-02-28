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

#ifndef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__SERVICE_TYPE_SUPPORT_H_
#define ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__SERVICE_TYPE_SUPPORT_H_

#include <stdint.h>
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "./message_type_support.h"

typedef struct service_type_support_callbacks_t
{
  const char * package_name_;
  const char * service_name_;

  const rosidl_message_type_support_t * (* request_members_)();
  const rosidl_message_type_support_t * (* response_members_)();

} service_type_support_callbacks_t;

#endif  // ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__SERVICE_TYPE_SUPPORT_H_
