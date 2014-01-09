//  ===== ---- ofp/boost_asio_unittest.cpp -----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for asio utility functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/ipv6address.h"

using namespace ofp;
using namespace ofp::sys;

TEST(boost_asio, tcp) {
  auto endpt = makeEndpoint<tcp>(IPv6Address{"192.168.1.1"}, 80);

  log::debug("endpt", endpt.address().to_string());

  IPv6Address addr = makeIPv6Address(endpt.address());
  UInt16 port = endpt.port();

  EXPECT_EQ("192.168.1.1", addr.toString());
  EXPECT_EQ(80, port);
}

TEST(boost_asio, udp) {
  auto endpt = makeEndpoint<udp>(IPv6Address{"192.168.1.1"}, 80);

  IPv6Address addr = makeIPv6Address(endpt.address());
  UInt16 port = endpt.port();

  EXPECT_EQ("192.168.1.1", addr.toString());
  EXPECT_EQ(80, port);
}
