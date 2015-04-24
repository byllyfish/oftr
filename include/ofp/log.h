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
extern UInt32 GlobalOutputTraceFilter;

template <class T1, class T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p) {
  return os << '{' << p.first << ": " << p.second << '}';
}

inline std::ostream &operator<<(std::ostream &os, const std::error_code &e) {
  return os << "{msg: " << e.message() << ", err: " << e.value() << '}';
}

template <class Type1>
void write_(std::ostream &os, Level level, Type1 &&value1) {
  os << std::forward<Type1>(value1);
}

template <class Type1, class... Args>
void write_(std::ostream &os, Level level, Type1 &&value1,
            Args &&... args) {
  os << std::forward<Type1>(value1);
  os << ' ';
  write_(os, level, std::forward<Args>(args)...);
}

template <class... Args>
void write_(Level level, Args &&... args) {
#if !defined(LIBOFP_LOGGING_DISABLED)
  if (level >= detail::GlobalOutputLevelFilter) {
    std::ostringstream oss;
    write_(oss, level, std::forward<Args>(args)...);
    std::string buf = oss.str();
    GlobalOutputCallback(level, buf.data(), buf.size(),
                         GlobalOutputCallbackContext);
  }
#endif  // OFP_LOGGING_DISABLED
}

void trace_msg_internal(const char *type, UInt64 id, const void *data, size_t length);

void trace_rpc_internal(const char *type, UInt64 id, const void *data, size_t length);

}  // namespace detail

inline void trace_msg(const char *type, UInt64 id, const void *data, size_t length) {
  if ((detail::GlobalOutputTraceFilter & (1U << static_cast<int>(Trace::Msg))) != 0) {
    detail::trace_msg_internal(type, id, data, length);
  }
}

inline void trace_rpc(const char *type, UInt64 id, const void *data, size_t length) {
  if ((detail::GlobalOutputTraceFilter & (1U << static_cast<int>(Trace::Rpc))) != 0) {
    detail::trace_rpc_internal(type, id, data, length);
  }
}

template <class... Args>
inline void info(Args &&... args) {
  detail::write_(Level::Info, std::forward<Args>(args)...);
}

template <class... Args>
inline void debug(Args &&... args) {
#ifndef NDEBUG
  detail::write_(Level::Debug, std::forward<Args>(args)...);
#endif
}

template <class... Args>
inline void warning(Args &&... args) {
  detail::write_(Level::Warning, std::forward<Args>(args)...);
}

template <class... Args>
inline void error(Args &&... args) {
  detail::write_(Level::Error, std::forward<Args>(args)...);
}

template <class... Args>
[[noreturn]] inline void fatal(Args &&... args) {
  detail::write_(Level::Fatal, std::forward<Args>(args)...);
  std::abort();
}

template <class Ptr, class... Args>
inline Ptr fatal_if_null(Ptr value, Args &&... args) {
  return (value == nullptr) ? fatal("fatal_if_null", std::forward<Args>(args)...), value : value;
}

template <class... Args>
inline bool fatal_if_false(bool value, Args &&... args) {
  return !value ? fatal("fatal_if_false", std::forward<Args>(args)...), value : value;
}

// Use the LOG_LINE() macro to log source code file and line number.
// ie. a runtime assert can be written as:
//
//     log::fatal_if_false(condition, LOG_LINE());

#define LOG_LINE() std::make_pair(__FILE__, __LINE__)

template <class Type>
std::string hex(Type n) {
  return RawDataToHex(&n, sizeof(n));
}

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOG_H_
