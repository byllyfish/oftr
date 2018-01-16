// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/logger.h"
#include "ofp/timestamp.h"

using namespace ofp::log;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

Logger *const ofp::log::GLOBAL_Logger = new Logger();

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

static const char LEVEL_INITIALS[] = "dIWEFst";

inline char levelPrefix(Level level) {
  unsigned lvl = static_cast<unsigned>(level);
  return lvl < sizeof(LEVEL_INITIALS) - 1 ? LEVEL_INITIALS[lvl] : '?';
}

void Logger::configure(Level level, Trace trace, int fd) {
  setLevelFilter(level);
  setTraceFilter(trace);

  std::lock_guard<std::mutex> lock{outputMutex_};
  output_.reset(new llvm::raw_fd_ostream{fd, true});
  output_->SetBufferSize(4096);
}

void Logger::configure(Level level, Trace trace,
                       std::unique_ptr<llvm::raw_ostream> &&output) {
  setLevelFilter(level);
  setTraceFilter(trace);

  std::lock_guard<std::mutex> lock{outputMutex_};
  output_ = std::move(output);
  output_->SetBufferSize(4096);
}

void Logger::write(Level level, const char *data, size_t size) {
  auto ts = Timestamp::now().toStringUTC();

  std::lock_guard<std::mutex> lock{outputMutex_};
  *output_ << levelPrefix(level) << ts << ' ';
  output_->write(data, size);
  *output_ << '\n';
  output_->flush();
}
