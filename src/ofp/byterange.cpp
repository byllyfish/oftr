// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/byterange.h"
#include "ofp/bytelist.h"

using namespace ofp;

ByteRange::ByteRange(const ByteList &data)
    : begin_{data.begin()}, end_{data.end()} {
}

/// \brief Return true if contents of this byte range are printable ascii chars.
bool ByteRange::isPrintable() const {
  for (auto p = begin(); p < end(); ++p) {
    if (!std::isprint(*p) && !std::isspace(*p))
      return false;
  }
  return true;
}
