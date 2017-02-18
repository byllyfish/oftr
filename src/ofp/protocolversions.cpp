// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
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

bool ProtocolVersions::isOnlyOneVersionSupported() const {
  UInt32 bits = bitmap_;
  if (bits == 0)
    return false;

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
