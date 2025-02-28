// Copyright 2015 Open Source Robotics Foundation, Inc.
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

/// @file

#ifndef RCL__WAIT_H_
#define RCL__WAIT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>

#include "rcl/client.h"
#include "rcl/guard_condition.h"
#include "rcl/macros.h"
#include "rcl/service.h"
#include "rcl/subscription.h"
#include "rcl/timer.h"
#include "rcl/event.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

typedef struct rcl_wait_set_impl_s rcl_wait_set_impl_t;

/// Container for subscription's, guard condition's, etc to be waited on.
typedef struct rcl_wait_set_s
{
  /// Storage for subscription pointers.
  const rcl_subscription_t ** subscriptions;
  /// Number of subscriptions
  size_t size_of_subscriptions;
  /// Storage for guard condition pointers.
  const rcl_guard_condition_t ** guard_conditions;
  /// Number of guard_conditions
  size_t size_of_guard_conditions;
  /// Storage for timer pointers.
  const rcl_timer_t ** timers;
  /// Number of timers
  size_t size_of_timers;
  /// Storage for client pointers.
  const rcl_client_t ** clients;
  /// Number of clients
  size_t size_of_clients;
  /// Storage for service pointers.
  const rcl_service_t ** services;
  /// Number of services
  size_t size_of_services;
  /// Storage for event pointers.
  const rcl_event_t ** events;
  /// Number of events
  size_t size_of_events;
  /// Implementation specific storage.
  rcl_wait_set_impl_t * impl;
} rcl_wait_set_t;

/// Return a rcl_wait_set_t struct with members set to `NULL`.
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_wait_set_t
rcl_get_zero_initialized_wait_set(void);

