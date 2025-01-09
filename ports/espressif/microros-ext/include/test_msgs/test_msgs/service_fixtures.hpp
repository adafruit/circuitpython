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

#ifndef TEST_MSGS__SERVICE_FIXTURES_HPP_
#define TEST_MSGS__SERVICE_FIXTURES_HPP_

#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "test_msgs/message_fixtures.hpp"
#include "test_msgs/srv/arrays.hpp"
#include "test_msgs/srv/basic_types.hpp"
#include "test_msgs/srv/empty.hpp"


static inline
std::vector<
  std::pair<
    test_msgs::srv::Empty::Request::SharedPtr,
    test_msgs::srv::Empty::Response::SharedPtr
  >
>
get_services_empty()
{
  std::vector<
    std::pair<
      test_msgs::srv::Empty::Request::SharedPtr,
      test_msgs::srv::Empty::Response::SharedPtr
    >
  > services;
  auto request = std::make_shared<
    test_msgs::srv::Empty::Request>();
  auto reply = std::make_shared<
    test_msgs::srv::Empty::Response>();
  services.emplace_back(request, reply);
  return services;
}

static inline
std::vector<
  std::pair<
    test_msgs::srv::BasicTypes::Request::SharedPtr,
    test_msgs::srv::BasicTypes::Response::SharedPtr
  >
>
get_services_basic_types()
{
  std::vector<
    std::pair<
      test_msgs::srv::BasicTypes::Request::SharedPtr,
      test_msgs::srv::BasicTypes::Response::SharedPtr
    >
  > services;
  {
    auto request = std::make_shared<test_msgs::srv::BasicTypes::Request>();
    request->bool_value = false;
    request->byte_value = 0;
    request->char_value = 0;
    request->float32_value = 0.0f;
    request->float64_value = 0.0;
    request->int8_value = 0;
    request->uint8_value = 0;
    request->int16_value = 0;
    request->uint16_value = 0;
    request->int32_value = 0;
    request->uint32_value = 0;
    request->int64_value = 0;
    request->uint64_value = 0;
    request->string_value = "request";
    auto reply = std::make_shared<test_msgs::srv::BasicTypes::Response>();
    reply->bool_value = false;
    reply->byte_value = 0;
    reply->char_value = 0;
    reply->float32_value = 0.0f;
    reply->float64_value = 0.0;
    reply->int8_value = 0;
    reply->uint8_value = 0;
    reply->int16_value = 0;
    reply->uint16_value = 0;
    reply->int32_value = 0;
    reply->uint32_value = 0;
    reply->int64_value = 0;
    reply->uint64_value = 0;
    reply->string_value = "reply";
    services.emplace_back(request, reply);
  }
  {
    auto request = std::make_shared<test_msgs::srv::BasicTypes::Request>();
    request->bool_value = true;
    request->byte_value = 1;
    request->char_value = 1;
    request->float32_value = 1.125f;
    request->float64_value = 1.11;
    request->int8_value = 1;
    request->uint8_value = 2;
    request->int16_value = 3;
    request->uint16_value = 4;
    request->int32_value = 5;
    request->uint32_value = 6;
    request->int64_value = 7;
    request->uint64_value = 8;
    // check strings longer then 256 characters
    request->string_value = "";
    for (size_t i = 0; i < 20000; ++i) {
      request->string_value += std::to_string(i % 10);
    }
    auto reply = std::make_shared<test_msgs::srv::BasicTypes::Response>();
    reply->bool_value = true;
    reply->byte_value = 11;
    reply->char_value = 11;
    reply->float32_value = 11.125f;
    reply->float64_value = 11.11;
    reply->int8_value = 11;
    reply->uint8_value = 22;
    reply->int16_value = 33;
    reply->uint16_value = 44;
    reply->int32_value = 55;
    reply->uint32_value = 66;
    reply->int64_value = 77;
    reply->uint64_value = 88;
    // check strings longer then 256 characters
    reply->string_value = "";
    for (size_t i = 0; i < 20000; ++i) {
      reply->string_value += std::to_string(i % 10);
    }
    services.emplace_back(request, reply);
  }
  return services;
}

