// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/logger.h"
#include "ofp/timestamp.h"
#include <signal.h>
#include <unistd.h>     // for write
#include <execinfo.h>   // for backtrace, backtrace_symbols

using namespace ofp::log;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

Logger *const ofp::log::GLOBAL_Logger = new Logger();

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

static const char LEVEL_INITIALS[] = "dIWEFst";

inline char levelPrefix(Level level) {
  unsigned lvl = static_cast<unsigned>(level);
  return lvl < sizeof(LEVEL_INITIALS) - 1 ? LEVEL_INITIALS[lvl] : '?';
}

void Logger::configure(Level level, Trace trace, int fd, bool logSignals) {
  setLevelFilter(level);
  setTraceFilter(trace);
  fd_ = fd;

  std::lock_guard<std::mutex> lock{outputMutex_};
  output_.reset(new llvm::raw_fd_ostream{fd, true});
  output_->SetBufferSize(4096);

  if (logSignals) {
    initFatalSignalHandlers();
  }
}

void Logger::configure(Level level, Trace trace,
                       std::unique_ptr<llvm::raw_fd_ostream> &&output, bool logSignals) {
  setLevelFilter(level);
  setTraceFilter(trace);
  fd_ = output->fd();

  std::lock_guard<std::mutex> lock{outputMutex_};
  output_ = std::move(output);
  output_->SetBufferSize(4096);

  if (logSignals) {
    initFatalSignalHandlers();
  }
}

void Logger::write(Level level, const char *data, size_t size) {
  auto ts = Timestamp::now().toStringUTC();

  std::lock_guard<std::mutex> lock{outputMutex_};
  *output_ << levelPrefix(level) << ts << ' ';
  output_->write(data, size);
  *output_ << '\n';
  output_->flush();
}

// =============================================================================
// B a c k t r a c e   S u p p o r t 
// =============================================================================

OFP_BEGIN_IGNORE_PADDING

struct SignalInfo {
  int signum;
  const char *name;
};

OFP_END_IGNORE_PADDING

static const SignalInfo kFatalSignals[] = { 
    { SIGBUS, "SIGBUS (Bus Error)" }, 
    { SIGSEGV, "SIGSEG (Segmentation Fault)" },
    { SIGILL,  "SIGILL (Illegal Instruction)" },
    { SIGFPE, "SIGFPE (Floating Point Exception)" },
    { SIGSYS, "SIGSYS (Illegal System Call)" },
    { SIGQUIT, "SIGQUIT (Quit Program)" },
    { SIGABRT, "SIGABRT (Abort)" }
};

static const char *sFatalSignalName(int signum) {
  for (unsigned i = 0; i < ofp::ArrayLength(kFatalSignals); ++i) {
    if (kFatalSignals[i].signum == signum) {
      return kFatalSignals[i].name;
    }
  }
  return "??? (Unknown)";
}

static void sRestoreSignalHandlers() {
  // Restore signal handlers to their defaults.
  struct sigaction act;
  std::memset(&act, 0, sizeof(act));
  act.sa_handler = SIG_DFL;

  for (const auto &info: kFatalSignals) {
    ::sigaction(info.signum, &act, nullptr);
  }

  // Unmask blocked signals.
  sigset_t mask;
  sigfillset(&mask);
  ::sigprocmask(SIG_UNBLOCK, &mask, nullptr);
}

static void sFatalSignalHandler(int signum, siginfo_t *info, void *uc) {
  int fd = GLOBAL_Logger->fd();
  const char *name = sFatalSignalName(signum);

  // strlen async-signal-safe in POSIX.1-2016 and assumed safe before...
  ::write(fd, "\n********\nFatal Signal: ", 24);
  ::write(fd, name, std::strlen(name));
  ::write(fd, "\n********\n", 10);

  // Restore signal handling to the default state in case we crash in
  // the signal handler.

  sRestoreSignalHandlers();

  // Here is where we start using *backtrace* functions that are not 
  // async-signal-safe. This code could hang...

  void *buf[25];   // 200 bytes on 64-bit system
  int buflen = ::backtrace(buf, ofp::ArrayLength(buf));
  if (buflen > 0) {
    ::backtrace_symbols_fd(buf, buflen, fd);
  }

  // When we fall off the end of the signal handler, the fatal signal will be
  // raised again to be handled by the default handler. We need to manually
  // resend the signal however if it was sent manually. (See Google Breakpad)
  
  if (info->si_code <= 0 || signum == SIGABRT) {
    if (::kill(getpid(), signum) < 0) {
      std::_Exit(EXIT_FAILURE);
    }
  }
}

void Logger::initFatalSignalHandlers() {
  static stack_t sStack_GLOBAL;
  const size_t kSignalStackSize = SIGSTKSZ;

  // Install alternate signal stack.
  if (sStack_GLOBAL.ss_size == 0) {
    sStack_GLOBAL.ss_sp = malloc(kSignalStackSize);
    sStack_GLOBAL.ss_size = kSignalStackSize;
    if (sStack_GLOBAL.ss_sp == nullptr || sigaltstack(&sStack_GLOBAL, nullptr) < 0) {
      return;
    }
    fprintf(stderr, "initFatalSignalHandlers: %zu", kSignalStackSize);
  }

  struct sigaction act;
  std::memset(&act, 0, sizeof(act));
  sigfillset(&act.sa_mask);
  act.sa_sigaction = sFatalSignalHandler;
  act.sa_flags = SA_SIGINFO | SA_ONSTACK;

  for (const auto &info: kFatalSignals) {
    ::sigaction(info.signum, &act, nullptr);
  }
}
