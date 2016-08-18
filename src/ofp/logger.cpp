#include "ofp/logger.h"
#include "ofp/timestamp.h"
#include "llvm/Support/raw_ostream.h"

using namespace ofp::log;

Logger ofp::log::GLOBAL_Logger;
 
static const char LEVEL_INITIALS[] = "dIWEFst";

inline char levelPrefix(Level level) {
  unsigned lvl = static_cast<unsigned>(level);
  return lvl < sizeof(LEVEL_INITIALS) - 1 ? LEVEL_INITIALS[lvl] : '?';
}


void Logger::configure(Level level, Trace trace, int fd) {
    // `configure` may only be called once.
    assert(callback_ == noop);

    setDestination(fd);
    setLevelFilter(level);
    setTraceFilter(trace);
}


void Logger::configure(Level level, Trace trace, llvm::raw_ostream *output) {
    // `configure` may only be called once.
    assert(callback_ == noop);

    setDestination(output);
    setLevelFilter(level);
    setTraceFilter(trace);
}


void Logger::setDestination(int fd) {
    assert(fd >= 0);
    logstream_.reset(new llvm::raw_fd_ostream{fd, true});
    setDestination(logstream_.get());
}

void Logger::setDestination(llvm::raw_ostream *output) {
    std::lock_guard<std::mutex> lock{outputMutex_};
    output_ = output;
    callback_ = raw;
}


void Logger::raw(Logger *logger, Level level, const char *line, size_t size) {
  auto ts = Timestamp::now().toStringUTC();
  auto out = logger->output_;

  std::lock_guard<std::mutex> lock{logger->outputMutex_};
  *out << levelPrefix(level) << ts << ' ';
  out->write(line, size);
  *out << '\n';
  out->flush();
}
