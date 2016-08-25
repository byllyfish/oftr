// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LOGGER_H_
#define OFP_LOGGER_H_

#include <mutex>
#include "ofp/types.h"

namespace llvm {
class raw_ostream;
}  // namespace llvm

namespace ofp {
namespace log {

enum class Level {
  Debug = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
  Silent = 5,
  Trace = 6
};

enum class Trace {
  None = 0x00,
  Msg = 0x01,
  Rpc = 0x02,
};

enum FileDescriptor {
  STDERR = 2,
  // TODO(bfish) SYSLOG = -2
};

inline Trace operator&(Trace lhs, Trace rhs) {
  return static_cast<Trace>(static_cast<unsigned>(lhs) &
                            static_cast<unsigned>(rhs));
}

inline Trace operator|(Trace lhs, Trace rhs) {
  return static_cast<Trace>(static_cast<unsigned>(lhs) |
                            static_cast<unsigned>(rhs));
}

class Logger {
 public:
  Logger() = default;

  void configure(Level level, Trace trace, int fd);
  void configure(Level level, Trace trace,
                 std::unique_ptr<llvm::raw_ostream> &&output);

  Level levelFilter() const { return levelFilter_; }
  void setLevelFilter(Level level) { levelFilter_ = level; }

  Trace traceFilter() const { return traceFilter_; }
  void setTraceFilter(Trace trace) { traceFilter_ = trace; }

  bool enabled(Level level) const { return level >= levelFilter_; }
  bool enabled(Trace trace) const {
    return (trace & traceFilter_) != Trace::None;
  }

  void write(Level level, const char *data, size_t size);

 private:
  std::unique_ptr<llvm::raw_ostream> output_;
  std::mutex outputMutex_;  // protect `output_`
  Level levelFilter_ = Level::Silent;
  Trace traceFilter_ = Trace::None;
};

extern Logger *const GLOBAL_Logger;

inline void configure(Level level, Trace trace = Trace::None, int fd = STDERR) {
  GLOBAL_Logger->configure(level, trace, fd);
}

inline void configure(Level level, Trace trace,
                      std::unique_ptr<llvm::raw_ostream> output) {
  GLOBAL_Logger->configure(level, trace, std::move(output));
}

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOGGER_H_
