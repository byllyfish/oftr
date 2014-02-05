//  ===== ---- ofp/unittest.h ------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines useful functions and classes for unit tests.
///
/// This file should be ignored (or not included in the installer) by users.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_UNITTEST_H_
#define OFP_UNITTEST_H_

#include <gtest/gtest.h>
#include "ofp/types.h"
#include "ofp/memorychannel.h"
#include <cstdio>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

inline std::string hexclean(const char *data) {
  std::string s = ofp::HexToRawData(data);
  return ofp::RawDataToHex(s.data(), s.size());
}

#define EXPECT_HEX(hexstr, data, length)                                       \
  {                                                                            \
    auto hex_tmp__ = hexclean(hexstr);                                         \
    EXPECT_EQ(hex_tmp__, ofp::RawDataToHex(data, length));                     \
  }

#endif  // OFP_UNITTEST_H_
