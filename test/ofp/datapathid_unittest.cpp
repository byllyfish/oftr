//  ===== ---- ofp/datapathid_unittest.cpp -----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for DatapathID class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/datapathid.h"

using namespace ofp;

TEST(datapathid, test) {
  DatapathID a;
  EXPECT_HEX("0000 0000 0000 0000", &a, sizeof(a));

  DatapathID b{0x1234, EnetAddress{"00-01-02-03-04-05"}};
  EXPECT_EQ(0x1234, b.implementerDefined());
  EXPECT_EQ(EnetAddress{"00-01-02-03-04-05"}, b.macAddress());
  EXPECT_HEX("1234 00 01 02 03 04 05", &b, sizeof(b));

  EXPECT_EQ("0000-0000-0000-0000", a.toString());
  EXPECT_EQ("1234-0001-0203-0405", b.toString());

  EXPECT_LT(a, b);
  EXPECT_NE(a, b);

  DatapathID c{b};
  EXPECT_EQ(b, c);
}
