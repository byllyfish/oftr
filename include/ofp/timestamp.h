// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  Timestamp() : time_{0, 0} {}
  Timestamp(time_t seconds, UInt32 nanos) : time_{seconds, nanos} {}

  time_t seconds() const { return time_.first; }
  UInt32 nanoseconds() const { return time_.second; }
  UInt32 milliseconds() const { return nanoseconds() / 1000; }

  bool parse(const std::string &s);
  bool valid() const { return !(time_.first == 0 && time_.second == 0); }

  std::string toString() const;

  bool operator==(const Timestamp &rhs) const { return time_ == rhs.time_; }

  bool operator!=(const Timestamp &rhs) const { return time_ != rhs.time_; }

  bool operator>=(const Timestamp &rhs) const { return time_ >= rhs.time_; }

  static Timestamp now();

 private:
  std::pair<time_t, UInt32> time_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
