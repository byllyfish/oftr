// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  explicit Timestamp(UInt64 seconds = 0, UInt32 nanos = 0)
      : time_{seconds, nanos} {
    assert(nanos < NANO_UNITS);
  }

  explicit Timestamp(double ts) {
    assert(ts >= 0.0);
    double fracpart, intpart;
    fracpart = std::modf(ts, &intpart);
    time_ = {static_cast<UInt64>(intpart), static_cast<UInt32>(fracpart * NANO_UNITS)};
  }

  time_t unix_time() const { return static_cast<time_t>(seconds()); }
  UInt64 seconds() const { return time_.first; }
  UInt32 nanoseconds() const { return time_.second; }
  UInt32 microseconds() const { return nanoseconds() / 1000; }
  UInt32 milliseconds() const { return nanoseconds() / 1000000; }

  double secondsSince(const Timestamp &ts) const;

  bool parse(const std::string &s);
  bool valid() const { return !(time_.first == 0 && time_.second == 0); }

  std::string toString() const;
  std::string toStringUTC() const;

  static const size_t TS_BUFSIZE = 40;
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

  void addSeconds(int seconds);

  Timestamp operator+(const Timestamp &rhs) const;

  static const Timestamp kInfinity;

 private:
  static const UInt64 MAX_TIME = 0xFFFFFFFFFFFFFFFFUL;
  static const UInt32 NANO_UNITS = 1000000000;

  std::pair<UInt64, UInt32> time_;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const Timestamp &value);
};

OFP_END_IGNORE_PADDING

// A time interval is implemented using the Timestamp class.
using TimeInterval = Timestamp;

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
