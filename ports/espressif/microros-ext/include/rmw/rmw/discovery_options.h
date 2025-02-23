// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__DISCOVERY_OPTIONS_H_
#define RMW__DISCOVERY_OPTIONS_H_

#include "rcutils/allocator.h"

#include "rmw/macros.h"
#include "rmw/ret_types.h"
#include "rmw/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Used to control the range that nodes will be discovered
typedef enum RMW_PUBLIC_TYPE rmw_automatic_discovery_range_e
{
  /// The discovery range has not been set
  RMW_AUTOMATIC_DISCOVERY_RANGE_NOT_SET = 0,
  /// Force discovery off
  RMW_AUTOMATIC_DISCOVERY_RANGE_OFF = 1,
  /// Allows discovering nodes on the same host
  RMW_AUTOMATIC_DISCOVERY_RANGE_LOCALHOST = 2,
  /// Allows discovering nodes on the same subnet
  RMW_AUTOMATIC_DISCOVERY_RANGE_SUBNET = 3,
  /// Use discovery settings configured directly with the middleware
  RMW_AUTOMATIC_DISCOVERY_RANGE_SYSTEM_DEFAULT = 4,
} rmw_automatic_discovery_range_t;

/// Maximum length of a peer hostname or IP address
#define RMW_DISCOVERY_OPTIONS_STATIC_PEERS_MAX_LENGTH 256

/// Struct to typedef some of the peer addresses
typedef struct rmw_peer_address_s
{
  char peer_address[RMW_DISCOVERY_OPTIONS_STATIC_PEERS_MAX_LENGTH];
} rmw_peer_address_t;

/// Used to specify the options that control how discovery is performed
typedef struct RMW_PUBLIC_TYPE rmw_discovery_options_s
{
  /// How far to allow discovering nodes
  /**
   * This needs to be set to something other than RMW_AUTOMATIC_DISCOVERY_RANGE_NOT_SET,
   * because that is just a sentinel value to see if this was set, but rmw
   * implementations should error if this is what is given during context init.
   */
  rmw_automatic_discovery_range_t automatic_discovery_range;

  /// The list of manually-specified peers to perform static discovery with
  /**
   * Each peer is specified as a hostname or an IP address (IPv4 and IPv6 are both acceptable), or
   * a subnet, e.g. 192.168.0.0/24.
   */
  rmw_peer_address_t * static_peers;

  /// The number of manually-specified peers
  size_t static_peers_count;

  /// The allocator used to allocate static_peers
  rcutils_allocator_t allocator;
} rmw_discovery_options_t;

/// Return a zero-initialized discovery options structure.
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_discovery_options_t
rmw_get_zero_initialized_discovery_options(void);

/// Initialize a discovery options structure with a set number of static peers.
/**
 * This function initializes rmw_discovery_options_t with space for a set number of static peers.
 *
 * \param[in] discovery_options Pointer to a zero initialized option structure to be initialized on
 * success, but left unchanged on failure.
 * \param[in] size Number of static peers to allocate space for.
 * \param[in] allocator Allocator to be used to allocate memory.
 * \returns `RMW_RET_OK` if successful, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `discovery_options` is NULL, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `discovery_options` is not
 *   zero initialized, or
 * \returns `RMW_RET_INVALID_ARGUMENT` if `allocator` is invalid,
 *   by rcutils_allocator_is_valid() definition, or
 * \returns `RMW_BAD_ALLOC` if memory allocation fails, or
 * \returns `RMW_RET_ERROR` when an unspecified error occurs.
 * \remark This function sets the RMW error state on failure.
*/
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_discovery_options_init(
  rmw_discovery_options_t * discovery_options,
  size_t size,
  rcutils_allocator_t * allocator);

/// Compare two discovery parameter instances for equality.
/**
 * Equality means the automatic_discovery_range values are equal, they have the same
 * static_peers_count value, and each entry in static_peers is evaluated as
 * equal using strncmp.
 *
 * NOTE: If the two parameter structs list the static peers in different orders
 * then this will evaulate as NOT equal.
 *
 * \param[in] left - The first set of options to compare
 * \param[in] right - The second set of options to compare
 * \param[out] result - The result of the calculation.
 *
 * \return RMW_RET_OK when the input arguments are valid.
 * \return RMW_RET_INVALID_ARGUMENT will be returned when any input is a nullptr,
 * or if something in either struct was malformed, such as static_peers being
 * a nullptr while static_peers_count is non-zero.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_discovery_options_equal(
  const rmw_discovery_options_t * const left,
  const rmw_discovery_options_t * const right,
  bool * result);

/// Perform a deep copy of the discovery options from src into dst using the
/// given allocator.
/**
 * The dst will be left with an owned copy of the static peers array whose
 * string values match the src.
 * If successful, src and dst will evaluate as equal using
 * rmw_discovery_options_equal.
 *
 * \param[in] src discovery options to be copied.
 * \param[in] allocator to use.
 * \param[out] dst Destination options to use.
 * \return RMW_RET_OK if success.
 * \return RMW_RET_INVALID_ARGUMENT if either the src, allocator or dst is null, or
 * \return RMW_RET_INVALID_ARUGMENT if src and dst are the same object.
 * \return RMW_RET_BAD_ALLOC if allocation fails.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_discovery_options_copy(
  const rmw_discovery_options_t * src,
  rcutils_allocator_t * allocator,
  rmw_discovery_options_t * dst);

/// Destructor for rmw_discovery_options_t
/**
 * \param[in] discovery_options to destroy
 * \param[in] allocator to be used for destruction.
 * \return RMW_RET_OK if success.
 * \return RMW_RET_INVALID_ARGUMENT if allocator is invalid
 * or discovery_options is null.
 */
RMW_PUBLIC
RMW_WARN_UNUSED
rmw_ret_t
rmw_discovery_options_fini(
  rmw_discovery_options_t * discovery_options);
#ifdef __cplusplus
}
#endif

#endif  // RMW__DISCOVERY_OPTIONS_H_
