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

#ifndef RMW_MICROROS__TIME_SYNC_H_
#define RMW_MICROROS__TIME_SYNC_H_

#include <rmw/rmw.h>
#include <rmw/ret_types.h>
#include <rmw/init_options.h>
#include <uxr/client/util/time.h>

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

/** \addtogroup rmw micro-ROS RMW API
 *  @{
 */

/**
 * \brief Returns the time synchronization state of the epoch time.
 * \return true if last time synchronization succeded and false otherwise
 */
bool rmw_uros_epoch_synchronized();

/**
 * \brief Returns the epoch time in milliseconds taking into account the offset computed during the time synchronization.
 * \return epoch time in milliseconds.
 * \return 0 if session is not initialized.
 */
int64_t rmw_uros_epoch_millis();

/**
 * \brief Returns the epoch time in nanoseconds taking into account the offset computed during the time synchronization.
 * \return epoch time in nanoseconds.
 * \return 0 if session is not initialized.
 */
int64_t rmw_uros_epoch_nanos();

/**
 * \brief Synchronizes the session time using the NTP protocol.
 * \param[in] timeout_ms The waiting time in milliseconds.
 * \return RMW_RET_OK when success.
 * \return RMW_RET_ERROR If no session is running or the synchronization fails.
 */
rmw_ret_t rmw_uros_sync_session(
  const int timeout_ms);


/** @}*/

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS__TIME_SYNC_H_
