// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_UNITTEST_H_
#define OFP_UNITTEST_H_

#include <gtest/gtest.h>
#include <cstdio>
#include "ofp/types.h"
#include "ofp/memorychannel.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

inline std::string hexclean(const char *data) {
  std::string s = ofp::HexToRawData(data);
  return ofp::RawDataToHex(s.data(), s.size());
}

#define EXPECT_HEX(hexstr, data, length)                   \
  {                                                        \
    auto hex_tmp__ = hexclean(hexstr);                     \
    EXPECT_EQ(hex_tmp__, ofp::RawDataToHex(data, length)); \
  }

#endif  // OFP_UNITTEST_H_
