// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  explicit Timestamp(time_t seconds = 0, UInt32 nanos = 0)
      : time_{seconds, nanos} {}

  time_t seconds() const { return time_.first; }
  UInt32 nanoseconds() const { return time_.second; }
  UInt32 milliseconds() const { return nanoseconds() / 1000; }

  double secondsSince(const Timestamp &ts) const;
  
  bool parse(const std::string &s);
  bool valid() const { return !(time_.first == 0 && time_.second == 0); }
  std::string toString() const;

  void clear() { time_.first = 0; time_.second = 0; }

  bool operator==(const Timestamp &rhs) const { return time_ == rhs.time_; }
  bool operator!=(const Timestamp &rhs) const { return time_ != rhs.time_; }
  bool operator<(const Timestamp &rhs) const { return time_ < rhs.time_; }
  bool operator>(const Timestamp &rhs) const { return time_ > rhs.time_; }
  bool operator<=(const Timestamp &rhs) const { return time_ <= rhs.time_; }
  bool operator>=(const Timestamp &rhs) const { return time_ >= rhs.time_; }

  static Timestamp now();

 private:
  std::pair<time_t, UInt32> time_;
};

OFP_END_IGNORE_PADDING

inline std::ostream &operator<<(std::ostream &os, const Timestamp &value) {
  return os << value.toString();
}

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
