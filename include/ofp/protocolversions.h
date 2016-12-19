// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROTOCOLVERSIONS_H_
#define OFP_PROTOCOLVERSIONS_H_

#include <initializer_list>
#include <vector>
#include "ofp/constants.h"
#include "ofp/types.h"

namespace ofp {

/// \brief Represents a set of OpenFlow protocol versions.
class ProtocolVersions {
 public:
  enum : UInt32 {
    VersionBitmapAll = ~(~0U << (OFP_VERSION_LAST + 1)) & ~1U,
    VersionBitmapNone = 0U
  };

  /* implicit NOLINT */ ProtocolVersions(std::initializer_list<UInt8> versions);

  constexpr bool empty() const { return (bitmap_ == 0); }
  bool isOnlyOneVersionSupported() const;
  bool containsVersion(UInt8 version) const;

  UInt8 highestVersion() const;
  UInt8 negotiateVersion(UInt8 msgVersion, ProtocolVersions msgVersions) const;

  constexpr UInt32 bitmap() const { return bitmap_; }
  std::vector<UInt8> versions() const;

  constexpr ProtocolVersions intersection(ProtocolVersions versions) const {
    return ProtocolVersions(bitmap_ & versions.bitmap_);
  }

  std::string toString() const;

  constexpr static ProtocolVersions fromBitmap(UInt32 bitmap) {
    return ProtocolVersions(bitmap);
  }

  static ProtocolVersions fromVector(const std::vector<UInt8> &versions);

  static const ProtocolVersions None;
  static const ProtocolVersions All;

 private:
  UInt32 bitmap_;

  // N.B. You must call this constructor using (), not {}.
  constexpr explicit ProtocolVersions(UInt32 bitmap) noexcept
      : bitmap_(bitmap) {}

  enum {
    MinVersionSupported = OFP_VERSION_1,
    MaxVersionSupported = 30  // Maximum version supported by Big32 bitmap.
  };
};

}  // namespace ofp

#endif  // OFP_PROTOCOLVERSIONS_H_
