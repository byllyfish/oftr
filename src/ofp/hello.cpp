// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/hello.h"

#include "ofp/channel.h"
#include "ofp/constants.h"
#include "ofp/log.h"

using namespace ofp;

// Hello element types
enum {
  OFPHET_VERSIONBITMAP = 1,  // Bitmap of version supported.
};

ProtocolVersions detail::HelloElement::versionBitMap() const {
  if (length_ >= sizeof(UInt32) + 4) {
    return ProtocolVersions::fromBitmap(*Big32_cast(BytePtr(this) + 4));
  }

  log_info("HelloElement::versionBitMap unexpected size", length_);
  return ProtocolVersions::fromBitmap(0);
}

ProtocolVersions Hello::protocolVersions() const {
  for (auto &elem : helloElements()) {
    if (elem.type() == OFPHET_VERSIONBITMAP)
      return elem.versionBitMap();
  }
  return ProtocolVersions::None;
}

/// \return Range of hello elements, or the empty range if invalid.
detail::HelloRange Hello::helloElements() const {
  detail::HelloRange elems = msgBody();

  Validation context;
  if (!elems.validateInput(&context)) {
    return {};
  }

  return elems;
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
  // version number.

  Header &hdr = msg_.header_;
  UInt8 version = hdr.version();
  ProtocolVersions versionsBitmap = ProtocolVersions::fromBitmap(bitmap_);

  if (version == 0) {
    // Infer header version from value of versions bitmap.
    // If versions bitmap is empty, use latest version with a bitmap of all
    // versions.

    if (versionsBitmap.empty()) {
      version = OFP_VERSION_LAST;
      versionsBitmap = ProtocolVersions::All;
    } else {
      version = versionsBitmap.highestVersion();
    }
    hdr.setVersion(version);

  } else if (version >= OFP_VERSION_4 && versionsBitmap.empty()) {
    // Version is 1.3 or later, but the bitmap is empty. Set bitmap to
    // contain just the header version.
    versionsBitmap = ProtocolVersions{version};
  }

  size_t msgLen = sizeof(msg_);
  if (versionsBitmap.includeInHelloMsg(version)) {
    // Include our hello element only if multiple versions are supported.
    bitmap_ = versionsBitmap.bitmap();
    msgLen += sizeof(elem_) + sizeof(bitmap_);
  }

  UInt32 xid = channel->nextXid();
  hdr.setLength(msgLen);
  hdr.setXid(xid);

  elem_.setType(OFPHET_VERSIONBITMAP);
  elem_.setLength(sizeof(elem_) + sizeof(bitmap_));

  channel->write(this, msgLen);
  channel->flush();

  return xid;
}
