// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LOGGER_H_
#define OFP_LOGGER_H_

#include <mutex>
#include "ofp/types.h"

namespace llvm {
class raw_fd_ostream;
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

enum { kTraceMsg = 0, kTraceRpc = 1 };

enum class Trace {
  None = 0x00,
  Msg = 1 << kTraceMsg,
  Rpc = 1 << kTraceRpc,
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

OFP_BEGIN_IGNORE_PADDING

class Logger {
 public:
  Logger() = default;

  void configure(Level level, Trace trace, int fd, bool logSignals);
  void configure(Level level, Trace trace,
                 std::unique_ptr<llvm::raw_fd_ostream> &&output, bool logSignals);

  Level levelFilter() const { return levelFilter_; }
  void setLevelFilter(Level level) { levelFilter_ = level; }

  Trace traceFilter() const { return traceFilter_; }
  void setTraceFilter(Trace trace) { traceFilter_ = trace; }

  bool enabled(Level level) const { return level >= levelFilter_; }
  bool enabled(Trace trace) const {
    return (trace & traceFilter_) != Trace::None;
  }

  void write(Level level, const char *data, size_t size);

  int fd() const { return fd_; }

 private:
  std::unique_ptr<llvm::raw_fd_ostream> output_;
  std::mutex outputMutex_;  // protect `output_`
  Level levelFilter_ = Level::Silent;
  Trace traceFilter_ = Trace::None;
  int fd_ = -1;

  void initFatalSignalHandlers();
};

OFP_END_IGNORE_PADDING

extern Logger *const GLOBAL_Logger;

inline void configure(Level level, Trace trace = Trace::None, int fd = STDERR, bool logSignals = false) {
  GLOBAL_Logger->configure(level, trace, fd, logSignals);
}

inline void configure(Level level, Trace trace,
                      std::unique_ptr<llvm::raw_fd_ostream> output, bool logSignals = false) {
  GLOBAL_Logger->configure(level, trace, std::move(output), logSignals);
}

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOGGER_H_