static inline
std::vector<
  std::pair<
    test_msgs::srv::Arrays::Request::SharedPtr,
    test_msgs::srv::Arrays::Response::SharedPtr
  >
>
get_services_arrays()
{
  std::vector<
    std::pair<
      test_msgs::srv::Arrays::Request::SharedPtr,
      test_msgs::srv::Arrays::Response::SharedPtr
    >
  > services;
  {
    auto request = std::make_shared<test_msgs::srv::Arrays::Request>();
    request->bool_values = {{false, true, false}};
    request->byte_values = {{0, 0xff, 0}};
    request->char_values = {{0, 255, 0}};
    request->float32_values = {{0.0f, 1.125f, -2.125f}};
    request->float64_values = {{0, 1.125, -2.125}};
    request->int8_values = {{
      0, (std::numeric_limits<int8_t>::max)(), (std::numeric_limits<int8_t>::min)()}};
    request->uint8_values = {{0, (std::numeric_limits<uint8_t>::max)(), 0}};
    request->int16_values = {{
      0, (std::numeric_limits<int16_t>::max)(), (std::numeric_limits<int16_t>::min)()}};
    request->uint16_values = {{0, (std::numeric_limits<uint16_t>::max)(), 0}};
    request->int32_values = {{
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::max)(),
      (std::numeric_limits<int32_t>::min)()
    }};
    request->uint32_values = {{0, (std::numeric_limits<uint32_t>::max)(), 0}};
    request->int64_values[0] = 0;
    request->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    request->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    request->uint64_values = {{0, (std::numeric_limits<uint64_t>::max)(), 0}};
    request->string_values = {{"", "max value", "min value"}};
    auto basic_types_msgs = get_messages_basic_types();
    request->basic_types_values[0] = *basic_types_msgs[0];
    request->basic_types_values[1] = *basic_types_msgs[1];
    request->basic_types_values[2] = *basic_types_msgs[2];

    auto reply = std::make_shared<test_msgs::srv::Arrays::Response>();
    reply->bool_values = {{true, false, false}};
    reply->byte_values = {{0xff, 0, 0}};
    reply->char_values = {{255, 0, 0}};
    reply->float32_values = {{1.125f, 0.0f, -2.125f}};
    reply->float64_values = {{1.125, 0, -2.125}};
    reply->int8_values = {{
      (std::numeric_limits<int8_t>::max)(), 0, (std::numeric_limits<int8_t>::min)()}};
    reply->uint8_values = {{(std::numeric_limits<uint8_t>::max)(), 0, 0}};
    reply->int16_values = {{
      (std::numeric_limits<int16_t>::max)(), 0, (std::numeric_limits<int16_t>::min)()}};
    reply->uint16_values = {{(std::numeric_limits<uint16_t>::max)(), 0, 0}};
    reply->int32_values = {{
      (std::numeric_limits<int32_t>::max)(),
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::min)()
    }};
    reply->uint32_values = {{(std::numeric_limits<uint32_t>::max)(), 0, 0}};
    reply->int64_values[0] = (std::numeric_limits<int64_t>::max)();
    reply->int64_values[1] = 0;
    reply->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    reply->uint64_values = {{(std::numeric_limits<uint64_t>::max)(), 0, 0}};
    reply->string_values = {{"max value", "", "min value"}};
    reply->basic_types_values[0] = *basic_types_msgs[1];
    reply->basic_types_values[1] = *basic_types_msgs[0];
    reply->basic_types_values[2] = *basic_types_msgs[2];

    services.emplace_back(request, reply);
  }
  return services;
}

#endif  // TEST_MSGS__SERVICE_FIXTURES_HPP_
