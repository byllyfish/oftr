// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_LOG_H_
#define OFP_LOG_H_

#include <sstream>
#include "ofp/loglevel.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {

// Allow llvm::StringRef to write to a std::ostream. Needs to be in llvm
// namespace for ADL to work.
inline std::ostream &operator<<(std::ostream &os, const StringRef &s) {
  return os.write(s.data(), ofp::Signed_cast(s.size()));
}

}  // namespace llvm

namespace ofp {
namespace log {
namespace detail {

extern OutputCallback GlobalOutputCallback;
extern void *GlobalOutputCallbackContext;
extern Level GlobalOutputLevelFilter;

template <class T1, class T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p) {
  return os << '{' << p.first << ": " << p.second << '}';
}

inline std::ostream &operator<<(std::ostream &os, const std::error_code &e) {
  return os << "{msg: " << e.message() << ", err: " << e.value() << '}';
}

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
#endif  // OFP_LOGGING_DISABLED
}

}  // namespace detail

void trace(const char *type, UInt64 id, const void *data, size_t length);

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
inline void warning(const Args &... args) {
  detail::write_(Level::Warning, args...);
}

template <class... Args>
inline void error(const Args &... args) {
  detail::write_(Level::Error, args...);
}

template <class... Args>
[[noreturn]] inline void fatal(const Args &... args) {
  detail::write_(Level::Fatal, args...);
  std::abort();
}

template <class Ptr>
inline Ptr fatal_if_null(Ptr value) {
  return (value == nullptr) ? fatal("Unexpected null ptr"), value : value;
}

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOG_H_
