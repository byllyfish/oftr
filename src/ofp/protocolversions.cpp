//  ===== ---- ofp/protocolversions.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements a concrete class that represents a set of OpenFlow
/// protocol versions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/protocolversions.h"
#include <sstream>

namespace ofp {  // <namespace ofp>

const ProtocolVersions ProtocolVersions::All(
    ProtocolVersions::VersionBitmapAll);
const ProtocolVersions ProtocolVersions::None(
    ProtocolVersions::VersionBitmapNone);

ProtocolVersions::ProtocolVersions(std::initializer_list<UInt8> versions)
    : bitmap_{0} {
  for (auto v : versions) {
    if (v >= MinVersionSupported && v <= MaxVersionSupported) {
      bitmap_ |= (1 << v);
    }
  }
}

UInt8 ProtocolVersions::highestVersion() const {
  UInt32 bits = bitmap_;

  for (unsigned i = MaxVersionSupported; i > 0; --i) {
    if ((bits >> i) & 1) {
      return UInt8_narrow_cast(i);
    }
  }
  
  return 0;
}

/// \brief Negotiate a protocol version given Hello msgVersion and optional
/// HelloElement with supported versions.
UInt8 ProtocolVersions::negotiateVersion(UInt8 msgVersion,
                                         ProtocolVersions msgVersions) const {
  UInt8 myVersion = highestVersion();

  if (myVersion == msgVersion) {
    return myVersion;
  } else if (myVersion >= OFP_VERSION_4 && !msgVersions.empty()) {
    ProtocolVersions inCommon = intersection(msgVersions);
    return inCommon.highestVersion();
  } else if (myVersion < msgVersion) {
    return myVersion;
  } else if (containsVersion(msgVersion)) {
    return msgVersion;
  } else {
    return 0;
  }
}

bool ProtocolVersions::isOnlyOneVersionSupported() const {
  UInt32 bits = bitmap_;
  if (bits == 0) return false;

  while ((bits & 0x01) == 0) {
    bits >>= 1;
  }

  return (bits == 1);
}

bool ProtocolVersions::containsVersion(UInt8 version) const {
  return ((bitmap_ >> version) & 1);
}

std::vector<UInt8> ProtocolVersions::versions() const {
  std::vector<UInt8> result;

  for (UInt8 i = 1; i <= MaxVersionSupported; ++i) {
    if ((bitmap_ >> i) & 1) {
      result.push_back(i);
    }
  }

  return result;
}

std::string ProtocolVersions::toString() const {
  std::stringstream out;
  bool needComma = false;
  out << "[";

  for (UInt8 i = 1; i <= MaxVersionSupported; ++i) {
    if ((bitmap_ >> i) & 1) {
      if (needComma) out << ", ";
      out << int(i);
      needComma = true;
    }
  }

  out << "]";
  return out.str();
}

ProtocolVersions ProtocolVersions::fromVector(
    const std::vector<UInt8> &versions) {
  ProtocolVersions result{};

  for (auto v : versions) {
    if (v >= MinVersionSupported && v <= MaxVersionSupported) {
      result.bitmap_ |= (1 << v);
    }
  }

  return result;
}

}  // </namespace ofp>
