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

#ifndef OFP_PROTOCOL_VERSIONS_H
#define OFP_PROTOCOL_VERSIONS_H

#include "ofp/types.h"
#include "ofp/constants.h"
#include <initializer_list>
#include <vector>

namespace ofp { // <namespace ofp>

/// \brief Represents a set of OpenFlow protocol versions.
class ProtocolVersions {
public:
  enum {
    MinVersion = OFP_VERSION_1,
    MaxVersion = OFP_VERSION_LAST
  };

  enum Setting : UInt32 {
    All = ~(~0U << (MaxVersion + 1)) & ~1U,
    None = 0U
  };

  ProtocolVersions() : bitmap_{All} {}

  /* implicit */ ProtocolVersions(Setting setting) : bitmap_{setting} {}

  /* implicit */ ProtocolVersions(std::initializer_list<UInt8> versions);

  explicit ProtocolVersions(const std::vector<UInt8> &versions);

  bool empty() const { return (bitmap_ == 0); }

  UInt8 highestVersion() const;

  UInt32 bitmap() const { return bitmap_; }

  ProtocolVersions intersection(ProtocolVersions versions) const {
    return fromBitmap(bitmap_ & versions.bitmap_);
  }

  static ProtocolVersions fromBitmap(UInt32 bitmap);

  std::vector<UInt8> versions() const;

private:
  UInt32 bitmap_;
};

} // </namespace ofp>

#endif // OFP_PROTOCOL_VERSIONS_H
