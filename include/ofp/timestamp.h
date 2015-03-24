// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  Timestamp() : time_{0, 0} {}
  Timestamp(time_t seconds, UInt32 nanos) : time_{seconds, nanos} {}

  bool parse(const std::string &s);
  bool valid() const { return !(time_.first == 0 && time_.second == 0); }

  std::string toString() const;

  bool operator==(const Timestamp &rhs) const {
    return time_ == rhs.time_;
  }

  bool operator!=(const Timestamp &rhs) const {
    return time_ != rhs.time_;
  }

  bool operator>=(const Timestamp &rhs) const {
    return time_ >= rhs.time_;
  }

 private:
  std::pair<time_t, UInt32> time_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
