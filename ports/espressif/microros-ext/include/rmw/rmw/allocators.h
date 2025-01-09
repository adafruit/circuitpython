// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__ALLOCATORS_H_
#define RMW__ALLOCATORS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rmw/types.h"
#include "rmw/visibility_control.h"

/// Allocate memory of size in bytes using rcutils default allocator's allocate()
/**
 * \param[in] size The number of bytes to allocate
 * \return pointer to allocated memory
 */
RMW_PUBLIC
void *
rmw_allocate(size_t size);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] pointer pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_free(void * pointer);

/// Allocate memory for an `rmw_node_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_node_t *
rmw_node_allocate(void);

/// Free memory allocated to this node pointer using rcutils default allocator's deallocate()
/**
 * \param[in] node pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_node_free(rmw_node_t * node);

/// Allocate memory for an `rmw_publisher_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_publisher_t *
rmw_publisher_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] publisher pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_publisher_free(rmw_publisher_t * publisher);

/// Allocate memory for an `rmw_subscription_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_subscription_t *
rmw_subscription_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] subscription pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_subscription_free(rmw_subscription_t * subscription);

/// Allocate memory for an `rmw_guard_condition_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_guard_condition_t *
rmw_guard_condition_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] guard_condition pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_guard_condition_free(rmw_guard_condition_t * guard_condition);

/// Allocate memory for an `rmw_client_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_client_t *
rmw_client_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] client pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_client_free(rmw_client_t * client);

/// Allocate memory for an `rmw_service_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_service_t *
rmw_service_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] service pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_service_free(rmw_service_t * service);

/// Allocate memory for an `rmw_wait_set_t` using rcutils default allocator's allocate()
/**
 * \return pointer to allocated memory
 */
RMW_PUBLIC
rmw_wait_set_t *
rmw_wait_set_allocate(void);

/// Free memory using rcutils default allocator's deallocate()
/**
 * \param[in] wait_set pointer to allocated memory
 */
RMW_PUBLIC
void
rmw_wait_set_free(rmw_wait_set_t * wait_set);

#ifdef __cplusplus
}
#endif

#endif  // RMW__ALLOCATORS_H_
