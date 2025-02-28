// NOLINT: This file starts with a BOM since it contain non-ASCII characters
//
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

#ifndef TEST_MSGS__MESSAGE_FIXTURES_HPP_
#define TEST_MSGS__MESSAGE_FIXTURES_HPP_

#include <cassert>
#include <limits>
#include <memory>
#include <vector>
#include <utility>
#include <string>

#include "test_msgs/msg/arrays.hpp"
#include "test_msgs/msg/basic_types.hpp"
#include "test_msgs/msg/bounded_plain_sequences.hpp"
#include "test_msgs/msg/bounded_sequences.hpp"
#include "test_msgs/msg/builtins.hpp"
#include "test_msgs/msg/constants.hpp"
#include "test_msgs/msg/defaults.hpp"
#include "test_msgs/msg/empty.hpp"
#include "test_msgs/msg/multi_nested.hpp"
#include "test_msgs/msg/nested.hpp"
#include "test_msgs/msg/strings.hpp"
#include "test_msgs/msg/unbounded_sequences.hpp"
#include "test_msgs/msg/w_strings.hpp"

static inline std::string
from_u8string(const std::string & s)
{
  return s;
}

static inline std::string
from_u8string(std::string && s)
{
  return std::move(s);
}

#if defined(__cpp_lib_char8_t)
static inline std::string
from_u8string(const std::u8string & s)
{
  return std::string(s.begin(), s.end());
}
#endif

static inline std::vector<test_msgs::msg::Empty::SharedPtr>
get_messages_empty()
{
  std::vector<test_msgs::msg::Empty::SharedPtr> messages;
  auto msg = std::make_shared<test_msgs::msg::Empty>();
  messages.push_back(msg);
  return messages;
}

