// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MATCH_H_
#define OFP_MATCH_H_

#include "ofp/standardmatch.h"
#include "ofp/matchheader.h"

namespace ofp {

class Match {
 public:
  Match() = default;
  explicit Match(const MatchHeader *matchHeader);

  OXMIterator begin() const { return oxm_.begin(); }
  OXMIterator end() const { return oxm_.end(); }

  OXMRange toRange() const { return oxm_.toRange(); }

 private:
  OXMList oxm_;
};

}  // namespace ofp

#endif  // OFP_MATCH_H_
