//  ===== ---- ofp/ipv6address.cpp -------------------------*- C++ -*- =====  //
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
/// \brief Implements IPv6Address class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/ipv6address.h"
#include "ofp/sys/asio_utils.h"
#include <arpa/inet.h>
#include "ofp/log.h"

using namespace ofp;

IPv6Address::IPv6Address(const IPv4Address &addr) {
  std::memcpy(&addr_[12], &addr, 4);
  addr_[11] = 0xFF;
  std::memset(addr_.data(), 0, 11);
}

IPv6Address::IPv6Address(const ArrayType &a) : addr_(a) {}

IPv6Address::IPv6Address(const std::string &s) {
  if (!parse(s))
    addr_.fill(0);
}

bool IPv6Address::parse(const std::string &s) {
  std::error_code err;

  auto addr6 = asio::ip::address_v6::from_string(s, err);
  if (!err) {
    addr_ = addr6.to_bytes();
    return true;
  }

  auto addr4 = asio::ip::address_v4::from_string(s, err);
  if (!err) {
    auto a = addr4.to_bytes();
    std::copy(a.data(), a.data() + 4, &addr_[12]);
    addr_[11] = 0xFF;
    std::memset(addr_.data(), 0, 11);
    return true;
  }

  return false;
}

void IPv6Address::clear() { std::memset(addr_.data(), 0, sizeof(addr_)); }

std::string IPv6Address::toString() const {
  if (isV4Mapped()) {
    IPv4Address v4 = toV4();
    return v4.toString();
  }

  char ipv6str[INET6_ADDRSTRLEN] = {};
  const char *result =
      inet_ntop(AF_INET6, addr_.data(), ipv6str, sizeof(ipv6str));
  return result ? ipv6str : "<inet_ntop_error6>";
}
