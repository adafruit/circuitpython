// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__SERIALIZED_MESSAGE_H_
#define RMW__SERIALIZED_MESSAGE_H_

#if __cplusplus
extern "C"
{
#endif

#include "rcutils/types/uint8_array.h"

/**
 * \brief Serialized message as a string of bytes.
 *
 * It includes (but it is not limited to) the following members:
 *   \e \c buffer the reference to internal storage, as a pointer
 *   \e \c buffer_length the size of stored contents, as an unsigned integer
 *   \e \c buffer_capacity the capacity of internal storage, as an unsigned integer
 */
/* For now this is a simple aliasing from a serialized message to a uint8 array.
 * However, in future developments this serialized message can become something
 * more complex and is therefore aliased.
 */
typedef rcutils_uint8_array_t rmw_serialized_message_t;

/// Return a zero initialized serialized message struct.
#define rmw_get_zero_initialized_serialized_message rcutils_get_zero_initialized_uint8_array

/// Initialize a serialized message, zero initializing its contents.
/**
 * Given serialized message must have been zero initialized.
 *
 * \param[inout] serialized_message a pointer to the serialized message to initialize
 * \param[in] message_capacity the size of the memory to allocate
 * \param[in] allocator the allocator to use for the memory allocation
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if any arguments are invalid, or
 * \return 'RMW_RET_BAD_ALLOC` if no memory could be allocated correctly, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs
 */
#define rmw_serialized_message_init(serialized_message, message_capacity, allocator) \
  rcutils_uint8_array_init(serialized_message, message_capacity, allocator)

/// Finalize a serialized message.
/**
 * Given serialized message must have been initialized with `rmw_serialized_message_init()`.
 *
 * \remarks If serialized message is zero initialized, then `RMW_RET_INVALID_ARGUMENT` is returned.
 *
 * \param[in] serialized_message pointer to the serialized message to be cleaned up
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if serialized_message is invalid, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs
 */
#define rmw_serialized_message_fini(serialized_message) \
  rcutils_uint8_array_fini(serialized_message)

/// Resize the internal buffer of the serialized message.
/**
 * The internal buffer of the serialized message can be resized dynamically
 * if needed.
 * If the new size is smaller than the current capacity, then the memory is
 * truncated.
 *
 * \pre Given serialized message must have been initialized with `rmw_serialized_message_init()`.
 *
 * \remarks If serialized message is zero initialized, then `RMW_RET_INVALID_ARGUMENT`
 *   is returned.
 *
 * \warning Be aware that this might deallocate the memory and therefore
 *   invalidate any pointers to the internal buffer.
 *
 * \param[inout] serialized_message pointer to the serialized message
 *   to be resized
 * \param[in] new_size the new size of the internal buffer
 * \return `RMW_RET_OK` if successful, or
 * \return `RMW_RET_INVALID_ARGUMENT` if serialized_message is invalid
 *   or new_size is set to zero, or
 * \return `RMW_RET_BAD_ALLOC` if memory allocation failed, or
 * \return `RMW_RET_ERROR` if an unexpected error occurs
 */
#define rmw_serialized_message_resize(serialized_message, new_size) \
  rcutils_uint8_array_resize(serialized_message, new_size)

#if __cplusplus
}
#endif

#endif  // RMW__SERIALIZED_MESSAGE_H_
