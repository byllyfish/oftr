// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  static const size_t TS_BUFSIZE = 40;

  explicit Timestamp(time_t seconds = 0, UInt32 nanos = 0)
      : time_{seconds, nanos} {}

  time_t seconds() const { return time_.first; }
  UInt32 nanoseconds() const { return time_.second; }
  UInt32 microseconds() const { return nanoseconds() / 1000; }
  UInt32 milliseconds() const { return nanoseconds() / 1000000; }

  double secondsSince(const Timestamp &ts) const;
  void addSeconds(int seconds) { time_.first += seconds; }

  bool parse(const std::string &s);
  bool valid() const { return !(time_.first == 0 && time_.second == 0); }

  std::string toString() const;
  std::string toStringUTC() const;
  size_t toStringUTC(char (&buf)[TS_BUFSIZE]) const;

  void clear() {
    time_.first = 0;
    time_.second = 0;
  }

  bool operator==(const Timestamp &rhs) const { return time_ == rhs.time_; }
  bool operator!=(const Timestamp &rhs) const { return time_ != rhs.time_; }
  bool operator<(const Timestamp &rhs) const { return time_ < rhs.time_; }
  bool operator>(const Timestamp &rhs) const { return time_ > rhs.time_; }
  bool operator<=(const Timestamp &rhs) const { return time_ <= rhs.time_; }
  bool operator>=(const Timestamp &rhs) const { return time_ >= rhs.time_; }

  static Timestamp now();

 private:
  std::pair<time_t, UInt32> time_;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const Timestamp &value);
};

OFP_END_IGNORE_PADDING

inline std::ostream &operator<<(std::ostream &os, const Timestamp &value) {
  return os << value.toString();
}

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
