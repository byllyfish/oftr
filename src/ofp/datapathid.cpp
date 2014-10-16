//  ===== ---- ofp/datapathid.cpp --------------------------*- C++ -*- =====  //
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
/// \brief Implements DatapathID class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/datapathid.h"

using namespace ofp;

DatapathID::DatapathID(Big16 implementerDefined, EnetAddress macAddress) {
  std::memcpy(dpid_.data(), &implementerDefined, sizeof(implementerDefined));
  std::memcpy(dpid_.data() + 2, &macAddress, sizeof(macAddress));
}

DatapathID::DatapathID(const std::string &dpid) {
  if (!parse(dpid))
    clear();
}

Big16 DatapathID::implementerDefined() const {
  Big16 result;
  std::memcpy(&result, dpid_.data(), sizeof(result));
  return result;
}

EnetAddress DatapathID::macAddress() const {
  EnetAddress result;
  std::memcpy(&result, dpid_.data() + 2, sizeof(result));
  return result;
}

std::string DatapathID::toString() const {
  return RawDataToHexDelimitedLowercase(dpid_);
}

bool DatapathID::parse(const std::string &s) {
  return HexToRawData(s, dpid_.data(), sizeof(dpid_)) >= sizeof(dpid_);
}
