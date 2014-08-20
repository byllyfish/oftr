//  ===== ---- ofp/log.h -----------------------------------*- C++ -*- =====  //
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
/// \brief Implements logging hooks for the library.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_LOG_H_
#define OFP_LOG_H_

#include <sstream>
#include "llvm/ADT/StringRef.h"
#include "ofp/types.h"

// Allow llvm::StringRef to write to a std::ostream.

namespace llvm {

std::ostream &operator<<(std::ostream &os, const StringRef &s);

inline std::ostream &operator<<(std::ostream &os, const StringRef &s) {
  return os.write(s.data(), ofp::Signed_cast(s.size()));
}

}  // namespace llvm

namespace ofp {
namespace log {

enum class Level {
  Debug = 0,
  Trace = 1,
  Info = 2,
  Error = 3,
  Fatal = 4,
  Silent = 5
};

const char *levelToString(Level level);

/// Type of function called when an event is logged. There can be at most one
/// output callback function specified at any one time.
typedef void (*OutputCallback)(Level level, const char *line, size_t size,
                               void *context);

/// \brief Sets the output callback function. This function will be called for
/// each line of log output with the specified context pointer.
void setOutputCallback(OutputCallback callback, void *context);

/// \brief Sets the output callback to log each line to the specified output
/// stream (e.g. std::cerr).
void setOutputStream(std::ostream *outputStream);

/// \brief Sets the minimum desired level of output.
void setOutputLevelFilter(Level level);

namespace detail {

extern OutputCallback GlobalOutputCallback;
extern void *GlobalOutputCallbackContext;
extern Level GlobalOutputLevelFilter;

template <class Type1>
void write_(std::ostream &os, Level level, const Type1 &value1) {
  os << value1;
}

template <class Type1, class... Args>
void write_(std::ostream &os, Level level, const Type1 &value1,
            const Args &... args) {
  os << value1;
  os << ' ';
  write_(os, level, args...);
}

template <class... Args>
void write_(Level level, const Args &... args) {
#if !defined(LIBOFP_LOGGING_DISABLED)
  if (level >= detail::GlobalOutputLevelFilter) {
    std::ostringstream oss;
    write_(oss, level, args...);
    std::string buf = oss.str();
    GlobalOutputCallback(level, buf.data(), buf.size(),
                         GlobalOutputCallbackContext);
  }
#endif //OFP_LOGGING_DISABLED
}

}  // namespace detail

void trace(const char *type, const void *data, size_t length);

template <class... Args>
inline void info(const Args &... args) {
  detail::write_(Level::Info, args...);
}

template <class... Args>
inline void debug(const Args &... args) {
#ifndef NDEBUG
  detail::write_(Level::Debug, args...);
#endif
}

template <class... Args>
inline void error(const Args &... args) {
  detail::write_(Level::Error, args...);
}

template <class... Args>
inline void fatal(const Args &... args) {
  detail::write_(Level::Fatal, args...);
}

class Lifetime {
 public:
  /* implicit NOLINT */ Lifetime(const char *description) noexcept
      : description_{description} {
    debug("Create", description_);
  }

  Lifetime(const Lifetime &lifetime) noexcept : description_{lifetime.description_} {}

  ~Lifetime() { debug("Dispose", description_); }

  Lifetime &operator=(const Lifetime &lifetime) noexcept {
    description_ = lifetime.description_;
    return *this;
  }

 private:
  const char *description_;
};

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOG_H_
