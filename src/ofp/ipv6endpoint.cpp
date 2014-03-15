//  ===== ---- ofp/ipv6endpoint.cpp ------------------------*- C++ -*- =====  //
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
/// \brief Implements the IPv6Endpoint class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/ipv6endpoint.h"
#include <sstream>
#include <iostream>

using namespace ofp;

static bool scan(std::istream &stream, char delimiter) {
  auto pos = stream.tellg();

  char ch;
  if (stream >> ch && ch == delimiter) {
    return true;
  }

  stream.clear();
  stream.seekg(pos);

  return false;
}

static bool scan(std::istream &stream, std::string &str, char delimiter) {
  auto pos = stream.tellg();

  if (std::getline(stream, str, delimiter) && !stream.eof()) return true;

  stream.clear();
  stream.seekg(pos);

  return false;
}

bool IPv6Endpoint::parse(const std::string &s) {
  std::stringstream iss{s};
  std::string addrStr;

  if (scan(iss, '[')) {
    if (!scan(iss, addrStr, ']')) {
      return false;
    }
    if (!scan(iss, ':')) {
      return false;
    }

  } else if (!scan(iss, addrStr, ':')) {
    addrStr.clear();
  }

  UInt16 port = 0;
  iss >> port;

  if (iss.eof() && port != 0) {
    if (addrStr.empty()) {
      addr_.clear();
    } else if (!addr_.parse(addrStr)) {
      return false;
    }

    port_ = port;
    return true;
  }

  return false;
}

void IPv6Endpoint::clear() {
  addr_.clear();
  port_ = 0;
}

std::string IPv6Endpoint::toString() const {
  std::string result;

  if (addr_.isV4Mapped()) {
    result += addr_.toString();
  } else {
    result += '[';
    result += addr_.toString();
    result += ']';
  }

  result += ':';
  result += std::to_string(port_);

  return result;
}
