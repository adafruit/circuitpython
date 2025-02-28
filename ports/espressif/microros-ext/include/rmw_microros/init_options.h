// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/**
 * @file
 */

#ifndef RMW_MICROROS__INIT_OPTIONS_H_
#define RMW_MICROROS__INIT_OPTIONS_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <rmw_microxrcedds_c/config.h>
#include <ucdr/microcdr.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Parses command line args and fills rmw implementation-specific options.
 * `rmw_init_options allocator` is used to allocate the specific rmw options.
 *
 * \param[in] argc Number of arguments.
 * \param[in] argv Arguments.
 * \param[in,out] rmw_options Updated options with rmw specifics.
 * \return RMW_RET_OK If arguments were valid and set in rmw_init_options.
 * \return RMW_RET_INVALID_ARGUMENT If rmw_init_options is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_init_options(
  int argc,
  const char * const argv[],
  rmw_init_options_t * rmw_options);

/**
 * \brief Fills rmw implementation-specific options with the given parameters.
 *
 * \param[in] dev Serial device.
 * \param[in,out] rmw_options Updated options with rmw specifics.
 * \return RMW_RET_OK If arguments were valid and set in rmw_init_options.
 * \return RMW_RET_INVALID_ARGUMENT If rmw_init_options is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_options_set_serial_device(
  const char * dev,
  rmw_init_options_t * rmw_options);

/**
 * \brief Fills rmw implementation-specific options with the given parameters.
 *
 * \param[in] ip Agent IP address.
 * \param[in] port Agent UDP port.
 * \param[in,out] rmw_options Updated options with rmw specifics.
 * \return RMW_RET_OK If arguments were valid and set in rmw_init_options.
 * \return RMW_RET_INVALID_ARGUMENT If rmw_init_options is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_options_set_udp_address(
  const char * ip,
  const char * port,
  rmw_init_options_t * rmw_options);

/**
 * \brief Fills rmw implementation-specific options with the given parameters.
 *
 * \param[in] client_key MicroXRCE-DDS client key.
 * \param[in,out] rmw_options Updated options with rmw specifics.
 * \return RMW_RET_OK If arguments were valid and set in rmw_init_options.
 * \return RMW_RET_INVALID_ARGUMENT If rmw_init_options is not valid or unexpected arguments.
 */
rmw_ret_t rmw_uros_options_set_client_key(
  uint32_t client_key,
  rmw_init_options_t * rmw_options);

/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__INIT_OPTIONS_H_
