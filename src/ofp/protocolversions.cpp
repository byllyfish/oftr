// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/protocolversions.h"
#include <sstream>

using namespace ofp;

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
  if (msgVersion >= OFP_VERSION_4 && !msgVersions.empty()) {
    ProtocolVersions inCommon = intersection(msgVersions);
    return inCommon.highestVersion();
  }

  UInt8 myVersion = highestVersion();
  if (myVersion <= msgVersion) {
    return myVersion;
  }

  if (containsVersion(msgVersion)) {
    return msgVersion;
  }

  return 0;
}

/// Return true if we should include the versions bitmap in a Hello
/// message.
bool ProtocolVersions::includeInHelloMsg(UInt8 version) const {
  // Hello element was only added in version 1.3.
  if (version < OFP_VERSION_4) {
    return false;
  }

  // If bitmap *only* contains `version`, don't include hello versions.
  if (bitmap_ == (1U << version)) {
    return false;
  }

  // If bitmap contains *all* versions <= `version`, don't include hello
  // versions.
  UInt32 allVersions = (1 << (version + 1)) - 2;
  if (bitmap_ == allVersions) {
    return false;
  }

  return true;
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
      if (needComma)
        out << ", ";
      out << static_cast<int>(i);
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
