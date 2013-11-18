//  ===== ---- ofp/header.h --------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Header class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HEADER_H
#define OFP_HEADER_H

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class Header {
public:
  Header(OFPType type) : type_{type} {}

  UInt8 version() const { return version_; }
  void setVersion(UInt8 version) { version_ = version; }

  OFPType type() const { return type_; }
  void setType(OFPType type) { type_ = type; }
  void setType(deprecated::v1::OFPType type) {
    type_ = static_cast<OFPType>(type);
  }

  UInt16 length() const { return length_; }
  void setLength(UInt16 length) { length_ = length; }

  UInt32 xid() const { return xid_; }
  void setXid(UInt32 xid) { xid_ = xid; }

  static OFPType translateType(UInt8 version, UInt8 type, UInt8 newVersion);

  bool validateInput(UInt8 negotiatedVersion) const;

private:
  Big8 version_ = 0; // OFP_VERSION.
  OFPType type_;     // One of the OFPT_ constants.
  Big16 length_ = 0; // Length including this ofp_header.
  Big32 xid_ = 0;    // Transaction id for this packet.

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(Header) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Header>(), "Expected standard layout.");

} // </namespace ofp>

#endif // OFP_HEADER_H
