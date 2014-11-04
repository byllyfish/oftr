// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/byterange.h"
#include "ofp/bytelist.h"

using namespace ofp;

ByteRange::ByteRange(const ByteList &data)
    : begin_{data.begin()}, end_{data.end()} {}
