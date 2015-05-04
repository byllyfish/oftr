// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/log.h"
#if defined(LIBOFP_TARGET_DARWIN)
#include <syslog.h>  // for LOG_ constants
#endif               // defined(LIBOFP_TARGET_DARWIN)
#include <chrono>
#include <iomanip>
#include "ofp/yaml/decoder.h"

namespace ofp {
namespace log {

// Print the current timestamp into the given buffer. The format is exactly
// 24 characters (including a trailing \0):
//
//   YYYYMMDD HHMMSS.ssssss \0
//
static size_t timestamp_now(char *buf, size_t buflen) {
  using namespace std::chrono;

  assert(buflen >= 24);

  auto now = system_clock::now();
  auto secs = system_clock::to_time_t(now);
  UInt32 msec = UInt32_narrow_cast(
      (duration_cast<microseconds>(now.time_since_epoch()) % 1000000).count());

  struct tm date;
  localtime_r(&secs, &date);
  date.tm_year += 1900;
  date.tm_mon += 1;

  int rc = snprintf(buf, buflen, "%04d%02d%02d %02d%02d%02d.%06d ",
                    date.tm_year, date.tm_mon, date.tm_mday, date.tm_hour,
                    date.tm_min, date.tm_sec, msec);

  assert(rc == 23);

  return Unsigned_cast(rc);
}

// Terminal Colors
#define RED(s) "\033[31m" s
#define YELLOW(s) "\033[33m" s
#define GREEN(s) "\033[32m" s
#define GRAY(s) "\033[90m" s
#define BLUE(s) "\033[34m" s

static char levelPrefix(Level level) {
  switch (level) {
    case Level::Debug:
      return 'd';
    case Level::Trace:
      return 't';
    case Level::Info:
      return 'I';
    case Level::Warning:
      return 'W';
    case Level::Error:
      return 'E';
    case Level::Fatal:
      return 'F';
    case Level::Silent:
      return 's';
  }
  return '?';
}

static const char *levelPrefixColor(Level level) {
  switch (level) {
    case Level::Debug:
      return GRAY("d");
    case Level::Trace:
      return BLUE("t");
    case Level::Info:
      return GREEN("I");
    case Level::Warning:
      return YELLOW("W");
    case Level::Error:
      return RED("E");
    case Level::Fatal:
      return RED("F");
    case Level::Silent:
      return "s";
  }
  return "?";
}

#ifndef NDEBUG
const Level kDefaultLevel = Level::Debug;
const UInt32 kDefaultTrace = 0xFF;
#else
const Level kDefaultLevel = Level::Info;
const UInt32 kDefaultTrace = 0;
#endif

namespace detail {

OutputCallback GlobalOutputCallback = nullptr;
void *GlobalOutputCallbackContext = nullptr;
Level GlobalOutputLevelFilter = Level::Silent;
UInt32 GlobalOutputTraceFilter = 0;

}  // namespace detail

void setOutputCallback(OutputCallback callback, void *context) {
  detail::GlobalOutputCallback = callback;
  detail::GlobalOutputCallbackContext = context;
  if (callback == nullptr) {
    detail::GlobalOutputLevelFilter = Level::Silent;
    detail::GlobalOutputTraceFilter = 0;
  } else if (detail::GlobalOutputLevelFilter == Level::Silent) {
    detail::GlobalOutputLevelFilter = kDefaultLevel;
    detail::GlobalOutputTraceFilter = kDefaultTrace;
  }
}

void setOutputLevelFilter(Level level) {
  if (detail::GlobalOutputCallback != nullptr)
    detail::GlobalOutputLevelFilter = level;
}

void setOutputTraceFilter(UInt32 trace) {
  if (detail::GlobalOutputCallback != nullptr)
    detail::GlobalOutputTraceFilter = trace;
}

static void streamOutputCallback(Level level, const char *line, size_t size,
                                 void *context) {
  std::ostream *os = reinterpret_cast<std::ostream *>(context);

  *os << levelPrefix(level);

  char tbuf[32];
  size_t tlen = timestamp_now(tbuf, sizeof(tbuf));
  os->write(tbuf, Signed_cast(tlen));
  os->write(line, Signed_cast(size));

  *os << '\n';
  os->flush();
}

static void rawStreamOutputCallback(Level level, const char *line, size_t size,
                                    void *context) {
  llvm::raw_ostream *os = reinterpret_cast<llvm::raw_ostream *>(context);

  *os << levelPrefix(level);

  char tbuf[32];
  size_t tlen = timestamp_now(tbuf, sizeof(tbuf));
  os->write(tbuf, tlen);
  os->write(line, size);

  *os << '\n';
  os->flush();
}

static void rawStreamColorOutputCallback(Level level, const char *line,
                                         size_t size, void *context) {
  llvm::raw_ostream *os = reinterpret_cast<llvm::raw_ostream *>(context);

  *os << levelPrefixColor(level);

  char tbuf[32];
  size_t tlen = timestamp_now(tbuf, sizeof(tbuf));
  os->write(tbuf, tlen);
  os->write(line, size);

  *os << "\n\033[0m";
  os->flush();
}

void setOutputStream(std::ostream *outputStream) {
  setOutputCallback(streamOutputCallback, outputStream);
}

void setOutputStream(llvm::raw_ostream *outputStream) {
  if (true) {  // FIXME(bfish): use has_colors() when working...
    setOutputCallback(rawStreamColorOutputCallback, outputStream);
  } else {
    setOutputCallback(rawStreamOutputCallback, outputStream);
  }
}

#if defined(LIBOFP_TARGET_DARWIN)

static void aslStreamOutputCallback(Level level, const char *line, size_t size,
                                    void *context) {
  aslclient client = static_cast<aslclient>(context);

  int priority;
  switch (level) {
    case ofp::log::Level::Debug:
      priority = LOG_DEBUG;
      break;
    case ofp::log::Level::Trace:
      priority = LOG_INFO;
      break;
    case ofp::log::Level::Info:
      priority = LOG_NOTICE;
      break;
    case ofp::log::Level::Warning:
      priority = LOG_WARNING;
      break;
    case ofp::log::Level::Error:
      priority = LOG_ERR;
      break;
    default:
      priority = LOG_NOTICE;
      break;
  }

  asl_log(client, NULL, priority, "%s", line);
}

void setOutputStream(aslclient outputStream) {
  setOutputCallback(aslStreamOutputCallback, outputStream);
}

#endif  // defined(LIBOFP_TARGET_DARWIN)

static void trace1(const char *type, UInt64 id, const void *data,
                   size_t length) {
  if (length < sizeof(Header)) {
    detail::write_(Level::Trace, type, length, "Invalid Data:",
                   RawDataToHex(data, length));
    return;
  }

  Message message{data, length};
  message.transmogrify();

  yaml::Decoder decoder{&message};

  if (decoder.error().empty()) {
    detail::write_(Level::Trace, type, length, "bytes",
                   std::make_pair("connid", id), '\n', decoder.result(),
                   RawDataToHex(data, length));
  } else {
    detail::write_(Level::Trace, type, length, "bytes",
                   std::make_pair("connid", id), '\n', decoder.error(),
                   RawDataToHex(data, length));
  }
}

namespace detail {

void trace_msg_internal(const char *type, UInt64 id, const void *data,
                        size_t length) {
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

void trace_rpc_internal(const char *type, UInt64 id, const void *data,
                        size_t length) {
  const char *msg = static_cast<const char *>(data);

  // Remove trailing newline, if it exists.
  if (length > 0 && msg[length - 1] == '\n')
    --length;

  detail::write_(Level::Trace, type, length, "bytes",
                 std::make_pair("connid", id), '\n',
                 llvm::StringRef{msg, length});
}

}  // namespace detail
}  // namespace log
}  // namespace ofp
