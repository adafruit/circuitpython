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

#ifndef RCUTILS__SECURITY_DIRECTORY_H_
#define RCUTILS__SECURITY_DIRECTORY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcutils/allocator.h"
#include "rcutils/visibility_control.h"

#ifndef ROS_SECURITY_NODE_DIRECTORY_VAR_NAME
  #define ROS_SECURITY_NODE_DIRECTORY_VAR_NAME "ROS_SECURITY_NODE_DIRECTORY"
#endif

#ifndef ROS_SECURITY_ROOT_DIRECTORY_VAR_NAME
  #define ROS_SECURITY_ROOT_DIRECTORY_VAR_NAME "ROS_SECURITY_ROOT_DIRECTORY"
#endif

#ifndef ROS_SECURITY_LOOKUP_TYPE_VAR_NAME
  #define ROS_SECURITY_LOOKUP_TYPE_VAR_NAME "ROS_SECURITY_LOOKUP_TYPE"
#endif

/// Return the secure root directory associated with a node given its validated name and namespace.
/**
 * E.g. for a node named "c" in namespace "/a/b", the secure root path will be
 * "a/b/c", where the delimiter "/" is native for target file system (e.g. "\\" for _WIN32).
 * If no exact match is found for the node name, a best match would be used instead
 * (by performing longest-prefix matching).
 *
 * However, this expansion can be overridden by setting the secure node directory environment
 * variable, allowing users to explicitly specify the exact secure root directory to be utilized.
 * Such an override is useful for where the FQN of a node is non-deterministic before runtime,
 * or when testing and using additional tools that may not otherwise be easily provisioned.
 *
 * \param[in] node_name validated node name (a single token)
 * \param[in] node_namespace validated, absolute namespace (starting with "/")
 * \param[in] allocator the allocator to use for allocation
 * \returns machine specific (absolute) node secure root path or NULL on failure
 *          returned pointer must be deallocated by the caller of this function
 */
RCUTILS_PUBLIC
char * rcutils_get_secure_root(
  const char * node_name,
  const char * node_namespace,
  const rcutils_allocator_t * allocator
);

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__SECURITY_DIRECTORY_H_