static inline std::vector<test_msgs::msg::BasicTypes::SharedPtr>
get_messages_basic_types()
{
  std::vector<test_msgs::msg::BasicTypes::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::BasicTypes>();
    msg->bool_value = false;
    msg->byte_value = 0;
    msg->char_value = 0;
    msg->float32_value = 0.0f;
    msg->float64_value = 0;
    msg->int8_value = 0;
    msg->uint8_value = 0;
    msg->int16_value = 0;
    msg->uint16_value = 0;
    msg->int32_value = 0;
    msg->uint32_value = 0;
    msg->int64_value = 0;
    msg->uint64_value = 0;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::BasicTypes>();
    msg->bool_value = true;
    msg->byte_value = 255;
    msg->char_value = 255;
    msg->float32_value = 1.125f;
    msg->float64_value = 1.125;
    msg->int8_value = (std::numeric_limits<int8_t>::max)();
    msg->uint8_value = (std::numeric_limits<uint8_t>::max)();
    msg->int16_value = (std::numeric_limits<int16_t>::max)();
    msg->uint16_value = (std::numeric_limits<uint16_t>::max)();
    msg->int32_value = (std::numeric_limits<int32_t>::max)();
    msg->uint32_value = (std::numeric_limits<uint32_t>::max)();
    msg->int64_value = (std::numeric_limits<int64_t>::max)();
    msg->uint64_value = (std::numeric_limits<uint64_t>::max)();
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::BasicTypes>();
    msg->bool_value = false;
    msg->byte_value = 0;
    msg->char_value = 0;
    msg->float32_value = -2.125f;
    msg->float64_value = -2.125;
    msg->int8_value = (std::numeric_limits<int8_t>::min)();
    msg->uint8_value = 0;
    msg->int16_value = (std::numeric_limits<int16_t>::min)();
    msg->uint16_value = 0;
    msg->int32_value = (std::numeric_limits<int32_t>::min)();
    msg->uint32_value = 0;
    msg->int64_value = (std::numeric_limits<int64_t>::min)();
    msg->uint64_value = 0;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::BasicTypes>();
    msg->bool_value = true;
    msg->byte_value = 1;
    msg->char_value = 1;
    msg->float32_value = 1.0f;
    msg->float64_value = 1;
    msg->int8_value = 1;
    msg->uint8_value = 1;
    msg->int16_value = 1;
    msg->uint16_value = 1;
    msg->int32_value = 1;
    msg->uint32_value = 1;
    msg->int64_value = 1;
    msg->uint64_value = 1;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Constants::SharedPtr>
get_messages_constants()
{
  std::vector<test_msgs::msg::Constants::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::Constants>();
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Defaults::SharedPtr>
get_messages_defaults()
{
  std::vector<test_msgs::msg::Defaults::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::Defaults>();
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Strings::SharedPtr>
get_messages_strings()
{
  std::vector<test_msgs::msg::Strings::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::Strings>();
    msg->string_value = "";
    msg->bounded_string_value = "";
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::Strings>();
    msg->string_value = "Hello world!";
    msg->bounded_string_value = "Hello world!";
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::Strings>();
    msg->string_value = from_u8string(u8"Hell\u00F6 W\u00F6rld!");  // using umlaut
    msg->bounded_string_value = from_u8string(u8"Hell\u00F6 W\u00F6rld!");  // using umlaut
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::Strings>();
    msg->string_value = "";
    msg->bounded_string_value = "";
    for (size_t i = 0; i < 20000; ++i) {
      msg->string_value += std::to_string(i % 10);
    }
    for (size_t i = 0; i < 22; ++i) {
      msg->bounded_string_value += std::to_string(i % 10);
    }
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Arrays::SharedPtr>
get_messages_arrays()
{
  auto basic_types_msgs = get_messages_basic_types();
  std::vector<test_msgs::msg::Arrays::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::Arrays>();
    msg->bool_values = {{false, true, false}};
    msg->byte_values = {{0, 0xff, 0}};
    msg->char_values = {{0, 255, 0}};
    msg->float32_values = {{0.0f, 1.125f, -2.125f}};
    msg->float64_values = {{0, 1.125, -2.125}};
    msg->int8_values = {{
      0, (std::numeric_limits<int8_t>::max)(), (std::numeric_limits<int8_t>::min)()}};
    msg->uint8_values = {{0, (std::numeric_limits<uint8_t>::max)(), 0}};
    msg->int16_values = {{
      0, (std::numeric_limits<int16_t>::max)(), (std::numeric_limits<int16_t>::min)()}};
    msg->uint16_values = {{0, (std::numeric_limits<uint16_t>::max)(), 0}};
    msg->int32_values = {{
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::max)(),
      (std::numeric_limits<int32_t>::min)()
    }};
    msg->uint32_values = {{0, (std::numeric_limits<uint32_t>::max)(), 0}};
    msg->int64_values[0] = 0;
    msg->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    msg->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    msg->uint64_values = {{0, (std::numeric_limits<uint64_t>::max)(), 0}};
    msg->string_values = {{"", "max value", "min value"}};
    msg->basic_types_values[0] = *basic_types_msgs[0];
    msg->basic_types_values[1] = *basic_types_msgs[1];
    msg->basic_types_values[2] = *basic_types_msgs[2];
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::UnboundedSequences::SharedPtr>
get_messages_unbounded_sequences()
{
  auto basic_types_msgs = get_messages_basic_types();
  std::vector<test_msgs::msg::UnboundedSequences::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
    msg->bool_values = {};
    msg->byte_values = {};
    msg->char_values = {};
    msg->float32_values = {};
    msg->float64_values = {};
    msg->int8_values = {};
    msg->uint8_values = {};
    msg->int16_values = {};
    msg->uint16_values = {};
    msg->int32_values = {};
    msg->uint32_values = {};
    msg->int64_values = {};
    msg->uint64_values = {};
    msg->string_values = {};
    msg->basic_types_values = {};
    msg->alignment_check = 0;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
    msg->bool_values = {true};
    msg->byte_values = {0xff};
    msg->char_values = {255};
    msg->float32_values = {1.125f};
    msg->float64_values = {1.125};
    msg->int8_values = {(std::numeric_limits<int8_t>::max)()};
    msg->uint8_values = {(std::numeric_limits<uint8_t>::max)()};
    msg->int16_values = {(std::numeric_limits<int16_t>::max)()};
    msg->uint16_values = {(std::numeric_limits<uint16_t>::max)()};
    msg->int32_values = {(std::numeric_limits<int32_t>::max)()};
    msg->uint32_values = {(std::numeric_limits<uint32_t>::max)()};
    msg->int64_values = {(std::numeric_limits<int64_t>::max)()};
    msg->uint64_values = {(std::numeric_limits<uint64_t>::max)()};
    msg->string_values = {{"max value"}};
    msg->basic_types_values = {{*basic_types_msgs[0]}};
    msg->alignment_check = 1;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
    msg->bool_values = {{false, true}};
    msg->byte_values = {{0, 0xff}};
    msg->char_values = {{0, 255}};
    msg->float32_values = {{0.0f, 1.125f, -2.125f}};
    msg->float64_values = {{0, 1.125, -2.125}};
    msg->int8_values = {{
      0, (std::numeric_limits<int8_t>::max)(), (std::numeric_limits<int8_t>::min)()}};
    msg->uint8_values = {{0, (std::numeric_limits<uint8_t>::max)()}};
    msg->int16_values = {{
      0, (std::numeric_limits<int16_t>::max)(), (std::numeric_limits<int16_t>::min)()}};
    msg->uint16_values = {{0, (std::numeric_limits<uint16_t>::max)()}};
    // The narrowing static cast is required to avoid build errors on Windows.
    msg->int32_values = {{
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::max)(),
      (std::numeric_limits<int32_t>::min)()
    }};
    msg->uint32_values = {{0, (std::numeric_limits<uint32_t>::max)()}};
    msg->int64_values.resize(3);
    msg->int64_values[0] = 0;
    msg->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    msg->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    msg->uint64_values = {{0, (std::numeric_limits<uint64_t>::max)()}};
    msg->string_values = {{"", "max value", "optional min value"}};
    msg->basic_types_values.resize(3);
    msg->basic_types_values[0] = *basic_types_msgs[0];
    msg->basic_types_values[1] = *basic_types_msgs[1];
    msg->basic_types_values[2] = *basic_types_msgs[2];
    msg->alignment_check = 2;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
    // check sequences with more then 100 elements
    const size_t size = 1000;
    msg->bool_values.resize(size);
    msg->byte_values.resize(size);
    msg->char_values.resize(size);
    msg->float32_values.resize(size);
    msg->float64_values.resize(size);
    msg->int8_values.resize(size);
    msg->uint8_values.resize(size);
    msg->int16_values.resize(size);
    msg->uint16_values.resize(size);
    msg->int32_values.resize(size);
    msg->uint32_values.resize(size);
    msg->int64_values.resize(size);
    msg->uint64_values.resize(size);
    msg->string_values.resize(size);
    msg->basic_types_values.resize(size);
    for (size_t i = 0; i < size; ++i) {
      msg->bool_values[i] = (i % 2 != 0) ? true : false;
      msg->byte_values[i] = static_cast<uint8_t>(i);
      msg->char_values[i] = static_cast<char>(i % (1 << 8));
      msg->float32_values[i] = 1.125f * i;
      msg->float64_values[i] = 1.125 * i;
      msg->int8_values[i] = static_cast<int8_t>(i);
      msg->uint8_values[i] = static_cast<uint8_t>(i);
      msg->int16_values[i] = static_cast<int16_t>(i);
      msg->uint16_values[i] = static_cast<uint16_t>(i);
      msg->int32_values[i] = static_cast<int32_t>(i);
      msg->uint32_values[i] = static_cast<uint32_t>(i);
      msg->int64_values[i] = i;
      msg->uint64_values[i] = i;
      msg->string_values[i] = std::to_string(i);
      msg->basic_types_values[i] = *basic_types_msgs[i % basic_types_msgs.size()];
    }
    msg->alignment_check = 3;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
    // check default sequences
    msg->alignment_check = 4;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::BoundedPlainSequences::SharedPtr>
get_messages_bounded_plain_sequences()
{
  auto basic_types_msgs = get_messages_basic_types();
  auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
  std::vector<test_msgs::msg::BoundedPlainSequences::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::BoundedPlainSequences>();
    msg->bool_values = {{false, true, false}};
    msg->byte_values = {{0, 1, 0xff}};
    msg->char_values = {{0, 1, 255}};
    msg->float32_values = {{0.0f, 1.125f, -2.125f}};
    msg->float64_values = {{0, 1.125, -2.125}};
    msg->int8_values = {{
      0, (std::numeric_limits<int8_t>::max)(), (std::numeric_limits<int8_t>::min)()}};
    msg->uint8_values = {{0, 1, (std::numeric_limits<uint8_t>::max)()}};
    msg->int16_values = {{
      0, (std::numeric_limits<int16_t>::max)(), (std::numeric_limits<int16_t>::min)()}};
    msg->uint16_values = {{0, 1, (std::numeric_limits<uint16_t>::max)()}};
    // The narrowing static cast is required to avoid build errors on Windows.
    msg->int32_values = {{
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::max)(),
      (std::numeric_limits<int32_t>::min)()
    }};
    msg->uint32_values = {{0, 1, (std::numeric_limits<uint32_t>::max)()}};
    msg->int64_values.resize(3);
    msg->int64_values[0] = 0;
    msg->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    msg->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    msg->uint64_values = {{0, 1, (std::numeric_limits<uint64_t>::max)()}};
    msg->basic_types_values.resize(3);
    msg->basic_types_values[0] = *basic_types_msgs[0];
    msg->basic_types_values[1] = *basic_types_msgs[1];
    msg->basic_types_values[2] = *basic_types_msgs[2];
    msg->alignment_check = 2;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::BoundedPlainSequences>();
    // check default sequences
    msg->alignment_check = 4;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::BoundedSequences::SharedPtr>
get_messages_bounded_sequences()
{
  auto basic_types_msgs = get_messages_basic_types();
  auto msg = std::make_shared<test_msgs::msg::UnboundedSequences>();
  std::vector<test_msgs::msg::BoundedSequences::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::BoundedSequences>();
    msg->bool_values = {{false, true, false}};
    msg->byte_values = {{0, 1, 0xff}};
    msg->char_values = {{0, 1, 255}};
    msg->float32_values = {{0.0f, 1.125f, -2.125f}};
    msg->float64_values = {{0, 1.125, -2.125}};
    msg->int8_values = {{
      0, (std::numeric_limits<int8_t>::max)(), (std::numeric_limits<int8_t>::min)()}};
    msg->uint8_values = {{0, 1, (std::numeric_limits<uint8_t>::max)()}};
    msg->int16_values = {{
      0, (std::numeric_limits<int16_t>::max)(), (std::numeric_limits<int16_t>::min)()}};
    msg->uint16_values = {{0, 1, (std::numeric_limits<uint16_t>::max)()}};
    // The narrowing static cast is required to avoid build errors on Windows.
    msg->int32_values = {{
      static_cast<int32_t>(0),
      (std::numeric_limits<int32_t>::max)(),
      (std::numeric_limits<int32_t>::min)()
    }};
    msg->uint32_values = {{0, 1, (std::numeric_limits<uint32_t>::max)()}};
    msg->int64_values.resize(3);
    msg->int64_values[0] = 0;
    msg->int64_values[1] = (std::numeric_limits<int64_t>::max)();
    msg->int64_values[2] = (std::numeric_limits<int64_t>::min)();
    msg->uint64_values = {{0, 1, (std::numeric_limits<uint64_t>::max)()}};
    msg->string_values = {{"", "max value", "optional min value"}};
    msg->basic_types_values.resize(3);
    msg->basic_types_values[0] = *basic_types_msgs[0];
    msg->basic_types_values[1] = *basic_types_msgs[1];
    msg->basic_types_values[2] = *basic_types_msgs[2];
    msg->alignment_check = 2;
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::BoundedSequences>();
    // check default sequences
    msg->alignment_check = 4;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::MultiNested::SharedPtr>
get_messages_multi_nested()
{
  auto arrays_msgs = get_messages_arrays();
  auto bounded_sequences_msgs = get_messages_bounded_sequences();
  auto unbounded_sequences_msgs = get_messages_unbounded_sequences();
  const std::size_t num_arrays = arrays_msgs.size();
  const std::size_t num_bounded_sequences = bounded_sequences_msgs.size();
  const std::size_t num_unbounded_sequences = unbounded_sequences_msgs.size();
  std::vector<test_msgs::msg::MultiNested::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::MultiNested>();
    for (std::size_t i = 0u; i < msg->array_of_arrays.size(); ++i) {
      msg->array_of_arrays[i] = *arrays_msgs[i % num_arrays];
    }
    for (std::size_t i = 0u; i < msg->array_of_bounded_sequences.size(); ++i) {
      msg->array_of_bounded_sequences[i] = *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    for (std::size_t i = 0u; i < msg->array_of_unbounded_sequences.size(); ++i) {
      msg->array_of_unbounded_sequences[i] = *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }
    const std::size_t sequence_size = 3u;
    msg->bounded_sequence_of_arrays.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->bounded_sequence_of_arrays[i] = *arrays_msgs[i % num_arrays];
    }
    msg->bounded_sequence_of_bounded_sequences.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->bounded_sequence_of_bounded_sequences[i] =
        *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    msg->bounded_sequence_of_unbounded_sequences.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->bounded_sequence_of_unbounded_sequences[i] =
        *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }
    msg->unbounded_sequence_of_arrays.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->unbounded_sequence_of_arrays[i] = *arrays_msgs[i % num_arrays];
    }
    msg->unbounded_sequence_of_bounded_sequences.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->unbounded_sequence_of_bounded_sequences[i] =
        *bounded_sequences_msgs[i % num_bounded_sequences];
    }
    msg->unbounded_sequence_of_unbounded_sequences.resize(sequence_size);
    for (std::size_t i = 0u; i < sequence_size; ++i) {
      msg->unbounded_sequence_of_unbounded_sequences[i] =
        *unbounded_sequences_msgs[i % num_unbounded_sequences];
    }
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Nested::SharedPtr>
get_messages_nested()
{
  std::vector<test_msgs::msg::Nested::SharedPtr> messages;
  auto basic_types_msgs = get_messages_basic_types();
  for (auto basic_types_msg : basic_types_msgs) {
    auto msg = std::make_shared<test_msgs::msg::Nested>();
    msg->basic_types_value = *basic_types_msg;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::Builtins::SharedPtr>
get_messages_builtins()
{
  std::vector<test_msgs::msg::Builtins::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::Builtins>();
    msg->duration_value.sec = -1234567890;
    msg->duration_value.nanosec = 123456789;
    msg->time_value.sec = -1234567890;
    msg->time_value.nanosec = 987654321;
    messages.push_back(msg);
  }
  return messages;
}

static inline std::vector<test_msgs::msg::WStrings::SharedPtr>
get_messages_wstrings()
{
  std::vector<test_msgs::msg::WStrings::SharedPtr> messages;
  {
    auto msg = std::make_shared<test_msgs::msg::WStrings>();
    msg->wstring_value = u"";
    msg->array_of_wstrings[0] = u"1";
    msg->array_of_wstrings[1] = u"two";
    msg->array_of_wstrings[2] = u"三";  // "One" in Japanese
    msg->bounded_sequence_of_wstrings.resize(2);
    msg->bounded_sequence_of_wstrings[0] = u"one";
    msg->bounded_sequence_of_wstrings[1] = u"二";  // "Two" in Japanese
    msg->unbounded_sequence_of_wstrings.resize(4);
    msg->unbounded_sequence_of_wstrings[0] = u".";
    msg->unbounded_sequence_of_wstrings[1] = u"..";
    msg->unbounded_sequence_of_wstrings[2] = u"...";
    msg->unbounded_sequence_of_wstrings[3] = u"四";  // "Four" in Japanese
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::WStrings>();
    msg->wstring_value = u"ascii";
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::WStrings>();
    msg->wstring_value = u"Hell\u00F6 W\u00F6rld!";  // using umlaut
    messages.push_back(msg);
  }
  {
    auto msg = std::make_shared<test_msgs::msg::WStrings>();
    msg->wstring_value = u"ハローワールド";  // "Hello world" in Japanese
    messages.push_back(msg);
  }
  return messages;
}

#endif  // TEST_MSGS__MESSAGE_FIXTURES_HPP_
