// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
    log_debug("Unknown matchType:", type);
  }
}
