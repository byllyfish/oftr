//  ===== ---- ofp/log.cpp ---------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements logging functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/log.h"
#include "ofp/yaml/decoder.h"
#include <chrono>
#include <iomanip>

namespace ofp {  // <namespace ofp>
namespace log {  // <namespace log>

using Time = std::pair<std::time_t, Milliseconds>;

static Time currentTime() {
  using namespace std::chrono;

  auto now = system_clock::now();
  milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch());
  time_t t = system_clock::to_time_t(now);

  return {t, ms % 1000};
}

static std::ostream &operator<<(std::ostream &os, const Time &t) {
  return os << t.first << '.' << std::setfill('0') << std::setw(3)
            << t.second.count();
}

const char *levelToString(Level level) {
  switch (level) {
    case Level::Debug:
      return "[debug]";
    case Level::Trace:
      return "[trace]";
    case Level::Info:
      return "[info]";
    case Level::Warning:
      return "[warning]";
    case Level::Error:
      return "[error]";
    case Level::Fatal:
      return "[fatal]";
    case Level::Silent:
      return "[silent]";
  }
  return "";
}

#ifndef NDEBUG
 const Level kDefaultLevel = Level::Debug;
#else
 const Level kDefaultLevel = Level::Info;
#endif 

namespace detail {

OutputCallback GlobalOutputCallback = nullptr;
void *GlobalOutputCallbackContext = nullptr;
Level GlobalOutputLevelFilter = Level::Silent;

}  // namespace detail

void setOutputCallback(OutputCallback callback, void *context) {
  detail::GlobalOutputCallback = callback;
  detail::GlobalOutputCallbackContext = context;
  if (callback == nullptr) {
    detail::GlobalOutputLevelFilter = Level::Silent;
  } else if (detail::GlobalOutputLevelFilter == Level::Silent) {
    detail::GlobalOutputLevelFilter = kDefaultLevel;
  }
}

void setOutputLevelFilter(Level level) {
  if (detail::GlobalOutputCallback != nullptr)
    detail::GlobalOutputLevelFilter = level;
}

static void streamOutputCallback(Level level, const char *line, size_t size,
                                 void *context) {
  std::ostream &os = *reinterpret_cast<std::ostream *>(context);
  os << currentTime() << ' ' << levelToString(level) << ' ' << line << '\n';
}

void setOutputStream(std::ostream *outputStream) {
  setOutputCallback(streamOutputCallback, outputStream);
}

static void trace1(const char *type, UInt64 id, const void *data, size_t length) {
  if (length < sizeof(Header)) {
    detail::write_(Level::Trace, type, length, "Invalid Data:",
                   RawDataToHex(data, length));
    return;
  }

  Message message{data, length};
  message.transmogrify();

  yaml::Decoder decoder{&message};

  if (decoder.error().empty()) {
    detail::write_(Level::Trace, type, length, "bytes", std::make_pair("connid", id), '\n', decoder.result(),
                   RawDataToHex(data, length));
  } else {
    detail::write_(Level::Trace, type, length, "bytes", std::make_pair("connid", id), '\n', decoder.error(),
                   RawDataToHex(data, length));
  }
}

void trace(const char *type, UInt64 id, const void *data, size_t length) {
  if (Level::Trace < detail::GlobalOutputLevelFilter)
    return;

  // The memory buffer may contain multiple messages. We need to log each one
  // separately.

  size_t remaining = length;
  const UInt8 *ptr = BytePtr(data);

  const Header *header = reinterpret_cast<const Header *>(ptr);
  while (remaining >= sizeof(Header) && header->length() <= remaining) {
    trace1(type, id, ptr, header->length());

    remaining -= header->length();
    ptr += header->length();
    header = reinterpret_cast<const Header *>(ptr);
  }

  if (remaining > 0) {
    detail::write_(Level::Trace, type, "Invalid Leftover:",
                   RawDataToHex(ptr, remaining));
  }
}

}  // </namespace log>
}  // </namespace ofp>
