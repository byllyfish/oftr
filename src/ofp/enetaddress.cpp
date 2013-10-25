//  ===== ---- ofp/enetaddress.cpp -------------------------*- C++ -*- =====  //
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
/// \brief Implements EnetAddress class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/enetaddress.h"

using namespace ofp;

EnetAddress::EnetAddress(const std::string &s) {
  if (!parse(s)) {
    clear();
  }
}

bool EnetAddress::parse(const std::string &s) {
  return HexToRawData(s, addr_.data(), sizeof(addr_)) >= sizeof(addr_);
}

std::string EnetAddress::toString() const {
  return RawDataToHex(addr_.data(), sizeof(addr_), '-', 1);
}
