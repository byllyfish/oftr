// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/match.h"

using namespace ofp;

Match::Match(const MatchHeader *matchHeader) {
  UInt16 type = matchHeader->type();

  if (type == OFPMT_OXM) {
    oxm_ = matchHeader->oxmRange();
  } else if (type == OFPMT_STANDARD) {
    assert(matchHeader->length() == deprecated::OFPMT_STANDARD_LENGTH);
    oxm_ = matchHeader->stdMatch()->toOXMList();
  } else {
    log::debug("Unknown matchType:", type);
  }
}