/// Initialize a rcl wait set with space for items to be waited on.
/**
 * This function allocates space for the subscriptions and other wait-able
 * entities that can be stored in the wait set.
 * It also sets the allocator to the given allocator and initializes the pruned
 * member to be false.
 *
 * The wait_set struct should be allocated and initialized to `NULL`.
 * If the wait_set is allocated but the memory is uninitialized the behavior is
 * undefined.
 * Calling this function on a wait set that has already been initialized will
 * result in an error.
 * A wait set can be reinitialized if rcl_wait_set_fini() was called on it.
 *
 * To use the default allocator use rcl_get_default_allocator().
 *
 * Expected usage:
 *
 * ```c
 * #include <rcl/wait.h>
 *
 * rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
 * rcl_ret_t ret =
 *   rcl_wait_set_init(&wait_set, 42, 42, 42, 42, 42, &context, rcl_get_default_allocator());
 * // ... error handling, then use it, then call the matching fini:
 * ret = rcl_wait_set_fini(&wait_set);
 * // ... error handling
 * ```
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] wait_set the wait set struct to be initialized
 * \param[in] number_of_subscriptions non-zero size of the subscriptions set
 * \param[in] number_of_guard_conditions non-zero size of the guard conditions set
 * \param[in] number_of_timers non-zero size of the timers set
 * \param[in] number_of_clients non-zero size of the clients set
 * \param[in] number_of_services non-zero size of the services set
 * \param[in] number_of_events non-zero size of the events set
 * \param[in] context the context that the wait set should be associated with
 * \param[in] allocator the allocator to use when allocating space in the sets
 * \return #RCL_RET_OK if the wait set is initialized successfully, or
 * \return #RCL_RET_ALREADY_INIT if the wait set is not zero initialized, or
 * \return #RCL_RET_NOT_INIT if the given context is invalid, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is not destroyed properly, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_init(
  rcl_wait_set_t * wait_set,
  size_t number_of_subscriptions,
  size_t number_of_guard_conditions,
  size_t number_of_timers,
  size_t number_of_clients,
  size_t number_of_services,
  size_t number_of_events,
  rcl_context_t * context,
  rcl_allocator_t allocator);

/// Finalize a rcl wait set.
/**
 * Deallocates any memory in the wait set that was allocated in
 * rcl_wait_set_init() using the allocator given in the initialization.
 *
 * Calling this function on a zero initialized wait set will do nothing and
 * return RCL_RET_OK.
 * Calling this function on uninitialized memory results in undefined behavior.
 * After calling this function the wait set will once again be zero initialized
 * and so calling this function or rcl_wait_set_init() immediately after will
 * succeed.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] wait_set the wait set struct to be finalized.
 * \return #RCL_RET_OK if the finalization was successful, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is not destroyed properly, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_fini(rcl_wait_set_t * wait_set);

/// Retrieve the wait set's allocator.
/**
 * The allocator must be an allocated rcl_allocator_t struct, as the result is
 * copied into this variable.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] wait_set the handle to the wait set
 * \param[out] allocator the rcl_allocator_t struct to which the result is copied
 * \return #RCL_RET_OK if the allocator was successfully retrieved, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is invalid, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_get_allocator(const rcl_wait_set_t * wait_set, rcl_allocator_t * allocator);

/// Store a pointer to the given subscription in the next empty spot in the set.
/**
 * This function does not guarantee that the subscription is not already in the
 * wait set.
 *
 * Also add the rmw representation to the underlying rmw array and increment
 * the rmw array count.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] wait_set struct in which the subscription is to be stored
 * \param[in] subscription the subscription to be added to the wait set
 * \param[out] index the index of the added subscription in the storage container.
 *   This parameter is optional and can be set to `NULL` to be ignored.
 * \return #RCL_RET_OK if added successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is zero initialized, or
 * \return #RCL_RET_WAIT_SET_FULL if the subscription set is full, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_subscription(
  rcl_wait_set_t * wait_set,
  const rcl_subscription_t * subscription,
  size_t * index);

/// Remove (sets to `NULL`) all entities in the wait set.
/**
 * This function should be used after passing using rcl_wait, but before
 * adding new entities to the set.
 * Sets all of the entries in the underlying rmw array to `NULL`, and sets the
 * count in the rmw array to `0`.
 *
 * Calling this on an uninitialized (zero initialized) wait set will fail.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] wait_set struct to have its entities cleared
 * \return #RCL_RET_OK if cleared successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is zero initialized, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_clear(rcl_wait_set_t * wait_set);

/// Reallocate space for entities in the wait set.
/**
 * This function will deallocate and reallocate the memory for all entity sets.
 *
 * A size of 0 will just deallocate the memory and assign `NULL` to the array.
 *
 * Allocation and deallocation is done with the allocator given during the
 * wait set's initialization.
 *
 * After calling this function all values in the set will be set to `NULL`,
 * effectively the same as calling rcl_wait_set_clear().
 * Similarly, the underlying rmw representation is reallocated and reset:
 * all entries are set to `NULL` and the count is set to zero.
 *
 * If the requested size matches the current size, no allocation will be done.
 *
 * This can be called on an uninitialized (zero initialized) wait set.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[inout] wait_set struct to be resized
 * \param[in] subscriptions_size a size for the new subscriptions set
 * \param[in] guard_conditions_size a size for the new guard conditions set
 * \param[in] timers_size a size for the new timers set
 * \param[in] clients_size a size for the new clients set
 * \param[in] services_size a size for the new services set
 * \param[in] events_size a size for the new events set
 * \return #RCL_RET_OK if resized successfully, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_BAD_ALLOC if allocating memory failed, or
 * \return #RCL_RET_ERROR if an unspecified error occurs.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_resize(
  rcl_wait_set_t * wait_set,
  size_t subscriptions_size,
  size_t guard_conditions_size,
  size_t timers_size,
  size_t clients_size,
  size_t services_size,
  size_t events_size);

/// Store a pointer to the guard condition in the next empty spot in the set.
/**
 * This function behaves exactly the same as for subscriptions.
 * \see rcl_wait_set_add_subscription
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_guard_condition(
  rcl_wait_set_t * wait_set,
  const rcl_guard_condition_t * guard_condition,
  size_t * index);

/// Store a pointer to the timer in the next empty spot in the set.
/**
 * This function behaves exactly the same as for subscriptions.
 * \see rcl_wait_set_add_subscription
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_timer(
  rcl_wait_set_t * wait_set,
  const rcl_timer_t * timer,
  size_t * index);

/// Store a pointer to the client in the next empty spot in the set.
/**
 * This function behaves exactly the same as for subscriptions.
 * \see rcl_wait_set_add_subscription
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_client(
  rcl_wait_set_t * wait_set,
  const rcl_client_t * client,
  size_t * index);

/// Store a pointer to the service in the next empty spot in the set.
/**
 * This function behaves exactly the same as for subscriptions.
 * \see rcl_wait_set_add_subscription
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_service(
  rcl_wait_set_t * wait_set,
  const rcl_service_t * service,
  size_t * index);

/// Store a pointer to the event in the next empty spot in the set.
/**
 * This function behaves exactly the same as for subscriptions.
 * \see rcl_wait_set_add_subscription
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait_set_add_event(
  rcl_wait_set_t * wait_set,
  const rcl_event_t * event,
  size_t * index);

/// Block until the wait set is ready or until the timeout has been exceeded.
/**
 * This function will collect the items in the rcl_wait_set_t and pass them
 * to the underlying rmw_wait function.
 *
 * The items in the wait set will be either left untouched or set to `NULL` after
 * this function returns.
 * Items that are not `NULL` are ready, where ready means different things based
 * on the type of the item.
 * For subscriptions this means there may be messages that can be taken, or
 * perhaps that the state of the subscriptions has changed, in which case
 * rcl_take may succeed but return with taken == false.
 * For guard conditions this means the guard condition was triggered.
 *
 * Expected usage:
 *
 * ```c
 * #include <rcl/rcl.h>
 *
 * // rcl_init() called successfully before here...
 * rcl_node_t node;  // initialize this, see rcl_node_init()
 * rcl_subscription_t sub1;  // initialize this, see rcl_subscription_init()
 * rcl_subscription_t sub2;  // initialize this, see rcl_subscription_init()
 * rcl_guard_condition_t gc1;  // initialize this, see rcl_guard_condition_init()
 * rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
 * rcl_ret_t ret = rcl_wait_set_init(&wait_set, 2, 1, 0, 0, 0, rcl_get_default_allocator());
 * // ... error handling
 * do {
 *   ret = rcl_wait_set_clear(&wait_set);
 *   // ... error handling
 *   ret = rcl_wait_set_add_subscription(&wait_set, &sub1);
 *   // ... error handling
 *   ret = rcl_wait_set_add_subscription(&wait_set, &sub2);
 *   // ... error handling
 *   ret = rcl_wait_set_add_guard_condition(&wait_set, &gc1);
 *   // ... error handling
 *   ret = rcl_wait(&wait_set, RCL_MS_TO_NS(1000));  // 1000ms == 1s, passed as ns
 *   if (ret == RCL_RET_TIMEOUT) {
 *     continue;
 *   }
 *   for (int i = 0; i < wait_set.size_of_subscriptions; ++i) {
 *     if (wait_set.subscriptions[i]) {
 *       // The subscription is ready...
 *     }
 *   }
 *   for (int i = 0; i < wait_set.size_of_guard_conditions; ++i) {
 *     if (wait_set.guard_conditions[i]) {
 *       // The subscription is ready...
 *     }
 *   }
 * } while(check_some_condition());
 * // ... fini node, and subscriptions and guard conditions...
 * ret = rcl_wait_set_fini(&wait_set);
 * // ... error handling
 * ```
 *
 * The wait set struct must be allocated, initialized, and should have been
 * cleared and then filled with items, e.g. subscriptions and guard conditions.
 * Passing a wait set with no wait-able items in it will fail.
 * `NULL` items in the sets are ignored, e.g. it is valid to have as input:
 *  - `subscriptions[0]` = valid pointer
 *  - `subscriptions[1]` = `NULL`
 *  - `subscriptions[2]` = valid pointer
 *  - `size_of_subscriptions` = 3
 * Passing an uninitialized (zero initialized) wait set struct will fail.
 * Passing a wait set struct with uninitialized memory is undefined behavior.
 *
 * The unit of timeout is nanoseconds.
 * If the timeout is negative then this function will block indefinitely until
 * something in the wait set is valid or it is interrupted.
 * If the timeout is 0 then this function will be non-blocking; checking what's
 * ready now, but not waiting if nothing is ready yet.
 * If the timeout is greater than 0 then this function will return after
 * that period of time has elapsed or the wait set becomes ready, which ever
 * comes first.
 * Passing a timeout struct with uninitialized memory is undefined behavior.
 *
 * This function is thread-safe for unique wait sets with unique contents.
 * This function cannot operate on the same wait set in multiple threads, and
 * the wait sets may not share content.
 * For example, calling rcl_wait() in two threads on two different wait sets
 * that both contain a single, shared guard condition is undefined behavior.
 *
 * \param[inout] wait_set the set of things to be waited on and to be pruned if not ready
 * \param[in] timeout the duration to wait for the wait set to be ready, in nanoseconds
 * \return #RCL_RET_OK something in the wait set became ready, or
 * \return #RCL_RET_INVALID_ARGUMENT if any arguments are invalid, or
 * \return #RCL_RET_WAIT_SET_INVALID if the wait set is zero initialized, or
 * \return #RCL_RET_WAIT_SET_EMPTY if the wait set contains no items, or
 * \return #RCL_RET_TIMEOUT if the timeout expired before something was ready, or
 * \return #RCL_RET_ERROR an unspecified error occur.
 */
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_wait(rcl_wait_set_t * wait_set, int64_t timeout);

/// Return `true` if the wait set is valid, else `false`.
/**
 * A wait set is invalid if:
 *   - the implementation is `NULL` (rcl_wait_set_init not called or failed)
 *   - the wait set has been finalized with rcl_wait_set_fini
 *
 * Also return `false` if the wait set pointer is `NULL`.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] wait_set the rcl_wait_set_t to be validated
 * \return `true` if the wait_set is valid, otherwise `false`.
 */
RCL_PUBLIC
bool
rcl_wait_set_is_valid(const rcl_wait_set_t * wait_set);

#ifdef __cplusplus
}
#endif

#endif  // RCL__WAIT_H_
