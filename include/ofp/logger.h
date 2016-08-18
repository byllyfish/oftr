#ifndef OFP_LOGGER_H_
#define OFP_LOGGER_H_

#include "ofp/types.h"
#include <mutex>

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
  return static_cast<Trace>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

inline Trace operator|(Trace lhs, Trace rhs) {
  return static_cast<Trace>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

class Logger {
public:
    Logger() = default;

    void configure(Level level, Trace trace, int fd);
    void configure(Level level, Trace trace, llvm::raw_ostream *output);

    bool enabled(Level level) const { return level >= levelFilter_; }
    bool enabled(Trace trace) const { return (trace & traceFilter_) != Trace::None; }

    Level levelFilter() const { return levelFilter_; }
    void setLevelFilter(Level level) { levelFilter_ = level; }

    Trace traceFilter() const { return traceFilter_; }
    void setTraceFilter(Trace trace) { traceFilter_ = trace;}

    void write(Level level, const char *data, size_t size) {
        callback_(this, level, data, size);
    }

private:
    using LoggerCallback = void(*)(Logger *,Level, const char *, size_t);

    LoggerCallback callback_ = noop;
    llvm::raw_ostream *output_ = nullptr;
    std::mutex outputMutex_;                      // protect `output_`
    std::unique_ptr<llvm::raw_ostream> logstream_;
    Level levelFilter_ = Level::Silent;
    Trace traceFilter_ = Trace::None;

    void setDestination(int fd);
    void setDestination(llvm::raw_ostream *output);

    static void noop(Logger *logger, Level level, const char *line, size_t size) {}
    static void raw(Logger *logger, Level level, const char *line, size_t size);
};

extern Logger GLOBAL_Logger;

inline void configure(Level level, Trace trace = Trace::None, int fd = STDERR) {
  ofp::log::GLOBAL_Logger.configure(level, trace, fd);
}

inline void configure(Level level, Trace trace, llvm::raw_ostream *output) {
  ofp::log::GLOBAL_Logger.configure(level, trace, output);
}

}  // namespace log
}  // namespace ofp

#endif // OFP_LOGGER_H_
