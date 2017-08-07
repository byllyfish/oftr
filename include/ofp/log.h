// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LOG_H_
#define OFP_LOG_H_

#include "ofp/logger.h"

namespace ofp {
namespace log {
namespace detail {

template <class T1, class T2>
llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                              const std::pair<T1, T2> &p) {
  return os << '{' << p.first << ": " << p.second << '}';
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                     const std::error_code &e) {
  return os << "{msg: " << e.message() << ", err: " << e.value() << '}';
}

// Print out UInt8 as an integer, not the char value.
// inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os, UInt8 n) {
//  return os << static_cast<int>(n);
//}

template <class Type>
void write_(llvm::raw_ostream &os, const Type &value1) {
  os << value1;
}

inline void write_(llvm::raw_ostream &os, const char *value1) {
  os << value1;
}

inline void write_(llvm::raw_ostream &os, UInt8 value1) {
  // Print out UInt8 as an integer, not the char value.
  os << static_cast<unsigned>(value1);
}

template <class Type, class... Args>
void write_(llvm::raw_ostream &os, const Type &value1, const Args &... args) {
  os << value1 << ' ';
  write_(os, args...);
}

template <class... Args>
void write_(llvm::raw_ostream &os, const char *value1, const Args &... args) {
  os << value1 << ' ';
  write_(os, args...);
}

template <class... Args>
void write_(llvm::raw_ostream &os, UInt8 value1, const Args &... args) {
  // Print out UInt8 as an integer, not the char value.
  os << static_cast<int>(value1) << ' ';
  write_(os, args...);
}

template <class... Args>
void write_(Level level, const Args &... args) {
// Logging is disabled when building oxm helper tools.
#if !defined(LIBOFP_LOGGING_DISABLED)
  std::string buf;
  llvm::raw_string_ostream oss{buf};
  write_(oss, args...);
  oss.flush();
  GLOBAL_Logger->write(level, buf.data(), buf.size());
#endif  // !defined(LIBOFP_LOGGING_DISABLED)
}

void trace_msg_(const char *type, UInt64 id, const void *data, size_t length);
void trace_rpc_(const char *type, UInt64 id, const void *data, size_t length);

}  // namespace detail

// N.B. Arguments of `log_` macros are only evaluated at the given log level.
#define LOG_IF_LEVEL_(LVL_, ...)                             \
  (void)(ofp::log::GLOBAL_Logger->enabled(ofp::log::LVL_) && \
         (ofp::log::detail::write_(ofp::log::LVL_, __VA_ARGS__), true))

#define log_error(...) LOG_IF_LEVEL_(Level::Error, __VA_ARGS__)
#define log_warning(...) LOG_IF_LEVEL_(Level::Warning, __VA_ARGS__)
#define log_info(...) LOG_IF_LEVEL_(Level::Info, __VA_ARGS__)

#if defined(NDEBUG)
#define log_debug(...) (void)0
#else
#define log_debug(...) LOG_IF_LEVEL_(Level::Debug, __VA_ARGS__)
#endif

inline void trace_msg(const char *type, UInt64 id, const void *data,
                      size_t length) {
  if (GLOBAL_Logger->enabled(Trace::Msg)) {
    detail::trace_msg_(type, id, data, length);
  }
}

inline void trace_rpc(const char *type, UInt64 id, const void *data,
                      size_t length) {
  if (GLOBAL_Logger->enabled(Trace::Rpc)) {
    detail::trace_rpc_(type, id, data, length);
  }
}

template <class... Args>
[[noreturn]] inline void fatal(const char *msg, const Args &... args) {
  detail::write_(Level::Fatal, msg, args...);
  std::abort();
}

template <class Ptr, class... Args>
inline Ptr fatal_if_null(Ptr value, const Args &... args) {
  return (value == nullptr) ? fatal("fatal_if_null", args...), value : value;
}

template <class... Args>
inline bool fatal_if_false(bool value, const Args &... args) {
  return !value ? fatal("fatal_if_false", args...), value : value;
}

template <class Type>
std::string hex(Type n) {
  return RawDataToHex(&n, sizeof(n));
}

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOG_H_
