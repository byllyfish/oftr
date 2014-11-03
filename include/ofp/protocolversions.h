//  ===== ---- ofp/protocolversions.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the ProtocolVersions class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PROTOCOLVERSIONS_H_
#define OFP_PROTOCOLVERSIONS_H_

#include <initializer_list>
#include <vector>
#include "ofp/types.h"
#include "ofp/constants.h"

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
  constexpr explicit ProtocolVersions(UInt32 bitmap) : bitmap_(bitmap) {}

  enum {
    MinVersionSupported = OFP_VERSION_1,
    MaxVersionSupported = 30  // Maximum version supported by Big32 bitmap.
  };
};

}  // namespace ofp

#endif  // OFP_PROTOCOLVERSIONS_H_
