//  ===== ---- ofp/enetaddress_unittest.cpp ----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for EnetAddress class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/enetaddress.h"

using namespace ofp;

TEST(enetaddress, valid) {
  EnetAddress enet{"01-02-03-04-05-06"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("01-02-03-04-05-06", enet.toString());

  EnetAddress enet2{"01-02-03-04-05-06-07"}; // okay

  EXPECT_TRUE(enet2.valid());
  EXPECT_EQ("01-02-03-04-05-06", enet2.toString());
}

TEST(enetaddress, invalid) {
  EnetAddress enet{"01-02-03-04-05"};

  EXPECT_FALSE(enet.valid());
  EXPECT_EQ("00-00-00-00-00-00", enet.toString());
}

TEST(enetaddress, hash) {
  EnetAddress enet1{"01-02-03-04-05-06"};

  std::hash<EnetAddress> hasher;
  EXPECT_EQ(176514621, hasher(enet1));

  EnetAddress enet2{"01-02-03-04-05-07"};

  EXPECT_EQ(205143772, hasher(enet2));
}
