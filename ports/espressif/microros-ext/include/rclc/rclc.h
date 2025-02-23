// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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

#ifndef RCLC__RCLC_H_
#define RCLC__RCLC_H_

#if __cplusplus
extern "C"
{
#endif

#include <stdbool.h>  // For bool
#include <stddef.h>  // For size_t

#include "rclc/init.h"
#include "rclc/node.h"
#include "rclc/publisher.h"
#include "rclc/subscription.h"
#include "rclc/timer.h"
#include "rclc/client.h"
#include "rclc/service.h"
#include "rclc/action_client.h"
#include "rclc/action_server.h"
#include "rclc/types.h"
#include "rclc/visibility_control.h"
#if __cplusplus
}
#endif

#endif  // RCLC__RCLC_H_
