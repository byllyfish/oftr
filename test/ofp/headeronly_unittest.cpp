//  ===== ---- ofp/headeronly_unittest.cpp -----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for headeronly message classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/headeronly.h"

using namespace ofp;

TEST(headeronly, barrierrequestbuilder_v1) {
  BarrierRequestBuilder builder;

  MemoryChannel channel{OFP_VERSION_1};
  builder.send(&channel);

  EXPECT_EQ(0x08, channel.size());
  EXPECT_HEX("0112000800000001", channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_EQ(BarrierRequest::type(), message.type());

  const BarrierRequest *req = BarrierRequest::cast(&message);
  EXPECT_TRUE(req);

  EXPECT_HEX("0114000800000001", req, message.size());
}

TEST(headeronly, barrierrequestbuilder_v4) {
  BarrierRequestBuilder builder;

  MemoryChannel channel{OFP_VERSION_4};
  builder.send(&channel);

  EXPECT_EQ(0x08, channel.size());
  EXPECT_HEX("0414000800000001", channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_EQ(BarrierRequest::type(), message.type());

  const BarrierRequest *req = BarrierRequest::cast(&message);
  EXPECT_TRUE(req);

  EXPECT_HEX("0414000800000001", req, message.size());
}
