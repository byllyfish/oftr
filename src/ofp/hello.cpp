//  ===== ---- ofp/hello.cpp -------------------------------*- C++ -*- =====  //
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
/// \brief Implements Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/hello.h"
#include "ofp/log.h"
#include "ofp/constants.h"
#include "ofp/channel.h"

using namespace ofp;

ProtocolVersions detail::HelloElement::versionBitMap() const {
  if (length_ >= sizeof(UInt32) + 4) {
    return ProtocolVersions::fromBitmap(*Big32_cast(BytePtr(this) + 4));
  }

  log::info("HelloElement::versionBitMap unexpected size", length_);
  return ProtocolVersions::fromBitmap(0);
}

bool Hello::validateInput(Validation *context) const {
  return helloElements().validateInput(context);
}

ProtocolVersions Hello::protocolVersions() const {
  for (auto &elem : helloElements()) {
    if (elem.type() == OFPHET_VERSIONBITMAP)
      return elem.versionBitMap();
  }
  return ProtocolVersions::None;
}

/// Construct a HelloBuilder from the given Hello message.
HelloBuilder::HelloBuilder(const Hello *msg) : msg_{*msg} {
  setProtocolVersions(msg->protocolVersions());
}

ProtocolVersions HelloBuilder::protocolVersions() const {
  return ProtocolVersions::fromBitmap(bitmap_);
}

void HelloBuilder::setProtocolVersions(ProtocolVersions versions) {
  bitmap_ = versions.bitmap();
}

UInt32 HelloBuilder::send(Writable *channel) {
  // HelloRequest is the only message type that doesn't use the channel's
  // version number. If the highest version number is 4, don't send any
  // HelloElements; POX doesn't support the extended part of the Hello
  // request.

  Header &hdr = msg_.header_;
  UInt8 version = hdr.version();

  if (version == 0) {
    // Infer header version from value of versions bitmap.
    // If versions bitmap is empty, use latest version with a bitmap of all
    // versions.
    
    if (!bitmap_) {
      version = OFP_VERSION_LAST;
      bitmap_ = ProtocolVersions::All.bitmap();
    } else {
      version = ProtocolVersions::fromBitmap(bitmap_).highestVersion();
    }
    hdr.setVersion(version);

  } else if (version >= OFP_VERSION_4 && !bitmap_) {
    // Version is 1.3 or later, but the bitmap is empty. Set bitmap to 
    // contain just the header version.
    bitmap_ = ProtocolVersions{version}.bitmap();
  }

  size_t msgLen = sizeof(msg_);
  if (version >= OFP_VERSION_4) {
    // Include our hello element.
    msgLen += sizeof(elem_) + sizeof(bitmap_);
  }

  UInt32 xid = channel->nextXid();
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  elem_.setType(OFPHET_VERSIONBITMAP);
  elem_.setLength(sizeof(elem_) + sizeof(bitmap_));

  channel->write(this, msgLen);
  channel->flush();

  return xid;
}
