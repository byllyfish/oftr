// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_UNITTEST_H_
#define OFP_UNITTEST_H_

#include <gtest/gtest.h>
#include <cstdio>
#include "ofp/memorychannel.h"
#include "ofp/types.h"

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
