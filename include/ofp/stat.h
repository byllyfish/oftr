// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_STAT_H_
#define OFP_STAT_H_

#include "ofp/oxmlist.h"
#include "ofp/statheader.h"

namespace ofp {

class Stat {
 public:
  Stat() = default;
  explicit Stat(const StatHeader *statHeader) { oxm_ = statHeader->oxmRange(); }

  OXMIterator begin() const { return oxm_.begin(); }
  OXMIterator end() const { return oxm_.end(); }

  OXMRange toRange() const { return oxm_.toRange(); }

 private:
  OXMList oxm_;
};

}  // namespace ofp

#endif  // OFP_STAT_H_
