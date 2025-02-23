// Copyright 2015-2016 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_RUNTIME_C__SERVICE_TYPE_SUPPORT_STRUCT_H_
#define ROSIDL_RUNTIME_C__SERVICE_TYPE_SUPPORT_STRUCT_H_

#include <stdint.h>

#include "rcutils/allocator.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"

#include "rosidl_typesupport_interface/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct rosidl_service_type_support_t rosidl_service_type_support_t;

typedef const rosidl_service_type_support_t * (* rosidl_service_typesupport_handle_function)(
  const rosidl_service_type_support_t *, const char *);

typedef struct rosidl_service_introspection_info_s
{
  uint8_t event_type;
  int32_t stamp_sec;
  uint32_t stamp_nanosec;
  uint8_t client_gid[16];
  int64_t sequence_number;
} rosidl_service_introspection_info_t;

/// Creates a ServiceEvent message for the service.
/**
 * Instantiates a ServiceEvent message with the given info and request/response message.
 * The message is allocated using the given allocator and must be deallocated using
 * the rosidl_service_introspection_destroy_handle
 *
 * \param[in] info POD fields of service_msgs/msg/ServiceEventInfo to be passed from rcl
 * \param[in] allocator The allocator to use for allocating the ServiceEvent message
 * \param[in] request_message type-erased handle to request message from rcl. Can be NULL.
 * \param[in] response_message type-erased handle to request message from rcl. Can be NULL.
 * \return The built ServiceEvent message. Will return NULL if the message could not be built.
 **/
typedef void * (* rosidl_event_message_create_handle_function_function)(
  const rosidl_service_introspection_info_t * info,
  rcutils_allocator_t * allocator,
  const void * request_message,
  const void * response_message);

/// Destroys a ServiceEvent message
/**
 * Destroys a ServiceEvent message returned by a rosidl_service_introspection_message_create_handle
 * by calling the corresponding __fini function then deallocating
 *
 * \param[in] event_message The message to destroy.
 * \param[in] allocator The allocator to use for deallocating the message.
 */
typedef bool (* rosidl_event_message_destroy_handle_function_function)(
  void * event_message,
  rcutils_allocator_t * allocator);

typedef const rosidl_type_hash_t *
(* rosidl_service_get_type_hash_function)(const rosidl_service_type_support_t *);

typedef const rosidl_runtime_c__type_description__TypeDescription *
(* rosidl_service_get_type_description_function)(const rosidl_service_type_support_t *);

typedef const rosidl_runtime_c__type_description__TypeSource__Sequence *
(* rosidl_service_get_type_description_sources_function)(const rosidl_service_type_support_t *);

/// Contains rosidl service type support data
struct rosidl_service_type_support_t
{
  /// String identifier for the type_support.
  const char * typesupport_identifier;
  /// Pointer to the service type support library
  const void * data;
  /// Pointer to the service type support handler function
  rosidl_service_typesupport_handle_function func;
  /// Service request message typesupport
  const rosidl_message_type_support_t * request_typesupport;
  /// Service response message typesupport
  const rosidl_message_type_support_t * response_typesupport;
  /// Service event message typesupport
  const rosidl_message_type_support_t * event_typesupport;
  /// Pointer to function to create the introspection message
  rosidl_event_message_create_handle_function_function event_message_create_handle_function;
  /// Pointer to function to finalize the introspection message
  rosidl_event_message_destroy_handle_function_function event_message_destroy_handle_function;
  /// Pointer to function to get the hash of the message's description
  rosidl_service_get_type_hash_function get_type_hash_func;
  /// Pointer to function to get the description of the type
  rosidl_service_get_type_description_function get_type_description_func;
  /// Pointer to function to get the text of the sources that defined the description of the type
  rosidl_service_get_type_description_sources_function get_type_description_sources_func;
};

/// Get the service type support handle specific to this identifier.
/**
 * The handle's message typesupport identifier function is returned or if the parameters are NULL
 * then an assert will happen.
 *
 * \param handle Handle to service type support
 * \param identifier The typesupport identifier to get the handle function for
 * \return The associated service typesupport handle function.
 */
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_service_type_support_t * get_service_typesupport_handle(
  const rosidl_service_type_support_t * handle, const char * identifier);

/// Get the service type support handle function specific to this identifier.
/**
* If the identifier is the same as this handle's typesupport_identifier the handle is simply
* returned or if the parameters are NULL then an assert will happen.
 *
 * \param handle Handle to service type support
 * \param identifier The typesupport identifier to get the handle function for
 * \return if the identifier match's the handle's identifier then the handle's function
 *   is returned.
 */
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_service_type_support_t * get_service_typesupport_handle_function(
  const rosidl_service_type_support_t * handle, const char * identifier);

/// Get the service type support given a provided action and package.
/*
 * \param PkgName Name of the package that contains the service
 * \param SrvSubfolder name of the subfolder (for example: srv)
 * \param SrvName service name
 * \return a rosidl_service_type_support_t struct if founded, otherwise NULL.
 */
#define ROSIDL_GET_SRV_TYPE_SUPPORT(PkgName, SrvSubfolder, SrvName) \
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME( \
    rosidl_typesupport_c, PkgName, SrvSubfolder, SrvName)()

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_RUNTIME_C__SERVICE_TYPE_SUPPORT_STRUCT_H_
